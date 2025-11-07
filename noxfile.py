"""Automation for enigma-v300."""

from __future__ import annotations

import nox

nox.options.sessions = ["tests"]


@nox.session(python=["3.12"])
def tests(session: nox.Session) -> None:
    session.install(".[test]")
    session.run("pytest", "--cov=enigma_v300", "--cov-report=term-missing")
