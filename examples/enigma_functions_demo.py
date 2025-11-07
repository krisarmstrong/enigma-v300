#!/usr/bin/env python3
"""
Demonstration script for the Enigma V300 functional API.

This script shows how to use the functional programming interface
for generating and validating option keys.

Author: Kris Armstrong
"""

import sys
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))

from enigma_v300 import functions


def print_section(title: str) -> None:
    """Print a formatted section header."""
    print(f"\n{'=' * 60}")
    print(f"  {title}")
    print('=' * 60)


def demo_nettool_keys() -> None:
    """Demonstrate NetTool key generation and validation."""
    print_section("NetTool (EnigmaC) Examples")

    # Generate a NetTool key
    serial = "0003333016"
    option = 4
    print(f"\nGenerating key for NetTool:")
    print(f"  Serial: {serial}")
    print(f"  Option: {option}")

    key = functions.generate_nettool_option_key(serial, option)
    formatted_key = functions.format_option_key(key)
    print(f"  Generated Key: {formatted_key}")

    # Validate the key
    is_valid = functions.check_nettool_option_key(option, key, serial)
    print(f"  Validation: {'PASS' if is_valid else 'FAIL'}")

    # Test encryption/decryption
    print(f"\n  Testing cipher:")
    test_input = "610333300040"
    encrypted = functions.enigma_c_encrypt(test_input)
    decrypted = functions.enigma_c_decrypt(encrypted)
    print(f"    Original:  {test_input}")
    print(f"    Encrypted: {encrypted}")
    print(f"    Decrypted: {decrypted}")
    print(f"    Match: {'YES' if test_input == decrypted else 'NO'}")


def demo_enigma2_keys() -> None:
    """Demonstrate Enigma2 key generation and validation."""
    print_section("Other Fluke Products (Enigma2C) Examples")

    # Generate an EtherScope key
    serial = "0000607"
    option = 7
    product = 6963
    print(f"\nGenerating key for EtherScope/MetroScope:")
    print(f"  Serial: {serial}")
    print(f"  Option: {option}")
    print(f"  Product: {product}")

    key = functions.generate_enigma2_option_key(serial, option, product)
    formatted_key = functions.format_option_key(key)
    print(f"  Generated Key: {formatted_key}")

    # Validate the key
    is_valid = functions.check_enigma2_option_key(option, key)
    print(f"  Validation: {'PASS' if is_valid else 'FAIL'}")

    # Decode the key
    print(f"\n  Decoding key:")
    prod_code, ser_num, opt_code = functions.decode_enigma2_key(key)
    print(f"    Product Code: {prod_code}")
    print(f"    Serial Number: {ser_num}")
    print(f"    Option Code: {opt_code}")


def demo_onetouch_keys() -> None:
    """Demonstrate OneTouch AT key generation."""
    print_section("OneTouch AT Examples")

    serial = "1234567"
    option = 3
    product = 6964
    print(f"\nGenerating key for OneTouch AT:")
    print(f"  Serial: {serial}")
    print(f"  Option: {option} (Wi-Fi)")
    print(f"  Product: {product}")

    key = functions.generate_enigma2_option_key(serial, option, product)
    formatted_key = functions.format_option_key(key)
    print(f"  Generated Key: {formatted_key}")


def main() -> None:
    """Run all demonstrations."""
    print("\n")
    print("╔════════════════════════════════════════════════════════════╗")
    print("║        Enigma V300 - Functional API Demonstration         ║")
    print("║              Fluke Option Key Calculator                  ║")
    print("╚════════════════════════════════════════════════════════════╝")

    try:
        demo_nettool_keys()
        demo_enigma2_keys()
        demo_onetouch_keys()

        print_section("Summary")
        print("\nAll demonstrations completed successfully!")
        print("\nThe functional API provides pure functions for:")
        print("  - enigma_c_encrypt() / enigma_c_decrypt()")
        print("  - enigma2_c_encrypt() / enigma2_c_decrypt()")
        print("  - generate_nettool_option_key()")
        print("  - check_nettool_option_key()")
        print("  - generate_enigma2_option_key()")
        print("  - check_enigma2_option_key()")
        print("  - decode_enigma2_key()")
        print("  - format_option_key()")

    except Exception as e:
        print(f"\n\nError: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        return 1

    print("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
