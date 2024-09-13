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
 * This file is part of X2C. http://www.mechatronic-simulation.org/
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#include "Services.h"

/* monitor program version */
#define DEVINFO_MONITOR_VERSION ((uint16)0x0005)

#if defined(__GENERIC_TI_C28X__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8110) 
#elif defined(__GENERIC_MICROCHIP_DSPIC__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8210) 
#elif defined(__GENERIC_MICROCHIP_PIC32__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8220)
#elif defined(__GENERIC_MICROCHIP_PIC24__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8230) 
#elif defined(X2C_GENERIC_MICROCHIP_DSPIC33A)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8240)
#elif defined(__GENERIC_ARM_ARMV7__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8310)
#elif defined(__GENERIC_ARM_ARMV6__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8320)
#elif defined(__GENERIC_X86__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8410)
#elif defined(__GENERIC_X64__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x8420)

/* TMS320F240x */
#elif defined(__TMS320F2401__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0111)
#elif defined(__TMS320F2402__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0112)
#elif defined(__TMS320F2403__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0113)
#elif defined(__TMS320F2406__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0114)
#elif defined(__TMS320F2407__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0115)

/* TMS320F280x */
#elif defined(__TMS320F2801__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0121)
#elif defined(__TMS320F2802__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0122)
#elif defined(__TMS320F2806__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0123)
#elif defined(__TMS320F2808__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0124)
#elif defined(__TMS320F2809__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0125)

/* TMS320F281x */
#elif defined(__TMS320F2810__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0131)
#elif defined(__TMS320F2811__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0132)
#elif defined(__TMS320F2812__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0133)

/* TMS320F2833x */
#elif defined(__TMS320F28332__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0141)
#elif defined(__TMS320F28334__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0142)
#elif defined(__TMS320F28335__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0143)

/* TMS320F2803x */
#elif defined(__TMS320F28035__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0151)
#elif defined(__TMS320F28034__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0152)

/* TMS320F2806x */
#elif defined(__TMS320F28069__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0161)

/* TM4C */
#elif defined(__TM4C123GH6__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0171)
#elif defined(__TM4C123BE6__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0172)

/* TMS320F2802x */
#elif defined(__TMS320F28027__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0181)

/* DSPIC33F */
#elif defined(__DSPIC33FJ256MC710__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0221)
#elif defined(__DSPIC33FJ128MC706__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0222)
#elif defined(__DSPIC33FJ128MC506__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0223)
#elif defined(__DSPIC33FJ64GS610__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0224)
#elif defined(__DSPIC33FJ64GS406__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0225)
#elif defined(__DSPIC33FJ12GP202__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0226)
#elif defined(__DSPIC33FJ128MC802__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0228)

/* dsPIC33E */
#elif defined(__DSPIC33EP256MC506__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0231)
#elif defined(__DSPIC33EP128GP502__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0232)
#elif defined(__DSPIC33EP32GP502__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0233)
#elif defined(__DSPIC33EP256GP502__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0234)
#elif defined(__DSPIC33EP256MC502__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0235)
#elif defined(__DSPIC33EP128MC202__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0236)
#elif defined(__DSPIC33EP128GM604__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0237)

/* PIC32MZ */
#elif defined(__PIC32MZ2048EC__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0241)

/* PIC32MX */
#elif defined(__PIC32MX170F256__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0251)

/* STM32F103xx */
#elif defined(__STM32F103VB__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0311)
#elif defined(__STM32F103T6__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0312)
#elif defined(__STM32F103V8__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0313)
#elif defined(__STM32F103T4__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0314)
#elif defined(__STM32F103ZC__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0315)

/* STM32F101 */
#elif defined(__STM32F101C4__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0321)
#elif defined(__STM32F101C6__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0322)

/* STM32F100 */
#elif defined(__STM32F100C6__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0331)

/* STM32F4 */
#elif defined(__STM32F407ZG__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0341)
#elif defined(__STM32F407ZE__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0342)
#elif defined(__STM32F407VG__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0343)

/* STM32F0 */
#elif defined(__STM32F051R8__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0351)
#elif defined(__STM32F051C8__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0352)

#elif defined(__STM32F303RE__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0361)
#elif defined(__STM32F303RB__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0362)

/* MC56F8xxx */
#elif defined(__MC56F8XXX__)
/* mc56f8345 -> dsp id = 0x411 */
#define DEVINFO_PROCESSOR_ID ((uint16)0x0411)
#elif defined(__MPC5643L__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0421)

#elif defined(__RX62T__) || defined(__R5F562TA__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0511)
#elif defined(__RX62G__) || defined(__R5F562GA__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0512)

