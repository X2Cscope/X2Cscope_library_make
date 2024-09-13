/*
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
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
 *
 * Generic serial interface.
 *
 * $LastChangedRevision: 632 $
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
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

