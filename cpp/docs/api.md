# API Documentation

## Overview

This document describes the public and internal APIs for the Enigma v3.0.0 option key calculator.

## Core Constants

### Size Constants

```cpp
constexpr size_t PRODUCT_CODE_SIZE = 4;          // Product code length
constexpr size_t OPTION_CODE_SIZE = 3;           // Option code length
constexpr size_t SERIAL_NUMBER_SIZE_ENIGMA2 = 7; // 7-digit serial
constexpr size_t SERIAL_NUMBER_SIZE_ENIGMAC = 10; // 10-digit serial
constexpr size_t CHECK_SUM_SIZE = 2;             // Checksum length
constexpr size_t KEY_LENGTH = 16;                // Total key length
constexpr int MAX_CHECK_SUM = 26000;             // Checksum modulus
```

### Position Constants

```cpp
constexpr size_t SERIAL_LOCATION = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE;
constexpr size_t PRODUCT_LOCATION = CHECK_SUM_SIZE;
constexpr size_t OPTION_LOCATION = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2;
```

## Data Structures

### ProductInfo

Represents a Fluke product definition.

```cpp
struct ProductInfo {
    std::string code;  // 4-digit product code
    std::string abbr;  // Product abbreviation
    std::string name;  // Full product name
};
```

**Example:**
```cpp
{"6963", "Escope/MSv2", "EtherScope/MetroScope"}
```

### OptionInfo

Represents an option for a specific product.

```cpp
struct OptionInfo {
    std::string code;  // 3-digit option code
    std::string desc;  // Option description
};
```

**Example:**
```cpp
{"003", "Wi-Fi"}
```

### ProductOptions

Links a product to its available options.

```cpp
struct ProductOptions {
    std::string product_code;           // 4-digit product code
    std::vector<OptionInfo> options;    // Available options
};
```

## EnigmaC Functions (NetTool)

### enigma_c_encrypt()

Encrypts a key using the EnigmaC algorithm for NetTool devices.

```cpp
void enigma_c_encrypt(
    const std::string& input_key,    // Input key (hex)
    std::string& output_key,         // Output encrypted key
    size_t len                       // Key length (12)
);
```

**Parameters:**
- `input_key`: Hexadecimal input string (12 characters)
- `output_key`: Reference to store encrypted output
- `len`: Length of key (must be 12)

**Behavior:**
- Applies rotor-based XOR encryption
- Position-dependent transformation
- Outputs lowercase hexadecimal
- Exits on non-hex input

**Example:**
```cpp
std::string input = "610333300054";
std::string output;
enigma_c_encrypt(input, output, 12);
// output = "5dabade112dd"
```

### enigma_c_decrypt()

Decrypts a key encrypted with EnigmaC.

```cpp
void enigma_c_decrypt(
    const std::string& input_key,    // Encrypted key
    std::string& output_key,         // Decrypted output
    size_t len                       // Key length (12)
);
```

**Parameters:**
- `input_key`: Encrypted hexadecimal string
- `output_key`: Reference to store decrypted output
- `len`: Length of key (must be 12)

**Behavior:**
- Reverses EnigmaC encryption
- Validates hex format
- Position-dependent inverse transformation

### enigma_c_check_option_key()

Validates an option key against a serial number and option.

```cpp
bool enigma_c_check_option_key(
    int option,                      // Option number (0-9)
    const std::string& key,          // Option key to validate
    const std::string& serial_number // 10-digit serial
);
```

**Parameters:**
- `option`: Option number (0-9)
- `key`: 12-character hex key to validate
- `serial_number`: 10-digit serial number

**Returns:**
- `true` if key is valid for the serial/option combination
- `false` if validation fails

**Special Cases:**
- Returns `true` for key "bladerules" (master key)
- Returns `false` for empty key

## Enigma2C Functions (Other Products)

### enigma2_c_encrypt()

Encrypts a key using the Enigma2C algorithm for non-NetTool products.

```cpp
void enigma2_c_encrypt(
    const std::string& input_key,    // Input key (16 chars)
    std::string& output_key          // Output encrypted key
);
```

**Parameters:**
- `input_key`: 16-character string (format: 00PPPPSSSSSSSOOO)
  - 00: Placeholder for checksum
  - PPPP: 4-digit product code
  - SSSSSSS: 7-digit serial number
  - OOO: 3-digit option code
- `output_key`: Reference to store encrypted output

**Behavior:**
- Calculates and embeds checksum
- Applies dual-rotor encryption (separate for digits/letters)
- Outputs alphanumeric uppercase/digits
- Exits on incorrect input length

**Example:**
```cpp
std::string input = "0069630000607007";
std::string output;
enigma2_c_encrypt(input, output);
// output = "6406257948597747"
```

### enigma2_c_decrypt()

Decrypts a key encrypted with Enigma2C.

```cpp
void enigma2_c_decrypt(
    const std::string& input_key,    // Encrypted key
    std::string& output_key          // Decrypted output
);
```

**Parameters:**
- `input_key`: 16-character encrypted key
- `output_key`: Reference to store decrypted output (empty if invalid)

**Behavior:**
- Applies inverse dual-rotor transformation
- Validates checksum
- Clears output_key on checksum failure

**Checksum Validation:**
- Computes running sum during decryption
- Final sum must be divisible by 100
- Invalid checksum results in empty output

### enigma2_c_check_option_key()

Validates an Enigma2C option key.

```cpp
bool enigma2_c_check_option_key(
    int option,                      // Option code (integer)
    const std::string& key           // 16-char key to validate
);
```

**Parameters:**
- `option`: Expected option number
- `key`: 16-character key to validate

