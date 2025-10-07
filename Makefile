CC = gcc
CL = clang

CFLAGS = -std=c17 -g -Wall -Wextra -pedantic -fsanitize=address -Iuser_input -Icrypto -Imenu -lcrypto
CLFLAGS = -std=c17 -g -Wall -Wextra -pedantic -Iuser_input -Icrypto -Imenu -lcrypto

SRC_MAIN = main.c "user_input/user_input.c" "crypto/crypto.c" "menu/menu.c"

SRC_TEST_CRYPTO = "tests/crypto_test.c" "crypto/crypto.c" "user_input/user_input.c" 
SRC_TEST_INPUT = "tests/user_input_test.c" "user_input/user_input.c"

WRAP = -Wl,--wrap=fgetws,--wrap=getwchar,--wrap=tcsetattr

TARGET_DIR = $(PWD)/build
TEST_DIR = $(TARGET_DIR)

.PHONY: all test test-crypto test-crypto-run \
	test-input test-input-run clean run \
	create-out-dir clang-addsan clang-ubsan clang-memsan \
	clang-check clang-tidy clean-test

all: create-out-dir
	$(CC) $(SRC_MAIN) $(CFLAGS) -o $(TARGET_DIR)/main 

test: test-crypto test-input test-crypto-run test-input-run clean-test

test-crypto: create-out-dir
	$(CC) $(SRC_TEST_CRYPTO) $(CFLAGS) -lcmocka -o $(TEST_DIR)/test-crypto

test-input: create-out-dir
	$(CC) $(SRC_TEST_INPUT) $(CFLAGS) -lcmocka $(WRAP) -o $(TEST_DIR)/test-input

test-crypto-run:
	$(TEST_DIR)/test-crypto

test-input-run:
	$(TEST_DIR)/test-input

run: create-out-dir
	$(TARGET_DIR)/main

create-out-dir:
	mkdir -p build/out

clean: 
	rm -rf build/*

clean-test:
	rm build/out/.orig_pass_t__* build/out/.key_t__* build/out/.iv_t__* build/out/.pass_t__* build/out/.tag_t__*

clang-addsan: create-out-dir
	$(CL) $(SRC_MAIN) $(CLFLAGS) -fsanitize=address -o $(TARGET_DIR)/main

clang-ubsan: create-out-dir
	$(CL) $(SRC_MAIN) $(CLFLAGS) -fsanitize=undefined -o $(TARGET_DIR)/main

clang-memsan: create-out-dir
	$(CL) $(SRC_MAIN) $(CLFLAGS) -fsanitize=memory -o $(TARGET_DIR)/main

clang-tidy: create-out-dir
	clang-tidy $(SRC_MAIN) -- -Iuser_input -Icrypto -Imenu

clang-check: clang-addsan clang-ubsan clang-memsan clang-tidy