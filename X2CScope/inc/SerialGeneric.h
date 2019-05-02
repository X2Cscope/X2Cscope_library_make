/*
 * Generic serial interface.
 *
 * $LastChangedRevision: 609 $
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef __SERIALGENERIC_H__
#define __SERIALGENERIC_H__

#include "CommonFcts.h"

typedef struct {
	void (*send)(tInterface*, uint8);
	uint8 (*receive)(tInterface*);
	uint8 (*isReceiveDataAvailable)(tInterface*);
	uint8 (*isSendReady)(tInterface*);
	uint8 (*getTxFifoFree)(tInterface*);
	void (*flush)(tInterface*);
} tSerial;

/* public prototypes */
void initSerialGeneric(tSerial* serialP);
void linkSerial(tProtocol* lnkProtocol, tSerial* serialP);

#endif