**Returns:**
- `true` if key decrypts successfully and option matches
- `false` if decryption fails or option doesn't match

## User Interface Functions

### get_menu_choice()

Gets a validated integer choice from the user.

```cpp
int get_menu_choice(
    const std::string& prompt,       // Prompt to display
    int min_val,                     // Minimum valid value
    int max_val                      // Maximum valid value
);
```

**Parameters:**
- `prompt`: Text to display before input
- `min_val`: Minimum acceptable value
- `max_val`: Maximum acceptable value

**Returns:**
- Integer choice within [min_val, max_val]

**Behavior:**
- Loops until valid input received
- Handles non-numeric input gracefully
- Displays error messages for invalid input

### product_code_menu()

Displays product selection menu and returns chosen codes.

```cpp
bool product_code_menu(
    std::string& product_code,       // Output: selected product code
    std::string& option_code         // Output: selected option code
);
```

**Parameters:**
- `product_code`: Reference to store selected product code
- `option_code`: Reference to store selected option code

**Returns:**
- `true` if selection completed
- `false` if user chooses to exit

**Behavior:**
- Displays all products from PRODUCT_TABLE
- Shows product-specific options
- Allows custom code entry
- Validates all input

### print_option_key()

Formats and displays an option key with spacing.

```cpp
void print_option_key(const std::string& option_key);
```

**Parameters:**
- `option_key`: Key to display

**Output Format:**
- Adds space every 4 characters
- Example: "5dab ade1 12dd" or "6406 2579 4859 7747"

## High-Level Operations

### calculate_nettool_option_key()

Generates an option key for NetTool devices.

```cpp
void calculate_nettool_option_key(
    std::string& serial_number,      // Serial (empty for prompt)
    int option_number                // Option (-1 for prompt)
);
```

**Parameters:**
- `serial_number`: 10-digit serial (empty to prompt user)
- `option_number`: Option 0-9 (-1 to prompt user)

**Behavior:**
- Prompts for missing parameters
- Validates serial number format
- Reverses serial+option for encryption
- Displays formatted key

### check_nettool_option_key()

Validates a NetTool option key.

```cpp
void check_nettool_option_key(
    std::string& option_key          // Key to check (empty for prompt)
);
```

**Parameters:**
- `option_key`: 12-character hex key (empty to prompt)

**Behavior:**
- Prompts for serial number and option
- Validates key format
- Displays validation result

### calculate_enigma2_option_key()

Generates an option key for non-NetTool products.

```cpp
void calculate_enigma2_option_key(
    std::string& serial_number,      // Serial (empty for prompt)
    int option_number,               // Option (-1 for prompt)
    int product_code,                // Product (-1 for menu)
    bool assume_escope               // Skip menu if true
);
```

**Parameters:**
- `serial_number`: 7-digit serial (empty to prompt)
- `option_number`: Option code integer (-1 for menu)
- `product_code`: Product code integer (-1 for menu)
- `assume_escope`: If true, skip product menu

**Behavior:**
- Prompts for missing parameters
- Shows product/option menus if needed
- Assembles and encrypts key
- Displays formatted result

### check_enigma2_option_key()

Decrypts and displays an Enigma2C option key.

```cpp
void check_enigma2_option_key(
    std::string& option_key          // Key to decrypt (empty for prompt)
);
```

**Parameters:**
- `option_key`: 16-character key (empty to prompt)

**Behavior:**
- Validates key format
- Decrypts and extracts components
- Displays product, serial, and option
- Shows product name if recognized

## Main Menu

### main_menu()

Displays and processes the main menu.

```cpp
bool main_menu();
```

**Returns:**
- `true` to continue (show menu again)
- `false` to exit program

**Menu Options:**
1. Generate NetTool 10/100 Option Key
2. Check NetTool 10/100 Option Key
3. Generate Option Key for Other Fluke Products
4. Decrypt Option Key for Other Fluke Products
0. Exit

## Command-Line Interface

### main()

Program entry point with command-line argument support.

```cpp
int main(int argc, char* argv[]);
```

**Command-Line Flags:**
- `-n`: Calculate NetTool option key
- `-x`: Check NetTool option key
- `-e`: Generate EtherScope option key
- `-l`: Generate LinkRunner option key
- `-d`: Decrypt option key
- `-?` or `?`: Display usage help

**Argument Format:**
```
program [-flag] [serial|key] [option] [product]
```

**Examples:**
```bash
./enigma -n 0003333016 4
./enigma -e 0000607 7 6963
./enigma -d 6406257948597747
```

**Return Values:**
- 0: Success
- 1: Error or help displayed

## Global Data

### PRODUCT_TABLE

Vector of all supported products with codes, abbreviations, and names.

### PRODUCT_OPTIONS

Vector mapping product codes to their available options.

### Rotor Constants

- `ENIGMA_C_ROTOR`: 16-element permutation for EnigmaC
- `ENIGMA2_E_ROTOR_10`: 10-element encryption rotor for digits
- `ENIGMA2_E_ROTOR_26`: 26-element encryption rotor for letters
- `ENIGMA2_D_ROTOR_10`: 10-element decryption rotor for digits
- `ENIGMA2_D_ROTOR_26`: 26-element decryption rotor for letters

## Error Handling

All functions that validate input will:
- Display error messages to stderr
- Call `std::exit(1)` on fatal errors
- Return false on validation failures
- Use exceptions for numeric conversions (caught and handled)

## Thread Safety

This implementation is not thread-safe. Global data is read-only, but:
- User input functions access stdin
- Output functions access stdout/stderr
- No synchronization mechanisms present
