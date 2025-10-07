#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <wchar.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

// Для очистки ввода
#include "../user_input/user_input.h"


#define KEY128 16
#define KEY192 24
#define KEY256 32

enum menu_params {INPUT = 1, DECRYPT, DELETE, HELP, EXIT};

bool get_key_length(int *key_len);
void print_menu(void);
void print_help(void);
int choice_read(int *choice);

#endif
