/**
 * @file
 * @brief Generic X2C block services.
 *
 * $LastChangedRevision: 781 $
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#include "Services.h"
#include "TableStruct.h"
#include "BlockServicesX2C.h"

extern volatile tTableStruct *TableStruct;

/**
 * @brief Adds common block services to service table.
 *
 * @param[IN] protocol Protocol
 */
void addBlockServices(tProtocol* protocol)
{
    tSERVICEFunction* pServiceTable = (tSERVICEFunction*) protocol->pServiceTable;

    pServiceTable[SV_ID_SVGETRAMBLOCK] = getRAMBlock;
    pServiceTable[SV_ID_GETBLOCKDATA] = getBlockData;
    pServiceTable[SV_ID_PUTBLOCKDATA] = putBlockData;
}

/**
 * @brief Adds extended block services to service table.
 *
 * @param[IN] protocol Protocol
 */
void addExtendedBlockServices(tProtocol* protocol)
{
	tSERVICEFunction* serviceTable = (tSERVICEFunction*) protocol->pServiceTable;
	serviceTable[SV_ID_SVPUTRAMBLOCK] = putRAMBlock;
}

/**
 * @brief Writes data to target memory.
 *
 */
void putRAMBlock(tProtocol* protocol)
{
	void* addr;
	uint8 size, type;
	uint16 i;
	const uint8 addrOffset = (uint8)1;
#if defined(__ADDRESS_WIDTH_8BIT__)
	const uint8 typeOffset = addrOffset + (uint8)1;
#elif defined(__ADDRESS_WIDTH_16BIT__)
	const uint8 typeOffset = addrOffset + (uint8)2;
#elif defined(__ADDRESS_WIDTH_32BIT__)
	const uint8 typeOffset = addrOffset + (uint8)4;
#else
#error ADDRESS WIDTH NOT DEFINED
#endif
	const uint8 dataOffset = typeOffset + (uint8)1;

#if defined(__ADDRESS_WIDTH_8BIT__)
	addr = (void*)((uint8)protocol->ucFRAMEData[addrOffset]);
#elif defined(__ADDRESS_WIDTH_16BIT__)
	addr = (void*)((uint16)protocol->ucFRAMEData[addrOffset] + \
			((uint16)protocol->ucFRAMEData[addrOffset + 1] << 8));
#elif defined(__ADDRESS_WIDTH_32BIT__)
	addr = (void*)((uint32)protocol->ucFRAMEData[addrOffset] + \
			((uint32)protocol->ucFRAMEData[addrOffset + 1] << 8) + \
			((uint32)protocol->ucFRAMEData[addrOffset + 2] << 16) + \
			((uint32)protocol->ucFRAMEData[addrOffset + 3] << 24));
#else
#error ADDRESS WIDTH NOT DEFINED
#endif

	size = protocol->ucFRAMESize - dataOffset;
	type = protocol->ucFRAMEData[typeOffset];

	if (size % type)
	{
		sendError(protocol, ERRORFormat);
		return;
	}

	switch (type)
	{
		case DATATYPE_8BIT:
		{
			uint8* addr8 = (uint8*)addr;
			for (i=0;i<size;i++)
			{
				*addr8++ = (uint8)protocol->ucFRAMEData[dataOffset + i] & 0xFF;
			}
			break;
		}
		case DATATYPE_16BIT:
		{
			uint16* addr16 = (uint16*)addr;
			for (i=0;i<size/2;i++)
			{
				*addr16++ = (uint16)protocol->ucFRAMEData[dataOffset + 2*i] + \
						((uint16)protocol->ucFRAMEData[dataOffset + 2*i+1] << 8);
			}
			break;
		}
		case DATATYPE_32BIT:
		{
			uint32* addr32 = (uint32*)addr;
			for (i=0;i<size/4;i++)
			{
				*addr32++ = (uint32)protocol->ucFRAMEData[dataOffset + 4*i] + \
						((uint32)protocol->ucFRAMEData[dataOffset + 4*i+1] << 8) + \
						((uint32)protocol->ucFRAMEData[dataOffset + 4*i+2] << 16) + \
						((uint32)protocol->ucFRAMEData[dataOffset + 4*i+3] << 24);
			}
			break;
		}
		default:
		{
			sendError(protocol, ERRORFormat);
			return;
		}
	}

	protocol->ucFRAMESize = (uint8)2;
	protocol->ucFRAMEData[1] = ERRORSuccess;
	protocol->pSnd_Enable((struct tProtocol*)protocol);
}

