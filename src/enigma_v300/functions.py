#!/usr/bin/env python3
"""
Project Title: Enigma V300 Functions

Calculates and validates Fluke option keys using functional programming approach.

Implements pure functions for EnigmaC (NetTool, 10-digit serial) and
Enigma2C (other Fluke products, 7-digit serial) ciphers.

Author: Kris Armstrong
"""

from typing import List, Tuple

# Constants
SOFTWARE_VERSION = "3.0.0"
SERIAL_NUMBER_SIZE_ENIGMAC = 10
SERIAL_NUMBER_SIZE_ENIGMA2 = 7
PRODUCT_CODE_SIZE = 4
OPTION_CODE_SIZE = 3
CHECK_SUM_SIZE = 2
KEY_LENGTH = PRODUCT_CODE_SIZE + OPTION_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2 + CHECK_SUM_SIZE

# Rotor tables
ENIGMA_C_ROTOR: List[int] = [5, 4, 14, 11, 1, 8, 10, 13, 7, 3, 15, 0, 2, 12, 9, 6]
ENIGMA2_E_ROTOR_10: List[int] = [5, 4, 1, 8, 7, 3, 0, 2, 9, 6]
ENIGMA2_E_ROTOR_26: List[int] = [16, 8, 25, 5, 23, 21, 18, 17, 2, 1, 7, 24, 15, 11, 9, 6, 3, 0, 19, 12, 22, 14, 10, 4, 20, 13]
ENIGMA2_D_ROTOR_10: List[int] = [6, 2, 7, 5, 1, 0, 9, 4, 3, 8]
ENIGMA2_D_ROTOR_26: List[int] = [17, 9, 8, 16, 23, 3, 15, 10, 1, 14, 22, 13, 19, 25, 21, 12, 0, 7, 6, 18, 24, 5, 20, 4, 11, 2]
MAX_CHECK_SUM = 26000


# EnigmaC Functions
def enigma_c_encrypt(input_key: str) -> str:
    """Encrypt the input key using EnigmaC cipher.

    Args:
        input_key: Hex string to encrypt.

    Returns:
        Encrypted hex string.

    Raises:
        ValueError: If input contains non-hex characters.
    """
    output_key = ""
    output_value = 0
    for index, char in enumerate(input_key.lower()):
        if char not in "0123456789abcdef":
            raise ValueError("Input contains non-hex characters")
        input_value = int(char, 16)
        output_value = ENIGMA_C_ROTOR[(input_value + index) % len(ENIGMA_C_ROTOR)] ^ output_value
        output_key += hex(output_value)[2:]
    return output_key


def enigma_c_decrypt(input_key: str) -> str:
    """Decrypt the input key using EnigmaC cipher.

    Args:
        input_key: Hex string to decrypt.

    Returns:
        Decrypted hex string.

    Raises:
        ValueError: If input contains non-hex characters.
    """
    output_key = ""
    xor_value = 0
    for index, char in enumerate(input_key.lower()):
        if char not in "0123456789abcdef":
            raise ValueError("Input contains non-hex characters")
        old_output = int(char, 16)
        input_value = old_output ^ xor_value
        output_value = ENIGMA_C_ROTOR.index(input_value)
        temp = (output_value - index) % len(ENIGMA_C_ROTOR)
        output_key += hex(temp)[2:]
        xor_value = old_output
    return output_key


def check_nettool_option_key(option: int, key: str, serial_number: str) -> bool:
    """Check if the NetTool option key is valid.

    Args:
        option: Option number to validate.
        key: Option key to check.
        serial_number: Device serial number.

    Returns:
        True if key is valid, False otherwise.

    Raises:
        ValueError: If key is empty.
    """
    if not key:
        raise ValueError("Key cannot be empty")
    if key == "bladerules":
        return True
    decrypted_key = enigma_c_decrypt(key)
    reversed_serial = decrypted_key[:10][::-1]
    if reversed_serial != serial_number:
        return False
    opt = int(decrypted_key[10:12], 10)
    return opt == option


def generate_nettool_option_key(serial_number: str, option_number: int) -> str:
    """Generate NetTool option key.

    Args:
        serial_number: Device serial number (10 digits).
        option_number: Option number to encode.

    Returns:
        Generated option key.

    Raises:
        ValueError: If serial number is invalid.
    """
    if len(serial_number) != SERIAL_NUMBER_SIZE_ENIGMAC or not serial_number.isdigit():
        raise ValueError("Serial number must be 10 digits")

    if option_number < 0 or option_number > 9:
        raise ValueError("Option number must be 0-9")

    input_key = serial_number + str(option_number) + "0"
    reversed_key = input_key[::-1]
    return enigma_c_encrypt(reversed_key)


