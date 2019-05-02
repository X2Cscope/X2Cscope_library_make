/*
 * $LastChangedRevision: 995 $
 * $LastChangedDate:: 2016-08-11 15:52:41 +0200#$
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#include "CommonFcts.h"
#include "Scope_Main.h"

/* private prototypes */
static uint8 isTriggerEvent(SCOPE_MAIN *pTScope, uint64 curTrgValue);
static uint64 getTriggerValue(SCOPE_MAIN *pTScope);
static void sampleData(SCOPE_MAIN *pTScope);

ALIGNTYPE ScopeArray[SCOPE_SIZE];

#if defined(__COMPILER_CODEWARRIOR_ECLIPSE__)
#pragma push
#pragma section sdata_type ".ptrTableStruct" ".ptrTableStruct"
extern volatile tTableStruct *TableStruct;
#pragma pop
#else
extern volatile tTableStruct* TableStruct;
#endif

/* private prototypes */
static void* getBlockAddress(const tParameterTable* paramTable, uint16 paramId);
static tBlockFunctions* getBlockFunction(const tBlockFunctions* blockFuncTable, uint16 blockId);

/**
 * Scope version.
 */
#define SCOPE_VERSION ((uint8)2)

#define SOURCE_TYPE_ADDRESS ((uint8)0)
#define SOURCE_TYPE_CONTROLBLOCK ((uint8)1)
#define SOURCE_TYPE_INPORTBLOCK ((uint8)2)
#define SOURCE_TYPE_OUTPORTBLOCk ((uint8)3)

#define TRG_MODE_AUTO ((uint8)0)
#define TRG_MODE_NORMAL ((uint8)1)

/************************************************/
/*  Scope_Main_Init                             */
/************************************************/
void Scope_Main_Init(SCOPE_MAIN *pTScope)
{
	uint8 i;

	pTScope->ID = SCOPE_MAIN_ID;
	for (i=0;i<MAX_SCOPE_CHANNELS;i++)
	{
		pTScope->channelAddr[i] = (void*)0;
		pTScope->dataSize[i] = (uint8)0;
	}

	pTScope->arrayAddr = (void*)ScopeArray;
	pTScope->trgLevel = (int32)0;
	pTScope->trgLastValue = (int32)0;
	pTScope->trgAddr = (void*)0;
	pTScope->trgDataType = (uint8)0;
	pTScope->state = SCOPE_IDLE;
	pTScope->offlinePtr = (uint32)0;
	pTScope->dataSizeTotal = (uint8)0;
	pTScope->noChannels = (uint8)0;
	pTScope->stf = (uint16)0;
    pTScope->stfCnt = (uint16)0;
	pTScope->timestamp = (uint16)0;
    pTScope->txFrameSize = (uint8)0;
	pTScope->trgDelay = (int32)0;
	pTScope->trgEventPos = (int32)0;
	pTScope->trgCountReached = (uint8)0;
	pTScope->trgCount = (uint32)0;
	pTScope->maxUsedLength = SCOPE_SIZE;
	pTScope->trgEdge = EDGE_RISING;
	pTScope->arraySize = SCOPE_SIZE;

	TableStruct->piScope = pTScope;
}



