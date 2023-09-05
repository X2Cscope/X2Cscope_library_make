/*
 * Initializes application version & -date.
 *
 * $LastChangedRevision: 380 $
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef __VERSIONINFO_H__
#define __VERSIONINFO_H__

#include "CommonFcts.h"
#include "../../interface/X2CScope.h"
/* public prototypes */
void initVersionInfo(volatile tTableStruct* tblStruct, const uint16 appVersion, compilationDate_t compilationDate);

#endif
