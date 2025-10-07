

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <locale.h>

#include <../user_input/user_input.h>



wchar_t *__wrap_fgetws(wchar_t *ws, int n, FILE *stream)
{
        check_expected_ptr(stream);
        const wchar_t *test_input = mock_ptr_type(const wchar_t *);
        if (test_input == NULL) 
                return NULL;
        
        wcsncpy(ws, test_input, n - 1);
        ws[n-1] = L'\0';

        return(wcslen(ws) > 0) ? ws : NULL;
}

wint_t __wrap_getwchar(void)
{
        return WEOF;
}

int __wrap_tcsetattr(void)
{
        return 0;
}

static void test_get_login_correct(void **state)
{
        (void) state;

        const wchar_t *test_logins_correct[] = {
                L"user123\n",
                L"пользователь123\n",
                L"承る承る承る\n",
                NULL
        };
        
        for (size_t i = 0; test_logins_correct[i] != NULL; ++i) {
                struct user_data *data = user_data_init();
                expect_any(__wrap_fgetws, stream);
                will_return(__wrap_fgetws, test_logins_correct[i]);
                bool result = get_login(data);

                if (result != true)
                        cm_print_error("\n[   FAIL   ] TEST CASE: %zu\n", i + 1);

                cleanup_data(data);       
                assert_true(result);
                
        }
}

static void test_get_login_wrong(void **state)
{
        (void) state;

        const wchar_t *test_logins_wrong[] = {
                L"123user\n",
                L"123пользователь\n",
                L"     \n",
                L"____\n",
                L"user###user$$$\n",
                L"useruseruseruseruseruser\n",
                L"user_😀😀😀\n",
                L"us\n",
                L"\n",
                NULL
        };
        
        for (size_t i = 0; test_logins_wrong[i] != NULL; ++i) {
                struct user_data *data = user_data_init();
                expect_any(__wrap_fgetws, stream);
                will_return(__wrap_fgetws, test_logins_wrong[i]);
                bool result = get_login(data);

                if (result != false)
                        cm_print_error("\n[   FAIL   ] TEST CASE: %zu\n", i + 1);

                cleanup_data(data);
                assert_false(result);
        }

        
}

static void test_get_login_null_pointer(void **state)
{
        (void) state;

        struct user_data *data = user_data_init();
        expect_any(__wrap_fgetws, stream);
        will_return(__wrap_fgetws, NULL);
        bool result = get_login(data);

        cleanup_data(data);
        assert_false(result);
}

static void test_get_password_correct(void **state)
{
        (void) state;

        const wchar_t *test_password_correct[] = {
                L"passPASS123\n",
                L"парольПАРОЛЬ123\n",
                L"承る承る承るpasd1123asdAAAA\n",
                L"password123AA###  password$$$\n",
                L"password123AA_😀😀😀\n",
                NULL
        };
        
        for (size_t i = 0; test_password_correct[i] != NULL; ++i) {
                struct user_data *data = user_data_init();
                expect_any(__wrap_fgetws, stream);
                will_return(__wrap_fgetws, test_password_correct[i]);
                bool result = get_password(data);

                if (result != true)
                        cm_print_error("\n[   FAIL   ] TEST CASE: %zu\n", i + 1);

                cleanup_data(data);       
                assert_true(result);
        }
}

static void test_get_password_wrong(void **state)
{
        (void) state;

        const wchar_t *test_password_wrong[] = {
                L"us\n",
                L"\n",
                L"passwordPASSWORDpasswordPASSWORDpasswordPASSWORDpasswordPASSWORD\n",
                L"123password\n",
                L"123пароль\n",
                L"      \n",
                L"承る承る承る\n",
                NULL
        };
        
        for (size_t i = 0; test_password_wrong[i] != NULL; ++i) {
                
                struct user_data *data = user_data_init();
                expect_any(__wrap_fgetws, stream);
                will_return(__wrap_fgetws, test_password_wrong[i]);
                bool result = get_password(data);

                if (result != false)
                        cm_print_error("\n[   FAIL   ] TEST CASE: %zu\n", i + 1);

                cleanup_data(data);
                assert_false(result);
        }

        
}

static void test_get_password_null_pointer(void **state)
{
        (void) state;

        struct user_data *data = user_data_init();
        expect_any(__wrap_fgetws, stream);
        will_return(__wrap_fgetws, NULL);
        bool result = get_password(data);

        cleanup_data(data);

        assert_false(result);
}

int main(void) 
{
        setlocale(LC_ALL, "ru_RU.UTF-8");

        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_get_login_correct),
                cmocka_unit_test(test_get_login_wrong),
                cmocka_unit_test(test_get_login_null_pointer),
                cmocka_unit_test(test_get_password_correct),
                cmocka_unit_test(test_get_password_wrong),
                cmocka_unit_test(test_get_password_null_pointer),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}