#!/usr/bin/env python3
"""Tests for Enigma V300 functions."""

import sys
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent / "python"))

from enigma_v300_functions import (
    SOFTWARE_VERSION,
    PRODUCT_TABLE,
    PRODUCT_OPTIONS,
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

    def test_known_products(self):
        """Known products should be in the table."""
        codes = [p["code"] for p in PRODUCT_TABLE]
        assert "3001" in codes  # NetTool Series II
        assert "7001" in codes  # LinkRunner Pro Duo
        assert "6964" in codes  # OneTouch AT


class TestProductOptions:
    """Test product options configuration."""

    def test_product_options_not_empty(self):
        """Product options should have entries."""
        assert len(PRODUCT_OPTIONS) > 0

    def test_onetouch_options(self):
        """OneTouch AT (6964) should have options."""
        assert "6964" in PRODUCT_OPTIONS
        options = PRODUCT_OPTIONS["6964"]
        assert "000" in options  # Registered
        assert "001" in options  # Wired


class TestConstants:
    """Test constant values."""

    def test_size_constants(self):
        """Size constants should be positive integers."""
        from enigma_v300_functions import (
            PRODUCT_CODE_SIZE,
            OPTION_CODE_SIZE,
            SERIAL_NUMBER_SIZE_ENIGMA2,
            SERIAL_NUMBER_SIZE_ENIGMAC,
            CHECK_SUM_SIZE,
        )

        assert PRODUCT_CODE_SIZE == 4
        assert OPTION_CODE_SIZE == 3
        assert SERIAL_NUMBER_SIZE_ENIGMA2 == 7
        assert SERIAL_NUMBER_SIZE_ENIGMAC == 10
        assert CHECK_SUM_SIZE == 2
