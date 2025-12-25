#!/usr/bin/env python3
"""
Project Title: Enigma V300 Functions

Calculates and validates Fluke option keys using EnigmaC and Enigma2C ciphers.

Implements a functional design (no classes) with EnigmaC (NetTool, 10-digit serial)
and Enigma2C (other Fluke products, 7-digit serial) cipher functions.

Author: Kris Armstrong
"""

import argparse
import logging
import logging.handlers
import sys
from importlib.metadata import PackageNotFoundError
from importlib.metadata import version as _pkg_version
from pathlib import Path


def _find_pyproject(start: Path) -> Path | None:
    for parent in (start, *start.parents):
        candidate = parent / "pyproject.toml"
        if candidate.is_file():
            return candidate
    return None


def _read_pyproject_version() -> str:
    try:
        import tomllib  # Python 3.11+
    except ModuleNotFoundError:
        return "0.0.0"

    pyproject = _find_pyproject(Path(__file__).resolve())
    if not pyproject:
        return "0.0.0"
    try:
        data = tomllib.loads(pyproject.read_text())
    except Exception:
        return "0.0.0"
    return data.get("project", {}).get("version", "0.0.0")


try:
    __version__ = _pkg_version("enigma-v300")
except PackageNotFoundError:
    __version__ = _read_pyproject_version()

# Configure logging
logger = logging.getLogger(__name__)


def setup_logging(verbose: bool, logfile: str | None = None) -> None:
    """Configure logging with console and optional file handlers."""
    level = logging.DEBUG if verbose else logging.INFO
    formatter = logging.Formatter("%(asctime)s [%(levelname)s] %(message)s")

    console_handler = logging.StreamHandler()
    console_handler.setFormatter(formatter)
    logger.handlers = [console_handler]
    logger.setLevel(level)

    if logfile:
        file_handler = logging.handlers.RotatingFileHandler(
            logfile, maxBytes=10 * 1024 * 1024, backupCount=5
        )
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)


# Global constants
SOFTWARE_VERSION: str = "3.0.0"
PRODUCT_CODE_SIZE: int = 4
OPTION_CODE_SIZE: int = 3
SERIAL_NUMBER_SIZE_ENIGMA2: int = 7
SERIAL_NUMBER_SIZE_ENIGMAC: int = 10
CHECK_SUM_SIZE: int = 2
KEY_LENGTH: int = PRODUCT_CODE_SIZE + OPTION_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2 + CHECK_SUM_SIZE
SERIAL_LOCATION: int = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE
PRODUCT_LOCATION: int = CHECK_SUM_SIZE
OPTION_LOCATION: int = CHECK_SUM_SIZE + PRODUCT_CODE_SIZE + SERIAL_NUMBER_SIZE_ENIGMA2
MAX_CHECK_SUM: int = 26000

PRODUCT_TABLE: list[dict[str, str]] = [
    {"code": "3001", "abbr": "NTs2", "name": "NetTool Series II"},
    {"code": "7001", "abbr": "LRPro", "name": "LinkRunner Pro Duo"},
    {"code": "6963", "abbr": "Escope/MSv2", "name": "EtherScope/MetroScope"},
    {"code": "6964", "abbr": "OneTouch", "name": "OneTouch AT"},
    {"code": "2186", "abbr": "OptiView", "name": "OptiView XG"},
    {"code": "1890", "abbr": "ClearSight", "name": "ClearSight Analyzer"},
    {"code": "1895", "abbr": "iClearSight", "name": "iClearSight Analyzer"},
]

PRODUCT_OPTIONS: dict[str, dict[str, str]] = {
    "6964": {
        "000": "Registered",
        "001": "Wired (Was Copper)",
        "002": "Obsolete (was fiber)",
        "003": "Wi-Fi",
        "004": "Obsolete (was inline)",
        "005": "Capture",
        "006": "Advanced Tests",
        "007": "XGR-to-ATX Upgrade",
        "008": "Claimed (Cloud Tools)",
        "009": "LatTests (China LAN Tests)",
        "064": "XGReflector (Future)",
        "065": "Performance Peer (Future)",
    },
    "6963": {
        "000": "MetroScope Base, EtherScope LAN",
        "001": "MetroScope WLAN, EtherScope WLAN",
        "002": "MetroScope Multi, EtherScope ITO",
        "003": "MetroScope VoIP, EtherScope Fiber",
        "004": "MetroScope LT, EtherScope LT",
    },
    "7001": {
        "000": "802.1x",
        "002": "Reports",
        "003": "LAN",
    },
    "2186": {
        "000": "Wireless Analyzer Option",
        "001": "Enables Network Test Ports A-D",
        "002": "10Gb Ethernet Analyzer Option",
        "003": "LAN / 10Gb Ethernet Analyzer Option",
        "004": "NPT - Network Performance Option",
        "007": "Everything",
    },
    "1890": {"000": "Activation Code", "007": "All Options"},
    "1895": {"000": "Activation Code", "003": "All Options"},
    "3001": {
        "003": "Personalization",
        "004": "VoIP",
        "005": "NetSecure",
        "008": "Dicom",
    },
}

