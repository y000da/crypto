#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdbool.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/rand.h>
#include <openssl/evp.h>

// Здесь определена структура пользовательских данных
#include "../user_input/user_input.h"
// Здесь определены константы длины ключа
#include "../menu/menu.h"

// TODO: Придумать как выделить в этих определениях общую часть
#define PASS_FILE_PREFIX        L"build/out/.pass_"
#define KEY_FILE_PREFIX         L"build/out/.key_"
#define IV_FILE_PREFIX          L"build/out/.iv_"
#define TAG_FILE_PREFIX         L"build/out/.tag_"
#define ORIG_PASS_FILE_PREFIX   L"build/out/.orig_pass_"

static const int IV_LEN = 12;
static const int TAG_LEN = 16;
static const int INIT_CIPHERPASS_LEN = 1024;

// Шифрование пароля по методу AES-GCM с заданной длиной ключа и сохранение оригинального
bool encrypt(const struct user_data *data, const int key_len);

// Расшифровка пароля на основе логина и сравнение с сохраненным оригинальным
bool decrypt(const wchar_t *login);

bool delete_data(const wchar_t *login);

#endif
