/**
 * @file
 * @brief Generic X2C block services.
 *
 * $LastChangedRevision: 781 $
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef __BLOCKSERVICESX2C_H__
#define __BLOCKSERVICESX2C_H__

#include "CommonFcts.h"

/* public prototypes */
void addBlockServices(tProtocol* protocol);
void addExtendedBlockServices(tProtocol* protocol);
void putBlockData(tProtocol* protocol);
void getBlockData(tProtocol* protocol);
void getRAMBlock(tProtocol* protocol);
void putRAMBlock(tProtocol* protocol);

#endif