# Enigma2C Functions
def enigma2_c_encrypt(input_key: str) -> str:
    """Encrypt the input key using Enigma2C cipher.

    Args:
        input_key: Alphanumeric string to encrypt.

    Returns:
        Encrypted string.

    Raises:
        ValueError: If input key length is invalid.
    """
    if len(input_key) != KEY_LENGTH:
        raise ValueError(f"Input key length must be {KEY_LENGTH}")

    output_key = list(input_key)
    checksum = 1
    for i in range(2, len(input_key)):
        temp_sum = int(input_key[i]) if input_key[i].isdigit() else ord(input_key[i]) - ord("A")
        checksum += i + temp_sum + (i * temp_sum)
    checksum = 100 - (checksum % 100)
    output_key[0] = str(checksum % 10)
    output_key[1] = str((checksum // 10) % 10)

    running_sum = 0
    for i in range(len(output_key)):
        temp_sum = int(output_key[i]) if output_key[i].isdigit() else ord(output_key[i]) - ord("A")
        if output_key[i].isdigit():
            output_key[i] = str(ENIGMA2_E_ROTOR_10[(temp_sum + MAX_CHECK_SUM - running_sum) % 10])
        else:
            output_key[i] = chr(ord("A") + ENIGMA2_E_ROTOR_26[(temp_sum + MAX_CHECK_SUM - running_sum) % 26])
        running_sum += i + temp_sum + (i * temp_sum)
    return "".join(output_key)


def enigma2_c_decrypt(input_key: str) -> str:
    """Decrypt the input key using Enigma2C cipher.

    Args:
        input_key: Alphanumeric string to decrypt.

    Returns:
        Decrypted string or empty if invalid.

    Raises:
        ValueError: If input key length is invalid.
    """
    if len(input_key) != KEY_LENGTH:
        raise ValueError(f"Input key length must be {KEY_LENGTH}")

    output_key = list(input_key)
    checksum = 0
    for i in range(len(input_key)):
        temp_sum = (
            (ENIGMA2_D_ROTOR_10[int(output_key[i])] + checksum) % 10
            if output_key[i].isdigit()
            else (ENIGMA2_D_ROTOR_26[ord(output_key[i]) - ord("A")] + checksum) % 26
        )
        output_key[i] = str(temp_sum) if output_key[i].isdigit() else chr(ord("A") + temp_sum)
        checksum += i + temp_sum + (i * temp_sum)
    checksum += 8 * int(output_key[1])
    return "".join(output_key) if checksum % 100 == 0 else ""


def check_enigma2_option_key(option: int, key: str) -> bool:
    """Check if the Enigma2C option key is valid.

    Args:
        option: Option number to validate.
        key: Option key to check.

    Returns:
        True if key is valid, False otherwise.

    Raises:
        ValueError: If key is empty.
    """
    if not key:
        raise ValueError("Key cannot be empty")
    decrypted_key = enigma2_c_decrypt(key)
    if not decrypted_key:
        return False
    option_location = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2
    opt = int(decrypted_key[option_location: option_location + OPTION_CODE_SIZE])
    return opt == option


def generate_enigma2_option_key(serial_number: str, option_number: int, product_code: int) -> str:
    """Generate Enigma2 option key.

    Args:
        serial_number: Device serial number (7 digits).
        option_number: Option number to encode (3 digits).
        product_code: Product code (4 digits).

    Returns:
        Generated option key.

    Raises:
        ValueError: If inputs are invalid.
    """
    if len(serial_number) != SERIAL_NUMBER_SIZE_ENIGMA2 or not serial_number.isdigit():
        raise ValueError("Serial number must be 7 digits")

    product_code_str = str(product_code).zfill(4)
    option_str = str(option_number).zfill(3)

    input_key = "00" + product_code_str + serial_number + option_str
    return enigma2_c_encrypt(input_key)


def decode_enigma2_key(key: str) -> Tuple[str, str, str]:
    """Decode an Enigma2 key and extract product, serial, and option.

    Args:
        key: 16-character option key.

    Returns:
        Tuple of (product_code, serial_number, option_code).

    Raises:
        ValueError: If key is invalid.
    """
    if len(key) != KEY_LENGTH or not key.isalnum():
        raise ValueError("Option key must be 16 alphanumeric characters")

    decrypted_key = enigma2_c_decrypt(key)
    if not decrypted_key:
        raise ValueError("Decryption failed: invalid checksum")

    product_location = CHECK_SUM_SIZE
    serial_location = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE
    option_location = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2

    product_code = decrypted_key[product_location: product_location + PRODUCT_CODE_SIZE]
    serial_number = decrypted_key[serial_location: serial_location + SERIAL_NUMBER_SIZE_ENIGMA2]
    option_code = decrypted_key[option_location: option_location + OPTION_CODE_SIZE]

    return product_code, serial_number, option_code


def format_option_key(option_key: str, group_size: int = 4) -> str:
    """Format option key with spaces for readability.

    Args:
        option_key: Key to format.
        group_size: Number of characters per group.

    Returns:
        Formatted key with spaces.
    """
    output = ""
    for i, char in enumerate(option_key):
        if i > 0 and i % group_size == 0:
            output += " "
        output += char
    return output
