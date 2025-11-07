# enigma-v300

One repo for everything related to the Enigma V300 option key algorithms:
- Python CLI/SDK (successor to `enigma_v300_classes` and `enigma_v300_functions`).
- Reference C and C++ implementations under `extras/` (from `enigma_v300_pure_c` and `enigma_v300_pure_cpp`).

```bash
pip install .
enigma-v300 --help
```

## Commands
- `-n / --nettool SERIAL OPTION` generate NetTool keys (10-digit serial, numeric option).
- `-x / --check-nettool KEY SERIAL` validate a NetTool key.
- `-e / --encrypt SERIAL OPTION PRODUCT` create keys for other products (7-digit serial, 3-digit option, product code).
- `-d / --decrypt KEY` decode a key and print the embedded serial/option/product information.

All commands support `--verbose` and `--logfile`.

## Library API
```python
from enigma_v300.core import EnigmaMenu, EnigmaC, Enigma2C

menu = EnigmaMenu()
menu.calculate_nettool_option_key("0003333016", 4)
```

## Development
```bash
python -m venv .venv && source .venv/bin/activate
pip install -e .[test]
python -m pytest
```

Run CI locally via `nox -s tests`. Versions come from git tags via `setuptools_scm`.
