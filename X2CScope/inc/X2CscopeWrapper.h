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
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
 */

#ifndef __X2CSCOPEWRAPPER_H__
#define __X2CSCOPEWRAPPER_H__

#include "TableStruct.h"
#include "LNet.h"
#include "Services.h"
#include "BlockServicesX2C.h"
#include "Hardware.h"
#include "Target.h"
#include "X2C.h"
#include "VersionInfo.h"

/* generated X2C parameter identifier & block function table */
extern const tBlockFunctions blockFunctionTable[];
extern const tParameterTable parameterIdTable[];

/* X2C maintenance structure */
volatile tTableStruct TheTableStruct;
volatile tTableStruct *TableStruct = &TheTableStruct;

/* LNet protocol buffersize & node identifier */
#define LNET_BUFFERSIZE ((uint8)255)
#define LNET_NODEID ((uint8)1)

static tSerial interface;
static tLNet protocol;
static uint8 bufferLNet[LNET_BUFFERSIZE];

void X2Cscope_Initialise(void* scopeArray, uint16_t scopeSize, const uint16_t appVersion, compilationDate_t compilationDate);
void X2Cscope_Communicate();
void X2Cscope_Update();

#endif