ENIGMA_C_ROTOR: list[int] = [5, 4, 14, 11, 1, 8, 10, 13, 7, 3, 15, 0, 2, 12, 9, 6]
ENIGMA2_E_ROTOR_10: list[int] = [5, 4, 1, 8, 7, 3, 0, 2, 9, 6]
ENIGMA2_E_ROTOR_26: list[int] = [
    16,
    8,
    25,
    5,
    23,
    21,
    18,
    17,
    2,
    1,
    7,
    24,
    15,
    11,
    9,
    6,
    3,
    0,
    19,
    12,
    22,
    14,
    10,
    4,
    20,
    13,
]
ENIGMA2_D_ROTOR_10: list[int] = [6, 2, 7, 5, 1, 0, 9, 4, 3, 8]
ENIGMA2_D_ROTOR_26: list[int] = [
    17,
    9,
    8,
    16,
    23,
    3,
    15,
    10,
    1,
    14,
    22,
    13,
    19,
    25,
    21,
    12,
    0,
    7,
    6,
    18,
    24,
    5,
    20,
    4,
    11,
    2,
]


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


def enigma_c_check_option_key(option: int, key: str, serial_number: str) -> bool:
    """Check if the option key is valid.

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
            output_key[i] = chr(
                ord("A") + ENIGMA2_E_ROTOR_26[(temp_sum + MAX_CHECK_SUM - running_sum) % 26]
            )
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


def enigma2_c_check_option_key(option: int, key: str) -> bool:
    """Check if the option key is valid.

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
    opt = int(decrypted_key[OPTION_LOCATION : OPTION_LOCATION + OPTION_CODE_SIZE])
    return opt == option


def get_menu_choice(prompt: str, min_val: int, max_val: int) -> int:
    """Get a valid menu choice from user.

    Args:
        prompt: Prompt to display.
        min_val: Minimum valid choice.
        max_val: Maximum valid choice.

    Returns:
        Selected choice.
    """
    while True:
        try:
            choice = int(input(prompt))
            if min_val <= choice <= max_val:
                return choice
            logger.warning("Invalid choice, please try again.")
        except ValueError:
            logger.warning("Invalid input, please enter a number.")


def product_code_menu() -> tuple[str, str]:
    """Display product menu and get codes.

    Returns:
        Tuple of product code and option code, or empty strings if cancelled.
    """
    logger.info("--- Product Code Menu ---")
    for i, product in enumerate(PRODUCT_TABLE, 1):
        logger.info(f"{i}. {product['code']} - {product['name']}")
    logger.info("8. Custom Product Code")
    logger.info("0. Exit")

    choice = get_menu_choice("Choose your option: ", 0, 8)
    if choice == 0:
        return "", ""

    if choice == 8:
        while True:
            product_code = input("Enter Product Code (4 digits): ").strip()[:4]
            if len(product_code) == 4 and product_code.isdigit():
                break
            logger.warning("Product code must be 4 digits.")
        while True:
            option_code = input("Enter Option Code (3 digits): ").strip()[:3]
            if len(option_code) == 3 and option_code.isdigit():
                break
            logger.warning("Option code must be 3 digits.")
        return product_code, option_code

    product = PRODUCT_TABLE[choice - 1]
    product_code = product["code"]
    options = PRODUCT_OPTIONS.get(product_code, {})
    if not options:
        logger.warning(f"No options defined for {product['name']}.")
        return "", ""

    logger.info(f"--- Options for {product['name']} ---")
    sorted_options = sorted(options.items(), key=lambda x: x[0])
    for i, (code, desc) in enumerate(sorted_options, 1):
        logger.info(f"{i}. {code} - {desc}")
    logger.info("8. Custom Option Code")
    logger.info("0. Exit")

    opt_choice = get_menu_choice("Choose your option: ", 0, 8)
    if opt_choice == 0:
        return "", ""

    if opt_choice == 8:
        while True:
            option_code = input("Enter Option Code (3 digits): ").strip()[:3]
            if len(option_code) == 3 and option_code.isdigit():
                break
            logger.warning("Option code must be 3 digits.")
    else:
        option_code = sorted_options[opt_choice - 1][0]
    return product_code, option_code


def print_option_key(option_key: str) -> None:
    """Log the option key with formatting.

    Args:
        option_key: Key to display.
    """
    output = "Option Key:"
    for i, char in enumerate(option_key):
        if i % 4 == 0:
            output += " "
        output += char
    logger.info(output)


