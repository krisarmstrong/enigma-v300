cmake_minimum_required(VERSION 3.28)
project(enigma_v300_pure_c C)

set(CMAKE_C_STANDARD 11)

add_executable(enigma_v300_pure_c
        src/main.c
        src/enigma_v300_pure_c.c)

include(CTest)
if (BUILD_TESTING)
    add_test(NAME cli_help
            COMMAND $<TARGET_FILE:enigma_v300_pure_c> --help)
    set_tests_properties(cli_help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

    add_test(NAME cli_version
            COMMAND $<TARGET_FILE:enigma_v300_pure_c> --version)
    set_tests_properties(cli_version PROPERTIES PASS_REGULAR_EXPRESSION "Enigma v3\\.0\\.0")

    add_test(NAME cli_list_products
            COMMAND $<TARGET_FILE:enigma_v300_pure_c> --list-products)
    set_tests_properties(cli_list_products PROPERTIES PASS_REGULAR_EXPRESSION "Known Products:")

    add_test(NAME cli_list_options_6964
            COMMAND $<TARGET_FILE:enigma_v300_pure_c> --list-options 6964)
    set_tests_properties(cli_list_options_6964 PROPERTIES PASS_REGULAR_EXPRESSION "Options for 6964:")

    add_test(NAME cli_list_options_invalid
            COMMAND "${CMAKE_SOURCE_DIR}/tests/expect_fail.sh" "Unknown product code: 1234"
                    $<TARGET_FILE:enigma_v300_pure_c> --list-options 1234)
endif ()
