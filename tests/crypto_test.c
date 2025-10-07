#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <wchar.h>
#include <stdbool.h>
#include <locale.h>


#include "../crypto/crypto.h" 
#include "../user_input/user_input.h"


static void test_encrypt_correct(void **state)
{
        (void) state;

        struct user_data correct_data_cases[] = {
                {L"t__login", L"PASSword123", wcslen(L"t__login"), wcslen(L"PASSword123")},
                {L"t__логин", L"PASSword123", wcslen(L"t__логин"), wcslen(L"PASSword123")},
                {L"t__login", L"ПАРоль123", wcslen(L"t__login"), wcslen(L"ПАРоль123")},
                {L"t__логин", L"ПАРоль123", wcslen(L"t__логин"), wcslen(L"ПАРоль123")},
                {L"t__承る承る承る", L"PASSword123", wcslen(L"t__承る承る承る"), wcslen(L"PASSword123")},
                {L"t__login", L"承る承る承るpasd1123asdAAAA", wcslen(L"t__login"), wcslen(L"承る承る承るpasd1123asdAAAA")},
                {L"t__login", L"password123AA###  password$$$", wcslen(L"t__login"), wcslen(L"password123AA###  password$$$")},
                {L"t__login", L"password123AA_😀😀😀", wcslen(L"t__login"), wcslen(L"password123AA_😀😀😀")},
        };

        for (size_t i = 0; i < sizeof(correct_data_cases) / sizeof(correct_data_cases[0]); ++i) {
                bool result128 = encrypt(&correct_data_cases[i], KEY128);
                bool result192 = encrypt(&correct_data_cases[i], KEY192);
                bool result256 = encrypt(&correct_data_cases[i], KEY256);
                
                if (result128 != true)
                        cm_print_error("\n[   FAIL   ] TEST CASE: %zu\n", i + 1);

                assert_true(result128);
                assert_true(result192);
                assert_true(result256);
        }
}

static void test_encrypt_wrong(void **state)
{
        (void) state;

        struct user_data wrong_data_cases[] = {
                {NULL, NULL, 0, 0},
                {NULL, L"PASSword123", 0, wcslen(L"PASSword123")},
                {L"login", NULL, wcslen(L"login"), 0},
                {L"login", L"PASSword123", 10, 10},
                {L"login", L"PASSword123", wcslen(L"login"), 5},
                {L"login", L"PASSword123", 10, wcslen(L"PASSword123")},
                {L"loginloginloginloginloginloginlogin", L"PASSword123", wcslen(L"loginloginloginloginloginloginlogin"), wcslen(L"PASSword123")},
                {L"", L"PASSword123", wcslen(L""), wcslen(L"PASSword123")},
                {L"123user", L"PASSword123", wcslen(L"123user"), wcslen(L"PASSword123")},
                {L"123пользователь", L"PASSword123", wcslen(L"123пользователь"), wcslen(L"PASSword123")},
                {L"us", L"PASSword123", wcslen(L"us"), wcslen(L"PASSword123")},
                {L"user###  user$$$", L"PASSword123", wcslen(L"user###  user$$$"), wcslen(L"PASSword123")},
                {L"user_😀😀😀", L"PASSword123", wcslen(L"user_😀😀😀"), wcslen(L"PASSword123")},
                {L"___", L"PASSword123", wcslen(L"___"), wcslen(L"PASSword123")},
                {L"login", L"123password", wcslen(L"login"), wcslen(L"123password")},
                {L"login", L"123пароль", wcslen(L"login"), wcslen(L"123пароль")},
                {L"login", L"us", wcslen(L"login"), wcslen(L"us")},
                {L"login", L"", wcslen(L"login"), wcslen(L"")},
                {L"login", L"passwordPASSWORDpasswordPASSWORDpasswordPASSWORDpasswordPASSWORD", wcslen(L"login"), wcslen(L"passwordPASSWORDpasswordPASSWORDpasswordPASSWORDpasswordPASSWORD")},
                {L"login", L"      ", wcslen(L"login"), wcslen(L"      ")},
        };

        for (size_t i = 0; i < sizeof(wrong_data_cases) / sizeof(wrong_data_cases[0]); ++i) {
                bool result128 = encrypt(&wrong_data_cases[i], KEY128);
                bool result192 = encrypt(&wrong_data_cases[i], KEY192);
                bool result256 = encrypt(&wrong_data_cases[i], KEY256);
                
                if (result128 != false)
                        cm_print_error("\n[   FAIL   ] TEST CASE: %zu\n", i + 1);

                assert_false(result128);
                assert_false(result192);
                assert_false(result256);
        }
}

static void test_encrypt_data_null_pointer(void **state)
{
        (void) state;

        bool result = encrypt(NULL, KEY128); 

        assert_int_equal(result, false);
}

static void test_encrypt_wrong_key_length(void **state)
{
        (void) state;

        struct user_data data = {
                .login = L"login", 
                .password = L"PASSword123", 
                .login_len = wcslen(L"login"), 
                .password_len = wcslen(L"PASSword123")
        };

        bool result = encrypt(&data, 0);

        assert_int_equal(result, false);
}

static void test_decrypt_correct(void **state)
{
        (void) state;

        const wchar_t *correct_data_cases[] = {
                L"t__login",
                L"t__логин",
                L"t__承る承る承る",
                NULL
        };

        for (size_t i = 0; correct_data_cases[i] != NULL; ++i) {
                bool result = decrypt(correct_data_cases[i]);

                if (result != true)
                        cm_print_error("\n[   FAIL   ] TEST CASE: %zu\n", i + 1);

                assert_true(result);
        }
}

static void test_decrypt_wrong(void **state)
{
        (void) state;

        const wchar_t *wrong_data_cases[] = {
                L"loginloginloginloginloginloginlogin",
                L"",
                L"123user",
                L"123пользователь",
                L"us",
                L"user###  user$$$",
                L"user_😀😀😀",
                L"___",
                NULL
        };

        for (size_t i = 0; wrong_data_cases[i] != NULL; ++i) {
                bool result = decrypt(wrong_data_cases[i]);

                if (result != false)
                        cm_print_error("\n[   FAIL   ] TEST CASE: %zu\n", i + 1);

                assert_false(result);
        }
}

static void test_decrypt_data_null_pointer(void **state)
{
        (void) state;

        bool result = decrypt(NULL); 

        assert_int_equal(result, false);
}

int main(void) 
{
        setlocale(LC_ALL, "ru_RU.UTF-8");

        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_encrypt_correct),
                cmocka_unit_test(test_encrypt_wrong),
                cmocka_unit_test(test_encrypt_data_null_pointer),
                cmocka_unit_test(test_encrypt_wrong_key_length),
                cmocka_unit_test(test_decrypt_correct),
                cmocka_unit_test(test_decrypt_wrong),
                cmocka_unit_test(test_decrypt_data_null_pointer),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}