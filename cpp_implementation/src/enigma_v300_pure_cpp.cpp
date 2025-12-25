// File: enigma_v300_pure_cpp.cpp
// Author: Kris Armstrong
// Version: 3.0.0
// Last Modified: 2025-12-25
// Description: C++20 implementation of Fluke option key calculator with enhanced menu.
// License: MIT

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <iomanip>

constexpr auto SOFTWARE_VERSION = "3.0.0";
constexpr size_t PRODUCT_CODE_SIZE = 4;
constexpr size_t OPTION_CODE_SIZE = 3;
constexpr size_t SERIAL_NUMBER_SIZE_ENIGMA2 = 7;
constexpr size_t SERIAL_NUMBER_SIZE_ENIGMAC = 10;
constexpr size_t CHECK_SUM_SIZE = 2;
constexpr size_t KEY_LENGTH = PRODUCT_CODE_SIZE + OPTION_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2 + CHECK_SUM_SIZE;
constexpr size_t SERIAL_LOCATION = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE;
constexpr size_t PRODUCT_LOCATION = CHECK_SUM_SIZE;
constexpr size_t OPTION_LOCATION = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2;
constexpr int MAX_CHECK_SUM = 26000;

struct ProductInfo
{
    std::string code;
    std::string abbr;
    std::string name;
};

const std::vector<ProductInfo> PRODUCT_TABLE = {
    {"3001", "NTs2", "NetTool Series II"},
    {"7001", "LRPro", "LinkRunner Pro Duo"},
    {"6963", "Escope/MSv2", "EtherScope/MetroScope"},
    {"6964", "OneTouch", "OneTouch AT"},
    {"2186", "OptiView", "OptiView XG"},
    {"1890", "ClearSight", "ClearSight Analyzer"},
    {"1895", "iClearSight", "iClearSight Analyzer"},
};

struct OptionInfo
{
    std::string code;
    std::string desc;
};

struct ProductOptions
{
    std::string product_code;
    std::vector<OptionInfo> options;
};

const std::vector<ProductOptions> PRODUCT_OPTIONS = {
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
        }
    },
    {
        "6963", {
            {"000", "MetroScope Base, EtherScope LAN"},
            {"001", "MetroScope WLAN, EtherScope WLAN"},
            {"002", "MetroScope Multi, EtherScope ITO"},
            {"003", "MetroScope VoIP, EtherScope Fiber"},
            {"004", "MetroScope LT, EtherScope LT"}
        }
    },
    {
        "7001", {
            {"000", "802.1x"},
            {"002", "Reports"},
            {"003", "LAN"}
        }
    },
    {
        "2186", {
            {"000", "Wireless Analyzer Option"},
            {"001", "Enables Network Test Ports A-D"},
            {"002", "10Gb Ethernet Analyzer Option"},
            {"003", "LAN / 10Gb Ethernet Analyzer Option"},
            {"004", "NPT - Network Performance Option"},
            {"007", "Everything"}
        }
    },
    {
        "1890", {
            {"000", "Activation Code"},
            {"007", "All Options"}
        }
    },
    {
        "1895", {
            {"000", "Activation Code"},
            {"003", "All Options"}
        }
    },
    {
        "3001", {
            {"003", "Personalization"},
            {"004", "VoIP"},
            {"005", "NetSecure"},
            {"008", "Dicom"}
        }
    },
};

const std::vector<int> ENIGMA_C_ROTOR = {5, 4, 14, 11, 1, 8, 10, 13, 7, 3, 15, 0, 2, 12, 9, 6};
const std::vector<int> ENIGMA2_E_ROTOR_10 = {5, 4, 1, 8, 7, 3, 0, 2, 9, 6};
const std::vector<int> ENIGMA2_E_ROTOR_26 = {
    16, 8, 25, 5, 23, 21, 18, 17, 2, 1, 7, 24, 15, 11, 9, 6, 3, 0, 19, 12, 22, 14, 10, 4, 20, 13
};
const std::vector<int> ENIGMA2_D_ROTOR_10 = {6, 2, 7, 5, 1, 0, 9, 4, 3, 8};
const std::vector<int> ENIGMA2_D_ROTOR_26 = {
    17, 9, 8, 16, 23, 3, 15, 10, 1, 14, 22, 13, 19, 25, 21, 12, 0, 7, 6, 18, 24, 5, 20, 4, 11, 2
};