def calculate_nettool_option_key(serial_number: str, option_number: int) -> None:
    """Calculate NetTool option key.

    Args:
        serial_number: Device serial number.
        option_number: Option number to encode.

    Raises:
        ValueError: If serial number is invalid.
    """
    if not serial_number:
        while len(serial_number) != SERIAL_NUMBER_SIZE_ENIGMAC:
            serial_number = input("Enter Serial Number (10 digits): ").strip()[
                :SERIAL_NUMBER_SIZE_ENIGMAC
            ]
            if len(serial_number) == SERIAL_NUMBER_SIZE_ENIGMAC and serial_number.isdigit():
                break
            logger.warning("Serial number must be 10 digits.")

    if len(serial_number) != SERIAL_NUMBER_SIZE_ENIGMAC or not serial_number.isdigit():
        raise ValueError("Serial number must be 10 digits")

    if option_number < 0:
        logger.info("NetTool Options: 0=Inline 1=Reports/Ping 3=Personal 4=VoIP 5=SwitchWizard")
        option_input = input("Enter Option Number (1 digit): ").strip()[:1]
        option_number = int(option_input) if option_input.isdigit() else 0

    if option_number < 0 or option_number > 9:
        option_number = 0

    input_key = serial_number + str(option_number) + "0"
    reversed_key = input_key[::-1]

    logger.info("Encrypting with Enigma 1...")
    output_key = enigma_c_encrypt(reversed_key)
    print_option_key(output_key)


def check_nettool_option_key(option_key: str, serial_number: str = "") -> None:
    """Check NetTool option key validity.

    Args:
        option_key: Key to validate.
        serial_number: Device serial number (optional).

    Raises:
        ValueError: If inputs are invalid.
    """
    if not serial_number:
        while len(serial_number) != SERIAL_NUMBER_SIZE_ENIGMAC:
            serial_number = input("Enter Serial Number (10 digits): ").strip()[
                :SERIAL_NUMBER_SIZE_ENIGMAC
            ]
            if len(serial_number) == SERIAL_NUMBER_SIZE_ENIGMAC and serial_number.isdigit():
                break
            logger.warning("Serial number must be 10 digits.")

    if not option_key:
        while len(option_key) != 12:
            option_key = input("Enter Option Key (12 hex digits): ").strip()[:12]
            if len(option_key) == 12 and all(c in "0123456789abcdefABCDEF" for c in option_key):
                break
            logger.warning("Option key must be 12 hex digits.")

    if len(option_key) != 12 or not all(c in "0123456789abcdefABCDEF" for c in option_key):
        raise ValueError("Option key must be 12 hex digits")

    option_input = input("Enter Option Number (1 digit): ").strip()[:1]
    option_number = int(option_input) if option_input.isdigit() else 0
    if option_number < 0 or option_number > 9:
        option_number = 0

    logger.info("EnigmaC::checkOptionKey()...")
    logger.debug(f"serialNum: {serial_number}")
    logger.debug(f"optionKey: {option_key}")
    logger.debug(f"optionNum: {hex(option_number)}")
    result = enigma_c_check_option_key(option_number, option_key, serial_number)
    logger.info(f"Option {'valid' if result else 'invalid'}")


def calculate_enigma2_option_key(
    serial_number: str, option_number: int, product_code: int, assume_escope: bool
) -> None:
    """Calculate Enigma2 option key.

    Args:
        serial_number: Device serial number.
        option_number: Option number to encode.
        product_code: Product code.
        assume_escope: Assume EtherScope if True.

    Raises:
        ValueError: If serial number is invalid.
    """
    product_code_str = str(product_code).zfill(4) if product_code >= 0 else ""
    option_str = str(option_number).zfill(3) if option_number >= 0 else ""

    if not serial_number:
        while len(serial_number) != SERIAL_NUMBER_SIZE_ENIGMA2:
            serial_number = input("Enter Serial Number (7 digits): ").strip()[
                :SERIAL_NUMBER_SIZE_ENIGMA2
            ]
            if len(serial_number) == SERIAL_NUMBER_SIZE_ENIGMA2 and serial_number.isdigit():
                break
            logger.warning("Serial number must be 7 digits.")

    if len(serial_number) != SERIAL_NUMBER_SIZE_ENIGMA2 or not serial_number.isdigit():
        raise ValueError("Serial number must be 7 digits")

    logger.debug(f"SerialNum= {serial_number}")

    if not product_code_str or not option_str:
        product_code_str, option_str = product_code_menu()
        if not product_code_str:
            logger.info("Operation cancelled.")
            return

    input_key = "00" + product_code_str + serial_number + option_str

    logger.info("Encrypting with Enigma 2...")
    output_key = enigma2_c_encrypt(input_key)
    print_option_key(output_key)


