/*
 * Copyright (c) [2012-2020] Microchip Technology Inc.
 * All rights reserved.
 *
 * This file is part of X2Cscope firmware implementation.
 *
 * X2CscopeComm.h — TCP/IP (lwIP) example
 *
 * Copy this file and X2CscopeComm.c into your project's X2Cscope folder.
 * Adapt the TCP server include path and X2CSCOPE_TCP_PORT in X2CscopeComm.c.
 *
 * Call X2Cscope_Communicate() from your main/idle loop as normal — the TCP
 * receive callback pushes incoming bytes into the ring buffer transparently.
 */
#ifndef X2CSCOPECOMMUNICATION_H
#define X2CSCOPECOMMUNICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "X2Cscope.h"  /* compilationDate_t */

/**
 * @brief Unified configuration structure for X2Cscope initialization.
 *
 * Use with X2CSCOPE_CONFIG_INIT() and X2Cscope_InitialiseEx() for single-call,
 * compile-time-safe initialization. All fields except flush are required.
 */
typedef struct {
    void    (*sendByte)(uint8_t);               /**< Required: Send one byte              */
    uint8_t (*receiveByte)(void);               /**< Required: Receive one byte           */
    bool    (*isByteAvailable)(void);           /**< Required: Returns true when RX data is waiting */
    bool    (*isSendReady)(void);               /**< Required: Returns true when TX buffer has space */
    void    (*flush)(void);                     /**< Optional: Flush TX buffer (or NULL)  */
    void*    scopeArray;                        /**< Required: Scope data buffer pointer  */
    uint16_t scopeSize;                         /**< Required: Scope buffer size in bytes */
    uint16_t          appVersion;               /**< Application version identifier       */
    const compilationDate_t *compilationDate;   /**< Pointer to build timestamp � must have static storage duration */
} X2Cscope_Config_t;

/**
 * @brief Compile-time-safe initializer macro for X2Cscope_Config_t.
 * All 9 arguments are required. Wrong count or order → compiler error.
 */
#define X2CSCOPE_CONFIG_INIT(send_, recv_, rxAvail_, txReady_, flush_,  \
                             buf_, bufSize_, ver_, compDate_)           \
{                                                                        \
    .sendByte                 = (send_),                                 \
    .receiveByte              = (recv_),                                 \
    .isByteAvailable          = (rxAvail_),                              \
    .isSendReady              = (txReady_),                              \
    .flush                    = (flush_),                                \
    .scopeArray               = (buf_),                                  \
    .scopeSize                = (bufSize_),                              \
    .appVersion               = (ver_),                                  \
    .compilationDate          = &(compDate_)                              \
}

/* Required callbacks, implemented in X2CscopeComm.c */
void    X2CscopeComm_SendByte(uint8_t data);
uint8_t X2CscopeComm_ReceiveByte(void);
bool    X2CscopeComm_IsByteAvailable(void);
bool    X2CscopeComm_IsSendReady(void);

/* Library function implemented in the X2Cscope .a */
void X2Cscope_InitialiseEx(const X2Cscope_Config_t* config);

/* Optional hooks: the X2Cscope library contains a default for each, see interface/X2CscopeComm.h.
 * This example implements X2CscopeComm_Flush() and X2CscopeComm_PostInit() in X2CscopeComm.c. */
void    X2CscopeComm_Flush(void);
void    X2CscopeComm_PostInit(void);
bool    X2CscopeComm_ResetDevice(void);

#ifdef __cplusplus
}
#endif

#endif /* X2CSCOPECOMMUNICATION_H */
