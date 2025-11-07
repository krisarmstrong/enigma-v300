"""Command line interface for the Enigma V300 option key tools."""

from __future__ import annotations

import argparse
import sys

from .core import EnigmaMenu, __version__, setup_logging

CLI_DESCRIPTION = "Fluke option key calculator for NetTool and other products."
CLI_EPILOG = (
    "Examples:\n"
    "  enigma-v300 -n 0003333016 4 --logfile enigma.log\n"
    "  enigma-v300 -e 0000607 7 6963 --verbose"
)


def build_parser() -> argparse.ArgumentParser:
    """Construct the CLI argument parser."""
    parser = argparse.ArgumentParser(
        description=CLI_DESCRIPTION,
        epilog=CLI_EPILOG,
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
        "-d",
        "--decrypt",
        metavar="KEY",
        help="Decrypt option key for other products (16-char key)",
    )
    parser.add_argument("--verbose", action="store_true", help="Enable debug logging")
    parser.add_argument("--logfile", help="Log to file (rotates at 10MB)")
    parser.add_argument(
        "--version",
        action="version",
        version=f"enigma-v300 {__version__}",
    )
    return parser


def main(argv: list[str] | None = None) -> None:
    """Entry point for the CLI."""
    parser = build_parser()
    args = parser.parse_args(argv)
    setup_logging(args.verbose, args.logfile)

    menu = EnigmaMenu()
    try:
        if args.nettool:
            serial, option = args.nettool
            menu.calculate_nettool_option_key(serial, int(option))
        elif args.check_nettool:
            key, serial = args.check_nettool
            menu.check_nettool_option_key(key, serial)
        elif args.encrypt:
            serial, option, product = args.encrypt
            menu.calculate_enigma2_option_key(serial, int(option), int(product), False)
        elif args.decrypt:
            menu.check_enigma2_option_key(args.decrypt)
        else:
            while menu.main_menu():
                pass
    except KeyboardInterrupt:
        print("Received KeyboardInterrupt, shutting down gracefully...", file=sys.stderr)
        sys.exit(0)
    except Exception as exc:  # pragma: no cover - fatal error path
        print(f"Fatal error: {exc}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
