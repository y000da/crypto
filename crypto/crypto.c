#include "crypto.h"


// Собирает имя файла из wchar_t строки с нужным префиксом, нужно для open_file_w_prefix()
static char* prefix_file_name(const wchar_t *login, const wchar_t *prefix)
{
	// Определяем суммарную длину имени
	const size_t total_fname_len = wcslen(login) + wcslen(prefix) + 1;

	wchar_t *fname = (wchar_t *)malloc(total_fname_len * sizeof(wchar_t));
	if (fname == NULL) {
		wprintf(L"\nFAIL: Memory allocation failure\n");
		return NULL;
	}

	// Собираем строку -- имя файла с префиксом
	wcscpy(fname, prefix);
	wcscat(fname, login);

	size_t mb_size = wcstombs(NULL, fname, 0);

        char *mb_fname = (char *)malloc(mb_size + 1);
	if (mb_fname == NULL) {
		wprintf(L"\nFAIL: Memory allocation failure\n");
		free(fname);
		return NULL;
	}

        wcstombs(mb_fname, fname, mb_size + 1);

	free(fname);

	return mb_fname;
}

// Открывает файл с заданным именем на основе prefix_file_name
static FILE* open_file_w_prefix(const wchar_t *login, const wchar_t *prefix, const char *mode)
{
	char *mb_fname = prefix_file_name(login, prefix);
	if (mb_fname == NULL) {
		wprintf(L"\nFAIL: Memory allocation failure\n");
		free(mb_fname);
		return NULL;
	}

    	FILE* prefix_file = fopen(mb_fname, mode);
	if(prefix_file == NULL) {;
		free(mb_fname);
		return NULL;
	}

	free(mb_fname);

	return prefix_file;
}

// Запись в файл с заданным именем
static bool write_file_w_prefix(const unsigned char *buffer, const int buffer_len, const wchar_t *login, const wchar_t *prefix)
{
	FILE *tmp_file = open_file_w_prefix(login, prefix, "wb");
	if (tmp_file == NULL) {
		wprintf(L"FAIL: Failed to open file\n");
		return false;
	}

	fwrite(&buffer_len, sizeof(int), 1, tmp_file);

	for(int i = 0; i < buffer_len; ++i) {
		fputc(buffer[i], tmp_file);
	}

	fclose(tmp_file);

	return true;
}

// Чтение buffer_len байтов из файла с заданным именем
static bool read_file_w_prefix(unsigned char *buffer, const int buffer_len, const wchar_t *login, const wchar_t *prefix)
{
	FILE *tmp_file = open_file_w_prefix(login, prefix, "rb");
	if (tmp_file == NULL) {
		wprintf(L"\nFAIL: Incorrect login\n");
		return false;
	}

	fseek(tmp_file, sizeof(buffer_len), SEEK_SET);

	for(int i = 0; i < buffer_len; ++i) {
		buffer[i] = fgetc(tmp_file);
	}

	fclose(tmp_file);

	return true;
}

// Чтение длины из файла с заданным именем
static int read_length(const wchar_t *login, const wchar_t *prefix)
{
	FILE *tmp_file = open_file_w_prefix(login, prefix, "rb");
	if (tmp_file == NULL) {
		wprintf(L"\nFAIL: Incorrect login\n");
		return 0;
	}
	int tmp = 0;
	fread(&tmp, sizeof(int), 1, tmp_file);
	fclose(tmp_file);
	return tmp;
}

// Сохранение в файл оригинального пароля
static bool save_orig_pass(const char *buffer, const size_t buffer_len, const wchar_t *login) 
{
	FILE* tmp_file = open_file_w_prefix(login, ORIG_PASS_FILE_PREFIX, "wb");
	if (tmp_file == NULL) {
		wprintf(L"\nFAIL: Incorrect login\n");
		return false;
	}

	fwrite(&buffer_len, sizeof(int), 1, tmp_file);
	for(size_t i = 0; i < buffer_len; ++i) {
		fputc(buffer[i], tmp_file);
	}
	fclose(tmp_file);

	return true;

}

