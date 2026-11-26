# X2Cscope Migration Guide

## Overview

This guide covers what changed in the X2Cscope initialization API, what you
need to update in existing code, and how to use the current API.

---

## Breaking Changes

### 1. Legacy init functions removed

`X2Cscope_HookUARTFunctions()` and `X2Cscope_Initialise()` no longer exist.
Replace them with a single call to `X2Cscope_InitialiseEx()` as shown below.

### 2. `compilationDate_t` typedef no longer carries `const`

The old typedef was:
```c
typedef const struct compilationDate_type { ... } compilationDate_t;
```

It is now:
```c
typedef struct compilationDate_type { ... } compilationDate_t;
```

Declare your variable with `const` explicitly:
```c
const compilationDate_t compilationDate = {__DATE__, __TIME__};
```

### 3. `compilationDate_t` is passed by pointer, not by value

`X2Cscope_Config_t` holds a `const compilationDate_t *`. The variable must
have **static storage duration** (file-scope global or `static` local) because
the library stores the pointer internally. Passing a stack-allocated variable
would result in a dangling pointer.

### 4. `compilationDate_t` fields changed from `uint8_t` to `char`

The old struct used `uint8_t[11]` / `uint8_t[8]`. These are now `char[12]` /
`char[9]` to match the type and null-terminated length of `__DATE__` and
`__TIME__`.

### 5. "Byte available" and "send ready" callbacks return `bool`

The return type of these two callbacks changed from `uint8_t` to `bool`
(names as of this change; see section 7 for the current names).

```c
/* Before */
uint8_t isReceiveDataAvailable(void) { return UART1_IsRxReady(); }
uint8_t isSendReady(void)            { return UART1_IsTxReady(); }

/* After */
bool isReceiveDataAvailable(void) { return UART1_IsRxReady(); }
bool isSendReady(void)            { return UART1_IsTxReady(); }
```

### 6. `X2CSCOPE_BUFFER_SIZE` and `X2CSCOPE_APP_VERSION` moved to `X2Cscope.c`

`X2Cscope.h` now contains only the public API (`compilationDate_t` and the
function prototypes). The two configuration defaults are defined in
`interface/X2Cscope.c`, the only file that uses them.

- If you override them on the compiler command line
  (e.g. `-DX2CSCOPE_BUFFER_SIZE=8000`), nothing changes.
- If you changed the values directly in `X2Cscope.h`, apply the same change in
  `X2Cscope.c`, or the defaults (5000 bytes, `0x0001`) are used.
- If your own code referenced either macro through `X2Cscope.h`, define it
  where you use it.

### 7. Communication functions and config fields renamed

The functions you implement in `X2CscopeComm.c` now use the `X2CscopeComm_`
prefix, like `X2CscopeComm_PostInit()`. The old names (`sendSerial`,
`receiveSerial`, ...) were global symbols that could clash with functions of
the same name elsewhere in the application. "Serial" is also dropped, because
the same functions are used for CAN and TCP/IP.

| Old function | New function |
|---|---|
| `sendSerial(uint8_t)` | `X2CscopeComm_SendByte(uint8_t)` |
| `receiveSerial(void)` | `X2CscopeComm_ReceiveByte(void)` |
| `isReceiveDataAvailable(void)` | `X2CscopeComm_IsByteAvailable(void)` |
| `isSendReady(void)` | `X2CscopeComm_IsSendReady(void)` |
| `flushSerial(void)` | `X2CscopeComm_Flush(void)` |

The matching `X2Cscope_Config_t` fields are renamed for consistency:

| Old field | New field |
|---|---|
| `sendSerial` | `sendByte` |
| `receiveSerial` | `receiveByte` |
| `isReceiveDataAvailable` | `isByteAvailable` |
| `isSendReady` | `isSendReady` (unchanged) |
| `flushSerial` | `flush` |

What to update:
- Rename the five functions in your `X2CscopeComm.c` and in the arguments of
  `X2CSCOPE_CONFIG_INIT()` in `X2Cscope.c`, or copy the new `interface/` files.
- Use the new `interface/X2CscopeComm.h` with the new library; the field order
  is unchanged, so `X2CSCOPE_CONFIG_INIT()` keeps the same argument order.
- If you fill `X2Cscope_Config_t` by field name instead of with the macro,
  use the new field names.

---

## Current API

### Initialization

Declare the scope buffer and the build timestamp at file scope, then fill a
config struct and call `X2Cscope_InitialiseEx()`:

```c
/* Configuration defaults; override with -D on the compiler command line. */
#ifndef X2CSCOPE_BUFFER_SIZE
#define X2CSCOPE_BUFFER_SIZE 5000   /* scope buffer size in bytes (RAM) */
#endif
#ifndef X2CSCOPE_APP_VERSION
#define X2CSCOPE_APP_VERSION 0x0001 /* app version; 0x1000 - 0x3FFF are reserved */
#endif

/* Scope data buffer. */
int8_t X2CscopeArray[X2CSCOPE_BUFFER_SIZE];

/* Build timestamp — must be global or static; the library stores a pointer to it. */
const compilationDate_t compilationDate = {__DATE__, __TIME__};

void X2Cscope_Init(void)
{
    X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
        X2CscopeComm_SendByte,          /* send one byte          (required) */
        X2CscopeComm_ReceiveByte,       /* receive one byte       (required) */
        X2CscopeComm_IsByteAvailable,   /* RX data ready flag     (required) */
        X2CscopeComm_IsSendReady,       /* TX buffer not full     (required) */
        X2CscopeComm_Flush,             /* flush TX buffer        (NULL if unused) */
        (void*)X2CscopeArray,           /* scope data buffer      (required) */
        X2CSCOPE_BUFFER_SIZE,           /* scope buffer size      (required) */
        X2CSCOPE_APP_VERSION,           /* app version identifier (required) */
        compilationDate                 /* build timestamp        (required) */
    );
    X2Cscope_InitialiseEx(&config);
    X2CscopeComm_PostInit();
}
```

Pass `NULL` for `X2CscopeComm_Flush` if your peripheral does not need an explicit
flush step (e.g. a direct byte-at-a-time UART).

### Runtime calls

```c
/* Call from main/idle loop: */
X2Cscope_Communicate();

/* Call at a fixed rate (timer ISR or fixed-rate task): */
X2Cscope_Update();
```

---

## API Reference

### `compilationDate_t`

```c
typedef struct compilationDate_type {
    char date[12];  /* __DATE__: "Mon DD YYYY" + null terminator */
    char time[9];   /* __TIME__: "HH:MM:SS"   + null terminator */
} compilationDate_t;
```

### `X2Cscope_Config_t`

```c
typedef struct {
    void    (*sendByte)(uint8_t);             /* Required */
    uint8_t (*receiveByte)(void);             /* Required */
    bool    (*isByteAvailable)(void);         /* Required */
    bool    (*isSendReady)(void);             /* Required */
    void    (*flush)(void);                   /* Optional — NULL if unused */
    void*    scopeArray;                      /* Required */
    uint16_t scopeSize;                       /* Required */
    uint16_t appVersion;                      /* Required */
    const compilationDate_t *compilationDate; /* Required — must have static storage duration */
} X2Cscope_Config_t;
```

### `X2CSCOPE_CONFIG_INIT` macro

Positional initializer macro for `X2Cscope_Config_t`. All 9 arguments are
required. Omitting or reordering any argument causes a compiler error.

```c
X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
    send, recv, rxAvail, txReady, flush, buf, bufSize, ver, compDate
);
```

### `X2Cscope_InitialiseEx`

```c
void X2Cscope_InitialiseEx(const X2Cscope_Config_t *config);
```

Validates required fields (debug builds), hooks the communication interface,
and initializes the scope buffer. Call once from `X2Cscope_Init()`.

### `X2CscopeComm_PostInit`

```c
void X2CscopeComm_PostInit(void);
```

User-implemented, called from `X2Cscope_Init()` after `X2Cscope_InitialiseEx()`.
Implement in `X2CscopeComm.c` for any steps that must follow library init
(enabling UART TX interrupts, starting a TCP server, starting DMA, etc.).
Leave the body empty if no post-init steps are needed.

### `X2CscopeComm_Reboot` (optional)

```c
bool X2CscopeComm_Reboot(void);
```

Implement in `X2CscopeComm.c` if the host (e.g. pyX2Cscope) shall be able to
reboot the device. Called from `X2Cscope_Communicate()`. Bring the application
into a safe state (e.g. disable the power stage), then reset the device.

| Return | Meaning |
|---|---|
| `true` | The device is reset (now or shortly afterwards). |
| `false` | The reboot is not allowed now (e.g. motor running). |

Optional: existing projects without this function keep working unchanged.
`interface/X2CscopeComm.c` contains an example with the reset for dsPIC/PIC24,
PIC32M and Cortex-M devices.

### `X2Cscope_Communicate`

```c
void X2Cscope_Communicate(void);
```

Processes the LNet protocol. Call from the main/idle loop.

### `X2Cscope_Update`

```c
void X2Cscope_Update(void);
```

Advances the scope sampler. Call at a fixed rate from a timer ISR or
fixed-rate task.

---

## Communication Layer Examples

Ready-to-use `X2CscopeComm.h` and `X2CscopeComm.c` implementations are
provided under `interface/examples/` for three transport interfaces:

| Folder | Interface |
|---|---|
| `interface/examples/uart/` | UART — MCC Melody `uart_drv_interface_t` |
| `interface/examples/can/` | CAN 2.0 — MCC Melody `can1` driver, ring-buffered RX, 8-byte frame packing |
| `interface/examples/tcpip/` | TCP/IP — lwIP over 10BASE-T1S; includes `tcp_server` and `dmaspi` driver files |

Each example folder contains a `README.md` with MCC configuration steps,
hardware requirements, integration checklist, and pyX2Cscope connection
instructions.