void enigma_c_encrypt(const std::string& input_key, std::string& output_key, size_t len)
{
    output_key.resize(len);
    int output_value = 0;
    for (size_t index = 0; index < len; ++index)
    {
        char c = std::tolower(input_key.at(index));
        if (!std::isxdigit(static_cast<unsigned char>(c)))
        {
            std::cerr << "Input contains non-hex characters\n";
            std::exit(1);
        }
        int input_value = (c >= '0' && c <= '9') ? (c - '0') : (c - 'a' + 10);
        output_value = ENIGMA_C_ROTOR.at((input_value + index) % 16) ^ output_value;
        int temp = output_value % 16;
        // Narrowing conversion is intentional: temp is guaranteed to be in range 0-15
        output_key.at(index) = (temp < 10) ? static_cast<char>(temp + '0') : static_cast<char>(temp - 10 + 'a');
    }
}

void enigma_c_decrypt(const std::string& input_key, std::string& output_key, size_t len)
{
    output_key.resize(len);
    int xor_value = 0;
    for (size_t index = 0; index < len; ++index)
    {
        char c = std::tolower(input_key.at(index));
        if (!std::isxdigit(static_cast<unsigned char>(c)))
        {
            std::cerr << "Input contains non-hex characters\n";
            std::exit(1);
        }
        int old_output = (c >= '0' && c <= '9') ? (c - '0') : (c - 'a' + 10);
        int input_value = old_output ^ xor_value;
        int output_value = 0;
        for (int i = 0; i < 16; ++i)
        {
            if (ENIGMA_C_ROTOR.at(i) == input_value)
            {
                output_value = i;
                break;
            }
        }
        int temp = (output_value - static_cast<int>(index)) % 16;
        if (temp < 0) temp += 16;
        // Narrowing conversion is intentional: temp is guaranteed to be in range 0-15
        output_key.at(index) = (temp < 10) ? static_cast<char>(temp + '0') : static_cast<char>(temp - 10 + 'a');
        xor_value = old_output;
    }
}

bool enigma_c_check_option_key(int option, const std::string& key, const std::string& serial_number)
{
    if (key.empty())
    {
        std::cerr << "Key cannot be empty\n";
        return false;
    }
    if (key == "bladerules") return true;
    std::string decrypted_key;
    enigma_c_decrypt(key, decrypted_key, 12);
    std::string reversed_serial(10, '0'); // Fixed initialization
    for (int i = 0; i < 10; ++i)
    {
        reversed_serial.at(i) = decrypted_key.at(9 - i);
    }
    if (reversed_serial != serial_number) return false;
    std::string opt_str = decrypted_key.substr(10, 2);
    int opt = std::stoi(opt_str);
    return opt == option;
}

void enigma2_c_encrypt(const std::string& input_key, std::string& output_key)
{
    if (input_key.length() != KEY_LENGTH)
    {
        std::cerr << "Input key length must be " << KEY_LENGTH << "\n";
        std::exit(1);
    }
    output_key = input_key;
    int checksum = 1;
    for (size_t i = 2; i < KEY_LENGTH; ++i)
    {
        int temp_sum = std::isdigit(static_cast<unsigned char>(input_key.at(i)))
                           ? (input_key.at(i) - '0')
                           : (input_key.at(i) - 'A');
        checksum += static_cast<int>(i) + temp_sum + (static_cast<int>(i) * temp_sum);
    }
    checksum = 100 - (checksum % 100);
    output_key.at(0) = static_cast<char>((checksum % 10) + '0');
    output_key.at(1) = static_cast<char>(((checksum / 10) % 10) + '0');
    int running_sum = 0;
    for (size_t i = 0; i < KEY_LENGTH; ++i)
    {
        int temp_sum = std::isdigit(static_cast<unsigned char>(output_key.at(i)))
                           ? (output_key.at(i) - '0')
                           : (output_key.at(i) - 'A');
        if (std::isdigit(static_cast<unsigned char>(output_key.at(i))))
        {
            // Narrowing conversion is intentional: result is guaranteed to be in range 0-9
            output_key.at(i) = static_cast<char>(ENIGMA2_E_ROTOR_10.at((temp_sum + MAX_CHECK_SUM - running_sum) % 10) +
                '0');
        }
        else
        {
            // Narrowing conversion is intentional: result is guaranteed to be in range 0-25
            output_key.at(i) = static_cast<char>('A' + ENIGMA2_E_ROTOR_26.at(
                (temp_sum + MAX_CHECK_SUM - running_sum) % 26));
        }
        running_sum += static_cast<int>(i) + temp_sum + (static_cast<int>(i) * temp_sum);
    }
}

