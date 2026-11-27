#!/bin/sh
# Build and run the host tests on Linux (x86_64, gcc). See tests/host/README.md.
# Run from the repository root, natively (Linux/WSL) or in a container:
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

# run_test <name> "<sources>" ["<extra defines>"]
run_test() {
    name=$1
    sources=$2
    extra=$3
    gcc $CFLAGS $DEFINES $extra $INCLUDES $sources -o "$out/${name}_plain"
    gcc $CFLAGS $SANITIZE $DEFINES $extra $INCLUDES $sources -o "$out/${name}_sanitized"
    echo "== $name: plain build"
    "$out/${name}_plain"
    echo "== $name: sanitized build (ASan + UBSan)"
    ASAN_OPTIONS=handle_sigfpe=0 "$out/${name}_sanitized"
}

run_test ram_block_services "tests/host/test_ram_block_services.c X2CScope/src/BlockServicesX2C.c"

# Whole library; the known library warnings (pointer/int casts of the 32-bit LNet addresses on a
# 64-bit host, TableStruct.c:72 and Scope_Main.h:58 pointer types, which gcc >= 14 reports as
# errors by default) are not errors here.
LIBRARY="$(ls X2CScope/src/*.c | tr '\n' ' ')"
CFLAGS_LIB_BACKUP=$CFLAGS
CFLAGS="$CFLAGS -Wno-error -w -Wno-error=incompatible-pointer-types"
run_test reset_service_default "tests/host/test_reset_service.c $LIBRARY"
run_test reset_service_app "tests/host/test_reset_service.c $LIBRARY" "-DAPP_RESET"

# Library as a static archive, linked like an application with interface/X2Cscope.c: the weak defaults
# of the optional hooks must be found in the archive.
rm -f "$out"/lib_*.o "$out/libx2cscope_host.a"
for src in $LIBRARY; do
    gcc $CFLAGS $DEFINES $INCLUDES -c "$src" -o "$out/lib_$(basename "$src" .c).o"
done
ar rcs "$out/libx2cscope_host.a" "$out"/lib_*.o
run_test optional_hooks_default "tests/host/test_optional_hooks.c interface/X2Cscope.c $out/libx2cscope_host.a"
run_test optional_hooks_app "tests/host/test_optional_hooks.c interface/X2Cscope.c $out/libx2cscope_host.a" "-DAPP_HOOKS"
CFLAGS=$CFLAGS_LIB_BACKUP
