#include "X2CScopeWrapper.h"

void X2CScope_Initialise(uint16_t scopeSize, const uint16_t appVersion, compilationDate_t compilationDate) {
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
    
    X2C_Init(scopeSize);
}

void X2CScope_Communicate() {
     protocol.pCommunicate((tProtocol*)&protocol);
}

void X2CScope_Update() {
    X2C_Update();
}

static void (*sendSerialFcn)(uint8_t);
static uint8_t (*receiveSerialFcn)();
static uint8_t (*isReceiveDataAvailableFcn)();
static uint8_t (*isSendReadyFcn)();

void X2CScope_HookUARTFunctions(void (*sendSerialFcnPntr)(uint8_t), uint8_t (*receiveSerialFcnPntr)(), 
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