#elif defined(__KECONTROL__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0611)

#elif defined(__XMC4800F144K2048__)
#define DEVINFO_PROCESSOR_ID ((uint16)0x0711)

#else
#error PROCESSOR TYPE NOT DEFINED
#endif

/* service table definition */
static tSERVICE_Table serviceTable;

#if defined(__COMPILER_CODEWARRIOR_ECLIPSE__)
#pragma push
#pragma section sdata_type ".ptrTableStruct" ".ptrTableStruct"
extern volatile tTableStruct *TableStruct;
#pragma pop
#else
extern volatile tTableStruct* TableStruct;
#endif

/* private prototypes */
static void getDeviceInfo(tProtocol* protocol);
static void getTargetState(tProtocol* protocol);
static void setTargetState(tProtocol* protocol);
static void loadParameter(tProtocol* protocol);
static void saveParameter(tProtocol* protocol);
static void* getBlockAddress(const tParameterTable* paramTable, uint16 paramId, uint8* error);

/* send service not available error */
void sendSvNotAvailable(tProtocol* protocol)
{
	protocol->ucFRAMESize = 2;
	protocol->ucFRAMEData[1] = ERRORServiceNotAvail;
	protocol->pSnd_Enable((struct tProtocol*)protocol);
}

/* send error */
void sendError(tProtocol* protocol, uint8 ucErrorNr)
{
	protocol->ucFRAMESize = 2;
	protocol->ucFRAMEData[1] = ucErrorNr;
	protocol->pSnd_Enable((struct tProtocol*)protocol);
}

/*
 * Returns target device information.
 */
static void getDeviceInfo(tProtocol* protocol)
{
	uint8 i;
	uint32 tableStructAddr;

	/* check frame buffer length */
	if (protocol->ucMaxCommSize < 46)
	{
		sendError(protocol, ERRORSizeTooLarge);
		return;
	}

	protocol->ucFRAMESize = 46;
	protocol->ucFRAMEData[1] = ERRORSuccess;

	/* monitor program version */
	protocol->ucFRAMEData[2] = (uint8)(DEVINFO_MONITOR_VERSION & 0x00FF);
	protocol->ucFRAMEData[3] = (uint8)(DEVINFO_MONITOR_VERSION >> 8);

	/* frame program version */
	protocol->ucFRAMEData[4] = \
			(uint8)(TableStruct->framePrgVersion & (uint16)0x00FF);
	protocol->ucFRAMEData[5] = (uint8)(TableStruct->framePrgVersion >> 8);

	/* max comm size */
	protocol->ucFRAMEData[6] = protocol->ucMaxCommSize;

	protocol->ucFRAMEData[7] = (uint8)(DEVINFO_PROCESSOR_ID & 0x00FF);
	protocol->ucFRAMEData[8] = (uint8)(DEVINFO_PROCESSOR_ID >> 8);

	/* monitor program compilation date as ASCII string */
	/* 9 ... 11 -> first 3 letters of month (e.g. Oct, Dec) */
	/* 12 ... 13 -> day as DD */
	/* 14 ... 17 -> year as YYYY */
	protocol->ucFRAMEData[9] = __DATE__[0];
	protocol->ucFRAMEData[10] = __DATE__[1];
	protocol->ucFRAMEData[11] = __DATE__[2];
	protocol->ucFRAMEData[12] = __DATE__[4];
	protocol->ucFRAMEData[13] = __DATE__[5];
	protocol->ucFRAMEData[14] = __DATE__[7];
	protocol->ucFRAMEData[15] = __DATE__[8];
	protocol->ucFRAMEData[16] = __DATE__[9];
	protocol->ucFRAMEData[17] = __DATE__[10];

	/* monitor program compilation time as ASCII string */
	/* 18 ... 19 -> hour as HH */
	/* 20 ... 21 -> minute as MM */
	protocol->ucFRAMEData[18] = __TIME__[0];
	protocol->ucFRAMEData[19] = __TIME__[1];
	protocol->ucFRAMEData[20] = __TIME__[3];
	protocol->ucFRAMEData[21] = __TIME__[4];

	if (TableStruct->framePrgCompDateTime == 0)
	{
		i = 12;
		do
		{
			protocol->ucFRAMEData[22+i] = '-';
		} while (i-- > 0);
	}
	else
	{
		/* frame program compilation date as ASCII string */
		/* 22 ... 24 -> first 3 letters of month (e.g. Oct, Dec) */
		/* 25 ... 26 -> day as DD */
		/* 27 ... 30 -> year as YYYY */
		/* frame program compilation time as ASCII string */
		/* 31 ... 32 -> hour as HH */
		/* 23 ... 24 -> minute as MM */
		protocol->ucFRAMEData[22] = TableStruct->framePrgCompDateTime[0];
		protocol->ucFRAMEData[23] = TableStruct->framePrgCompDateTime[1];
		protocol->ucFRAMEData[24] = TableStruct->framePrgCompDateTime[2];
		protocol->ucFRAMEData[25] = TableStruct->framePrgCompDateTime[4];
		protocol->ucFRAMEData[26] = TableStruct->framePrgCompDateTime[5];
		protocol->ucFRAMEData[27] = TableStruct->framePrgCompDateTime[7];
		protocol->ucFRAMEData[28] = TableStruct->framePrgCompDateTime[8];
		protocol->ucFRAMEData[29] = TableStruct->framePrgCompDateTime[9];
		protocol->ucFRAMEData[30] = TableStruct->framePrgCompDateTime[10];
		protocol->ucFRAMEData[31] = TableStruct->framePrgCompDateTime[11];
		protocol->ucFRAMEData[32] = TableStruct->framePrgCompDateTime[12];
		protocol->ucFRAMEData[33] = TableStruct->framePrgCompDateTime[14];
		protocol->ucFRAMEData[34] = TableStruct->framePrgCompDateTime[15];
	}

	protocol->ucFRAMEData[35] = (uint8)(TableStruct->DSPState & 0x00FF);
	protocol->ucFRAMEData[36] = (uint8)(TableStruct->eventType & 0x00FF);
	protocol->ucFRAMEData[37] = (uint8)(TableStruct->eventType >> 8);
	protocol->ucFRAMEData[38] = (uint8)(TableStruct->eventId & 0x00FF);
	protocol->ucFRAMEData[39] = (uint8)(TableStruct->eventId >> 8);
	protocol->ucFRAMEData[40] = (uint8)(TableStruct->eventId >> 16);
	protocol->ucFRAMEData[41] = (uint8)(TableStruct->eventId >> 24);

	tableStructAddr = (uint32)TableStruct;
	protocol->ucFRAMEData[42] = (uint8)(tableStructAddr & 0xFF);
	protocol->ucFRAMEData[43] = (uint8)((tableStructAddr >> 8) & 0xFF);
	protocol->ucFRAMEData[44] = (uint8)((tableStructAddr >> 16) & 0xFF);
	protocol->ucFRAMEData[45] = (uint8)((tableStructAddr >> 24) & 0xFF);

	protocol->pSnd_Enable((struct tProtocol*)protocol);
}

