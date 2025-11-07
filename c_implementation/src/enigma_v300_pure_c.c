// File: enigma_v300_pure_c.c
// Author: Kris Armstrong
// Version: 3.0.0
// Last Modified: 2025-04-12
// Description: C11 implementation of Fluke option key calculator with enhanced menu.
// License: MIT

/*
MIT License

Copyright (c) 2025 Kris Armstrong

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "enigma_v300_pure_c.h"

#define SOFTWARE_VERSION "3.0.0"
#define PRODUCT_CODE_SIZE 4
#define OPTION_CODE_SIZE 3
#define SERIAL_NUMBER_SIZE_ENIGMA2 7
#define SERIAL_NUMBER_SIZE_ENIGMAC 10
#define CHECK_SUM_SIZE 2
#define KEY_LENGTH (PRODUCT_CODE_SIZE + OPTION_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2 + CHECK_SUM_SIZE)
#define SERIAL_LOCATION (CHECK_SUM_SIZE + PRODUCT_CODE_SIZE)
#define PRODUCT_LOCATION (CHECK_SUM_SIZE)
#define OPTION_LOCATION (CHECK_SUM_SIZE + PRODUCT_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2)
#define MAX_CHECK_SUM 26000

typedef struct
{
    const char* code;
    const char* abbr;
    const char* name;
} ProductInfo;

const ProductInfo PRODUCT_TABLE[] = {
    {"3001", "NTs2", "NetTool Series II"},
    {"7001", "LRPro", "LinkRunner Pro Duo"},
    {"6963", "Escope/MSv2", "EtherScope/MetroScope"},
    {"6964", "OneTouch", "OneTouch AT"},
    {"2186", "OptiView", "OptiView XG"},
    {"1890", "ClearSight", "ClearSight Analyzer"},
    {"1895", "iClearSight", "iClearSight Analyzer"},
};

const int PRODUCT_TABLE_SIZE = sizeof(PRODUCT_TABLE) / sizeof(PRODUCT_TABLE[0]);

typedef struct
{
    const char* code;
    const char* desc;
} OptionInfo;

typedef struct
{
    const char* product_code;
    OptionInfo options[12];
    int option_count;
} ProductOptions;

const ProductOptions PRODUCT_OPTIONS[] = {
    {
        "6964", {
            {"000", "Registered"},
            {"001", "Wired (Was Copper)"},
            {"002", "Obsolete (was fiber)"},
            {"003", "Wi-Fi"},
            {"004", "Obsolete (was inline)"},
            {"005", "Capture"},
            {"006", "Advanced Tests"},
            {"007", "XGR-to-ATX Upgrade"},
            {"008", "Claimed (Cloud Tools)"},
            {"009", "LatTests (China LAN Tests)"},
            {"064", "XGReflector (Future)"},
            {"065", "Performance Peer (Future)"}
        },
        12
    },
    {
        "6963", {
            {"000", "MetroScope Base, EtherScope LAN"},
            {"001", "MetroScope WLAN, EtherScope WLAN"},
            {"002", "MetroScope Multi, EtherScope ITO"},
            {"003", "MetroScope VoIP, EtherScope Fiber"},
            {"004", "MetroScope LT, EtherScope LT"}
        },
        5
    },
    {
        "7001", {
            {"000", "802.1x"},
            {"002", "Reports"},
            {"003", "LAN"}
        },
        3
    },
    {
        "2186", {
            {"000", "Wireless Analyzer Option"},
            {"001", "Enables Network Test Ports A-D"},
            {"002", "10Gb Ethernet Analyzer Option"},
            {"003", "LAN / 10Gb Ethernet Analyzer Option"},
            {"004", "NPT - Network Performance Option"},
            {"007", "Everything"}
        },
        6
    },
    {
        "1890", {
            {"000", "Activation Code"},
            {"007", "All Options"}
        },
        2
    },
    {
        "1895", {
            {"000", "Activation Code"},
            {"003", "All Options"}
        },
        2
    },
    {
        "3001", {
            {"003", "Personalization"},
            {"004", "VoIP"},
            {"005", "NetSecure"},
            {"008", "Dicom"}
        },
        4
    },
};

const int PRODUCT_OPTIONS_SIZE = sizeof(PRODUCT_OPTIONS) / sizeof(PRODUCT_OPTIONS[0]);

const int ENIGMA_C_ROTOR[] = {5, 4, 14, 11, 1, 8, 10, 13, 7, 3, 15, 0, 2, 12, 9, 6};
const int ENIGMA2_E_ROTOR_10[] = {5, 4, 1, 8, 7, 3, 0, 2, 9, 6};
const int ENIGMA2_E_ROTOR_26[] = {
    16, 8, 25, 5, 23, 21, 18, 17, 2, 1, 7, 24, 15, 11, 9, 6, 3, 0, 19, 12, 22, 14, 10, 4, 20, 13
};
const int ENIGMA2_D_ROTOR_10[] = {6, 2, 7, 5, 1, 0, 9, 4, 3, 8};
const int ENIGMA2_D_ROTOR_26[] = {
    17, 9, 8, 16, 23, 3, 15, 10, 1, 14, 22, 13, 19, 25, 21, 12, 0, 7, 6, 18, 24, 5, 20, 4, 11, 2
};

static void print_usage(const char* program_name)
{
    printf("Enigma %s - Fluke option key utility\n", SOFTWARE_VERSION);
    printf("Usage:\n");
    printf("  %s [mode] [args...]\n\n", program_name);
    printf("Modes:\n");
    printf("  -n SERIAL OPTION        Generate NetTool option key\n");
    printf("  -x OPTION_KEY           Check NetTool option key\n");
    printf("  -e SERIAL [OPTION [PRODUCT]]  Generate EtherScope/MetroScope option key\n");
    printf("  -l SERIAL [OPTION]      Generate LinkRunner Pro option key\n");
    printf("  -d OPTION_KEY           Decrypt EtherScope/MetroScope option key\n\n");
    printf("Utility flags:\n");
    printf("  -h, --help, -?          Show this help text\n");
    printf("  -V, --version           Show version information\n");
    printf("  --list-products         List known product codes\n");
    printf("  --list-options CODE     List options for a product code\n");
    printf("\nRun without arguments to launch the interactive menu.\n");
}

static void print_version(void)
{
    printf("Enigma v%s\n", SOFTWARE_VERSION);
}

static void list_products(void)
{
    printf("Known Products:\n");
    for (int i = 0; i < PRODUCT_TABLE_SIZE; ++i)
    {
        printf("  %s\t%s\n", PRODUCT_TABLE[i].code, PRODUCT_TABLE[i].name);
    }
}

static bool list_options_for_code(const char* product_code)
{
    for (int i = 0; i < PRODUCT_OPTIONS_SIZE; ++i)
    {
        if (strcmp(PRODUCT_OPTIONS[i].product_code, product_code) == 0)
        {
            printf("Options for %s:\n", product_code);
            for (int opt = 0; opt < PRODUCT_OPTIONS[i].option_count; ++opt)
            {
                printf("  %s\t%s\n", PRODUCT_OPTIONS[i].options[opt].code,
                       PRODUCT_OPTIONS[i].options[opt].desc);
            }
            return true;
        }
    }
    fprintf(stderr, "Unknown product code: %s\n", product_code);
    return false;
}

void enigma_c_encrypt(const char* input_key, char* output_key, size_t len)
{
    int output_value = 0;
    for (size_t index = 0; index < len; ++index)
    {
        char c = tolower(input_key[index]);
        if (!isxdigit(c))
        {
            fprintf(stderr, "Input contains non-hex characters\n");
            exit(1);
        }
        int input_value = c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
        output_value = ENIGMA_C_ROTOR[(input_value + index) % 16] ^ output_value;
        output_key[index] = (output_value % 16) < 10 ? (output_value % 16) + '0' : (output_value % 16) - 10 + 'a';
    }
    output_key[len] = '\0';
}

void enigma_c_decrypt(const char* input_key, char* output_key, size_t len)
{
    int xor_value = 0;
    for (size_t index = 0; index < len; ++index)
    {
        char c = tolower(input_key[index]);
        if (!isxdigit(c))
        {
            fprintf(stderr, "Input contains non-hex characters\n");
            exit(1);
        }
        int old_output = c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
        int input_value = old_output ^ xor_value;
        int output_value = 0;
        for (int i = 0; i < 16; ++i)
        {
            if (ENIGMA_C_ROTOR[i] == input_value)
            {
                output_value = i;
                break;
            }
        }
        int temp = (output_value - index) % 16;
        if (temp < 0) temp += 16;
        output_key[index] = temp < 10 ? temp + '0' : temp - 10 + 'a';
        xor_value = old_output;
    }
    output_key[len] = '\0';
}

int enigma_c_check_option_key(int option, const char* key, const char* serial_number)
{
    if (!key || strlen(key) == 0)
    {
        fprintf(stderr, "Key cannot be empty\n");
        return 0;
    }
    int is_blade_rules = strcmp(key, "bladerules") == 0;
    if (is_blade_rules) return 1;
    char decrypted_key[13];
    enigma_c_decrypt(key, decrypted_key, 12);
    char reversed_serial[11];
    for (int i = 0; i < 10; ++i)
    {
        reversed_serial[i] = decrypted_key[9 - i];
    }
    reversed_serial[10] = '\0';
    int serial_match = strcmp(reversed_serial, serial_number) == 0;
    if (!serial_match) return 0;
    char opt_str[3] = {decrypted_key[10], decrypted_key[11], '\0'};
    int opt = atoi(opt_str);
    return opt == option;
}

void enigma2_c_encrypt(const char* input_key, char* output_key)
{
    if (strlen(input_key) != KEY_LENGTH)
    {
        fprintf(stderr, "Input key length must be %d\n", KEY_LENGTH);
        exit(1);
    }
    strcpy(output_key, input_key);
    int checksum = 1;
    for (int i = 2; i < KEY_LENGTH; ++i)
    {
        int temp_sum = isdigit(input_key[i]) ? (input_key[i] - '0') : (input_key[i] - 'A');
        checksum += i + temp_sum + (i * temp_sum);
    }
    checksum = 100 - (checksum % 100);
    output_key[0] = (checksum % 10) + '0';
    output_key[1] = ((checksum / 10) % 10) + '0';
    int running_sum = 0;
    for (int i = 0; i < KEY_LENGTH; ++i)
    {
        int temp_sum = isdigit(output_key[i]) ? (output_key[i] - '0') : (output_key[i] - 'A');
        if (isdigit(output_key[i]))
        {
            output_key[i] = ENIGMA2_E_ROTOR_10[(temp_sum + MAX_CHECK_SUM - running_sum) % 10] + '0';
        }
        else
        {
            output_key[i] = 'A' + ENIGMA2_E_ROTOR_26[(temp_sum + MAX_CHECK_SUM - running_sum) % 26];
        }
        running_sum += i + temp_sum + (i * temp_sum);
    }
}

void enigma2_c_decrypt(const char* input_key, char* output_key)
{
    if (strlen(input_key) != KEY_LENGTH)
    {
        fprintf(stderr, "Input key length must be %d\n", KEY_LENGTH);
        exit(1);
    }
    strcpy(output_key, input_key);
    int checksum = 0;
    for (int i = 0; i < KEY_LENGTH; ++i)
    {
        int temp_sum = isdigit(output_key[i])
                           ? (ENIGMA2_D_ROTOR_10[output_key[i] - '0'] + checksum) % 10
                           : (ENIGMA2_D_ROTOR_26[output_key[i] - 'A'] + checksum) % 26;
        output_key[i] = isdigit(output_key[i]) ? temp_sum + '0' : 'A' + temp_sum;
        checksum += i + temp_sum + (i * temp_sum);
    }
    checksum += 8 * (output_key[1] - '0');
    if (checksum % 100 != 0) output_key[0] = '\0';
}

int enigma2_c_check_option_key(int option, const char* key)
{
    if (!key || strlen(key) == 0)
    {
        fprintf(stderr, "Key cannot be empty\n");
        return 0;
    }
    char decrypted_key[17];
    enigma2_c_decrypt(key, decrypted_key);
    int is_valid = strlen(decrypted_key) != 0;
    if (!is_valid) return 0;
    char opt_str[4];
    strncpy(opt_str, decrypted_key + OPTION_LOCATION, OPTION_CODE_SIZE);
    opt_str[OPTION_CODE_SIZE] = '\0';
    int opt = atoi(opt_str);
    return opt == option;
}

int get_menu_choice(const char* prompt, int min_val, int max_val)
{
    int choice;
    while (1)
    {
        printf("%s", prompt);
        char input[32];
        if (!fgets(input, sizeof(input), stdin)) continue;
        if (sscanf(input, "%d", &choice) != 1)
        {
            printf("Invalid input, please enter a number.\n");
            continue;
        }
        if (choice >= min_val && choice <= max_val) return choice;
        printf("Invalid choice, please try again.\n");
    }
}

int product_code_menu(char* product_code, char* option_code)
{
    printf("\n--- Product Code Menu ---\n");
    for (int i = 0; i < PRODUCT_TABLE_SIZE; ++i)
    {
        printf("%d. %s - %s\n", i + 1, PRODUCT_TABLE[i].code, PRODUCT_TABLE[i].name);
    }
    printf("8. Custom Product Code\n0. Exit\n");
    int choice = get_menu_choice("Choose your option: ", 0, 8);
    if (choice == 0) return 0;

    if (choice == 8)
    {
        while (1)
        {
            printf("Enter Product Code (4 digits): ");
            char input[32];
            if (!fgets(input, sizeof(input), stdin)) continue;
            input[strcspn(input, "\n")] = '\0';
            if (strlen(input) == 4 && strspn(input, "0123456789") == 4)
            {
                strncpy(product_code, input, 4);
                product_code[4] = '\0';
                break;
            }
            printf("Product code must be 4 digits.\n");
        }
        while (1)
        {
            printf("Enter Option Code (3 digits): ");
            char input[32];
            if (!fgets(input, sizeof(input), stdin)) continue;
            input[strcspn(input, "\n")] = '\0';
            if (strlen(input) == 3 && strspn(input, "0123456789") == 3)
            {
                strncpy(option_code, input, 3);
                option_code[3] = '\0';
                break;
            }
            printf("Option code must be 3 digits.\n");
        }
        return 1;
    }

    const ProductInfo* product = &PRODUCT_TABLE[choice - 1];
    strncpy(product_code, product->code, 4);
    product_code[4] = '\0';
    const ProductOptions* options = NULL;
    for (int i = 0; i < PRODUCT_OPTIONS_SIZE; ++i)
    {
        if (strcmp(PRODUCT_OPTIONS[i].product_code, product_code) == 0)
        {
            options = &PRODUCT_OPTIONS[i];
            break;
        }
    }
    if (options == NULL || options->option_count == 0)
    {
        printf("No options defined for %s.\n", product->name);
        return 0;
    }

    printf("\n--- Options for %s ---\n", product->name);
    for (int i = 0; i < options->option_count; ++i)
    {
        printf("%d. %s - %s\n", i + 1, options->options[i].code, options->options[i].desc);
    }
    printf("8. Custom Option Code\n0. Exit\n");
    int opt_choice = get_menu_choice("Choose your option: ", 0, 8);
    if (opt_choice == 0) return 0;

    if (opt_choice == 8)
    {
        while (1)
        {
            printf("Enter Option Code (3 digits): ");
            char input[32];
            if (!fgets(input, sizeof(input), stdin)) continue;
            input[strcspn(input, "\n")] = '\0';
            if (strlen(input) == 3 && strspn(input, "0123456789") == 3)
            {
                strncpy(option_code, input, 3);
                option_code[3] = '\0';
                break;
            }
            printf("Option code must be 3 digits.\n");
        }
    }
    else
    {
        strncpy(option_code, options->options[opt_choice - 1].code, 3);
        option_code[3] = '\0';
    }
    return 1;
}

void print_option_key(const char* option_key)
{
    printf("Option Key:");
    for (size_t i = 0; i < strlen(option_key); ++i)
    {
        if (i % 4 == 0) printf(" ");
        printf("%c", option_key[i]);
    }
    printf("\n");
}

void calculate_nettool_option_key(char* serial_number, int option_number)
{
    if (strlen(serial_number) == 0)
    {
        while (1)
        {
            printf("Enter Serial Number (10 digits): ");
            if (!fgets(serial_number, SERIAL_NUMBER_SIZE_ENIGMAC + 1, stdin)) continue;
            serial_number[strcspn(serial_number, "\n")] = '\0';
            if (strlen(serial_number) == SERIAL_NUMBER_SIZE_ENIGMAC && strspn(serial_number, "0123456789") ==
                SERIAL_NUMBER_SIZE_ENIGMAC)
                break;
            printf("Serial number must be 10 digits.\n");
        }
    }
    if (strlen(serial_number) != SERIAL_NUMBER_SIZE_ENIGMAC || strspn(serial_number, "0123456789") !=
        SERIAL_NUMBER_SIZE_ENIGMAC)
    {
        fprintf(stderr, "Serial number must be 10 digits\n");
        exit(1);
    }

    if (option_number < 0)
    {
        printf("\nNetTool Options: 0=Inline 1=Reports/Ping 3=Personal 4=VoIP 5=SwitchWizard\n");
        char input[32];
        printf("Enter Option Number (1 digit): ");
        if (!fgets(input, sizeof(input), stdin)) exit(1);
        input[strcspn(input, "\n")] = '\0';
        option_number = (strlen(input) > 0 && isdigit(input[0])) ? (input[0] - '0') : 0;
    }
    if (option_number < 0 || option_number > 9) option_number = 0;

    char input_key[13];
    snprintf(input_key, sizeof(input_key), "%s%d0", serial_number, option_number);
    char reversed_key[13];
    for (int i = 0; i < 12; ++i)
    {
        reversed_key[i] = input_key[11 - i];
    }
    reversed_key[12] = '\0';

    printf("\nEncrypting with Enigma 1...\n");
    char output_key[13];
    enigma_c_encrypt(reversed_key, output_key, 12);
    print_option_key(output_key);
}

void check_nettool_option_key(char* option_key)
{
    char serial_number[32];
    while (1)
    {
        printf("Enter Serial Number (10 digits): ");
        if (!fgets(serial_number, sizeof(serial_number), stdin)) continue;
        serial_number[strcspn(serial_number, "\n")] = '\0';
        if (strlen(serial_number) == SERIAL_NUMBER_SIZE_ENIGMAC && strspn(serial_number, "0123456789") ==
            SERIAL_NUMBER_SIZE_ENIGMAC)
            break;
        printf("Serial number must be 10 digits.\n");
    }

    if (strlen(option_key) == 0)
    {
        while (1)
        {
            printf("Enter Option Key (12 hex digits): ");
            if (!fgets(option_key, 13, stdin)) continue;
            option_key[strcspn(option_key, "\n")] = '\0';
            if (strlen(option_key) == 12 && strspn(option_key, "0123456789abcdefABCDEF") == 12)
                break;
            printf("Option key must be 12 hex digits.\n");
        }
    }
    if (strlen(option_key) != 12 || strspn(option_key, "0123456789abcdefABCDEF") != 12)
    {
        fprintf(stderr, "Option key must be 12 hex digits\n");
        exit(1);
    }

    char input[32];
    printf("Enter Option Number (1 digit): ");
    if (!fgets(input, sizeof(input), stdin)) exit(1);
    input[strcspn(input, "\n")] = '\0';
    int option_number = (strlen(input) > 0 && isdigit(input[0])) ? (input[0] - '0') : 0;
    if (option_number < 0 || option_number > 9) option_number = 0;

    printf("\nEnigmaC::checkOptionKey()...\n");
    printf("serialNum: %s\n", serial_number);
    printf("optionKey: %s\n", option_key);
    printf("optionNum: 0x%x\n", option_number);
    int result = enigma_c_check_option_key(option_number, option_key, serial_number);
    printf("Option %s\n", result ? "valid" : "invalid");
}

void calculate_enigma2_option_key(char* serial_number, int option_number, int product_code, int assume_escope)
{
    char product_code_str[5];
    char option_str[4];
    if (product_code >= 0)
    {
        snprintf(product_code_str, sizeof(product_code_str), "%04d", product_code);
    }
    else
    {
        product_code_str[0] = '\0';
    }
    if (option_number >= 0)
    {
        snprintf(option_str, sizeof(option_str), "%03d", option_number);
    }
    else
    {
        option_str[0] = '\0';
    }

    if (strlen(serial_number) == 0)
    {
        while (1)
        {
            printf("Enter Serial Number (7 digits): ");
            if (!fgets(serial_number, SERIAL_NUMBER_SIZE_ENIGMA2 + 1, stdin)) continue;
            serial_number[strcspn(serial_number, "\n")] = '\0';
            if (strlen(serial_number) == SERIAL_NUMBER_SIZE_ENIGMA2 && strspn(serial_number, "0123456789") ==
                SERIAL_NUMBER_SIZE_ENIGMA2)
                break;
            printf("Serial number must be 7 digits.\n");
        }
    }
    if (strlen(serial_number) != SERIAL_NUMBER_SIZE_ENIGMA2 || strspn(serial_number, "0123456789") !=
        SERIAL_NUMBER_SIZE_ENIGMA2)
    {
        fprintf(stderr, "Serial number must be 7 digits\n");
        exit(1);
    }

    printf("SerialNum= %s\n", serial_number);

    if (strlen(product_code_str) == 0 || strlen(option_str) == 0 || !assume_escope)
    {
        char prod_code[5], opt_code[4];
        if (!product_code_menu(prod_code, opt_code))
        {
            printf("Operation cancelled.\n");
            return;
        }
        strncpy(product_code_str, prod_code, 5);
        strncpy(option_str, opt_code, 4);
    }

    char input_key[17];
    snprintf(input_key, sizeof(input_key), "00%s%s%s", product_code_str, serial_number, option_str);

    printf("\nEncrypting with Enigma 2...\n");
    char output_key[17];
    enigma2_c_encrypt(input_key, output_key);
    print_option_key(output_key);
}

void check_enigma2_option_key(char* option_key)
{
    if (strlen(option_key) == 0)
    {
        while (1)
        {
            printf("Enter Option Key (16 characters): ");
            if (!fgets(option_key, 17, stdin)) continue;
            option_key[strcspn(option_key, "\n")] = '\0';
            if (strlen(option_key) == 16 && strspn(option_key, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") == 16)
                break;
            printf("Option key must be 16 alphanumeric characters.\n");
        }
    }
    if (strlen(option_key) != 16 || strspn(option_key, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 16)
    {
        fprintf(stderr, "Option key must be 16 alphanumeric characters\n");
        exit(1);
    }

    printf("Decrypting with Enigma 2...\n");
    char decrypted_key[17];
    enigma2_c_decrypt(option_key, decrypted_key);
    if (strlen(decrypted_key) == 0)
    {
        fprintf(stderr, "Decryption failed: invalid checksum\n");
        exit(1);
    }

    char product_code[5];
    strncpy(product_code, decrypted_key + PRODUCT_LOCATION, PRODUCT_CODE_SIZE);
    product_code[PRODUCT_CODE_SIZE] = '\0';
    printf("Product Code: %s -> ", product_code);
    int found = 0;
    for (int i = 0; i < PRODUCT_TABLE_SIZE; ++i)
    {
        if (strcmp(product_code, PRODUCT_TABLE[i].code) == 0)
        {
            printf("%s\n", PRODUCT_TABLE[i].name);
            found = 1;
            break;
        }
    }
    if (!found) printf("Unknown\n");
    char serial[8];
    strncpy(serial, decrypted_key + SERIAL_LOCATION, SERIAL_NUMBER_SIZE_ENIGMA2);
    serial[SERIAL_NUMBER_SIZE_ENIGMA2] = '\0';
    printf("SerialNumber: %s\n", serial);
    char option[4];
    strncpy(option, decrypted_key + OPTION_LOCATION, OPTION_CODE_SIZE);
    option[OPTION_CODE_SIZE] = '\0';
    printf("OptionNumber: %s\n", option);
}

int main_menu(void)
{
    printf("\n--- Enigma %s Main Menu ---\n", SOFTWARE_VERSION);
    printf("1. Generate NetTool 10/100 Option Key\n");
    printf("2. Check NetTool 10/100 Option Key\n");
    printf("3. Generate Option Key for Other Fluke Products\n");
    printf("4. Decrypt Option Key for Other Fluke Products\n");
    printf("0. Exit\n");

    int choice = get_menu_choice("Choose your option: ", 0, 4);
    if (choice == 0) return 0;

    char serial_number[32] = "";
    char option_key[32] = "";
    switch (choice)
    {
    case 1:
        calculate_nettool_option_key(serial_number, -1);
        break;
    case 2:
        check_nettool_option_key(option_key);
        break;
    case 3:
        calculate_enigma2_option_key(serial_number, -1, -1, 0);
        break;
    case 4:
        check_enigma2_option_key(option_key);
        break;
    default:
        fprintf(stderr, "Unexpected choice value: %d\n", choice);
        return 0;
    }
    return 1;
}

int run_enigma_cli(int argc, char* argv[])
{
    char serial_number[32] = "";
    char option_key[32] = "";
    int selection = 0;
    int option_number = -1;
    int product_code = -1;
    int assume_escope = 0;

    if (argc > 1)
    {
        if (strcmp(argv[1], "?") == 0 || strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 ||
            strcmp(argv[1], "--help") == 0)
        {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version") == 0)
        {
            print_version();
            return 0;
        }
        if (strcmp(argv[1], "--list-products") == 0)
        {
            list_products();
            return 0;
        }
        if (strcmp(argv[1], "--list-options") == 0)
        {
            if (argc < 3)
            {
                fprintf(stderr, "--list-options requires a product code argument\n");
                return 1;
            }
            return list_options_for_code(argv[2]) ? 0 : 1;
        }
        if (strcmp(argv[1], "-n") == 0)
        {
            selection = 1;
        }
        else if (strcmp(argv[1], "-x") == 0)
        {
            selection = 2;
        }
        else if (strcmp(argv[1], "-e") == 0)
        {
            selection = 3;
            product_code = 6963;
            assume_escope = 1;
        }
        else if (strcmp(argv[1], "-l") == 0)
        {
            selection = 3;
            product_code = 7001;
        }
        else if (strcmp(argv[1], "-d") == 0)
        {
            selection = 4;
        }
        else
        {
            selection = atoi(argv[1]);
        }

        if (argc > 2)
        {
            if (selection == 4)
            {
                strncpy(option_key, argv[2], 16);
                option_key[16] = '\0';
            }
            else if (selection == 2)
            {
                strncpy(option_key, argv[2], 12);
                option_key[12] = '\0';
            }
            else
            {
                strncpy(serial_number, argv[2], selection == 1 ? 10 : 7);
                serial_number[selection == 1 ? 10 : 7] = '\0';
            }
            if (argc > 3) option_number = atoi(argv[3]);
            if (argc > 4) product_code = atoi(argv[4]);
        }
    }

    if (selection < 1 || selection > 4)
    {
        while (main_menu())
        {
        }
    }
    else
    {
        switch (selection)
        {
        case 1:
            calculate_nettool_option_key(serial_number, option_number);
            break;
        case 2:
            check_nettool_option_key(option_key);
            break;
        case 3:
            calculate_enigma2_option_key(serial_number, option_number, product_code, assume_escope);
            break;
        case 4:
            check_enigma2_option_key(option_key);
            break;
        default:
            fprintf(stderr, "Invalid selection value: %d\n", selection);
            return 1;
        }
    }
    return 0;
}