void enigma2_c_decrypt(const std::string& input_key, std::string& output_key)
{
    if (input_key.length() != KEY_LENGTH)
    {
        std::cerr << "Input key length must be " << KEY_LENGTH << "\n";
        std::exit(1);
    }
    output_key = input_key;
    int checksum = 0;
    for (size_t i = 0; i < KEY_LENGTH; ++i)
    {
        int temp_sum = std::isdigit(static_cast<unsigned char>(output_key.at(i)))
                           ? (ENIGMA2_D_ROTOR_10.at(output_key.at(i) - '0') + checksum) % 10
                           : (ENIGMA2_D_ROTOR_26.at(output_key.at(i) - 'A') + checksum) % 26;
        output_key.at(i) = std::isdigit(static_cast<unsigned char>(output_key.at(i)))
                               ? static_cast<char>(temp_sum + '0')
                               : static_cast<char>('A' + temp_sum);
        checksum += static_cast<int>(i) + temp_sum + (static_cast<int>(i) * temp_sum);
    }
    checksum += 8 * (output_key.at(1) - '0');
    if (checksum % 100 != 0) output_key.clear();
}

bool enigma2_c_check_option_key(int option, const std::string& key)
{
    if (key.empty())
    {
        std::cerr << "Key cannot be empty\n";
        return false;
    }
    std::string decrypted_key;
    enigma2_c_decrypt(key, decrypted_key);
    if (decrypted_key.empty()) return false;
    std::string opt_str = decrypted_key.substr(OPTION_LOCATION, OPTION_CODE_SIZE);
    int opt = std::stoi(opt_str);
    return opt == option;
}

int get_menu_choice(const std::string& prompt, int min_val, int max_val)
{
    int choice = 0;
    std::string input;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, input);
        try
        {
            choice = std::stoi(input);
            if (choice >= min_val && choice <= max_val) return choice;
            std::cout << "Invalid choice, please try again.\n";
        }
        catch (const std::exception&)
        {
            std::cout << "Invalid input, please enter a number.\n";
        }
    }
}

bool product_code_menu(std::string& product_code, std::string& option_code)
{
    std::cout << "\n--- Product Code Menu ---\n";
    for (size_t i = 0; i < PRODUCT_TABLE.size(); ++i)
    {
        std::cout << (i + 1) << ". " << PRODUCT_TABLE.at(i).code << " - " << PRODUCT_TABLE.at(i).name << "\n";
    }
    std::cout << "8. Custom Product Code\n0. Exit\n";
    int choice = get_menu_choice("Choose your option: ", 0, 8);
    if (choice == 0) return false;

    if (choice == 8)
    {
        while (true)
        {
            std::cout << "Enter Product Code (4 digits): ";
            std::string input;
            std::getline(std::cin, input);
            if (input.length() == 4 && std::all_of(input.begin(), input.end(), ::isdigit))
            {
                product_code = input;
                break;
            }
            std::cout << "Product code must be 4 digits.\n";
        }
        while (true)
        {
            std::cout << "Enter Option Code (3 digits): ";
            std::string input;
            std::getline(std::cin, input);
            if (input.length() == 3 && std::all_of(input.begin(), input.end(), ::isdigit))
            {
                option_code = input;
                break;
            }
            std::cout << "Option code must be 3 digits.\n";
        }
        return true;
    }

    const ProductInfo& product = PRODUCT_TABLE.at(choice - 1);
    product_code = product.code;
    const ProductOptions* options = nullptr;
    for (const auto& opt : PRODUCT_OPTIONS)
    {
        if (opt.product_code == product_code)
        {
            options = &opt;
            break;
        }
    }
    if (!options || options->options.empty())
    {
        std::cout << "No options defined for " << product.name << ".\n";
        return false;
    }

    std::cout << "\n--- Options for " << product.name << " ---\n";
    for (size_t i = 0; i < options->options.size(); ++i)
    {
        std::cout << (i + 1) << ". " << options->options.at(i).code << " - " << options->options.at(i).desc << "\n";
    }
    std::cout << "8. Custom Option Code\n0. Exit\n";
    int opt_choice = get_menu_choice("Choose your option: ", 0, 8);
    if (opt_choice == 0) return false;

    if (opt_choice == 8)
    {
        while (true)
        {
            std::cout << "Enter Option Code (3 digits): ";
            std::string input;
            std::getline(std::cin, input);
            if (input.length() == 3 && std::all_of(input.begin(), input.end(), ::isdigit))
            {
                option_code = input;
                break;
            }
            std::cout << "Option code must be 3 digits.\n";
        }
    }
    else
    {
        option_code = options->options.at(opt_choice - 1).code;
    }
    return true;
}

