#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
g++ -std=c++17 -Wall -Wextra -Werror -Iinclude src/Protocol.cpp test/test_host.cpp -o /tmp/diyz_fase0_test
/tmp/diyz_fase0_test
echo 'PASS: parser, IDs, estados, escalonador'
