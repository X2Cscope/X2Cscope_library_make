/*
 * Table structure type definition and -functions.
 *
 * $LastChangedRevision: 443 $
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#include "TableStruct.h"

#if defined(__COMPILER_CODEWARRIOR_ECLIPSE__)
#pragma push
#pragma section sdata_type ".ptrTableStruct" ".ptrTableStruct"
extern volatile tTableStruct *TableStruct;
#pragma pop
#else
extern volatile tTableStruct* TableStruct;
#endif

/* init tablestruct */
void initTableStruct(void)
{
	uint8 i;

	TableStruct->TParamTable = (tParameterTable*)0;
	TableStruct->TFncTable = (tBlockFunctions*)0;
	TableStruct->TLimitSaveFncTable = (tLimitSaveFunctionTable*)0;

	TableStruct->DSPState = MONITOR_STATE;
	TableStruct->eventType = (uint16)0x0000;
	TableStruct->eventId = (uint32)0x00000000;

	TableStruct->protocolCount = (uint16)0;
	for (i = 0; i < MAX_PROTOCOLS; i++)
	{
		TableStruct->protocols[i] = (tProtocol*)0;
	}

	TableStruct->framePrgVersion = (uint16)0x0000;
	TableStruct->framePrgCompDateTime = (uint8*)0;

	TableStruct->piScope = (SCOPE_MAIN*)0;
	TableStruct->dynamicCodeData = (tDynamicCodeData*)0;
}


/* add tProtocol to TableStruct */
void addTableStructProtocol(tProtocol* linkProtocol)
{
	if (TableStruct->protocolCount < MAX_PROTOCOLS)
	{
		TableStruct->protocols[TableStruct->protocolCount++] = linkProtocol;
	}
}