void print_option_key(const std::string& option_key)
{
    std::cout << "Option Key:";
    for (size_t i = 0; i < option_key.length(); ++i)
    {
        if (i % 4 == 0) std::cout << " ";
        std::cout << option_key.at(i);
    }
    std::cout << "\n";
}

void calculate_nettool_option_key(std::string& serial_number, int option_number)
{
    if (serial_number.empty())
    {
        while (true)
        {
            std::cout << "Enter Serial Number (10 digits): ";
            std::getline(std::cin, serial_number);
            if (serial_number.length() == SERIAL_NUMBER_SIZE_ENIGMAC && std::all_of(
                serial_number.begin(), serial_number.end(), ::isdigit))
                break;
            std::cout << "Serial number must be 10 digits.\n";
        }
    }
    if (serial_number.length() != SERIAL_NUMBER_SIZE_ENIGMAC || !std::all_of(
        serial_number.begin(), serial_number.end(), ::isdigit))
    {
        std::cerr << "Serial number must be 10 digits\n";
        std::exit(1);
    }

    if (option_number < 0)
    {
        std::cout << "\nNetTool Options: 0=Inline 1=Reports/Ping 3=Personal 4=VoIP 5=SwitchWizard\n";
        std::string input;
        std::cout << "Enter Option Number (1 digit): ";
        std::getline(std::cin, input);
        option_number = (input.length() > 0 && std::isdigit(static_cast<unsigned char>(input.at(0))))
                            ? (input.at(0) - '0')
                            : 0;
    }
    if (option_number < 0 || option_number > 9) option_number = 0;

    std::string input_key = serial_number + std::to_string(option_number) + "0";
    std::string reversed_key(12, '0');
    for (int i = 0; i < 12; ++i)
    {
        reversed_key.at(i) = input_key.at(11 - i);
    }

    std::cout << "\nEncrypting with Enigma 1...\n";
    std::string output_key;
    enigma_c_encrypt(reversed_key, output_key, 12);
    print_option_key(output_key);
}

void check_nettool_option_key(std::string& option_key)
{
    std::string serial_number;
    while (true)
    {
        std::cout << "Enter Serial Number (10 digits): ";
        std::getline(std::cin, serial_number);
        if (serial_number.length() == SERIAL_NUMBER_SIZE_ENIGMAC && std::all_of(
            serial_number.begin(), serial_number.end(), ::isdigit))
            break;
        std::cout << "Serial number must be 10 digits.\n";
    }

    if (option_key.empty())
    {
        while (true)
        {
            std::cout << "Enter Option Key (12 hex digits): ";
            std::getline(std::cin, option_key);
            if (option_key.length() == 12 && std::all_of(option_key.begin(), option_key.end(), [](char c)
            {
                return std::isxdigit(static_cast<unsigned char>(c));
            }))
                break;
            std::cout << "Option key must be 12 hex digits.\n";
        }
    }
    if (option_key.length() != 12 || !std::all_of(option_key.begin(), option_key.end(),
                                                  [](char c) { return std::isxdigit(static_cast<unsigned char>(c)); }))
    {
        std::cerr << "Option key must be 12 hex digits\n";
        std::exit(1);
    }

    std::string input;
    std::cout << "Enter Option Number (1 digit): ";
    std::getline(std::cin, input);
    int option_number = (input.length() > 0 && std::isdigit(static_cast<unsigned char>(input.at(0))))
                            ? (input.at(0) - '0')
                            : 0;
    if (option_number < 0 || option_number > 9) option_number = 0;

    std::cout << "\nEnigmaC::checkOptionKey()...\n";
    std::cout << "serialNum: " << serial_number << "\n";
    std::cout << "optionKey: " << option_key << "\n";
    std::cout << "optionNum: 0x" << std::hex << option_number << std::dec << "\n";
    bool result = enigma_c_check_option_key(option_number, option_key, serial_number);
    std::cout << "Option " << (result ? "valid" : "invalid") << "\n";
}

