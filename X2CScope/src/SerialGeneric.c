/*
 * Generic serial interface.
 *
 * $LastChangedRevision: 632 $
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#include "SerialGeneric.h"


/* private prototypes */
static void sendSerial(tSerial* serialP, uint8 data);
static uint8 receiveSerial(tSerial* serialP);
static uint8 isReceiveDataAvailable(tSerial* serialP);
static uint8 isSendReady(tSerial* serialP);
static uint8 getTxFifoFree(tSerial* serialP);
static void flush(tSerial* serialP);


void initSerialGeneric(tSerial* serialP)
{
    serialP->send = (void (*)(tInterface*, uint8))sendSerial;
    serialP->receive = (uint8 (*)(tInterface*))receiveSerial;
    serialP->isReceiveDataAvailable = (uint8 (*)(tInterface*))isReceiveDataAvailable;
    serialP->isSendReady = (uint8 (*)(tInterface*))isSendReady;
    serialP->getTxFifoFree = (uint8 (*)(tInterface*))getTxFifoFree;
    serialP->flush = (void (*)(tInterface*))flush;
}

void linkSerial(tProtocol* lnkProtocol, tSerial* serialP)
{
    lnkProtocol->hwInterface = (tInterface*)serialP;
}

static void sendSerial(tSerial* serialP, uint8 data)
{
    // do nothing
}

static uint8 receiveSerial(tSerial* serialP)
{
    return ((uint8)0);
}

static uint8 isReceiveDataAvailable(tSerial* serialP)
{
    return ((uint8)0);
}

static uint8 isSendReady(tSerial* serialP)
{
    return ((uint8)0);
}

static uint8 getTxFifoFree(tSerial* serialP)
{
    return ((uint8)0);
}

static void flush(tSerial* serialP)
{
    /* do nothing because 'send' immediately sends byte */
}