/**
 * @brief Reads data from target memory.
 *        Service is protected by payload protection.
 *
 * @param[IN] protocol Protocol
 */
void getRAMBlock(tProtocol* protocol)
{
    void *addr;
    uint16 i;
    uint8 size, type;
    uint8 errorID = ERRORSuccess;
#if defined(__ADDRESS_WIDTH_8BIT__)
    const uint8 addrOffset = (uint8)2;
#elif defined(__ADDRESS_WIDTH_16BIT__)
    const uint8 addrOffset = (uint8)3;
#elif defined(__ADDRESS_WIDTH_32BIT__)
    const uint8 addrOffset = (uint8)5;
#else
#error ADDRESS WIDTH NOT DEFINED
#endif

    size = protocol->ucFRAMEData[addrOffset];
    type = protocol->ucFRAMEData[addrOffset + 1];

    if (((uint16) size + 2) > protocol->ucMaxCommSize)
    {
        errorID = ERRORSizeTooLarge;
    }
    else
    {
#if defined(__ADDRESS_WIDTH_8BIT__)
    	addr = (void*) ((uint8) protocol->ucFRAMEData[1];
#elif defined(__ADDRESS_WIDTH_16BIT__)
        addr = (void*) ((uint16) protocol->ucFRAMEData[1] + \
                ((uint16) protocol->ucFRAMEData[2] << 8));
#elif defined(__ADDRESS_WIDTH_32BIT__)
        addr = (void*) ((uint32) protocol->ucFRAMEData[1] + \
                ((uint32) protocol->ucFRAMEData[2] << 8) + \
                ((uint32) protocol->ucFRAMEData[3] << 16) + \
                ((uint32) protocol->ucFRAMEData[4] << 24));
#else
#error ADDRESS WIDTH NOT DEFINED
#endif

        switch (type)
        {
            case DATATYPE_8BIT:
            {
                uint8* addr8 = (uint8*) addr;
                for (i = 0; i < size; i++)
                {
                    protocol->ucFRAMEData[2 + i] = (uint8) *addr8++;
                }
                break;
            }
            case DATATYPE_16BIT:
            {
                uint16* addr16 = (uint16*) addr;
                for (i = 0; i < size; i += 2)
                {
                    protocol->ucFRAMEData[2 + i] = (uint8) *addr16;
                    protocol->ucFRAMEData[2 + i + 1] = (uint8) ((*addr16++) >> 8);
                }
                break;
            }
            case DATATYPE_32BIT:
            {
                uint32* addr32 = (uint32*) addr;
                for (i = 0; i < size; i += 4)
                {
                    protocol->ucFRAMEData[2 + i] = (uint8) *addr32;
                    protocol->ucFRAMEData[2 + i + 1] = (uint8) ((*addr32) >> 8);
                    protocol->ucFRAMEData[2 + i + 2] = (uint8) ((*addr32) >> 16);
                    protocol->ucFRAMEData[2 + i + 3] = (uint8) ((*addr32++) >> 24);
                }
                break;
            }
            default:
                errorID = ERRORFormat;
                break;
        }
    }

    if (errorID == ERRORSuccess)
    {
        protocol->ucFRAMESize = size + 2;
        protocol->ucFRAMEData[1] = errorID;
        protocol->pSnd_Enable((struct tProtocol*) protocol);
    }
    else
    {
        sendError(protocol, errorID);
    }
}

/**
 * @brief Downloads block data to target.
 *
 * This services downloads block data by using the blocks' global address.
 * The block save function is executed if the block was found within the function table.
 *
 * @note This service is not protected by payload length protection.
 *       It's not required because response frame length <= request frame.
 */
void putBlockData(tProtocol* protocol)
{
    uint16 id, i;
    uint16* addr;

#if defined(__ADDRESS_WIDTH_8BIT__)
    const uint16 addrOffset = (uint8)2;
    addr = (uint16*) ((uint8) protocol->ucFRAMEData[1];
#elif defined(__ADDRESS_WIDTH_16BIT__)
    const uint16 addrOffset = (uint8)3;
    addr = (uint16*) ((uint16) protocol->ucFRAMEData[1] + \
    		((uint16) protocol->ucFRAMEData[2] << 8));
#elif defined(__ADDRESS_WIDTH_32BIT__)
    const uint16 addrOffset = (uint8)5;
    addr = (uint16*) ((uint32) protocol->ucFRAMEData[1] + \
    		((uint32) protocol->ucFRAMEData[2] << 8) + \
    		((uint32) protocol->ucFRAMEData[3] << 16) + \
    		((uint32) protocol->ucFRAMEData[4] << 24));
#else
#error ADDRESS WIDTH NOT DEFINED
#endif
    id = *addr;

    i = 0;
    while ((id != TableStruct->TFncTable[i].iBlockID) && (TableStruct->TFncTable[i].iBlockID != 0))
    {
        i++;
    }
    if (TableStruct->TFncTable[i].iBlockID == 0)
    {
        /* if correct block id was not found -> return wrong block id error */
        sendError(protocol, ERRORBlkID);
    } else {
        if (TableStruct->TFncTable[i].pFSave(addr, protocol->ucFRAMEData + addrOffset,  \
                protocol->ucFRAMESize - addrOffset))
        {
            /* wrong data format */
            sendError(protocol, ERRORFormat);
        }
        else
        {
            protocol->ucFRAMESize = 2;
            protocol->ucFRAMEData[1] = ERRORSuccess;
            protocol->pSnd_Enable((struct tProtocol*) protocol);
        }
    }
}

/**
 * @brief Uploads block data from target.
 *
 * This services uploads block data by using the blocks' global address.
 * The block load function is executed if the block was found within the function table.
 *
 * @note This service is not protected by payload length protection.
 *       Implementation is currently not possible => see line with pFLoad instruction.
 */
void getBlockData(tProtocol* protocol)
{
    uint16 id, i;
    uint16 *addr;

#if defined(__ADDRESS_WIDTH_8BIT__)
    	addr = (uint16*) ((uint8) protocol->ucFRAMEData[1];
#elif defined(__ADDRESS_WIDTH_16BIT__)
        addr = (uint16*) ((uint16) protocol->ucFRAMEData[1] + \
                ((uint16) protocol->ucFRAMEData[2] << 8));
#elif defined(__ADDRESS_WIDTH_32BIT__)
        addr = (uint16*) ((uint32) protocol->ucFRAMEData[1] + \
                ((uint32) protocol->ucFRAMEData[2] << 8) + \
                ((uint32) protocol->ucFRAMEData[3] << 16) + \
                ((uint32) protocol->ucFRAMEData[4] << 24));
#else
#error ADDRESS WIDTH NOT DEFINED
#endif
    id = *addr;

    i = 0;
    while ((id != TableStruct->TFncTable[i].iBlockID) && (TableStruct->TFncTable[i].iBlockID != 0))
    {
        i++;
    }
    if (TableStruct->TFncTable[i].iBlockID == 0)
    {
        /* if correct block id was not found -> return wrong block id error */
        sendError(protocol, ERRORBlkID);
    }
    else
    {
        protocol->ucFRAMESize = 2 + TableStruct->TFncTable[i].pFLoad(addr, protocol->ucFRAMEData + 2);
        protocol->ucFRAMEData[1] = ERRORSuccess;
        protocol->pSnd_Enable((struct tProtocol*) protocol);
    }
}
