#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 2 ]; then
  echo "usage: $0 <expected-substring> <command> [args...]" >&2
  exit 2
fi

expected="$1"
shift

set +e
output="$("$@" 2>&1)"
status=$?
set -e

if [ "$status" -eq 0 ]; then
  echo "expect_fail.sh: command succeeded but a failure was expected" >&2
  printf '%s\n' "$output"
  exit 1
fi

if [[ "$output" != *"$expected"* ]]; then
  echo "expect_fail.sh: expected output to contain: $expected" >&2
  printf '%s\n' "$output"
  exit 1
fi

printf '%s\n' "$output"