def check_enigma2_option_key(option_key: str) -> None:
    """Decrypt and display Enigma2 option key details.

    Args:
        option_key: Key to validate.

    Raises:
        ValueError: If option key is invalid.
    """
    if not option_key:
        while len(option_key) != 16:
            option_key = input("Enter Option Key (16 characters): ").strip()[:16]
            if len(option_key) == 16 and option_key.isalnum():
                break
            logger.warning("Option key must be 16 alphanumeric characters.")

    if len(option_key) != 16 or not option_key.isalnum():
        raise ValueError("Option key must be 16 alphanumeric characters")

    logger.info("Decrypting with Enigma 2...")
    decrypted_key = enigma2_c_decrypt(option_key)
    if not decrypted_key:
        raise ValueError("Decryption failed: invalid checksum")

    product_code = decrypted_key[PRODUCT_LOCATION : PRODUCT_LOCATION + PRODUCT_CODE_SIZE]
    logger.info(f"Product Code: {product_code} -> ", end="")
    found = False
    for product in PRODUCT_TABLE:
        if product_code == product["code"]:
            logger.info(product["name"])
            found = True
            break
    if not found:
        logger.info("Unknown")
    logger.info(
        f"SerialNumber: {decrypted_key[SERIAL_LOCATION : SERIAL_LOCATION + SERIAL_NUMBER_SIZE_ENIGMA2]}"
    )
    logger.info(
        f"OptionNumber: {decrypted_key[OPTION_LOCATION : OPTION_LOCATION + OPTION_CODE_SIZE]}"
    )


def main_menu() -> bool:
    """Display main menu and handle choices.

    Returns:
        True to continue, False to exit.
    """
    logger.info(f"--- Enigma {SOFTWARE_VERSION} Main Menu ---")
    logger.info("1. Generate NetTool 10/100 Option Key")
    logger.info("2. Check NetTool 10/100 Option Key")
    logger.info("3. Generate Option Key for Other Fluke Products")
    logger.info("4. Decrypt Option Key for Other Fluke Products")
    logger.info("0. Exit")

    choice = get_menu_choice("Choose your option: ", 0, 4)
    if choice == 0:
        return False

    try:
        if choice == 1:
            calculate_nettool_option_key("", -1)
        elif choice == 2:
            check_nettool_option_key("")
        elif choice == 3:
            calculate_enigma2_option_key("", -1, -1, False)
        elif choice == 4:
            check_enigma2_option_key("")
    except Exception as e:
        logger.error(f"Error: {e}")
    return True


def main() -> None:
    """Main function to handle command-line or interactive mode."""
    parser = argparse.ArgumentParser(
        description="Fluke option key calculator for NetTool and other products (functional implementation).",
        epilog="Examples:\n"
        "  python enigma_v300_functions.py -n 0003333016 4 --logfile enigma.log\n"
        "  python enigma_v300_functions.py -e 0000607 7 6963 --verbose",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "-n",
        "--nettool",
        nargs=2,
        metavar=("SERIAL", "OPTION"),
        help="Generate NetTool option key (10-digit serial, option number)",
    )
    parser.add_argument(
        "-x",
        "--check-nettool",
        nargs=2,
        metavar=("KEY", "SERIAL"),
        help="Check NetTool option key (12-hex key, 10-digit serial)",
    )
    parser.add_argument(
        "-e",
        "--encrypt",
        nargs=3,
        metavar=("SERIAL", "OPTION", "PRODUCT"),
        help="Generate option key for other products (7-digit serial, option, product code)",
    )
    parser.add_argument(
        "-d", "--decrypt", metavar="KEY", help="Decrypt option key for other products (16-char key)"
    )
    parser.add_argument(
        "-l",
        "--linkrunner",
        nargs=2,
        metavar=("SERIAL", "OPTION"),
        help="Generate LinkRunner Pro option key (7-digit serial, option number)",
    )
    parser.add_argument("--verbose", action="store_true", help="Enable debug logging")
    parser.add_argument("--logfile", help="Log to file (rotates at 10MB)")

    args = parser.parse_args()
    setup_logging(args.verbose, args.logfile)

    try:
        if args.nettool:
            serial, option = args.nettool
            calculate_nettool_option_key(serial, int(option))
        elif args.check_nettool:
            key, serial = args.check_nettool
            check_nettool_option_key(key, serial)
        elif args.encrypt:
            serial, option, product = args.encrypt
            calculate_enigma2_option_key(serial, int(option), int(product), False)
        elif args.decrypt:
            check_enigma2_option_key(args.decrypt)
        elif args.linkrunner:
            serial, option = args.linkrunner
            calculate_enigma2_option_key(serial, int(option), 7001, False)
        else:
            while main_menu():
                pass
    except KeyboardInterrupt:
        logger.info("Received KeyboardInterrupt, shutting down gracefully...")
        sys.exit(0)
    except Exception as e:
        logger.error(f"Fatal error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