/*
 * Saves block data by parameter identifier.
 */
static void saveParameter(tProtocol* protocol)
{
	uint16 blockId, paramId;
	uint16 i;
	uint8 error;
	void* blockAddr;

	/* get parameter id from frame */
	paramId = (uint16)((protocol->ucFRAMEData[2]<<8) + protocol->ucFRAMEData[1]);

	blockAddr = getBlockAddress(TableStruct->TParamTable, paramId, &error);
	if (error != ERRORSuccess)
	{
		sendError(protocol, error);
		return;
	}


	/* try to execute limit-save function if limit-save function table is initialized */
	if (TableStruct->TLimitSaveFncTable != 0)
	{
		/* look if parameter id has limit function */
		i = 0;
		while ((paramId != (TableStruct->TLimitSaveFncTable)[i].uiParID) && \
				((TableStruct->TLimitSaveFncTable)[i].uiParID != 0))
		{
			i++;
		}

		if ((TableStruct->TLimitSaveFncTable)[i].uiParID != 0)
		{
			/* if limit-save function fails -> send error */
			if ((TableStruct->TLimitSaveFncTable)[i].pFLimitSave(blockAddr, protocol->ucFRAMEData+3, protocol->ucFRAMESize-3) != 0)
			{
				sendError(protocol, ERRORParLimit);
			}
			return;
		}
	}

	if (TableStruct->TFncTable == 0)
	{
		sendError(protocol, SvErrorFncTableNotInit);
		return;
	}

	/* get block identifier */
	blockId = *(uint16*)blockAddr;

	i = 0;
	while ((blockId != (TableStruct->TFncTable)[i].iBlockID) && ((TableStruct->TFncTable)[i].iBlockID != 0))
	{
		i++;
	}
	/* check if Block ID can be found in function table */
	if ((TableStruct->TFncTable)[i].iBlockID == 0)
	{
		sendError(protocol, ERRORBlkID);
		return;
	}

	/* if default save function fails -> send error */
	if ((TableStruct->TFncTable)[i].pFSave(blockAddr, protocol->ucFRAMEData+3, protocol->ucFRAMESize-3))
	{
		sendError(protocol, ERRORFormat);
		return;
	}

	protocol->ucFRAMESize = 2;
	protocol->ucFRAMEData[1] = ERRORSuccess;
	protocol->pSnd_Enable((struct tProtocol*)protocol);
}


