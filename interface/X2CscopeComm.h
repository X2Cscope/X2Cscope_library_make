/*
Copyright (c) [2012-2020] Microchip Technology Inc.  
    All rights reserved.
    You are permitted to use the accompanying software and its derivatives 
    with Microchip products. See the Microchip license agreement accompanying 
    this software, if any, for additional info regarding your rights and 
    obligations.
    
    MICROCHIP SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
    LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT 
    AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP OR ITS
    LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT 
    LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE 
    THEORY FOR ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT 
    LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
    OR OTHER SIMILAR COSTS. 
*/
/*
 * This file is licensed according to the BSD 3-clause license as follows:
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the "Linz Center of Mechatronics GmbH" and "LCM" nor
 *       the names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL "Linz Center of Mechatronics GmbH" BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * This file is part of X2Cscope firmware implementation.
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
 */
/**
 * This file collects the interfaces for X2Cscope_Init() function implementation. 
 * These function should not be accessed externally by the application.
 */
#ifndef X2CSCOPECOMMUNICATION_H
#define	X2CSCOPECOMMUNICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "X2Cscope.h"  /* compilationDate_t */

/**
 * @brief Unified configuration structure for X2Cscope initialization.
 *
 * Use with X2CSCOPE_CONFIG_INIT() macro and X2Cscope_InitialiseEx() for
 * single-call, compile-time-safe initialization.
 *
 * All function pointer fields except flush are required (must not be NULL).
 * flush is optional and may be set to NULL if not needed. X2Cscope_Init()
 * (X2Cscope.c) passes X2CscopeComm_Flush, which has a default (see below).
 */
typedef struct {
    /* Communication interface — all required except flush */
    void    (*sendByte)(uint8_t);               /**< Required: Send one byte over serial */
    uint8_t (*receiveByte)(void);               /**< Required: Receive one byte from serial */
    bool    (*isByteAvailable)(void);           /**< Required: Returns true when RX data is waiting */
    bool    (*isSendReady)(void);               /**< Required: Returns true when TX buffer has space */
    void    (*flush)(void);                     /**< Optional: Flush/commit TX buffer (set NULL if unused) */
    /* Scope buffer */
    void*    scopeArray;                        /**< Required: Pointer to scope data buffer allocated by the application */
    uint16_t scopeSize;                         /**< Required: Size of scopeArray in bytes (see X2CSCOPE_BUFFER_SIZE in X2Cscope.c) */
    /* Application info */
    uint16_t                appVersion;          /**< Application version identifier (0x0001 default) */
    const compilationDate_t *compilationDate;   /**< Pointer to build timestamp — must have static storage duration */
} X2Cscope_Config_t;

/**
 * @brief Compile-time-safe initializer macro for X2Cscope_Config_t.
 *
 * All 9 arguments are positional and required. Omitting any argument or
 * providing arguments in the wrong order causes a compiler error or warning,
 * preventing the most common integration mistakes (wrong buffer, wrong size).
 *
 * Usage:
 * @code
 *   X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
 *       X2CscopeComm_SendByte, X2CscopeComm_ReceiveByte,
 *       X2CscopeComm_IsByteAvailable, X2CscopeComm_IsSendReady,
 *       X2CscopeComm_Flush,        // optional hook with a default, or NULL
 *       (void*)X2CscopeArray, X2CSCOPE_BUFFER_SIZE,
 *       X2CSCOPE_APP_VERSION, compilationDate   // macro takes the address
 *   );
 *   X2Cscope_InitialiseEx(&config);
 *   X2CscopeComm_PostInit();
 * @endcode
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
    .compilationDate          = &(compDate_)                             \
}

/* ---- Required: implement in X2CscopeComm.c ------------------------------ */
void    X2CscopeComm_SendByte(uint8_t data);
uint8_t X2CscopeComm_ReceiveByte(void);
bool    X2CscopeComm_IsByteAvailable(void);
bool    X2CscopeComm_IsSendReady(void);

/* ---- Implemented in the X2Cscope library -------------------------------- */

/**
 * @brief Single-call initialisation using a unified configuration struct.
 *
 * Validates required fields (in debug builds), hooks the communication
 * interface, and initialises the scope buffer — all in one call.
 * Follow with X2CscopeComm_PostInit() for any peripheral-level post-init steps.
 */
void X2Cscope_InitialiseEx(const X2Cscope_Config_t* config);

/* ---- Optional application hooks -----------------------------------------
 * The X2Cscope library contains a default for each of these functions.
 * To replace a default, implement the function in X2CscopeComm.c with exactly
 * this name and signature (the prototype checks the signature, not the name:
 * a misspelled name silently keeps the default). See X2CscopeComm.c for
 * examples.
 */

/**
 * @brief Send the bytes collected by X2CscopeComm_SendByte(), called when a
 * frame is complete. Needed if the peripheral buffers TX data (e.g. CAN, TCP/IP).
 * Default: does nothing.
 */
void X2CscopeComm_Flush(void);

/**
 * @brief Called from X2Cscope_Init() after X2Cscope_InitialiseEx(), for steps
 * that must follow library init: starting the TCP server, enabling UART TX
 * interrupts, starting DMA transfers, etc.
 * Default: does nothing.
 */
void X2CscopeComm_PostInit(void);

/**
 * @brief Reset the device when requested from the host.
 *
 * WARNING: when this function resets the device, the running application stops
 * immediately and all outputs go to their reset state.
 *
 * Called from X2Cscope_Communicate().
 *
 * @return true  if the device is reset (now or shortly afterwards).
 * @return false if the device cannot be reset now for any safety reason.
 *
 * Default: the device is not reset; the host is told that the reset is not supported.
 */
bool X2CscopeComm_ResetDevice(void);

#ifdef __cplusplus
}
#endif

#endif	/* X2CSCOPECOMMUNICATION_H */