bool encrypt(const struct user_data *data, const int key_len)
{	
	if (data == NULL)
		return false;
	
	if (valid_login(data->login, data->login_len) == INVALID)
		return false;

	if (valid_password(data->password, data->password_len) == INVALID)
		return false;

	if (key_len != KEY128 && key_len != KEY192 && key_len != KEY256)
		return false;

	wprintf(L"\nEncrypting...\n");
	// Преобразование wchar_t пароля в char*
	size_t mb_pass_len = wcstombs(NULL, data->password, 0);
	char mb_pass[mb_pass_len + 1];
        wcstombs(mb_pass, data->password, mb_pass_len + 1);

	// Сохранение исходного пароля в файл
	wprintf(L"\nSaving original pass...\n");
	save_orig_pass(mb_pass, mb_pass_len, data->login);
	
	// Получение ключа
	unsigned char key[key_len];
	if (RAND_bytes(key, key_len) != 1) { 
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}

	// Запись ключа в файл
	write_file_w_prefix(key, key_len, data->login, KEY_FILE_PREFIX);

	// Получение IV
	unsigned char iv[IV_LEN];
	if (RAND_bytes(iv, IV_LEN) != 1) { 
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}

	// Запись IV в файл
	write_file_w_prefix(iv, IV_LEN, data->login, IV_FILE_PREFIX);

	// Выбор метода шифрования
	const EVP_CIPHER *cipher;

	switch(key_len) {
	case KEY128: 
		cipher = EVP_aes_128_gcm();
		break;
	case KEY192:
		cipher = EVP_aes_192_gcm();
		break;
	case KEY256:
		cipher = EVP_aes_256_gcm();
		break;
	default: 
		wprintf(L"\nFAIL: Incorrect key length\n");
		return false;
	}

	// Инициализация контекста шифрования
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	// Инициализация шифрования
	if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}
	
	// Объявление результата шифрования
	unsigned char cipherpass[INIT_CIPHERPASS_LEN];
	int len = 0;
	int cipherpass_len = 0;

	// Шифрование
	if (EVP_EncryptUpdate(ctx, cipherpass, &len, 
				(unsigned char *)mb_pass, mb_pass_len) != 1) {
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}
	cipherpass_len = len;
	if (EVP_EncryptFinal_ex(ctx, cipherpass + len, &len) != 1) {
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}
	cipherpass_len += len;

	// Объявление и инициализация тэга
	unsigned char tag[TAG_LEN];
	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LEN, tag) != 1) {
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}

	// Запись тэга в файл
	write_file_w_prefix(tag, TAG_LEN, data->login, TAG_FILE_PREFIX);

	// Запись зашифрованного пароля в файл
	wprintf(L"\nSaving encrypted pass...\n");
	write_file_w_prefix(cipherpass, cipherpass_len, data->login, PASS_FILE_PREFIX);

	wprintf(L"\nDONE: %d-bit encrypted: %d bytes\n", key_len * 8, cipherpass_len);

	// Очистка после работы
	EVP_CIPHER_CTX_free(ctx);

	return true;
}

