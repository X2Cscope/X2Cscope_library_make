#!/bin/sh
# Build and run the host tests on Linux (x86_64, gcc).
# Run from the repository root, e.g. in a container:
#   podman run --rm -v "$PWD:/src" -w /src docker.io/library/gcc:14 sh tests/host/run_host_tests.sh
#
# Each test is built twice:
#   plain     - reports every failed check (a division by zero is caught as SIGFPE and reported)
#   sanitized - AddressSanitizer + UndefinedBehaviorSanitizer, aborts on the first memory or UB error
set -e

out=${TMPDIR:-/tmp}/x2cscope_host_tests
mkdir -p "$out"

CFLAGS="-std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -O1 -g"
SANITIZE="-fsanitize=address,undefined -fno-sanitize=integer-divide-by-zero -fno-sanitize-recover=all"
DEFINES="-D__GENERIC_X86__"
INCLUDES="-IX2CScope/inc -Iinterface"
SOURCES="tests/host/test_ram_block_services.c X2CScope/src/BlockServicesX2C.c"

gcc $CFLAGS $DEFINES $INCLUDES $SOURCES -o "$out/test_plain"
gcc $CFLAGS $SANITIZE $DEFINES $INCLUDES $SOURCES -o "$out/test_sanitized"

echo "== plain build"
"$out/test_plain"
echo "== sanitized build (ASan + UBSan)"
ASAN_OPTIONS=handle_sigfpe=0 "$out/test_sanitized"
