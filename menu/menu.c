#include "menu.h"


void print_menu(void) 
{
	wprintf(L"\nIt is a small password encrypt/decrypt program, please, choose action:\n\
\t%d -- Insert login and password and encrypt password\n\
\t%d -- Decrypt password by existing login\n\
\t%d -- Delete user data\n\
\t%d -- Help\n\
\t%d -- Exit\n", INPUT, DECRYPT, DELETE, HELP, EXIT);
	wprintf(L"> ");
}

void print_help(void)
{
	wprintf(L"\nIf you choose \"%d\" option you will able to \n\
insert your <LOGIN> and <PASSWORD>,\n\
then choose the length of <KEY> to encrypt inserted password.\n\
Then program will encrypt your PASSWORD associated with your LOGIN\n\
and save original <PASSWORD> in file to compare it with encrypted one.\n", INPUT);
	wprintf(L"\nIf you choose \"%d\" option you will able to \n\
compare encrypted <PASSWORD> with original one by inserting the associated <LOGIN>\n\
Program will display result of comparing.\n", DECRYPT);
	wprintf(L"\nIf you choose \"%d\" option you will able to \n\
delete <LOGIN> and associated <PASSWORD> from the data storage.\n", DELETE);
	wprintf(L"\nIf you choose \"%d\" option it will display this <HELP>\n", HELP);
	wprintf(L"\nIf you choose \"%d\" option program will be closed\n", EXIT);
}

bool get_key_length(int *key_len)
{
	wprintf(L"\nType in desired key length:\n"
		L"1 -- 128 bit\n"
		L"2 -- 192 bit\n"
		L"3 -- 256 bit\n");
	wprintf(L"> ");
	int key_len_choice = -1;
	choice_read(&key_len_choice);
	
	switch (key_len_choice) {
	case 1:	
		*key_len = KEY128;
		return true;
	case 2:	
		*key_len = KEY192;
		return true;
	case 3: 
		*key_len = KEY256;
		return true;
	default: 
		wprintf(L"\nFAIL: Wrong key length!\n");
		winput_clear();
		return false;
	}

	return true;
}

int choice_read(int *choice)
{
	const int BUFFER_SIZE = 10;
	wchar_t buffer[BUFFER_SIZE];
	wchar_t *endptr;
	int tmp;

	if (fgetws(buffer, sizeof(buffer)/sizeof(wchar_t), stdin) != NULL) {
		if (wcschr(buffer, L'\n') == NULL) {
			winput_clear();
			wprintf(L"\nFAIL: Incorrect input\n");
			return 0;
		} else {
			buffer[wcscspn(buffer, L"\n")] = L'\0';
		}
	} else {
		wprintf(L"\nFAIL: Incorrect input\n");
		winput_clear();
		return false;
	}

        tmp = wcstol(buffer, &endptr, BUFFER_SIZE);

	if (endptr != buffer && 
            *endptr != L'\0' && *endptr != L'\n') {
            wprintf(L"\nFAIL: Incorrect input\n");
            return 0;
        }
        
        if (errno == ERANGE || tmp < INT_MIN || tmp > INT_MAX) {
            wprintf(L"\nFAIL: Incorrect input\n");
            return 0;
        }
        
        if (endptr == buffer) {
            wprintf(L"\nFAIL: Incorrect input\n");
            return 0;
        }

        *choice = tmp;
	return true;
}
