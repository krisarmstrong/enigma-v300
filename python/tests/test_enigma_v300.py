#!/usr/bin/env python3
"""Comprehensive tests for Enigma V300 - verified against V200 reference implementation."""

import sys
from pathlib import Path

import pytest

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent / "python"))

from enigma_v300_functions import (
    SOFTWARE_VERSION,
    PRODUCT_TABLE,
    PRODUCT_OPTIONS,
    PRODUCT_CODE_SIZE,
    OPTION_CODE_SIZE,
    SERIAL_NUMBER_SIZE_ENIGMA2,
    SERIAL_NUMBER_SIZE_ENIGMAC,
    CHECK_SUM_SIZE,
    KEY_LENGTH,
    ENIGMA_C_ROTOR,
    ENIGMA2_E_ROTOR_10,
    ENIGMA2_E_ROTOR_26,
    ENIGMA2_D_ROTOR_10,
    ENIGMA2_D_ROTOR_26,
    enigma_c_encrypt,
    enigma_c_decrypt,
    enigma_c_check_option_key,
    enigma2_c_encrypt,
    enigma2_c_decrypt,
    enigma2_c_check_option_key,
    __version__,
)


class TestVersion:
    """Test version information."""

    def test_version_exists(self):
        """Version should be defined."""
        assert __version__ is not None
        assert isinstance(__version__, str)

    def test_software_version(self):
        """Software version should be 3.0.0."""
        assert SOFTWARE_VERSION == "3.0.0"


class TestConstants:
    """Test constant values."""

    def test_size_constants(self):
        """Size constants should match V200 specification."""
        assert PRODUCT_CODE_SIZE == 4
        assert OPTION_CODE_SIZE == 3
        assert SERIAL_NUMBER_SIZE_ENIGMA2 == 7
        assert SERIAL_NUMBER_SIZE_ENIGMAC == 10
        assert CHECK_SUM_SIZE == 2
        assert KEY_LENGTH == 16  # 2 + 4 + 7 + 3

    def test_rotor_sizes(self):
        """Rotor arrays should have correct sizes."""
        assert len(ENIGMA_C_ROTOR) == 16
        assert len(ENIGMA2_E_ROTOR_10) == 10
        assert len(ENIGMA2_E_ROTOR_26) == 26
        assert len(ENIGMA2_D_ROTOR_10) == 10
        assert len(ENIGMA2_D_ROTOR_26) == 26

    def test_rotor_values_are_permutations(self):
        """Rotor arrays should be valid permutations."""
        assert sorted(ENIGMA_C_ROTOR) == list(range(16))
        assert sorted(ENIGMA2_E_ROTOR_10) == list(range(10))
        assert sorted(ENIGMA2_E_ROTOR_26) == list(range(26))
        assert sorted(ENIGMA2_D_ROTOR_10) == list(range(10))
        assert sorted(ENIGMA2_D_ROTOR_26) == list(range(26))


class TestProductTable:
    """Test product table configuration."""

    def test_product_table_not_empty(self):
        """Product table should have entries."""
        assert len(PRODUCT_TABLE) > 0

    def test_product_table_structure(self):
        """Each product should have code, abbr, and name."""
        for product in PRODUCT_TABLE:
            assert "code" in product
            assert "abbr" in product
            assert "name" in product
            assert len(product["code"]) == 4
            assert product["code"].isdigit()

    def test_known_products(self):
        """Known products from V200 should be in the table."""
        codes = [p["code"] for p in PRODUCT_TABLE]
        assert "3001" in codes  # NetTool Series II
        assert "7001" in codes  # LinkRunner Pro Duo
        assert "6963" in codes  # EtherScope/MetroScope
        assert "6964" in codes  # OneTouch AT


