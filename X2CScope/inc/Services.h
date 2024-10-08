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
 * $LastChangedRevision: 1025 $
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
 *
 * This file is part of X2C. http://www.mechatronic-simulation.org/
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef SERVICES_H
#define SERVICES_H

#include "CommonFcts.h"

#define	MAX_SERVICE_ID ((uint8)26)

/* service identifiers */
#define SV_ID_SVDEVICEINFO    ((uint8)0x00)
#define SV_ID_GETTARGETSTATE  ((uint8)0x01)
#define SV_ID_SETTARGETSTATE  ((uint8)0x02)
#define SV_ID_SVERASEFLASH    ((uint8)0x04)
#define SV_ID_SVGETM2CBLOCK   ((uint8)0x07) /* deprecated - don't use */
#define SV_ID_SVPUTM2CBLOCK   ((uint8)0x08) /* deprecated - don't use */
#define SV_ID_GETBLOCKDATA   ((uint8)0x07)
#define SV_ID_PUTBLOCKDATA   ((uint8)0x08)
#define SV_ID_SVGETRAMBLOCK   ((uint8)0x09)
#define SV_ID_SVPUTRAMBLOCK   ((uint8)0x0A)
#define SV_ID_SVGETFLASHBLOCK ((uint8)0x0B)
#define SV_ID_SVPUTFLASHBLOCK ((uint8)0x0C)
#define SV_ID_SVLOADEE        ((uint8)0x0F)
#define SV_ID_SVSAVEEE        ((uint8)0x10)
#define SV_ID_SVLOADPARAM     ((uint8)0x11)
#define SV_ID_SVSAVEPARAM     ((uint8)0x12)
#define SV_ID_SVREBOOT        ((uint8)0x19)
#define SV_ID_SVOSMCONTROL    ((uint8)0x1A)

/* data type bitwidths */
#define DATATYPE_8BIT  ((uint8)1)
#define DATATYPE_16BIT ((uint8)2)
#define DATATYPE_32BIT ((uint8)4)
#define DATATYPE_64BIT ((uint8)8)

typedef void (*tSERVICEFunction)(tProtocol* protocol);
typedef tSERVICEFunction tSERVICE_Table[MAX_SERVICE_ID+1];

/* error identifiers */
#define ERRORSuccess             ((uint8)0x00)
#define ERRORChksum              ((uint8)0x13)
#define ERRORFormat              ((uint8)0x14)
#define ERRORSizeTooLarge        ((uint8)0x15)
#define ERRORServiceNotAvail     ((uint8)0x21)
#define SvErrorInvalidDspState   ((uint8)0x22)
#define ERRORFlashWrite          ((uint8)0x30)
#define ERRORFlashWriteProtect   ((uint8)0x31)
#define SvErrorInvalidParamId    ((uint8)0x40)
#define ERRORBlkID               ((uint8)0x41)
#define ERRORParLimit            ((uint8)0x42)
#define SvErrorParamTableNotInit ((uint8)0x43)
#define SvErrorFncTableNotInit   ((uint8)0x44)
#define ERRORPowerIsOn           ((uint8)0x50)

/* public prototypes */
void sendSvNotAvailable(tProtocol* protocol);
void sendError(tProtocol* protocol, uint8 ucErrorNr);
void initServiceTable(tProtocol* protocol);
void addCoreServices(tProtocol* protocol);

#endif
