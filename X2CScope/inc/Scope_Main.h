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

#if defined(SCOPE_SIZE)
/* Scope array size maximum size = 2^31-1 due to int32 variable types */
#if SCOPE_SIZE > 2147483647
#error SCOPE ARRAY SIZE MUST NOT EXCEED 2147483647 (2^31-1)
#endif

#else

#if defined(__TMS320F28XX__) || defined(__TMS320F2833X__) || \
		defined(__TMS320F2803X__) || defined(__TMS320F280X__) || \
		defined(__TMS320F2806X__) || defined(__TMS320F281X__) || defined(__SERIES_TM4C__)
#define SCOPE_SIZE ((uint16)4900)

#elif defined(__TMS320F2802X__)
#define SCOPE_SIZE ((uint16)512)

#elif defined(__STM32F10X__) || defined(__STM32F10XXX__) || defined(__SERIES_STM32F1__)
#define SCOPE_SIZE ((uint16)490)

#elif defined(__SERIES_STM32F4__) || defined(__SERIES_MPC5__)
#define SCOPE_SIZE ((uint16)4900)

#elif defined(__SERIES_DSPIC33F__) || defined(__SERIES_DSPIC33E__)
#define SCOPE_SIZE ((uint16)4900)

#elif defined(__SERIES_PIC32MZ__)
#define SCOPE_SIZE ((uint16)8192)

#elif defined(__RX6XX__)
#define SCOPE_SIZE ((uint16)4900)
#else
#error SCOPE ARRAY SIZE NOT DEFINED FOR CURRENT PROCESSOR
#endif

#endif

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
    (void  (*)(void*))Scope_Main_Init, \
    (uint8 (*)(void*, uint8*))Scope_Main_Load, \
    (uint8 (*)(void*, uint8*, uint8))Scope_Main_Save, \
	(void* (*)(void*, uint16))Scope_Main_GetAddress }

/* public prototypes */
void Scope_Main_Update(SCOPE_MAIN *pTScope);
void Scope_Main_Init(SCOPE_MAIN *pTScope);
uint8 Scope_Main_Load(SCOPE_MAIN *pTScope,uint8 *ucData);
uint8 Scope_Main_Save(SCOPE_MAIN *pTScope,uint8 *ucData, uint8 ucFRMlen);
void* Scope_Main_GetAddress(const SCOPE_MAIN* block, uint16 elementId);

#endif