/************************************************/
/*  Scope_Main_Update                           */
/************************************************/
void Scope_Main_Update(SCOPE_MAIN *pTScope)
{
    uint8 i, j;
	uint64 curTrgValue;

	switch (pTScope->state)
	{
		case SCOPE_IDLE:
			break;
		case SCOPE_SAMPLE_ONLINE:
			pTScope->timestamp++;
            pTScope->stfCnt++;
            break;
        case SCOPE_SAMPLE_OFFLINE:
            /* check if sample time prescaler is reached */
            if (pTScope->stfCnt++ >= pTScope->stf)
            {
                pTScope->stfCnt = (uint16)0;

				sampleData(pTScope);

                /* if size of next data size would exceed SCOPE_SIZE, the */
                /* offline sampling mode will be stopped (change to idle state) */
                if ((pTScope->offlinePtr + pTScope->dataSizeTotal) > SCOPE_SIZE)
                {
                    pTScope->state = SCOPE_IDLE;
                }
            }
            pTScope->timestamp++;
            break;
        /* wait for trigger in online mode */
        case SCOPE_WAITTRGPOS_ONLINE:
        case SCOPE_WAITTRGNEG_ONLINE:
        	curTrgValue = getTriggerValue(pTScope);
        	if (isTriggerEvent(pTScope, curTrgValue))
        	{
        		pTScope->state = SCOPE_SAMPLE_ONLINE;
        	}
        	else
        	{
        		pTScope->trgLastValue = curTrgValue;
        	}
            break;

        /* wait for trigger in offline mode */
		case SCOPE_WAITTRG_OFFLINE:
			if (pTScope->stfCnt++ >= pTScope->stf)
			{
				pTScope->stfCnt = (uint16)0;
				/* reset array ptr if next dataset would exceed scope buffer size */
				if (pTScope->offlinePtr + pTScope->dataSizeTotal > SCOPE_SIZE)
				{
					pTScope->offlinePtr = (int32)0;
				}

				i = 0;
				do
				   {
					j = 0;
					do
					{
						*(ALIGNCASTPTR pTScope->arrayAddr + pTScope->offlinePtr++) = \
							*(ALIGNCASTPTR pTScope->channelAddr[i] + j);
						j++;
					}
					while (j < pTScope->dataSize[i]);
					i++;
				}
				while(i < pTScope->noChannels);

				/* checks for minimum samples (= trigger delay) */
				if (pTScope->offlinePtr >= pTScope->trgCount)
				{
					pTScope->trgCountReached = 1;
				}
			}

			curTrgValue = getTriggerValue(pTScope);
			if (isTriggerEvent(pTScope, curTrgValue) && pTScope->trgCountReached)
			{
				if (pTScope->trgDelay < (int32)0)
				{
					pTScope->trgEventPos = pTScope->trgDelay;
					pTScope->offlinePtr = (uint32)0;
					pTScope->state = SCOPE_WAIT_TRG_NEG_DELAY;
				}
				else
				{
					if (pTScope->offlinePtr > 0)
					{
						/**
						 * In case of STF > 0:
						 * Trigger event position = current offline pointer because next sample is
						 * the first 'after trigger' sample.
						 * In case of STF == 0:
						 * Current sample is first 'after trigger' sample, so the 'trigger event sample'
						 * was the previous sample.
						 */
						if (pTScope->stfCnt != 0)
						{
							pTScope->trgEventPos = pTScope->offlinePtr;
						}
						else
						{
							pTScope->trgEventPos = pTScope->offlinePtr - pTScope->dataSizeTotal;
						}
					}
					else
					{
						pTScope->trgEventPos = pTScope->maxUsedLength - pTScope->dataSizeTotal;
					}
					pTScope->state = SCOPE_TRG_SAMPLE_OFFLINE;
				}
			}
			else
			{
				pTScope->trgLastValue = curTrgValue;
			}
			break;

		case SCOPE_WAIT_TRG_NEG_DELAY:
			/* in case of negative delay wait until delay time has been passed */
			if (pTScope->stfCnt++ >= pTScope->stf)
			{
				pTScope->stfCnt = (uint16)0;

				pTScope->trgEventPos += pTScope->dataSizeTotal;
				if (pTScope->trgEventPos >= 0)
				{
					pTScope->state = SCOPE_SAMPLE_OFFLINE;
				}
			}
			break;

		case SCOPE_TRG_SAMPLE_OFFLINE:
            if (pTScope->stfCnt++ >= pTScope->stf)
            {
                pTScope->stfCnt = (uint16)0;

				if ((pTScope->offlinePtr + pTScope->dataSizeTotal) > SCOPE_SIZE)
				{
					pTScope->offlinePtr = (uint32)0;
				}

				sampleData(pTScope);

				if (((pTScope->trgEventPos - pTScope->offlinePtr) <= pTScope->trgDelay) || \
					((pTScope->trgEventPos - pTScope->offlinePtr + pTScope->maxUsedLength) <= pTScope->trgDelay))
				{
					pTScope->state = SCOPE_IDLE;
				}

			}
			break;
	}
}

