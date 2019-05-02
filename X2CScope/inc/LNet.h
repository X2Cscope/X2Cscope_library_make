/**
 * @file
 * @brief LNet protocol
 */
/*
 * $LastChangedRevision: 12 $
 * $LastChangedDate:: 2017-03-01 18:11:38 +0100#$
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef LNET_H
#define LNET_H

#include "CommonFcts.h"

#define LNET_STD_SYN ((uint8)0x55)
#define LNET_OSM_SYN ((uint8)0x02)
/* can't be changed -> length dependency */
#define LNET_FILL    ((uint8)0x00)

/* state machine states */
typedef enum {LNET_STATE_SYN, LNET_STATE_SIZE, LNET_STATE_NODE, \
			  LNET_STATE_DATA, LNET_STATE_FILL, LNET_STATE_CRC} tLNetFrameState;
typedef struct {
	void (*pCommunicate)(tProtocol*);
	void (*pSnd_Enable)(tProtocol*);
	
	uint8 ucFrameSize;
	uint8 ucMaxCommSize;
	uint8* ucFrameData;
	
	void* pServiceTable;

	tInterface* hwInterface;

	/* slave node id */
	uint8 node;
	
	/* receive- & send state */
	tLNetFrameState ucRcvState;
	tLNetFrameState ucSndState;
	/* communincation state (rx or tx) */
	tCommState ucCommState;
	/* checksum */
	uint8 ucFrameCheck;
	/* framebuffer ptr to actual frame byte */
	uint8 ucFrameBufferPtr;
} tLNet;

/* public prototypes */
void initLNet(tLNet* protocol, uint8* dataBuffer, uint8 dataBufferSize, \
		uint8 node);

#endif
