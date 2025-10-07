# CMake generated Testfile for 
# Source directory: /home/y000da/develop/RTTtest_task-UTF-8/cmocka/example
# Build directory: /home/y000da/develop/RTTtest_task-UTF-8/cmocka/build/example
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(simple_test "simple_test")
set_tests_properties(simple_test PROPERTIES  _BACKTRACE_TRIPLES "/home/y000da/develop/RTTtest_task-UTF-8/cmocka/cmake/Modules/AddCMockaTest.cmake;116;add_test;/home/y000da/develop/RTTtest_task-UTF-8/cmocka/example/CMakeLists.txt;12;add_cmocka_test;/home/y000da/develop/RTTtest_task-UTF-8/cmocka/example/CMakeLists.txt;0;")
add_test(allocate_module_test "allocate_module_test")
set_tests_properties(allocate_module_test PROPERTIES  WILL_FAIL "1" _BACKTRACE_TRIPLES "/home/y000da/develop/RTTtest_task-UTF-8/cmocka/cmake/Modules/AddCMockaTest.cmake;116;add_test;/home/y000da/develop/RTTtest_task-UTF-8/cmocka/example/CMakeLists.txt;20;add_cmocka_test;/home/y000da/develop/RTTtest_task-UTF-8/cmocka/example/CMakeLists.txt;0;")
add_test(assert_macro_test "assert_macro_test")
set_tests_properties(assert_macro_test PROPERTIES  WILL_FAIL "1" _BACKTRACE_TRIPLES "/home/y000da/develop/RTTtest_task-UTF-8/cmocka/cmake/Modules/AddCMockaTest.cmake;116;add_test;/home/y000da/develop/RTTtest_task-UTF-8/cmocka/example/CMakeLists.txt;32;add_cmocka_test;/home/y000da/develop/RTTtest_task-UTF-8/cmocka/example/CMakeLists.txt;0;")
add_test(assert_module_test "assert_module_test")
set_tests_properties(assert_module_test PROPERTIES  WILL_FAIL "1" _BACKTRACE_TRIPLES "/home/y000da/develop/RTTtest_task-UTF-8/cmocka/cmake/Modules/AddCMockaTest.cmake;116;add_test;/home/y000da/develop/RTTtest_task-UTF-8/cmocka/example/CMakeLists.txt;44;add_cmocka_test;/home/y000da/develop/RTTtest_task-UTF-8/cmocka/example/CMakeLists.txt;0;")
subdirs("mock")
