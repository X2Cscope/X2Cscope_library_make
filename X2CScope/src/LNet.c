/*
 * $LastChangedRevision: 12 $
 * $LastChangedDate:: 2017-03-01 18:11:38 +0100#$
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#include "Services.h"
#include "LNet.h"

#define HW_INTERFACE (protocol->hwInterface)

/* private prototypes */
static void enableSend(tLNet* protocol);
static void enableReceive(tLNet* protocol);
static void communicate(tLNet* protocol);
static void LNet_OnlineScope(tLNet* protocol);
static void checkFill(tLNet* protocol, uint8 data, tLNetFrameState* stateVar, tLNetFrameState state);
static uint8 receiveSize(tLNet* protocol, uint8 size);
static void sendProtocolError(tLNet* protocol, uint8 serviceId, uint8 errorId);

#if defined(__COMPILER_CODEWARRIOR_ECLIPSE__)
#pragma push
#pragma section sdata_type ".ptrTableStruct" ".ptrTableStruct"
extern volatile tTableStruct *TableStruct;
#pragma pop
#else
extern volatile tTableStruct* TableStruct;
#endif

/* TODO move both variables to LNet structure */
static tLNetFrameState nextGlobalState;
static uint8 lastSyn;

/* enable send procedure / disable receive procedure */
static void enableSend(tLNet* protocol)
{
	protocol->ucCommState = SEND_ALLOWED;
	protocol->ucSndState = LNET_STATE_SYN;
}

/* enable receive procedure / disable send procedure */
static void enableReceive(tLNet* protocol)
{
	protocol->ucCommState = RECEIVE_ALLOWED;
	protocol->ucRcvState = LNET_STATE_SYN;
}

