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

#include <xc.h>
#include <stdint.h>
#include "X2Cscope.h"  /* For compilationDate_t */

/* Functions implemented in the X2Cscope library */

/**
 * @brief Full X2Cscope configuration structure
 * Combines communication interface and initialization parameters
 * into a single configuration object.
 */
typedef struct {
    /* Communication interface */
    void (*sendSerial)(uint8_t);              /**< Send single byte */
    uint8_t (*receiveSerial)(void);           /**< Receive single byte */
    uint8_t (*isReceiveDataAvailable)(void);  /**< Check if RX data available */
    uint8_t (*isSendReady)(void);             /**< Check if TX ready */
    void (*flushSerial)(void);                /**< Flush TX buffer (optional, can be NULL) */
    /* Scope buffer */
    void* scopeArray;                         /**< Pointer to scope data buffer */
    uint16_t scopeSize;                       /**< Size of scope buffer in bytes */
    /* Application info */
    uint16_t appVersion;                      /**< Application version identifier */
    compilationDate_t compilationDate;        /**< Build date/time stamp */
} X2Cscope_Config_t;

/**
 * @brief Initializer macro for X2Cscope_Config_t
 * Ensures all required fields are provided at compile time.
 * Missing arguments will cause a compiler error.
 *
 * @param send      sendSerial function pointer
 * @param recv      receiveSerial function pointer
 * @param avail     isReceiveDataAvailable function pointer
 * @param ready     isSendReady function pointer
 * @param flush     flushSerial function pointer (use NULL if not needed)
 * @param buf       Pointer to scope data buffer
 * @param bufSize   Size of scope buffer in bytes
 * @param ver       Application version identifier
 * @param compDate  compilationDate_t build timestamp
 */
#define X2CSCOPE_CONFIG_INIT(send, recv, avail, ready, flush, buf, bufSize, ver, compDate) \
    { \
        .sendSerial = (send), \
        .receiveSerial = (recv), \
        .isReceiveDataAvailable = (avail), \
        .isSendReady = (ready), \
        .flushSerial = (flush), \
        .scopeArray = (buf), \
        .scopeSize = (bufSize), \
        .appVersion = (ver), \
        .compilationDate = (compDate) \
    }

/**
 * @brief Initialise X2Cscope using full configuration structure (recommended)
 * @param config Pointer to full configuration structure
 *
 * This is the recommended single-call initialisation interface.
 * It hooks the communication functions and initialises the scope in one step.
 * Use X2CSCOPE_CONFIG_INIT() macro to populate the structure safely.
 */
void X2Cscope_InitialiseEx(const X2Cscope_Config_t* config);

/**
 * @brief Legacy 4-parameter UART hook + separate Initialise interface
 * @note Maintained for backward compatibility. New code should use X2Cscope_InitialiseEx()
 */
void X2Cscope_HookUARTFunctions(
    void (*sendSerialFcnPntr)(uint8_t),
    uint8_t (*receiveSerialFcnPntr)(),
    uint8_t (*isReceiveDataAvailableFcnPntr)(),
    uint8_t (*isSendReadyFcnPntr)());

/* Functions below must be implemented by the X2Cscope user.
 * Typically in X2CscopeComm.c */
void sendSerial(uint8_t data);
uint8_t receiveSerial(void);
uint8_t isReceiveDataAvailable(void);
uint8_t isSendReady(void);
void flushSerial(void);

/**
 * @brief User-level post-initialization function
 * Call this after X2Cscope_Initialise() to perform any additional setup.
 * Implement this function in X2CscopeComm.c.
 */
void X2Cscope_PostInit(void);

#ifdef __cplusplus
}
#endif

#endif	/* X2CSCOPECOMMUNICATION_H */