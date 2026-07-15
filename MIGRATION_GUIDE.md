# X2Cscope API Migration Guide

## Overview

This guide explains the X2Cscope initialization API and how to migrate existing code.

## What Changed?

### New Features
- **Unified Configuration Structure**: `X2Cscope_Config_t` combines communication interface and initialization parameters into a single object
- **Single-Call Init**: `X2Cscope_InitialiseEx()` replaces the previous two-step hook + init flow
- **Compile-Time Safety**: `X2CSCOPE_CONFIG_INIT()` macro ensures all required fields are provided
- **Optional Flush**: `flushSerial()` callback (can be NULL)
- **Post-Init Hook**: `X2CscopeComm_PostInit()` for protocol-specific setup

### Backward Compatibility
- **Legacy two-step API still works**: `X2Cscope_HookUARTFunctions()` + `X2Cscope_Initialise()`
- **Zero breaking changes** for existing code

## New API (Recommended)

### Using X2Cscope_Config_t with X2CSCOPE_CONFIG_INIT

```c
void X2Cscope_Init(void)
{
    X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
        sendSerial,                  // sendSerial
        receiveSerial,               // receiveSerial
        isReceiveDataAvailable,      // isReceiveDataAvailable
        isSendReady,                 // isSendReady
        flushSerial,                 // flushSerial (NULL if not needed)
        (void*)X2CscopeArray,        // scopeArray
        X2CSCOPE_BUFFER_SIZE,        // scopeSize
        X2CSCOPE_APP_VERSION,        // appVersion
        compilationDate              // compilationDate
    );
    X2Cscope_InitialiseEx(&config);
    X2CscopeComm_PostInit();
}
```

### Benefits
- **Single call**: No more separate hook + init steps
- **Compile-time safety**: Missing a macro argument causes a compiler error
- **Future-proof**: New fields can be added without breaking existing code
- **Self-documenting**: All configuration visible in one place
- **Zero overhead**: Macro expands to a plain struct initializer

## Legacy API (Still Supported)

### Two-Step Interface

```c
void X2Cscope_Init(void)
{
    X2Cscope_HookUARTFunctions(sendSerial, receiveSerial,
                               isReceiveDataAvailable, isSendReady);

    X2Cscope_Initialise((void*)X2CscopeArray, X2CSCOPE_BUFFER_SIZE,
                        X2CSCOPE_APP_VERSION, compilationDate);
}
```

**Note**: legacy `X2Cscope_HookUARTFunctions()` does not support `flushSerial` (sets it to NULL).

## API Reference

### X2Cscope_Config_t Structure

```c
typedef struct {
    /* Communication interface */
    void (*sendSerial)(uint8_t);              // Required: Send byte
    uint8_t (*receiveSerial)(void);           // Required: Receive byte
    uint8_t (*isReceiveDataAvailable)(void);  // Required: Check RX data available
    uint8_t (*isSendReady)(void);             // Required: Check TX ready
    void (*flushSerial)(void);                // Optional: Flush TX buffer (NULL if not used)
    /* Scope buffer */
    void* scopeArray;                         // Required: Pointer to scope data buffer
    uint16_t scopeSize;                       // Required: Size of scope buffer in bytes
    /* Application info */
    uint16_t appVersion;                      // Application version identifier
    compilationDate_t compilationDate;        // Build date/time stamp
} X2Cscope_Config_t;
```

### X2CSCOPE_CONFIG_INIT Macro

```c
X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
    send, recv, avail, ready, flush, buf, bufSize, ver, compDate
);
```

All 9 arguments are required. Omitting any argument causes a **compiler error**.
The macro expands to a designated initializer list with zero runtime overhead.

### Functions

#### X2Cscope_InitialiseEx (Recommended)
```c
void X2Cscope_InitialiseEx(const X2Cscope_Config_t* config);
```
- Single-call initialization: hooks communication and initializes scope
- Use with `X2CSCOPE_CONFIG_INIT()` for compile-time safety

#### X2Cscope_HookUARTFunctions (Legacy)
```c
void X2Cscope_HookUARTFunctions(
    void (*sendSerialFcnPntr)(uint8_t),
    uint8_t (*receiveSerialFcnPntr)(),
    uint8_t (*isReceiveDataAvailableFcnPntr)(),
    uint8_t (*isSendReadyFcnPntr)());
```
- Maintained for backward compatibility
- Sets `flushSerial` to NULL
- Must be followed by `X2Cscope_Initialise()`

#### X2Cscope_Initialise (Legacy)
```c
void X2Cscope_Initialise(void* scopeArray, uint16_t scopeSize,
    uint16_t appVersion, compilationDate_t compilationDate);
```
- Maintained for backward compatibility
- Must be preceded by `X2Cscope_HookUARTFunctions()`

#### X2CscopeComm_PostInit
```c
void X2CscopeComm_PostInit(void);
```
- User-implemented, called from `X2Cscope_Init()` after `X2Cscope_InitialiseEx()`
- Implement in `X2CscopeComm.c`
- Use for starting a TCP server, enabling UART TX interrupts, DMA setup, etc.
- Leave empty if no post-init steps are needed

#### X2Cscope_Communicate
```c
void X2Cscope_Communicate(void);
```
- Call in idle/main loop

#### X2Cscope_Update
```c
void X2Cscope_Update(void);
```
- Call with fixed period (e.g., in timer ISR or fixed-rate task)

## Migration Recommendations

### For New Projects
Use `X2Cscope_InitialiseEx()` with `X2CSCOPE_CONFIG_INIT()`.

### For Existing Projects
No action required - legacy API continues to work unchanged.
Optional: migrate to new API for single-call init and `flushSerial` support.

## Common Patterns

### No Flush Required
```c
X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
    sendSerial, receiveSerial, isReceiveDataAvailable, isSendReady,
    NULL,                            // No flush
    (void*)X2CscopeArray, X2CSCOPE_BUFFER_SIZE,
    X2CSCOPE_APP_VERSION, compilationDate
);
```

### With Flush
```c
X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
    sendSerial, receiveSerial, isReceiveDataAvailable, isSendReady,
    flushSerial,                     // Flush enabled
    (void*)X2CscopeArray, X2CSCOPE_BUFFER_SIZE,
    X2CSCOPE_APP_VERSION, compilationDate
);
```

## Support

For questions or issues:
- Review example in `interface/X2Cscope.c`
- Examine template in `interface/X2CscopeComm.c`
