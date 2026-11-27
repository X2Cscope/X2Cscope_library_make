# Host tests

Tests that run the library code (`X2CScope/src`) on a PC instead of a microcontroller. They are built for the `__GENERIC_X86__` target with gcc on Linux x86_64. No XC compiler and no hardware are needed.

| Test | Covers |
|---|---|
| `test_ram_block_services.c` | LNet Get/Put RAM Block (`BlockServicesX2C.c`): valid requests, malformed requests, no write past the LNet frame buffer, no write to target memory for rejected requests |
| `test_reset_service.c` | Device reset (LNet Reboot service) through the whole library (`X2Cscope_InitialiseEx()` + `X2Cscope_Communicate()` with an in-memory transport). Built without an application `X2CscopeComm_ResetDevice()` (library default answers 0x21) and with one (cannot reset → 0x22; accepted → no response, the next request is still answered); request with service data → 0x14 |

Every test is built twice: plain (reports every failed check) and with AddressSanitizer and UndefinedBehaviorSanitizer (aborts on the first memory or undefined-behaviour error).

## On GitHub

The workflow `.github/workflows/host-tests.yml` runs the tests on every pull request and on every push to `master` and `develop`. It can also be started manually (*Actions → Host tests → Run workflow*). Nothing has to be installed.

## Locally

Run from the repository root. Any of these environments works.

### Linux or WSL (Ubuntu)

```sh
sudo apt install gcc          # once
sh tests/host/run_host_tests.sh
```

On Windows, WSL with Ubuntu is set up with `wsl --install -d Ubuntu`. Run the commands inside the Ubuntu shell; the repository is available below `/mnt/c/...`.

### Windows with Podman

Install Podman once (see the official guide: <https://podman.io/docs/installation#windows>):

```powershell
winget install RedHat.Podman    # or Podman Desktop: winget install RedHat.Podman-Desktop
podman machine init             # once: creates the Linux VM (uses WSL 2)
```

Then, in PowerShell from the repository root:

```powershell
podman machine start            # after every Windows restart
podman run --rm -v "${PWD}:/src" -w /src docker.io/library/gcc:14 sh tests/host/run_host_tests.sh
```

The first run downloads the gcc image (about 500 MB).

### Windows with Docker Desktop

Same as Podman, with `docker` instead of `podman` (no `machine` commands):

```powershell
docker run --rm -v "${PWD}:/src" -w /src gcc:14 sh tests/host/run_host_tests.sh
```

### Expected output

```
== plain build
PASSED: 0 of 99 checks failed
== sanitized build (ASan + UBSan)
PASSED: 0 of 99 checks failed
```

The script exits with a non-zero code if a check fails or a sanitizer reports an error.

## Troubleshooting

| Message | Cause | Fix |
|---|---|---|
| `sh: 1: set: Illegal option -` or `$'\r': command not found` | The script has Windows line endings | `.gitattributes` keeps `*.sh` at LF; re-checkout the file: `git checkout -- tests/host/run_host_tests.sh` |
| `Cannot connect to Podman` | The Podman VM is not running | `podman machine start` |
| `AddressSanitizer:DEADLYSIGNAL` repeated at start (Linux) | High ASLR entropy of recent kernels with older gcc | `sudo sysctl vm.mmap_rnd_bits=28` |
| `mmap: Cannot allocate memory` | `MAP_32BIT` (memory below 4 GB) not available | Run on Linux x86_64; ARM hosts (e.g. Apple Silicon) are not supported |

## Adding a test

1. Add `tests/host/test_<name>.c` with a `main()` that returns 0 on success.
2. Add it to `run_host_tests.sh`, with the library sources it needs.
3. Keep tests independent of a real transport: stub `sendError()` or other functions from files that are not linked, as `test_ram_block_services.c` does.
