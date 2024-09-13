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

#include "X2CscopeWrapper.h"

void X2Cscope_Initialise(void* scopeArray, uint16_t scopeSize, const uint16_t appVersion, compilationDate_t compilationDate) {
     //X2C
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

    initVersionInfo(TableStruct, appVersion, compilationDate);
    TableStruct->TFncTable = blockFunctionTable;
    TableStruct->TParamTable = parameterIdTable;
    
    initSerial(&interface);
    
    X2C_Init(scopeArray,scopeSize);
}

void X2Cscope_Communicate() {
     protocol.pCommunicate((tProtocol*)&protocol);
}

void X2Cscope_Update() {
    X2C_Update();
}

static void (*sendSerialFcn)(uint8_t);
static uint8_t (*receiveSerialFcn)();
static uint8_t (*isReceiveDataAvailableFcn)();
static uint8_t (*isSendReadyFcn)();

void X2Cscope_HookUARTFunctions(void (*sendSerialFcnPntr)(uint8_t), uint8_t (*receiveSerialFcnPntr)(), 
        uint8_t (*isReceiveDataAvailableFcnPntr)(), uint8_t (*isSendReadyFcnPntr)()) {
    sendSerialFcn = sendSerialFcnPntr;
    receiveSerialFcn = receiveSerialFcnPntr;
    isReceiveDataAvailableFcn = isReceiveDataAvailableFcnPntr;
    isSendReadyFcn = isSendReadyFcnPntr;
}

void sendSerialWrapper(tSerial* serial, uint8 data) {
    sendSerialFcn((uint8_t) data);
}

uint8_t receiveSerialWrapper(tSerial* serial) {
    return (uint8) receiveSerialFcn();
}

uint8_t isReceiveDataAvailableWrapper(tSerial* serial) {
    return (uint8) isReceiveDataAvailableFcn();
}

uint8_t isSendReadyWrapper(tSerial* serial) {
    return (uint8) isSendReadyFcn();
}

void initSerial(tSerial* serial)
{
    serial->send = (void (*)(tInterface*, uint8))sendSerialWrapper;
    serial->receive = (uint8 (*)(tInterface*))receiveSerialWrapper;
    serial->isReceiveDataAvailable = (uint8 (*)(tInterface*))isReceiveDataAvailableWrapper;
    serial->isSendReady = (uint8 (*)(tInterface*))isSendReadyWrapper;
}