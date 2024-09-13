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
 * $LastChangedRevision: 12 $
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
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