class TestProductOptions:
    """Test product options configuration."""

    def test_product_options_not_empty(self):
        """Product options should have entries."""
        assert len(PRODUCT_OPTIONS) > 0

    def test_all_products_have_options(self):
        """All products should have option definitions."""
        for product in PRODUCT_TABLE:
            code = product["code"]
            assert code in PRODUCT_OPTIONS, f"Missing options for {code}"

    def test_option_codes_are_valid(self):
        """Option codes should be 3-digit strings."""
        for product_code, options in PRODUCT_OPTIONS.items():
            for option_code in options.keys():
                assert len(option_code) == 3, f"Invalid option code {option_code}"
                assert option_code.isdigit(), f"Option code {option_code} not numeric"


class TestEnigmaCEncryption:
    """Test EnigmaC cipher (original NetTool)."""

    def test_encrypt_basic(self):
        """Basic encryption should work."""
        result = enigma_c_encrypt("0123456789ab")
        assert len(result) == 12
        assert all(c in "0123456789abcdef" for c in result)

    def test_encrypt_decrypt_roundtrip(self):
        """Encrypt then decrypt should return original."""
        original = "0003333016"
        # Reverse and pad for NetTool format
        input_key = (original + "40")[::-1]
        encrypted = enigma_c_encrypt(input_key)
        decrypted = enigma_c_decrypt(encrypted)
        assert decrypted == input_key

    def test_encrypt_deterministic(self):
        """Same input should always produce same output."""
        result1 = enigma_c_encrypt("abcdef123456")
        result2 = enigma_c_encrypt("abcdef123456")
        assert result1 == result2

    def test_encrypt_invalid_hex(self):
        """Non-hex characters should raise ValueError."""
        with pytest.raises(ValueError):
            enigma_c_encrypt("ghijk")

    def test_decrypt_invalid_hex(self):
        """Non-hex characters should raise ValueError."""
        with pytest.raises(ValueError):
            enigma_c_decrypt("xyz123")


class TestEnigmaCCheckOptionKey:
    """Test EnigmaC option key validation."""

    def test_empty_key_raises(self):
        """Empty key should raise ValueError."""
        with pytest.raises(ValueError):
            enigma_c_check_option_key(4, "", "0003333016")

    def test_master_override(self):
        """Master override key should always validate."""
        assert enigma_c_check_option_key(0, "bladerules", "0003333016")
        assert enigma_c_check_option_key(4, "bladerules", "9999999999")


class TestEnigma2CEncryption:
    """Test Enigma2C cipher - verified against V200 reference."""

    # Reference test vectors from enigma V200
    TEST_VECTORS = [
        # (serial, option, product, expected_key)
        ("1234567", 7, "6963", "9225940719507747"),  # EtherScope opt 7
        ("1234567", 2, "7001", "8944937150971162"),  # LinkRunner Pro opt 2
        ("1234567", 4, "3001", "8042745901759933"),  # NetTool S2 opt 4
        ("0000607", 7, "6963", "6406257948597747"),  # EtherScope reference
    ]

    @pytest.mark.parametrize("serial,option,product,expected", TEST_VECTORS)
    def test_encrypt_matches_v200(self, serial, option, product, expected):
        """Encryption output should match V200 reference."""
        input_key = f"00{product}{serial}{option:03d}"
        result = enigma2_c_encrypt(input_key)
        assert result == expected, f"Mismatch for {product}/{serial}/{option}"

    def test_encrypt_deterministic(self):
        """Same input should always produce same output."""
        input_key = "0069631234567007"
        result1 = enigma2_c_encrypt(input_key)
        result2 = enigma2_c_encrypt(input_key)
        assert result1 == result2

    def test_encrypt_wrong_length_raises(self):
        """Wrong length input should raise ValueError."""
        with pytest.raises(ValueError):
            enigma2_c_encrypt("short")
        with pytest.raises(ValueError):
            enigma2_c_encrypt("this_is_way_too_long_for_the_key")


