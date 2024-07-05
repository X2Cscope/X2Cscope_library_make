/*
 * Initializes application version & -date.
 *
 * $LastChangedRevision: 380 $
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#include "VersionInfo.h"

void initVersionInfo(volatile tTableStruct* tblStruct, const uint16 appVersion, compilationDate_t compilationDate)
{
	tblStruct->framePrgVersion = appVersion;
	tblStruct->framePrgCompDateTime = (uint8*)&compilationDate;
}
