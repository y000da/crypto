#ifndef USER_INPUT_H
#define USER_INPUT_H


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wctype.h>
#include <wchar.h>


// Претензия на кроссплатформенность 
// TODO: Проверить на Windows
#ifdef _WIN32
	#include <conio.h>
#else
	#include <termios.h>
	#include <unistd.h>
#endif

#define HIDE_PASS

static const size_t LOGIN_MAX_LEN = 20;
static const size_t LOGIN_MIN_LEN = 3;
static const size_t PASSWORD_MAX_LEN = 50;
static const size_t PASSWORD_MIN_LEN = 8;

struct user_data {
	wchar_t *login;
	wchar_t *password;
	size_t login_len;
	size_t password_len;
};

enum valid_fail {INVALID, VALID};

struct user_data *user_data_init(void);

void winput_clear(void);

bool get_login(struct user_data *data);
bool get_password(struct user_data *data);

bool valid_login(const wchar_t *login, const size_t login_len);
bool valid_password(const wchar_t *password, size_t password_len);

void cleanup_data(struct user_data *data);

#endif