static void communicate(tLNet* protocol)
{
	/* service table pointer */
	tSERVICEFunction* svTable;
	/* current received message byte */
	uint8 msg;

	/* send procedure */
	while ((protocol->ucCommState == SEND_ALLOWED) && protocol->hwInterface->isSendReady(HW_INTERFACE))
	{
		switch (protocol->ucSndState)
		{
			case LNET_STATE_SYN:
				protocol->ucFrameCheck = LNET_STD_SYN;
				protocol->ucFrameBufferPtr = 0;
				protocol->hwInterface->send(HW_INTERFACE, LNET_STD_SYN);
				protocol->ucSndState = LNET_STATE_SIZE;
				break;

			case LNET_STATE_SIZE:
				protocol->ucFrameCheck += (protocol->ucFrameSize);
				protocol->hwInterface->send(HW_INTERFACE, protocol->ucFrameSize);
				checkFill(protocol, protocol->ucFrameSize, &protocol->ucSndState, LNET_STATE_NODE);
				break;

			case LNET_STATE_NODE:
				protocol->ucFrameCheck += protocol->node;
				protocol->hwInterface->send(HW_INTERFACE, protocol->node);
				checkFill(protocol, protocol->node, &protocol->ucSndState, LNET_STATE_DATA);
				break;

			case LNET_STATE_DATA:
				protocol->ucFrameData[protocol->ucFrameBufferPtr] &= 0xFF;
				protocol->ucFrameCheck += \
					protocol->ucFrameData[protocol->ucFrameBufferPtr];
				protocol->hwInterface->send(HW_INTERFACE, \
						protocol->ucFrameData[protocol->ucFrameBufferPtr]);

				if ((protocol->ucFrameBufferPtr + 1) >= protocol->ucFrameSize)
				{
					checkFill(protocol, protocol->ucFrameData[protocol->ucFrameBufferPtr], \
						&protocol->ucSndState, LNET_STATE_CRC);
				}
				else
				{
					checkFill(protocol, protocol->ucFrameData[protocol->ucFrameBufferPtr], \
						&protocol->ucSndState, LNET_STATE_DATA);
				}
				protocol->ucFrameBufferPtr++;
				break;

			case LNET_STATE_FILL:
				protocol->hwInterface->send(HW_INTERFACE, LNET_FILL);
				protocol->ucSndState = nextGlobalState;
				break;

			case LNET_STATE_CRC:
				protocol->ucFrameCheck &= 0xFF;
				if ((protocol->ucFrameCheck == LNET_STD_SYN) || (protocol->ucFrameCheck == LNET_OSM_SYN))
				{
					protocol->ucFrameCheck = (~protocol->ucFrameCheck) & 0xFF;
				}
				protocol->hwInterface->send(HW_INTERFACE, protocol->ucFrameCheck);

				/* frame complete -> flush tx buffer */
				protocol->hwInterface->flush(HW_INTERFACE);

				enableReceive(protocol);
				break;

			default:
				enableReceive(protocol);
				break;
		}	/* switch end */
	}		/* tx while end */

	/* receive procedure */
	while ((protocol->ucCommState == RECEIVE_ALLOWED) && protocol->hwInterface->isReceiveDataAvailable(HW_INTERFACE))
	{
		/* get 1 byte from rx buffer */
		msg = protocol->hwInterface->receive(HW_INTERFACE);
		
		switch (protocol->ucRcvState)
		{
			case LNET_STATE_SYN:
				if (msg == LNET_STD_SYN)
				{
					protocol->ucFrameCheck = LNET_STD_SYN;
					protocol->ucRcvState = LNET_STATE_SIZE;
				}
				break;

			case LNET_STATE_SIZE:
				if (receiveSize(protocol, msg))
				{
					checkFill(protocol, msg, &protocol->ucRcvState, protocol->ucRcvState);
				}
				break;

			case LNET_STATE_NODE:
				if (msg == protocol->node)
				{
					protocol->ucFrameCheck += msg;
					checkFill(protocol, msg, &protocol->ucRcvState, LNET_STATE_DATA);
				}
				else
				{
					protocol->ucRcvState = LNET_STATE_SYN;
				}
				break;

			case LNET_STATE_DATA:
				protocol->ucFrameCheck += msg;
				protocol->ucFrameData[protocol->ucFrameBufferPtr] = msg;
				protocol->ucFrameBufferPtr++;
				
				if (protocol->ucFrameBufferPtr >= protocol->ucFrameSize)
				{
					checkFill(protocol, msg, &protocol->ucRcvState, LNET_STATE_CRC);
				}
				else
				{
					checkFill(protocol, msg, &protocol->ucRcvState, LNET_STATE_DATA);
				}	
				break;

			case LNET_STATE_FILL:

				if (msg == LNET_FILL)
				{
					protocol->ucRcvState = nextGlobalState;
				}
				else
				{
					if (lastSyn == LNET_STD_SYN)
					{
						receiveSize(protocol, msg);
					}
					else
					{
						protocol->ucRcvState = LNET_STATE_SYN;
					}
				}	
				break;

			case LNET_STATE_CRC:
				protocol->ucFrameCheck &= 0xFF;
				if ((protocol->ucFrameCheck == LNET_STD_SYN) || (protocol->ucFrameCheck == LNET_OSM_SYN))
				{
					protocol->ucFrameCheck = (~protocol->ucFrameCheck) & 0xFF;
				}
				if (protocol->ucFrameCheck == msg)
				{
					if (protocol->ucFrameData[0] <= MAX_SERVICE_ID)
					{
						/* elements are function pointers */
						svTable = (tSERVICEFunction*)protocol->pServiceTable;
						svTable[protocol->ucFrameData[0]]((tProtocol*)protocol);
					}
					else
					{
						sendError((tProtocol*)protocol, ERRORServiceNotAvail);
					}
				}
				else
				{
					sendError((tProtocol*)protocol, ERRORChksum);
				}
				break;

			default:
				protocol->ucRcvState = LNET_STATE_SYN;
				break;
		}	/* switch end */
	}		/* rx while end */

	LNet_OnlineScope(protocol);
}


static void checkFill(tLNet* protocol, uint8 data, tLNetFrameState* stateVar, tLNetFrameState state)
{
	if ((data == LNET_STD_SYN) || (data == LNET_OSM_SYN))
	{
		lastSyn = data;
		nextGlobalState = state;
		*stateVar = LNET_STATE_FILL;
	}
	else
	{
		*stateVar = state;
	}
}


