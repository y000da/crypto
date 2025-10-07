#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "user_input/user_input.h"
#include "crypto/crypto.h"
#include "menu/menu.h"


int main(void) 
{
	setlocale(LC_ALL, "ru_RU.UTF-8");
	struct user_data *data;
	while(true) {
		print_menu();
		int choice = -1;
		while(!choice_read(&choice)) { print_menu(); }
		switch(choice) {
		case INPUT:
			data = user_data_init();
			while (!get_login(data)) { ; }
			while (!get_password(data)) { ; }
			int key_len = 0;
			while (!get_key_length(&key_len)) { ; }
			encrypt(data, key_len);
			cleanup_data(data);
			continue;
		case DECRYPT:
			data = user_data_init();
			while (!get_login(data)) { ; }
			decrypt(data->login);
			cleanup_data(data);
			continue;
		case DELETE:
			data = user_data_init();
			while(!get_login(data)) { ; }
			delete_data(data->login);
			cleanup_data(data);
			continue;
		case HELP:
			print_help();
			continue;
		case EXIT:
			wprintf(L"\nSee you soon!\n");
			exit(0);
		default:
			wprintf(L"\nIncorrect input");
			continue;
		} 
	}
	return 0;
}