class TestEnigma2CDecryption:
    """Test Enigma2C decryption - verified against V200 reference."""

    DECRYPT_VECTORS = [
        # (encrypted_key, expected_product, expected_serial, expected_option)
        ("9225940719507747", "6963", "1234567", "007"),
        ("8944937150971162", "7001", "1234567", "002"),
        ("8042745901759933", "3001", "1234567", "004"),
    ]

    @pytest.mark.parametrize("encrypted,product,serial,option", DECRYPT_VECTORS)
    def test_decrypt_matches_v200(self, encrypted, product, serial, option):
        """Decryption should extract correct components."""
        result = enigma2_c_decrypt(encrypted)
        assert result != "", "Decryption should succeed"
        # Check product code (positions 2-5)
        assert result[2:6] == product
        # Check serial (positions 6-12)
        assert result[6:13] == serial
        # Check option (positions 13-15)
        assert result[13:16] == option

    def test_decrypt_invalid_checksum(self):
        """Invalid checksum should return empty string."""
        result = enigma2_c_decrypt("0000000000000000")
        assert result == "", "Invalid checksum should fail"

    def test_decrypt_wrong_length_raises(self):
        """Wrong length input should raise ValueError."""
        with pytest.raises(ValueError):
            enigma2_c_decrypt("short")

    def test_encrypt_decrypt_roundtrip(self):
        """Encrypt then decrypt should preserve data."""
        for serial, option, product, _ in TestEnigma2CEncryption.TEST_VECTORS:
            input_key = f"00{product}{serial}{option:03d}"
            encrypted = enigma2_c_encrypt(input_key)
            decrypted = enigma2_c_decrypt(encrypted)
            # The first two chars are recalculated checksum
            assert decrypted[2:] == input_key[2:]


class TestEnigma2CCheckOptionKey:
    """Test Enigma2C option key validation."""

    def test_empty_key_raises(self):
        """Empty key should raise ValueError."""
        with pytest.raises(ValueError):
            enigma2_c_check_option_key(7, "")

    def test_valid_key_validates(self):
        """Valid key should pass validation."""
        # EtherScope option 7
        assert enigma2_c_check_option_key(7, "9225940719507747")

    def test_wrong_option_fails(self):
        """Key for different option should fail."""
        # This key is for option 7, checking with option 2
        assert not enigma2_c_check_option_key(2, "9225940719507747")


class TestAllProducts:
    """Test key generation for all known products."""

    def test_generate_for_all_products(self):
        """Should generate valid keys for all products."""
        test_serial = "1234567"
        for product in PRODUCT_TABLE:
            product_code = product["code"]
            options = PRODUCT_OPTIONS.get(product_code, {})
            if options:
                # Test first available option
                first_option = sorted(options.keys())[0]
                input_key = f"00{product_code}{test_serial}{first_option}"
                try:
                    encrypted = enigma2_c_encrypt(input_key)
                    assert len(encrypted) == 16
                    # Verify roundtrip
                    decrypted = enigma2_c_decrypt(encrypted)
                    assert decrypted != ""
                except Exception as e:
                    pytest.fail(f"Failed for {product['name']}: {e}")


class TestEdgeCases:
    """Test edge cases and boundary conditions."""

    def test_all_zeros_serial(self):
        """All zeros serial should work."""
        input_key = "0069630000000007"
        encrypted = enigma2_c_encrypt(input_key)
        decrypted = enigma2_c_decrypt(encrypted)
        assert decrypted[6:13] == "0000000"

    def test_all_nines_serial(self):
        """All nines serial should work."""
        input_key = "0069639999999007"
        encrypted = enigma2_c_encrypt(input_key)
        decrypted = enigma2_c_decrypt(encrypted)
        assert decrypted[6:13] == "9999999"

    def test_option_000(self):
        """Option 000 should work."""
        input_key = "0069631234567000"
        encrypted = enigma2_c_encrypt(input_key)
        decrypted = enigma2_c_decrypt(encrypted)
        assert decrypted[13:16] == "000"

    def test_option_999(self):
        """Maximum option 999 should work."""
        input_key = "0069631234567999"
        encrypted = enigma2_c_encrypt(input_key)
        decrypted = enigma2_c_decrypt(encrypted)
        assert decrypted[13:16] == "999"
