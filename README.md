# crypto
## Описание
Консольное приложение для шифрования пароля методом AES-GCM и проверки корректности шифрования с помощью open-source библиотеки OpenSSL. 
Реализованные операции: 
- Шифрование пароля с ключом заданной длины с сохранением исходного пароля
- Дешифровка пароля с проверкой корректности работы
- Удаление пользовательских данных по логину
- Тестирование основных модулей с помощью cmocka
- Корректная работа с UTF-8
## Зависимости
- **Компилятор:** GCC версии 9.0 и выше
- **Система сборки**: Make
- **Библиотеки:** OpenSSL 3.0.13 и выше
- **ОС:** Linux 

Проверьте установку зависимостей:
`gcc --version`
`make --version`
`openssl version`
## Сборка проекта
### Стандартная сборка
`make`
### Сборка для тестирования
`sudo make cmocka`
`make test`
### Удаление временных файлов и пользовательских данных
`make clean`
## Структура проекта
- crypto/
  - main.c/ # Главная программа
  - Makefile
  - build/  # Исполняемые (создаются при сборке) и пользовательские файлы
  - cmocka/ # Библиотека cmocka
  - crypto/
    - crypto.c # Основные функции encrypt() и decrypt()
    - crypto.h 
  - menu/
    - menu.c # Вспомогательные функции для построения меню
    - menu.h
  - tests/
    - crypto_test.c     # Тестирование функций encrypt() и decrypt()
    - user_input_test.c # Тестирование функций ввода логина и пароля
  - user_input
    - user_input.c # Функции ввода логина и пароля
    - user_input.h
## Использование
### Запуск программы
`build/crypto`
или 
`make run`
### Примеры ввода/вывода
#### Пример 1: Шифрование пароля
	It is a small password encrypt/decrypt program, please, choose action:
		1 -- Insert login and password and encrypt password
		2 -- Decrypt password by existing login
		3 -- Delete user data
		4 -- Help
		5 -- Exit
	\> 1
	login: login
	password: 
	Type in desired key length:
	1 -- 128 bit
	2 -- 192 bit
	3 -- 256 bit
	> 1
	
	Encrypting...
	
	Saving original pass...
	
	Saving encrypted pass...
	
	DONE: 128-bit encrypted: 9 bytes
#### Пример 2: Дешифровка пароля
	It is a small password encrypt/decrypt program, please, choose action:
		1 -- Insert login and password and encrypt password
		2 -- Decrypt password by existing login
		3 -- Delete user data
		4 -- Help
		5 -- Exit
	> 2
	login: login
	
	Decrypting...
	
	Comparing...
	
	Original password:	123asdASD
	Decrypted password:	123asdASD
	
	DONE: Passwords are equivalent!
#### Пример 3: Удаление пользовательских данных
	It is a small password encrypt/decrypt program, please, choose action:
		1 -- Insert login and password and encrypt password
		2 -- Decrypt password by existing login
		3 -- Delete user data
		4 -- Help
		5 -- Exit
	> 3
	login: login
	
	Deleting <login> data...
	
	DONE: <login>'s data deleted
## Основные функции
	struct user_data *user_data_init(void)
Инициализация структуры user_data, хранящей логин и пароль. 
Производится с выделением памяти malloc, поэтому после взаимодействий со структурой нужно использовать  функцию `void cleanup_data(struct user_data *data)` 

	bool get_login(struct user_data *data)
Безопасный ввод логина с последующей валидацией

	bool get_password(struct user_data *data)
Безопасный ввод пароля с последующей валидацией и сокрытием данных при вводе

	bool encrypt(const struct user_data *data, const int key_len)
Шифрование пароля по методу AES-GCM с использованием OpenSSL.
Исходный пароль сохраняется в файле для последующей проверки с дешифрованным.
Затем безопасно случайно генерируется ключ заданной (`const int key_len`) длины, IV и тэг. Каждый набор ключ-IV-тэг сохраняется в свои файлы, ассоциированные с логином.
Затем на основе этих данных шифруется пароль и сохраняется в свой файл, ассоциированный с логином.

	bool decrypt(const wchar_t *login)
Дешифровка пароля и сравнение его с исходным.
Из соответствующих файлов, ассоциированных с логином, берутся зашифрованный пароль, ключ, тэг и IV.
Затем пароль дешифруется, сравнивается с исходным и оба выводятся в консоль.