/************************************************/
/*  Scope_Main_Load                             */
/************************************************/
uint8 Scope_Main_Load(SCOPE_MAIN *pTScope,uint8 *ucData)
{
	ucData[0] = pTScope->state;
	ucData[1] = pTScope->noChannels;
	ucData[2] = (uint8)(pTScope->stf & 0xFF);
	ucData[3] = (uint8)((pTScope->stf >> 8) & 0xFF);
	ucData[4] = (uint8)(pTScope->offlinePtr & 0xFF);
	ucData[5] = (uint8)((pTScope->offlinePtr >> 8) & 0xFF);
	ucData[6] = (uint8)((pTScope->offlinePtr >> 16) & 0xFF);
	ucData[7] = (uint8)((pTScope->offlinePtr >> 24) & 0xFF);
	ucData[8] = (uint8)((uint32)pTScope->arrayAddr & 0xFF);
	ucData[9] = (uint8)(((uint32)pTScope->arrayAddr >> 8) & 0xFF);
	ucData[10] = (uint8)(((uint32)pTScope->arrayAddr >> 16) & 0xFF);
	ucData[11] = (uint8)(((uint32)pTScope->arrayAddr >> 24) & 0xFF);

	ucData[12] = (uint8)(pTScope->trgDelay & 0xFF);
	ucData[13] = (uint8)((pTScope->trgDelay >> 8) & 0xFF);
	ucData[14] = (uint8)((pTScope->trgDelay >> 16) & 0xFF);
	ucData[15] = (uint8)((pTScope->trgDelay >> 24) & 0xFF);
	ucData[16] = (uint8)(pTScope->trgEventPos & 0xFF);
	ucData[17] = (uint8)((pTScope->trgEventPos >> 8) & 0xFF);
	ucData[18] = (uint8)((pTScope->trgEventPos >> 16) & 0xFF);
	ucData[19] = (uint8)((pTScope->trgEventPos >> 24) & 0xFF);

	ucData[20] = (uint8)(pTScope->maxUsedLength & 0xFF);
	ucData[21] = (uint8)((pTScope->maxUsedLength >> 8) & 0xFF);
	ucData[22] = (uint8)((pTScope->maxUsedLength >> 16) & 0xFF);
	ucData[23] = (uint8)((pTScope->maxUsedLength >> 24) & 0xFF);

	ucData[24] = (uint8)(pTScope->arraySize & 0xFF);
	ucData[25] = (uint8)((pTScope->arraySize >> 8) & 0xFF);
	ucData[26] = (uint8)((pTScope->arraySize >> 16) & 0xFF);
	ucData[27] = (uint8)((pTScope->arraySize >> 24) & 0xFF);

	ucData[28] = (uint8)0x80 + SCOPE_VERSION;

	return ((uint8)29);
}

