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

#include <stddef.h>
#include <stdbool.h>
#include "X2CscopeWrapper.h"

/* Debug-build NULL-pointer assertion.
 * Spins in an infinite loop so a connected debugger immediately identifies
 * the mis-configured pointer.  Compiled away in release builds (NDEBUG). */
#ifndef NDEBUG
#define X2CS_ASSERT_NOT_NULL(ptr) do { if ((ptr) == NULL) { while(1){} } } while(0)
#else
#define X2CS_ASSERT_NOT_NULL(ptr) ((void)0)
#endif

// Static function pointers to the user's communication functions (X2Cscope_Config_t)
static void    (*sendByteFcn)(uint8_t);
static uint8_t (*receiveByteFcn)(void);
static bool    (*isByteAvailableFcn)(void);
static bool    (*isSendReadyFcn)(void);
static void    (*flushFcn)(void);

void X2Cscope_Communicate() {
     protocol.pCommunicate((tProtocol*)&protocol);
}

void X2Cscope_Update() {
    X2C_Update();
}

void X2Cscope_InitialiseEx(const X2Cscope_Config_t* config)
{
    /* Validate required fields in debug builds.
     * Each NULL check spins forever so a debugger catches the misconfiguration
     * immediately at the offending pointer. */
    X2CS_ASSERT_NOT_NULL(config);
    X2CS_ASSERT_NOT_NULL(config->sendByte);
    X2CS_ASSERT_NOT_NULL(config->receiveByte);
    X2CS_ASSERT_NOT_NULL(config->isByteAvailable);
    X2CS_ASSERT_NOT_NULL(config->isSendReady);
    X2CS_ASSERT_NOT_NULL(config->scopeArray);
    X2CS_ASSERT_NOT_NULL(config->compilationDate);

    /* Store communication function pointers */
    sendByteFcn        = config->sendByte;
    receiveByteFcn     = config->receiveByte;
    isByteAvailableFcn = config->isByteAvailable;
    isSendReadyFcn     = config->isSendReady;
    flushFcn           = config->flush;  /* may be NULL — valid */

    /* Initialise scope, LNet protocol, and version info */
    initTableStruct();
    initSerialGeneric(&interface);
    initLNet(&protocol, bufferLNet, LNET_BUFFERSIZE, LNET_NODEID);
    initServiceTable((tProtocol*)&protocol);
    addCoreServices((tProtocol*)&protocol);
    addBlockServices((tProtocol*)&protocol);
    addExtendedBlockServices((tProtocol*)&protocol);
    addTableStructProtocol((tProtocol*)&protocol);
    linkSerial((tProtocol*)&protocol, &interface);
    TableStruct->DSPState = PRG_LOADED_STATE;

    initVersionInfo(TableStruct, config->appVersion, config->compilationDate);
    TableStruct->TFncTable  = blockFunctionTable;
    TableStruct->TParamTable = parameterIdTable;

    initSerial(&interface);

    X2C_Init(config->scopeArray, config->scopeSize);
}

/* Adapters from the internal tSerial interface to the user's functions.
 * static: only used here, so they add no global symbols to the application. */
static void sendByteWrapper(tSerial* serial, uint8 data) {
    sendByteFcn((uint8_t) data);
}

static uint8_t receiveByteWrapper(tSerial* serial) {
    return (uint8) receiveByteFcn();
}

static bool isByteAvailableWrapper(tSerial* serial) {
    return isByteAvailableFcn();
}

static bool isSendReadyWrapper(tSerial* serial) {
    return isSendReadyFcn();
}

static void flushWrapper(tSerial* serial) {
    if (flushFcn != NULL) {
        flushFcn();
    }
}

void initSerial(tSerial* serial)
{
    serial->send = (void (*)(tInterface*, uint8))sendByteWrapper;
    serial->receive = (uint8 (*)(tInterface*))receiveByteWrapper;
    serial->isReceiveDataAvailable = (bool (*)(tInterface*))isByteAvailableWrapper;
    serial->isSendReady = (bool (*)(tInterface*))isSendReadyWrapper;
    serial->flush = (void (*)(tInterface*))flushWrapper;
}