/* online scope mode (osm) */
static void LNet_OnlineScope(tLNet* protocol)
{
	uint16 i, j;
	static uint32 txBufFull = (uint32)0;
	uint8 checksum;

	/* if scope pointer is not NULL, scope state is 5 (online running) and */
	/* prescaler counter is 0 -> send osm frame */
	if ((TableStruct->piScope != 0) && (TableStruct->piScope->state == SCOPE_SAMPLE_ONLINE))
	{
		/* call TxFifoFree function (pointer to function located in protocol, */
		/* functions itself is located in the communication interface file */
		/* (e.g. SCI) */
		/* tx fifo must have frame data + lnet overhead (+4) free bytes */
		if ((protocol->hwInterface->getTxFifoFree(HW_INTERFACE) >= (TableStruct->piScope->txFrameSize + 4)) && \
		    (TableStruct->piScope->stfCnt >= TableStruct->piScope->stf))
		{
            TableStruct->piScope->stfCnt = 0;
			/* send osm frame */
			protocol->hwInterface->send(HW_INTERFACE, LNET_OSM_SYN);
			protocol->hwInterface->send(HW_INTERFACE, TableStruct->piScope->txFrameSize);
			protocol->hwInterface->send(HW_INTERFACE, protocol->node);

			protocol->hwInterface->send(HW_INTERFACE, LOW(TableStruct->piScope->timestamp));
			protocol->hwInterface->send(HW_INTERFACE, HIGH(TableStruct->piScope->timestamp));

			checksum = LNET_OSM_SYN + TableStruct->piScope->txFrameSize + \
			    protocol->node + LOW(TableStruct->piScope->timestamp) + HIGH(TableStruct->piScope->timestamp);

            i = (uint16)0;
            do
            {
                j = (uint16)0;
                do
                {
#if defined(__DATA_WIDTH_16BIT__)
                    uint16 tmp;
                    tmp = *((uint16*)TableStruct->piScope->channelAddr[i] + j);
                    protocol->hwInterface->send(HW_INTERFACE, (uint8)tmp);
                    protocol->hwInterface->send(HW_INTERFACE, (uint8)(tmp >> 8));
                    checksum += (tmp & 0xFF);
                    checksum += (tmp >> 8);
#elif defined(__DATA_WIDTH_8BIT__)
                    /* procedure not tested yet */
                    uint8 tmp;
                    tmp = *((uint8*) TableStruct->piScope->channelAddr[i] + j);
                    protocol->hwInterface->send(HW_INTERFACE, tmp);
                    checksum += tmp;									
#else
#error DATA WIDTH NOT DEFINED
#endif
                    j++;
                }
                while (j < TableStruct->piScope->dataSize[i]);
                i++;
            }
            while(i < TableStruct->piScope->noChannels);

            checksum &= 0xFF;
			if (checksum == LNET_OSM_SYN)
			{
				checksum = ~checksum;
			}
			protocol->hwInterface->send(HW_INTERFACE, checksum);
		}
		else
		{
			/* debug: counter of failed transfers due to full tx fifo */
			txBufFull++;
		}
	}
}

/**
 * @brief Receives and checks size.
 *
 * @param protocol Protocol
 * @param size Size
 *
 * @return 1 (true) in case of valid size otherwise 0 (false)
 */
static uint8 receiveSize(tLNet* protocol, uint8 size)
{
	uint8 isValidSize = 0;

	if (size > 0)
	{
		if (size > protocol->ucMaxCommSize)
		{
			protocol->ucRcvState = LNET_STATE_SYN;
			/* sends size error with 'Device Info' service identifier */
			sendProtocolError(protocol, SV_ID_SVDEVICEINFO, ERRORSizeTooLarge);
		}
		else
		{
			protocol->ucFrameCheck = LNET_STD_SYN + size;
			protocol->ucFrameSize = size;
			protocol->ucFrameBufferPtr = 0;
			protocol->ucRcvState = LNET_STATE_NODE;
			isValidSize = 1;
		}
	}
	else
	{
		protocol->ucRcvState = LNET_STATE_SYN;
	}
	return (isValidSize);
}

static void sendProtocolError(tLNet* protocol, uint8 serviceId, uint8 errorId)
{
	protocol->ucFrameData[0] = serviceId;
	sendError((tProtocol*)protocol, errorId);
}

/*
 * Initializes LNet by using an external buffer.
 * Hence this kind of LNet initialization doesn't use malloc
 * and therefore needs no heap memory.
 */
void initLNet(tLNet* protocol, uint8* dataBuffer, uint8 dataBufferSize, uint8 node)
{
	protocol->ucFrameData = dataBuffer;
	protocol->ucMaxCommSize = dataBufferSize;
	protocol->node = node;

	/* hook functions */
	protocol->pCommunicate = (void (*)(tProtocol*))communicate;
	protocol->pSnd_Enable = (void (*)(tProtocol*))enableSend;

	/* set to receive state at begin */
	enableReceive(protocol);
}