/*
 * Loads block data by parameter identifier.
 */
static void loadParameter(tProtocol* protocol)
{
	void* blockAddr;
	uint16 blockId, paramId;
	uint16 i;
	uint8 error;

	/* get parameter id from frame */
	paramId = (uint16)((protocol->ucFRAMEData[2]<<8) + protocol->ucFRAMEData[1]);

	/* look for parameter identifier in parameter table */
	blockAddr = getBlockAddress(TableStruct->TParamTable, paramId, &error);
	if (error != ERRORSuccess)
	{
		sendError(protocol, error);
		return;
	}

	/* returns error if function table is not initialized */
	if (TableStruct->TFncTable == 0)
	{
		sendError(protocol, SvErrorFncTableNotInit);
		return;
	}

	blockId = *(uint16*)blockAddr;

	i = 0;
	while ((blockId != (TableStruct->TFncTable)[i].iBlockID) && ((TableStruct->TFncTable)[i].iBlockID != 0))
	{
		i++;
	}
	/* check if Block ID can be found in function table */
	if (((tBlockFunctions*)(TableStruct->TFncTable))[i].iBlockID == 0)
	{
		sendError(protocol, ERRORBlkID);
		return;
	}

	protocol->ucFRAMESize = (TableStruct->TFncTable)[i].pFLoad(blockAddr, protocol->ucFRAMEData+2) + 2;
	protocol->ucFRAMEData[1] = ERRORSuccess;
	protocol->pSnd_Enable((struct tProtocol*)protocol);
}

/*
 * Sets target state.
 */
static void setTargetState(tProtocol* protocol)
{
	uint8 error = ERRORSuccess;

	switch (protocol->ucFRAMEData[1])
	{
	case 0:
		TableStruct->DSPState = MONITOR_STATE;
		break;
	case 1:
		TableStruct->DSPState = PRG_LOADED_STATE;
		break;
	case 2:
		TableStruct->DSPState = IDLE_STATE;
		break;
	case 3:
		TableStruct->DSPState = INIT_STATE;
		break;
	case 4:
		TableStruct->DSPState = RUN_STATE_POWER_OFF;
		break;
	case 5:
		TableStruct->DSPState = RUN_STATE_POWER_ON;
		break;
	default:
		error = SvErrorInvalidDspState;
		break;
	}

	protocol->ucFRAMEData[1] = error;
	protocol->ucFRAMESize = (uint8)2;
	protocol->pSnd_Enable((struct tProtocol*)protocol);
}

/*
 * Returns target state.
 */
static void getTargetState(tProtocol* protocol)
{
	protocol->ucFRAMEData[1] = ERRORSuccess;
	protocol->ucFRAMEData[2] = (uint8)(TableStruct->DSPState & 0xFF);
	protocol->ucFRAMESize = (uint8)3;
	protocol->pSnd_Enable((struct tProtocol*)protocol);
}


/*
 * Initializes service table.
 */
void initServiceTable(tProtocol* protocol)
{
	uint16 i;

	for (i = 0; i <= MAX_SERVICE_ID; i++)
	{
		serviceTable[i] = sendSvNotAvailable;
	}

	/* adds service table pointer to protocol structure */
	protocol->pServiceTable = (void*)serviceTable;
}

/*
 * Adds core services to service table.
 */
void addCoreServices(tProtocol* protocol)
{
	tSERVICEFunction* svTable = (tSERVICEFunction*)protocol->pServiceTable;
	svTable[SV_ID_SVDEVICEINFO] = getDeviceInfo;
	svTable[SV_ID_GETTARGETSTATE] = getTargetState;
	svTable[SV_ID_SETTARGETSTATE] = setTargetState;
	svTable[SV_ID_SVSAVEPARAM] = saveParameter;
	svTable[SV_ID_SVLOADPARAM] = loadParameter;
}

/*
 * Returns block address from parameter table.
 * Also checks if parameter table has been initialized.
 */
static void* getBlockAddress(const tParameterTable* paramTable, uint16 paramId, uint8* error)
{
	void* blockAddr;
	uint16 i;

	/* send parameter ID error if no parameter table has been initialized */
	if (paramTable == 0)
	{
		blockAddr = (void*)0;
		*error = SvErrorParamTableNotInit;
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
			*error = SvErrorInvalidParamId;
		}
		else
		{
			blockAddr = paramTable[i].pAdr;
			*error = ERRORSuccess;
		}
	}
	return (blockAddr);
}