/************************************************/
/*  Scope_Main_Save                             */
/************************************************/
uint8 Scope_Main_Save(SCOPE_MAIN *pTScope, uint8 *ucData, uint8 ucFRMlen)
{
   const uint8 offset = (uint8)4;
   uint8 i = (uint8)0;
   uint8 ptr = offset;

   uint8 sourceType;
   uint16 paramId, elementId, blockId;
   void* blockAddr;
   tBlockFunctions* blockFunctions;

   if (ucFRMlen < offset)
       return ((uint8)1);
   else
   {
       /* send error if selected channels > max channels */
       if (ucData[1] > MAX_SCOPE_CHANNELS)
       {
    	   return ((uint8)1);
       }
       /* send error if no channels are configured */
       else if (ucData[1] == (uint8)0)
       {
    	   return ((uint8)1);
       }

	   /* IMPORTANT - DO NOT MOVE THIS SETION TO OTHER PARTS */
	   /* OF THE SAVE FUNCTION */
	   /* set state to idle & number of inputs to 0 to avoid */
	   /* undefined behaviour while updating the addresses */
	   /* & scope values if Scope Update function is */
	   /* executed in sample time irq */
	   pTScope->state = SCOPE_IDLE;
	   pTScope->noChannels = (uint8)0;

       /* Scope Save Frame contains the following parameters: */
       /* - Scope State (1 byte) */
       /* - Number of Inputs (1 byte) */
       /* - Sample Time Prescaler (2 bytes) */
	   /* - Channel #1 Source type (1 byte) */
       /* - Channel #1 Address (4 bytes) */
       /* - Channel #1 DataType (1 byte) */
       /* - ... */
	   /* - Channel #n Source type (1 byte) */
       /* - Channel #n Address (2 bytes) */
       /* - Channel #n DataType (1 byte) */
       /* - Trigger DataType (1 byte) */
	   /* - Trigger Source type (1 byte) */
       /* - Trigger Address (4 bytes) */
       /* - Trigger Level (length depends on trigger level size) */
	   /* - Trigger Delay (4 bytes) */
	   /* - Trigger Edge (1 byte) */
	   /* - Trigger mode (1 byte) */
       pTScope->timestamp = (uint16)0;
       pTScope->offlinePtr = (uint32)0;
       pTScope->stf = (uint16)ucData[2] + ((uint16)ucData[3] << 8);
       pTScope->stfCnt = pTScope->stf;
       pTScope->trgLastValue = (int32)0;

       pTScope->dataSizeTotal = (uint8)0;
       /* 2 bytes for 16 bit timestamp */
       pTScope->txFrameSize = (uint8)2;

	   pTScope->trgCountReached = (uint8)0;
	   pTScope->trgEventPos = (int32)0;

       while (i < ucData[1])
       {
    	   sourceType = ucData[ptr];
    	   ptr++;

    	   switch (sourceType)
    	   {
    	   case SOURCE_TYPE_CONTROLBLOCK:
			   elementId = (uint16)ucData[ptr] + ((uint16)ucData[ptr + 1] << 8);
			   paramId = (uint16)ucData[ptr + 2] + ((uint16)ucData[ptr + 3] << 8);

			   blockAddr = getBlockAddress(TableStruct->TParamTable, paramId);
			   if (blockAddr == (void*)0)
			   {
				   return ((uint8)1);
			   }
			   blockId = *(uint16*)blockAddr;
			   blockFunctions = getBlockFunction(TableStruct->TFncTable, blockId);
			   if (blockFunctions == (tBlockFunctions*)0)
			   {
				   return ((uint8)1);
			   }
			   pTScope->channelAddr[i] = blockFunctions->pFGetAddress(blockAddr, elementId);
			   if (pTScope->channelAddr[i] == (void*)0)
			   {
				   return ((uint8)1);
			   }
			   break;
    	   case SOURCE_TYPE_INPORTBLOCK:
    		   paramId = (uint16)ucData[ptr + 2] + ((uint16)ucData[ptr + 3] << 8);
			   blockAddr = getBlockAddress(TableStruct->TParamTable, paramId);
			   if (blockAddr == (void*)0)
			   {
				   return ((uint8)1);
			   }
			   pTScope->channelAddr[i] = blockAddr;
    		   break;
    	   case SOURCE_TYPE_OUTPORTBLOCk:
    		   paramId = (uint16)ucData[ptr + 2] + ((uint16)ucData[ptr + 3] << 8);
			   blockAddr = getBlockAddress(TableStruct->TParamTable, paramId);
			   if (blockAddr == (void*)0)
			   {
				   return ((uint8)1);
			   }
			   pTScope->channelAddr[i] = *(void**)blockAddr;
    		   break;
    	   case SOURCE_TYPE_ADDRESS:
    		   pTScope->channelAddr[i] = (void*)((uint32)ucData[ptr] + ((uint32)ucData[ptr + 1] << 8) + \
    				   ((uint32)ucData[ptr + 2] << 16) + ((uint32)ucData[ptr + 3] << 24));
    		   break;
    	   default:
    		   return ((uint8)1);
    	   }

          pTScope->dataSize[i] = (uint16)ucData[ptr + 4];

          pTScope->dataSizeTotal += pTScope->dataSize[i];
#if defined(__DATA_WIDTH_16BIT__)
          pTScope->txFrameSize += (pTScope->dataSize[i]*2);
#elif defined(__DATA_WIDTH_8BIT__)
          pTScope->txFrameSize += (pTScope->dataSize[i]);
#else
#error DATA WIDTH NOT DEFINED
#endif

          pTScope->maxUsedLength = SCOPE_SIZE - \
        		  (SCOPE_SIZE % pTScope->dataSizeTotal);

          ptr += (uint8)5;
          i++;
       }

       /* trigger parameters */

       /**
        * Trigger data type:
        * 0b 1000 0000
        *         ++++--- Data type byte size
        *       + ------- reserved
        *      +- ------- Signed (1) / Unsigned (0)
        *     +-- ------- Float (1) / Integer (0)
        *    +--- ------- New Version (1) / Old version (0)
        */
       pTScope->trgDataType = ucData[ptr];
       ptr++;
       sourceType = ucData[ptr];
       ptr++;
       switch (sourceType)
       {
	   case SOURCE_TYPE_CONTROLBLOCK:
		   elementId = (uint16)ucData[ptr] + ((uint16)ucData[ptr + 1] << 8);
		   paramId = (uint16)ucData[ptr + 2] + ((uint16)ucData[ptr + 3] << 8);

		   blockAddr = getBlockAddress(TableStruct->TParamTable, paramId);
		   if (blockAddr == (void*)0)
		   {
			   return ((uint8)1);
		   }
		   blockId = *(uint16*)blockAddr;
		   blockFunctions = getBlockFunction(TableStruct->TFncTable, blockId);
		   if (blockFunctions == (tBlockFunctions*)0)
		   {
			   return ((uint8)1);
		   }
		   pTScope->trgAddr = blockFunctions->pFGetAddress(blockAddr, elementId);
		   if (pTScope->trgAddr == (void*)0)
		   {
			   return ((uint8)1);
		   }
		   break;
	   case SOURCE_TYPE_INPORTBLOCK:
		   paramId = (uint16)ucData[ptr + 2] + ((uint16)ucData[ptr + 3] << 8);
		   blockAddr = getBlockAddress(TableStruct->TParamTable, paramId);
		   if (blockAddr == (void*)0)
		   {
			   return ((uint8)1);
		   }
		   pTScope->trgAddr = blockAddr;
		   break;
	   case SOURCE_TYPE_OUTPORTBLOCk:
		   paramId = (uint16)ucData[ptr + 2] + ((uint16)ucData[ptr + 3] << 8);
		   blockAddr = getBlockAddress(TableStruct->TParamTable, paramId);
		   if (blockAddr == (void*)0)
		   {
			   return ((uint8)1);
		   }
		   pTScope->trgAddr = *(void**)blockAddr;
		   break;
	   case SOURCE_TYPE_ADDRESS:
		   pTScope->trgAddr = (void*)((uint32)ucData[ptr] + ((uint32)ucData[ptr + 1] << 8) + \
				   ((uint32)ucData[ptr + 2] << 16) + ((uint32)ucData[ptr + 3] << 24));
		   break;
	   default:
		   return ((uint8)1);
       }
       ptr += (uint8)4;

       {
    	   uint8 trgLevelSize = pTScope->trgDataType & (uint8)0x0F;
    	   uint8 isNewVersion = pTScope->trgDataType & (uint8)0x80;

    	   switch (trgLevelSize)
    	   {
    	   case 1:
    		   pTScope->trgLevel = (uint8)ucData[ptr];
    		   break;
    	   case 2:
    		   pTScope->trgLevel = (uint16)ucData[ptr] + ((uint16)ucData[ptr + 1] << 8);
    		   break;
    	   case 4:
    		   pTScope->trgLevel = (uint32)ucData[ptr] + ((uint32)ucData[ptr + 1] << 8) + \
    		   	   ((uint32)ucData[ptr + 2] << 16) + ((uint32)ucData[ptr + 3] << 24);
    		   break;
    	   case 8:
    		   pTScope->trgLevel = (uint64)ucData[ptr] + ((uint64)ucData[ptr + 1] << 8) + \
    		   	   ((uint64)ucData[ptr + 2] << 16) + ((uint64)ucData[ptr + 3] << 24) + \
    		       ((uint64)ucData[ptr + 4] << 32) + ((uint64)ucData[ptr + 5] << 40) + \
    		       ((uint64)ucData[ptr + 6] << 48) + ((uint64)ucData[ptr + 7] << 56);
    		   break;
    	   default:
    		   /* data size not supported */
    		   return ((uint8)1);
    	   }

    	   if (!isNewVersion)
    	   {
    		   pTScope->trgDataType |= (uint8)0x20;
    		   ptr += (uint8)4;
    	   }
    	   else
    	   {
    		   ptr += trgLevelSize;
    	   }
       }

	   pTScope->trgDelay = ((int32)ucData[ptr] + ((int32)ucData[ptr + 1] << 8) + ((int32)ucData[ptr + 2] << 16) + \
			   ((int32)ucData[ptr + 3] << 24));
	   pTScope->trgCount = (uint32)getAbsValI32(pTScope->trgDelay);
	   ptr += (uint8)4;

	   pTScope->trgEdge = (tTrgEdgeType)ucData[ptr];
       ptr++;
       
       /* check if trigger mode information is being sent (compatibility check for previous Scope versions) */
       if (ucFRMlen > ptr)
       {
           /* get current trigger value if trigger mode is set to NORMAL */
           if (ucData[ptr] == TRG_MODE_NORMAL)
           {
               /* current value @ trg addr = last trg value */
               /* (avoids immediate trg event due to default last */
               /* trg value = 0 and current trg value > trg level */
               pTScope->trgLastValue = getTriggerValue(pTScope);
           }
       }
       else
       {
           pTScope->trgLastValue = getTriggerValue(pTScope);
       }

	   /* IMPORTANT - DO NOT MOVE THIS SETION TO OTHER PARTS */
	   /* OF THE SAVE FUNCTION */
	   /* 'number of inputs' & 'state' moved to end of save function */
	   /* to avoid undefined behaviour if save function is */
	   /* interrupted by sample time irq & program is executed */
	   /* in sample time irq */
	   pTScope->noChannels = ucData[1];
	   pTScope->state = (tScopeState)ucData[0];

       return ((uint8)0);
   }
}

