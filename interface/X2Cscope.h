/*
Copyright (c) [2012-2020] Microchip Technology Inc.  
    All rights reserved.
    You are permitted to use the accompanying software and its derivatives 
    with Microchip products. See the Microchip license agreement accompanying 
    this software, if any, for additional info regarding your rights and 
    obligations.
    
    MICROCHIP SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
    LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT 
    AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP OR ITS
    LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT 
    LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE 
    THEORY FOR ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT 
    LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
    OR OTHER SIMILAR COSTS. 
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
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
 */

/**
 * This file collects all X2Cscope external application interfaces that can be used.
 */
#ifndef X2CSCOPE_H
#define X2CSCOPE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <xc.h>
#include <stdint.h>

//X2Cscope buffer size in bytes, uses the specified bytes from RAM
#ifndef X2CSCOPE_BUFFER_SIZE
#define X2CSCOPE_BUFFER_SIZE 5000
#endif

//X2Cscope Get Device Info service, 2 bytes
//Application specific, can be customized. 
//Reserved APP versions, do not use: 0x1000 - 0x3FFF 
#ifndef X2CSCOPE_APP_VERSION
#define X2CSCOPE_APP_VERSION 0x0001
#endif

typedef const struct compilationDate_type{
	uint8_t date[11];
	uint8_t time[8];
}compilationDate_t;

/** Initialise X2Cscope buffer and LNET protocoll
 */
void X2Cscope_Initialise(void* scopeArray, uint16_t scopeSize, uint16_t appVersion, compilationDate_t compilationDate);

/* External X2Cscope API functions that must be used in the applications  */
void X2Cscope_Init(void); // Init X2Cscope and hook communication interfaces.
/* Below functions are implemented in X2Cscope library */
void X2Cscope_Communicate(void); // Communicate with X2Cscope GUI on PC side (Call in idle loop)
void X2Cscope_Update(void); // Sample point of scope functionality (Call with fixed periods)

#ifdef __cplusplus
}
#endif

#endif