void calculate_enigma2_option_key(std::string& serial_number, int option_number, int product_code, bool assume_escope)
{
    std::string product_code_str;
    std::string option_str;
    if (product_code >= 0)
    {
        product_code_str = std::to_string(product_code);
        product_code_str = std::string(4 - product_code_str.length(), '0') + product_code_str;
    }
    if (option_number >= 0)
    {
        option_str = std::to_string(option_number);
        option_str = std::string(3 - option_str.length(), '0') + option_str;
    }

    if (serial_number.empty())
    {
        while (true)
        {
            std::cout << "Enter Serial Number (7 digits): ";
            std::getline(std::cin, serial_number);
            if (serial_number.length() == SERIAL_NUMBER_SIZE_ENIGMA2 && std::all_of(
                serial_number.begin(), serial_number.end(), ::isdigit))
                break;
            std::cout << "Serial number must be 7 digits.\n";
        }
    }
    if (serial_number.length() != SERIAL_NUMBER_SIZE_ENIGMA2 || !std::all_of(
        serial_number.begin(), serial_number.end(), ::isdigit))
    {
        std::cerr << "Serial number must be 7 digits\n";
        std::exit(1);
    }

    std::cout << "SerialNum= " << serial_number << "\n";

    if (product_code_str.empty() || option_str.empty() || !assume_escope)
    {
        std::string prod_code;
        std::string opt_code;
        if (!product_code_menu(prod_code, opt_code))
        {
            std::cout << "Operation cancelled.\n";
            return;
        }
        product_code_str = prod_code;
        option_str = opt_code;
    }

    std::string input_key = "00" + product_code_str + serial_number + option_str;

    std::cout << "\nEncrypting with Enigma 2...\n";
    std::string output_key;
    enigma2_c_encrypt(input_key, output_key);
    print_option_key(output_key);
}

void check_enigma2_option_key(std::string& option_key)
{
    if (option_key.empty())
    {
        while (true)
        {
            std::cout << "Enter Option Key (16 characters): ";
            std::getline(std::cin, option_key);
            if (option_key.length() == 16 && std::all_of(option_key.begin(), option_key.end(), [](char c)
            {
                return std::isalnum(static_cast<unsigned char>(c)) && (
                    std::isdigit(static_cast<unsigned char>(c)) || (c >=
                        'A' && c <= 'Z'));
            }))
                break;
            std::cout << "Option key must be 16 alphanumeric characters.\n";
        }
    }
    if (option_key.length() != 16 || !std::all_of(option_key.begin(), option_key.end(), [](char c)
    {
        return std::isalnum(static_cast<unsigned char>(c)) && (
            std::isdigit(static_cast<unsigned char>(c)) || (c >= 'A' && c
                <= 'Z'));
    }))
    {
        std::cerr << "Option key must be 16 alphanumeric characters\n";
        std::exit(1);
    }

    std::cout << "Decrypting with Enigma 2...\n";
    std::string decrypted_key;
    enigma2_c_decrypt(option_key, decrypted_key);
    if (decrypted_key.empty())
    {
        std::cerr << "Decryption failed: invalid checksum\n";
        std::exit(1);
    }

    std::string product_code = decrypted_key.substr(PRODUCT_LOCATION, PRODUCT_CODE_SIZE);
    std::cout << "Product Code: " << product_code << " -> ";
    bool found = false;
    for (const auto& product : PRODUCT_TABLE)
    {
        if (product.code == product_code)
        {
            std::cout << product.name << "\n";
            found = true;
            break;
        }
    }
    if (!found) std::cout << "Unknown\n";
    std::string serial = decrypted_key.substr(SERIAL_LOCATION, SERIAL_NUMBER_SIZE_ENIGMA2);
    std::cout << "SerialNumber: " << serial << "\n";
    std::string option = decrypted_key.substr(OPTION_LOCATION, OPTION_CODE_SIZE);
    std::cout << "OptionNumber: " << option << "\n";
}

bool main_menu()
{
    std::cout << "\n--- Enigma " << SOFTWARE_VERSION << " Main Menu ---\n";
    std::cout << "1. Generate NetTool 10/100 Option Key\n";
    std::cout << "2. Check NetTool 10/100 Option Key\n";
    std::cout << "3. Generate Option Key for Other Fluke Products\n";
    std::cout << "4. Decrypt Option Key for Other Fluke Products\n";
    std::cout << "0. Exit\n";

    int choice = get_menu_choice("Choose your option: ", 0, 4);
    if (choice == 0) return false;

    std::string serial_number;
    std::string option_key;
    switch (choice)
    {
    case 1:
        calculate_nettool_option_key(serial_number, -1);
        return true;
    case 2:
        check_nettool_option_key(option_key);
        return true;
    case 3:
        calculate_enigma2_option_key(serial_number, -1, -1, false);
        return true;
    case 4:
        check_enigma2_option_key(option_key);
        return true;
    default:
        std::cerr << "Unexpected choice value: " << choice << "\n";
        return false;
    }
}