/**
 * Returns block element address.
 */
void* Scope_Main_GetAddress(const SCOPE_MAIN* block, uint16 elementId)
{
	/* Scope doesn't have any elements so return null pointer */
	void* addr = (void*)0;
	return (addr);
}


/* this function will return 1, if the trigger was hit and */
/* 0, if the trigger was not hit */
static uint8 isTriggerEvent(SCOPE_MAIN *pTScope, uint64 curTrgValue)
{
	uint8 size = pTScope->trgDataType & (uint8)0x0F;
	uint8 isFloat = pTScope->trgDataType & (uint8)0x40;
	uint8 isEvent = (uint8)0;
	if (isFloat)
	{
		switch (size)
		{
			case 4:
			{
				float32 trgLastValue = *(float32*)&pTScope->trgLastValue;
				float32 trgLevel = *(float32*)&pTScope->trgLevel;
				float32 curValue = *(float32*)&curTrgValue;
				if (pTScope->trgEdge == EDGE_RISING)
				{
					if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
					{
						isEvent = (uint8)1;
					}
					else
					{
						isEvent = (uint8)0;
					}
				}
				else
				{
					if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
					{
						isEvent = (uint8)1;
					}
					else
					{
						isEvent = (uint8)0;
					}
				}
				break;
			}
			case 8:
			{
				float64 trgLastValue = *(float64*)&pTScope->trgLastValue;
				float64 trgLevel = *(float64*)&pTScope->trgLevel;
				float64 curValue = *(float64*)&curTrgValue;
				if (pTScope->trgEdge == EDGE_RISING)
				{
					if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
					{
						isEvent = (uint8)1;
					}
					else
					{
						isEvent = (uint8)0;
					}
				}
				else
				{
					if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
					{
						isEvent = (uint8)1;
					}
					else
					{
						isEvent = (uint8)0;
					}
				}
				break;
			}
		}
	}
	else
	{
		uint8 isSigned = pTScope->trgDataType & (uint8)0x20;
		if (isSigned)
		{
			switch (size)
			{
				case 1:
				{
					int8 trgLastValue = (int8)pTScope->trgLastValue;
					int8 trgLevel = (int8)pTScope->trgLevel;
					int8 curValue = (int8)curTrgValue;
					if (pTScope->trgEdge == EDGE_RISING)
					{
						if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					else
					{
						if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					break;
				}
				case 2:
				{
					int16 trgLastValue = (int16)pTScope->trgLastValue;
					int16 trgLevel = (int16)pTScope->trgLevel;
					int16 curValue = (int16)curTrgValue;
					if (pTScope->trgEdge == EDGE_RISING)
					{
						if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					else
					{
						if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					break;
				}
				case 4:
				{
					int32 trgLastValue = (int32)pTScope->trgLastValue;
					int32 trgLevel = (int32)pTScope->trgLevel;
					int32 curValue = (int32)curTrgValue;
					if (pTScope->trgEdge == EDGE_RISING)
					{
						if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					else
					{
						if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					break;
				}
				case 8:
				{
					int64 trgLastValue = (int64)pTScope->trgLastValue;
					int64 trgLevel = (int64)pTScope->trgLevel;
					int64 curValue = (int64)curTrgValue;
					if (pTScope->trgEdge == EDGE_RISING)
					{
						if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					else
					{
						if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					break;
				}
			}
		}
		else
		{
			switch (size)
			{
				case 1:
				{
					uint8 trgLastValue = (uint8)pTScope->trgLastValue;
					uint8 trgLevel = (uint8)pTScope->trgLevel;
					uint8 curValue = (uint8)curTrgValue;
					if (pTScope->trgEdge == EDGE_RISING)
					{
						if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					else
					{
						if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					break;
				}
				case 2:
				{
					uint16 trgLastValue = (uint16)pTScope->trgLastValue;
					uint16 trgLevel = (uint16)pTScope->trgLevel;
					uint16 curValue = (uint16)curTrgValue;
					if (pTScope->trgEdge == EDGE_RISING)
					{
						if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					else
					{
						if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					break;
				}
				case 4:
				{
					uint32 trgLastValue = (uint32)pTScope->trgLastValue;
					uint32 trgLevel = (uint32)pTScope->trgLevel;
					uint32 curValue = (uint32)curTrgValue;
					if (pTScope->trgEdge == EDGE_RISING)
					{
						if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					else
					{
						if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					break;
				}
				case 8:
				{
					uint64 trgLastValue = (uint64)pTScope->trgLastValue;
					uint64 trgLevel = (uint64)pTScope->trgLevel;
					uint64 curValue = (uint64)curTrgValue;
					if (pTScope->trgEdge == EDGE_RISING)
					{
						if ((trgLastValue < trgLevel) && (curValue >= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					else
					{
						if ((trgLastValue > trgLevel) && (curValue <= trgLevel))
						{
							isEvent = (uint8)1;
						}
						else
						{
							isEvent = (uint8)0;
						}
					}
					break;
				}
			}
		} /* isSigned */
	} /* isFloat */
	return (isEvent);
}

/* return current trigger value */
static uint64 getTriggerValue(SCOPE_MAIN *pTScope)
{
	uint64 trgValue;
	uint8 trgLevelSize = pTScope->trgDataType & (uint8)0x0F;
	switch (trgLevelSize)
	{
	case 1:
		trgValue = *(uint8*)pTScope->trgAddr;
		break;
	case 2:
		trgValue = *(uint16*)pTScope->trgAddr;
		break;
	case 4:
		trgValue = *(uint32*)pTScope->trgAddr;
		break;
	case 8:
		trgValue = *(uint64*)pTScope->trgAddr;
		break;
	default:
		/* unsupported size */
		break;
	}
	return (trgValue);
}

/* sample offline data and write it into offline array */
static void sampleData(SCOPE_MAIN *pTScope)
{
	uint8 i,j;

	i = 0;
    do
    {
    	j = 0;
        do
        {
        	*(ALIGNCASTPTR pTScope->arrayAddr + pTScope->offlinePtr++) = \
				*(ALIGNCASTPTR pTScope->channelAddr[i] + j);
            j++;
        }
        while (j < pTScope->dataSize[i]);

        i++;
    }
    while(i < pTScope->noChannels);
}

/*
 * Returns block functions from block function table.
 */
static tBlockFunctions* getBlockFunction(const tBlockFunctions* blockFuncTable, uint16 blockId)
{
	uint16 i = 0;
	tBlockFunctions* blockFunctionAddr;

	while ((blockId != blockFuncTable[i].iBlockID) && (blockFuncTable[i].iBlockID != 0))
	{
		i++;
	}
	/* check if Block ID can be found in function table */
	if (blockFuncTable[i].iBlockID == 0)
	{
		blockFunctionAddr = (tBlockFunctions*)0;
	}
	else
	{
		blockFunctionAddr = (tBlockFunctions*)&blockFuncTable[i];
	}
	return (blockFunctionAddr);
}

/*
 * Returns block address from parameter table.
 */
static void* getBlockAddress(const tParameterTable* paramTable, uint16 paramId)
{
	void* blockAddr;
	uint16 i;

	/* send parameter ID error if no parameter table has been initialized */
	if (paramTable == 0)
	{
		blockAddr = (void*)0;
	}
	else
	{
		i = 0;
		while((paramId != paramTable[i].uiParID) && (paramTable[i].uiParID != 0))
		{
			i++;
		}
		/* check if parameter is located in parameter table */
		if (paramTable[i].uiParID == 0)
		{
			blockAddr = (void*)0;
		}
		else
		{
			blockAddr = paramTable[i].pAdr;
		}
	}
	return (blockAddr);
}