bool decrypt(const wchar_t *login)
{
	// Проверка
	if (login == NULL)
		return false;
	
	if (valid_login(login, wcslen(login)) == INVALID)
		return false;

	wprintf(L"\nDecrypting...\n");
	
	// Чтение зашифрованного пароля
	int cipherpass_len = read_length(login, PASS_FILE_PREFIX);
	if (cipherpass_len == 0) 
		return false;
	
	unsigned char cipherpass[cipherpass_len];
	read_file_w_prefix(cipherpass, cipherpass_len, login, PASS_FILE_PREFIX);

	// Чтение ключа
	int key_len = read_length(login, KEY_FILE_PREFIX);
	if (key_len == 0) 
		return false;
	
	unsigned char key[key_len];
	read_file_w_prefix(key, key_len, login, KEY_FILE_PREFIX);

	// Чтение IV
	int iv_len = read_length(login, IV_FILE_PREFIX);
	if (iv_len == 0) 
		return false;
	
	unsigned char iv[iv_len];
	read_file_w_prefix(iv, iv_len, login, IV_FILE_PREFIX);

	// Чтение тэга
	int tag_len = read_length(login, TAG_FILE_PREFIX);
	if (tag_len == 0) 
		return false;

	unsigned char tag[tag_len];
	read_file_w_prefix(tag, tag_len, login, TAG_FILE_PREFIX);

	// Выбор метода шифрования
	const EVP_CIPHER *cipher;
	switch(key_len) {
	case KEY128: 
		cipher = EVP_aes_128_gcm();
		break;
	case KEY192:
		cipher = EVP_aes_192_gcm();
		break;
	case KEY256:
		cipher = EVP_aes_256_gcm();
		break;
	default: 
		wprintf(L"\nFAIL: Incorrect key length\n");
		return false;
	}

	// Инициализация контекста шифрования
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}
	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LEN, (void *)tag) != 1) {
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}

	unsigned char *decrypted_pass = (unsigned char *)malloc(cipherpass_len);
	int len = 0;
	int decrypted_pass_len = 0;
	if (EVP_DecryptUpdate(ctx, decrypted_pass, &len, cipherpass, cipherpass_len) != 1) {
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}
	decrypted_pass_len = len;
	if (EVP_DecryptFinal_ex(ctx, decrypted_pass + len, &len) != 1) {
		wprintf(L"\nFAIL: Encryption failure\n");
		return false;
	}
	decrypted_pass_len += len;
	EVP_CIPHER_CTX_free(ctx);

	// Сравнение паролей
	wprintf(L"\nComparing...\n");

	// Чтение исходного пароля
	int orig_pass_len = read_length(login, ORIG_PASS_FILE_PREFIX);
	if (orig_pass_len == 0) 
		return false;
	
	unsigned char *orig_pass = (unsigned char *)malloc(orig_pass_len);
	read_file_w_prefix(orig_pass, orig_pass_len, login, ORIG_PASS_FILE_PREFIX);

	wprintf(L"\nOriginal password:\t%s\n", orig_pass);
	wprintf(L"Decrypted password:\t%s\n", decrypted_pass);

	// Побайтовое сравнение
	if (decrypted_pass_len != orig_pass_len) {
		wprintf(L"\nFAIL: Password aren't equivalent\n");
		free(orig_pass);
		free(decrypted_pass);
		return false;
	}

	if(memcmp(orig_pass, decrypted_pass, orig_pass_len) == 0) {
		wprintf(L"\nDONE: Passwords are equivalent!\n");
		free(orig_pass);
		free(decrypted_pass);
		return true;
	} else {
		wprintf(L"\nFAIL: Password aren't equivalent\n");
		free(orig_pass);
		free(decrypted_pass);
		return false;
	}

	free(orig_pass);
	free(decrypted_pass);
	return true;
}

bool delete_data(const wchar_t *login)
{
	wprintf(L"\nDeleting <%ls> data...\n", login);

	char *del_pass_file = prefix_file_name(login, PASS_FILE_PREFIX);
	if (remove(del_pass_file) != 0) {
		wprintf(L"\nFAIL: Incorrect login\n");
		free(del_pass_file);
		return false;
	}

	char *del_orig_pass_file = prefix_file_name(login, ORIG_PASS_FILE_PREFIX);
	if (remove(del_orig_pass_file) != 0) {
		wprintf(L"\nFAIL: Incorrect login\n");
		free(del_pass_file);
		free(del_orig_pass_file);
		return false;
	}

	char *del_iv_file = prefix_file_name(login, IV_FILE_PREFIX);
	if (remove(del_iv_file) != 0) {
		wprintf(L"\nFAIL: Incorrect login\n");
		free(del_pass_file);
		free(del_orig_pass_file);
		free(del_iv_file);
		return false;
	}

	char *del_key_file = prefix_file_name(login, KEY_FILE_PREFIX);
	if (remove(del_key_file) != 0) {
		wprintf(L"\nFAIL: Incorrect login\n");
		free(del_pass_file);
		free(del_orig_pass_file);
		free(del_iv_file);
		free(del_key_file);
		return false;
	}
	char *del_tag_file = prefix_file_name(login, TAG_FILE_PREFIX);
	if (remove(del_tag_file) != 0) {
		wprintf(L"\nFAIL: Incorrect login\n");
		free(del_pass_file);
		free(del_orig_pass_file);
		free(del_iv_file);
		free(del_key_file);
		free(del_tag_file);
		return false;
	}

	wprintf(L"\nDONE: <%ls>'s data deleted\n", login);

	free(del_pass_file);
	free(del_orig_pass_file);
	free(del_iv_file);
	free(del_key_file);
	free(del_tag_file);

	return true;
}
