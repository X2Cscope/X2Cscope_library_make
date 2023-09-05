/**
 * @file
 * @brief Scope
 */
/*
 * $LastChangedRevision: 994 $
 * $LastChangedDate:: 2016-08-10 18:47:26 +0200#$
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
    (void  (*)(void*, uint16))Scope_Main_Init, \
    (uint8 (*)(void*, uint8*))Scope_Main_Load, \
    (uint8 (*)(void*, uint8*, uint8))Scope_Main_Save, \
	(void* (*)(void*, uint16))Scope_Main_GetAddress }

/* public prototypes */
void Scope_Main_Update(SCOPE_MAIN *pTScope);
void Scope_Main_Init(SCOPE_MAIN *pTScope, uint16 scope_size);
uint8 Scope_Main_Load(SCOPE_MAIN *pTScope,uint8 *ucData);
uint8 Scope_Main_Save(SCOPE_MAIN *pTScope,uint8 *ucData, uint8 ucFRMlen);
void* Scope_Main_GetAddress(const SCOPE_MAIN* block, uint16 elementId);

#endif
