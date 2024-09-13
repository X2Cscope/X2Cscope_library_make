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
 * $LastChangedRevision: 994 $
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef SCOPE_MAIN_H
#define SCOPE_MAIN_H

#include "CommonFcts.h"

#define SCOPE_MAIN_ID ((uint16)65535)

#if defined(__DATA_WIDTH_8BIT__)
#define ALIGNTYPE int8
#define ALIGNCASTPTR (int8*)
#elif defined(__DATA_WIDTH_16BIT__)
#define ALIGNTYPE int16
#define ALIGNCASTPTR (int16*)
#else
#error DATA WIDTH NOT DEFINED
#endif

#define SCOPE_MAIN_FUNCTIONS { SCOPE_MAIN_ID, \
    (void  (*)(void*))Scope_Main_Update, \
    (void  (*)(void*, void*, uint16))Scope_Main_Init, \
    (uint8 (*)(void*, uint8*))Scope_Main_Load, \
    (uint8 (*)(void*, uint8*, uint8))Scope_Main_Save, \
	(void* (*)(void*, uint16))Scope_Main_GetAddress }

/* public prototypes */
void Scope_Main_Update(SCOPE_MAIN *pTScope);
void Scope_Main_Init(SCOPE_MAIN *pTScope,void *scopeArray, uint16 scope_size);
uint8 Scope_Main_Load(SCOPE_MAIN *pTScope,uint8 *ucData);
uint8 Scope_Main_Save(SCOPE_MAIN *pTScope,uint8 *ucData, uint8 ucFRMlen);
void* Scope_Main_GetAddress(const SCOPE_MAIN* block, uint16 elementId);

#endif
