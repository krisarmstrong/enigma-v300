# Code Review

**Primary languages:** C, C++, C/C++, JSON, Python, Shell, TypeScript, YAML
**Automated tests present:** ✅
**CI workflows present:** ✅

## Findings
1. Python sources are present without a pyproject.toml/PEP 621 definition; convert to a modern, typed package and document dependencies.

_Python-specific_: Target CPython 3.14 compatibility (type hints, stdlib changes, WASI builds). Enforce PEP8/PEP257, prefer Ruff/Black, and keep type hints complete for production readiness.
_C/C++ guidance_: Adopt modern C23/C++23 toolchains (enable -std=c2x / -std=c++23 plus sanitizers).