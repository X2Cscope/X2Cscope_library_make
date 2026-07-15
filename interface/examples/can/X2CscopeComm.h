/*
 * Copyright (c) [2012-2020] Microchip Technology Inc.
 * All rights reserved.
 *
 * This file is part of X2Cscope firmware implementation.
 *
 * X2CscopeComm.h — CAN example
 *
 * Copy this file and X2CscopeComm.c into your project's X2Cscope folder.
 * Adapt the CAN driver include and CAN IDs in X2CscopeComm.c to your hardware.
 *
 * Important — call X2CscopeComm_Communicate() from your main/idle loop
 * instead of X2Cscope_Communicate() directly. The CAN wrapper drains incoming
 * CAN frames into the receive ring buffer before driving the protocol engine.
 */
#ifndef X2CSCOPECOMMUNICATION_H
#define X2CSCOPECOMMUNICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <xc.h>
#include <stdint.h>
#include "X2Cscope.h"  /* compilationDate_t, X2CSCOPE_BUFFER_SIZE, X2CSCOPE_APP_VERSION */

/**
 * @brief Unified configuration structure for X2Cscope initialization.
 *
 * Use with X2CSCOPE_CONFIG_INIT() and X2Cscope_InitialiseEx() for single-call,
 * compile-time-safe initialization. All fields except flushSerial are required.
 */
typedef struct {
    void    (*sendSerial)(uint8_t);             /**< Required: Send one byte              */
    uint8_t (*receiveSerial)(void);             /**< Required: Receive one byte           */
    uint8_t (*isReceiveDataAvailable)(void);    /**< Required: Non-zero when RX data ready */
    uint8_t (*isSendReady)(void);               /**< Required: Non-zero when TX not full  */
    void    (*flushSerial)(void);               /**< Optional: Flush TX buffer (or NULL)  */
    void*    scopeArray;                        /**< Required: Scope data buffer pointer  */
    uint16_t scopeSize;                         /**< Required: Scope buffer size in bytes */
    uint16_t          appVersion;               /**< Application version identifier       */
    compilationDate_t compilationDate;          /**< Build timestamp (__DATE__, __TIME__) */
} X2Cscope_Config_t;

/**
 * @brief Compile-time-safe initializer macro for X2Cscope_Config_t.
 * All 9 arguments are required. Wrong count or order → compiler error.
 */
#define X2CSCOPE_CONFIG_INIT(send_, recv_, rxAvail_, txReady_, flush_,  \
                             buf_, bufSize_, ver_, compDate_)           \
{                                                                        \
    .sendSerial               = (send_),                                 \
    .receiveSerial            = (recv_),                                 \
    .isReceiveDataAvailable   = (rxAvail_),                              \
    .isSendReady              = (txReady_),                              \
    .flushSerial              = (flush_),                                \
    .scopeArray               = (buf_),                                  \
    .scopeSize                = (bufSize_),                              \
    .appVersion               = (ver_),                                  \
    .compilationDate          = (compDate_)                              \
}

/** Internal 5-parameter hook — do not call directly. */
void X2Cscope_HookUARTFunctions_v5(
    void    (*sendSerialFcnPntr)(uint8_t),
    uint8_t (*receiveSerialFcnPntr)(void),
    uint8_t (*isReceiveDataAvailableFcnPntr)(void),
    uint8_t (*isSendReadyFcnPntr)(void),
    void    (*flushSerialFcnPntr)(void));

/** @cond INTERNAL */
#define _X2CS_HOOK_GET6(_1,_2,_3,_4,_5,_6,...) _6
#define _X2CS_HOOK_NARGS(...) _X2CS_HOOK_GET6(__VA_ARGS__, 5, 4, 3, 2, 1, 0)
#define _X2CS_HOOK4(s,r,a,t)       X2Cscope_HookUARTFunctions_v5(s, r, a, t, NULL)
#define _X2CS_HOOK5(s,r,a,t,f)     X2Cscope_HookUARTFunctions_v5(s, r, a, t, f)
#define _X2CS_HOOK_PICK(n,...)     _X2CS_HOOK##n(__VA_ARGS__)
#define _X2CS_HOOK_DISPATCH(n,...) _X2CS_HOOK_PICK(n, __VA_ARGS__)
/** @endcond */

/** Legacy hook macro — 4 or 5 args, backward compatible. */
#define X2Cscope_HookUARTFunctions(...) \
    _X2CS_HOOK_DISPATCH(_X2CS_HOOK_NARGS(__VA_ARGS__), __VA_ARGS__)

/* Callbacks implemented in X2CscopeComm.c */
void    sendSerial(uint8_t data);
uint8_t receiveSerial(void);
uint8_t isReceiveDataAvailable(void);
uint8_t isSendReady(void);
void    flushSerial(void);

/* Library functions implemented in the X2Cscope .a */
void X2Cscope_InitialiseEx(const X2Cscope_Config_t* config);
void X2Cscope_Initialise(void* scopeArray, uint16_t scopeSize,
    uint16_t appVersion, compilationDate_t compilationDate);

/** User-implemented comm-layer post-init, called from X2Cscope_Init(). */
void X2CscopeComm_PostInit(void);

/**
 * @brief Poll CAN RX FIFO and drive the X2Cscope protocol engine.
 * Call from your main/idle loop instead of X2Cscope_Communicate() directly.
 */
void X2CscopeComm_Communicate(void);

#ifdef __cplusplus
}
#endif

#endif /* X2CSCOPECOMMUNICATION_H */