void print_help(const char* prog_name)
{
    std::cout << "Enigma " << SOFTWARE_VERSION << " - Fluke option key utility\n"
        << "Usage:\n"
        << "  " << prog_name << " [mode] [args...]\n\n"
        << "Modes:\n"
        << "  -n SERIAL OPTION        Generate NetTool option key\n"
        << "  -x OPTION_KEY           Check NetTool option key\n"
        << "  -e SERIAL [OPTION [PRODUCT]]  Generate EtherScope/MetroScope option key\n"
        << "  -l SERIAL OPTION        Generate LinkRunner Pro option key\n"
        << "  -d OPTION_KEY           Decrypt EtherScope/MetroScope option key\n\n"
        << "Utility flags:\n"
        << "  -h, --help, -?          Show this help text\n"
        << "  -V, --version           Show version information\n"
        << "  --list-products         List known product codes\n"
        << "  --list-options CODE     List options for a product code\n\n"
        << "Run without arguments to launch the interactive menu.\n";
}

void print_version()
{
    std::cout << "Enigma " << SOFTWARE_VERSION << " - Fluke option key utility\n";
}

void list_products()
{
    std::cout << "Known Product Codes:\n";
    for (const auto& product : PRODUCT_TABLE)
    {
        std::cout << "  " << product.code << " - " << product.name << "\n";
    }
}

void list_options(const std::string& product_code)
{
    for (const auto& po : PRODUCT_OPTIONS)
    {
        if (po.product_code == product_code)
        {
            std::string product_name = "Unknown";
            for (const auto& p : PRODUCT_TABLE)
            {
                if (p.code == product_code)
                {
                    product_name = p.name;
                    break;
                }
            }
            std::cout << "Options for " << product_code << " (" << product_name << "):\n";
            for (const auto& opt : po.options)
            {
                std::cout << "  " << opt.code << " - " << opt.desc << "\n";
            }
            return;
        }
    }
    std::cerr << "No options defined for product code " << product_code << "\n";
}

int main(int argc, char* argv[])
{
    std::string serial_number;
    std::string option_key;
    int selection = 0;
    int option_number = -1;
    int product_code = -1;
    bool assume_escope = false;

    if (argc > 1)
    {
        std::string arg1(argv[1]);

        // Help flags
        if (arg1 == "?" || arg1 == "-?" || arg1 == "-h" || arg1 == "--help")
        {
            print_help(argv[0]);
            return 0;
        }

        // Version flag
        if (arg1 == "-V" || arg1 == "--version")
        {
            print_version();
            return 0;
        }

        // List products
        if (arg1 == "--list-products")
        {
            list_products();
            return 0;
        }

        // List options
        if (arg1 == "--list-options")
        {
            if (argc > 2)
            {
                list_options(argv[2]);
                return 0;
            }
            else
            {
                std::cerr << "Error: --list-options requires a product code\n";
                return 1;
            }
        }

        // Mode flags
        if (arg1 == "-n")
        {
            selection = 1;
        }
        else if (arg1 == "-x")
        {
            selection = 2;
        }
        else if (arg1 == "-e")
        {
            selection = 3;
            product_code = 6963;
            assume_escope = true;
        }
        else if (arg1 == "-l")
        {
            selection = 3;
            product_code = 7001;  // LinkRunner Pro
            assume_escope = true;  // Skip product menu when all args provided
        }
        else if (arg1 == "-d")
        {
            selection = 4;
        }
        else
        {
            // Try to parse as number for interactive menu selection
            try
            {
                selection = std::stoi(arg1);
            }
            catch (const std::exception&)
            {
                std::cerr << "Unknown option: " << arg1 << "\n";
                print_help(argv[0]);
                return 1;
            }
        }

        if (argc > 2)
        {
            if (selection == 4 || selection == 2)
            {
                option_key = std::string(argv[2]);
            }
            else
            {
                serial_number = std::string(argv[2]);
            }
            if (argc > 3) option_number = std::stoi(argv[3]);
            if (argc > 4) product_code = std::stoi(argv[4]);
        }
    }

    if (selection < 1 || selection > 4)
    {
        while (main_menu())
        {
        }
        return 0;
    }

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
        std::cerr << "Invalid selection value: " << selection << "\n";
        return 1;
    }
    return 0;
}
