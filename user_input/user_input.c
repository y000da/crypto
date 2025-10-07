#include "user_input.h"


struct user_data *user_data_init(void) 
{
	struct user_data *tmp;
	tmp = (struct user_data *)malloc(sizeof(struct user_data));
	if (tmp == NULL) {
		wprintf(L"FAIL: Memory allocation failure");
		return NULL;
	}
	tmp->login = NULL;
	tmp->login_len = 0;
	tmp->password = NULL;
	tmp->password_len = 0;

	//wprintf(L"Init success\n");

	return tmp;
}

bool valid_login(const wchar_t *login, const size_t login_len)
{
	struct {
		unsigned int has_letter : 1;
	} flags_valid_login;

	if (login == NULL || login_len == 0) {
		wprintf(L"FAIL: Login cannot be empty\n");
		return INVALID;
	}

	if (login_len < LOGIN_MIN_LEN) {
		wprintf(L"FAIL: Login length should be more than %zu characters\n", \
			LOGIN_MIN_LEN);
		return INVALID;
	}

	// По идее эта проверка уже осуществляется при чтении логина,
	// но пока она наличествует для тестов
	if (login_len > LOGIN_MAX_LEN) {
		wprintf(L"FAIL: Login should be less than %zu characters\n", \
			LOGIN_MAX_LEN);
		return INVALID;
	}

	for (size_t i = 0; i < login_len; ++i) {
		if (!iswalnum(login[i]) && login[i] != '_') {
			wprintf(L"FAIL: Login can only contain letters, numbers, and the \"_\" symbol\n");
			return INVALID;
		}
	}

	if (iswdigit(login[0])) {
		wprintf(L"FAIL: Login cannot start with number\n");
		return INVALID;
	}

	flags_valid_login.has_letter = 0;
	for (size_t i = 0; i < login_len; ++i) {
		if (iswalpha(login[i]))
			flags_valid_login.has_letter = 1;
	}

	if (flags_valid_login.has_letter != 1) {
		wprintf(L"FAIL: Login should contain at least one letter\n");
		return INVALID;
	}

	return VALID;

}

bool valid_password(const wchar_t *password, size_t password_len)
{
	struct {
    		unsigned int has_upper : 1;
		unsigned int has_lower : 1;
		unsigned int has_digit : 1;
	} flags_valid_password;

	if (password == NULL || password_len == 0) {
		wprintf(L"FAIL: Password cannot be empty\n");
		return INVALID;
	}

	if (password_len < PASSWORD_MIN_LEN) {
		wprintf(L"FAIL: Password length should be more than %zu characters\n", \
			PASSWORD_MIN_LEN);
		return INVALID;
	}

	if (password_len > PASSWORD_MAX_LEN) {
		wprintf(L"FAIL: Password should be less than %zu characters\n", \
			PASSWORD_MAX_LEN);
		return INVALID;
	}

	flags_valid_password.has_upper = 0;
	flags_valid_password.has_lower = 0;
	flags_valid_password.has_digit = 0;
	for (size_t i = 0; i < password_len; ++i) {
		if (iswupper(password[i])) {
			flags_valid_password.has_upper = 1;
		} else if (iswlower(password[i])) {
			flags_valid_password.has_lower = 1;
		} else if (iswdigit(password[i])) {
			flags_valid_password.has_digit = 1;
		}
	}
	
	if (flags_valid_password.has_upper == 0) {
		wprintf(L"FAIL: Password should contain at least one uppercase letter\n");
		return INVALID;
	}

	if (flags_valid_password.has_lower == 0) {
		wprintf(L"FAIL: Password should contain at least one lowercase letter\n");
		return INVALID;
	}

	if (flags_valid_password.has_digit == 0) {
		wprintf(L"FAIL: Password should contain at least one digit\n");
		return INVALID;
	}

	return VALID;
}

bool get_login(struct user_data *data)
{
	// +2 т.к. должен влезть символ переноса строки и символ конца строки
	wchar_t buffer[LOGIN_MAX_LEN + 2];
	wprintf(L"login: ");
	
	// Считывание из стандартного ввода, проверяет успешность чтения.
	// Если введенный логин превышает размер буфера, очищает ввод и 
	// выводит ошибку
	if (fgetws(buffer, sizeof(buffer)/sizeof(wchar_t), stdin) != NULL) {
		if (wcschr(buffer, L'\n') == NULL) {
			winput_clear();
			wprintf(L"\nFAIL: Login is too long (>%zu characters)!\n", \
				LOGIN_MAX_LEN);
			return false;
		} else {
			buffer[wcscspn(buffer, L"\n")] = L'\0';
		}
	} else {
		wprintf(L"\nFAIL: Fail to read login\n");
		winput_clear();
		return false;
	}

	//printf("Buff read success\n");

	size_t len = wcslen(buffer);

	if (valid_login(buffer, len) == VALID) {
		if (data->login == NULL) {
			data->login = \
			(wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
		} else {
			data->login = \
			(wchar_t *)realloc(data->login, sizeof(wchar_t) * (len + 1));
		}
		
		if (data->login == NULL) {
			wprintf(L"\nFAIL: Memory allocation failure\n");
			return false;
		}

		wcscpy(data->login, buffer);
		data->login_len = len;
		return true;
	} else {
		return false;
	}

	return true;
}

bool get_password(struct user_data *data)
{
	wchar_t buffer[PASSWORD_MAX_LEN + 2];
	wprintf(L"password: ");

	#ifdef _WIN32
	static const int BACKSPACE = 8;
	int c;
	size_t i = 0;
	while ((c = _getch()) != '\r' && i < BUFFER_SIZE_PASSWORD - 1) {
		if (c == BACKSPACE) {
			if (i > 0) {
				--i;
				printf("\b \b");
			}
		} else {
			buffer[i++] = c;
			printf("*");
		}
	}
	#else

	#ifdef HIDE_PASS
	// Структура для сохранения настроек терминала
	struct termios old_tio, new_tio;

	tcgetattr(STDIN_FILENO, &old_tio);
	new_tio = old_tio;
	
	// Отключение эхо
	new_tio.c_lflag &= (~ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
	#endif

	if (fgetws(buffer, sizeof(buffer)/sizeof(wchar_t), stdin) != NULL) {
		if (wcschr(buffer, L'\n') == NULL) {
			winput_clear();
			wprintf(L"FAIL: Password is too long!(>%zu characters)!\n", \
				PASSWORD_MAX_LEN);
			return false;
		} else {
			buffer[wcscspn(buffer, L"\n")] = L'\0';
		}
	} else {
		wprintf(L"FAIL: Fail to read password\n");
		winput_clear();
		return false;

	}

	#ifdef HIDE_PASS
	tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
	#endif

	#endif

	//printf("Buff read success\n");

	size_t len = wcslen(buffer);

	if (valid_password(buffer, len) == VALID) {
		if (data->password == NULL) {
			data->password = \
			(wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
		} else {
			data->password = \
			(wchar_t *)realloc(data->password, sizeof(wchar_t) * (len + 1));
		}

		if (data->password == NULL) {
			wprintf(L"FAIL: Memory allocation failure\n");
			return false;
		}

		wcscpy(data->password, buffer);
		data->password_len = len;
		return true;
	} else {
		return false;
	}

	return true;
}

void cleanup_data(struct user_data *data) 
{
	free(data->login);
	free(data->password);
	free(data);
}

void winput_clear(void)
{
	wint_t c;
	while ((c = getwchar()) != L'\n' && c != WEOF);
}
