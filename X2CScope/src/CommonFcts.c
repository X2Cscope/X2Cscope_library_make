/* $LastChangedRevision: 842 $
 * $LastChangedDate:: 2015-11-26 15:56:03 +0100#$
 *
 * Copyright (c) 2015, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#include "CommonFcts.h"

/** Pseudo Initialization */
void Common_Init(void* pTCommon)
{
}

/** Pseudo Update */
void Common_Update(void* pTCommon)
{
}

/** Pseudo Load block data */
int16 Common_Load(const void* pTCommon, uint8 ucData[])
{
    return ((int16)0);
}

/** Pseudo Save block data */
int16 Common_Save(void* pTCommon, const uint8 ucData[], uint8 ucFrameLength)
{
	int16 retVal;

    if (ucFrameLength != (uint8)0)
    {
    	retVal = (int16)1;
    }
    else
    {
    	retVal = (int16)0;
    }
    return (retVal);
}

/** Pseudo Get Address */
void* Common_GetAddress(const void* pTCommon, uint16 elementId)
{
	return ((void*)0);
}

/* abs(x) functions */

/** @brief Calculation of 8-bit absolute value.
 *
 * out = |in|
 * 
 * @param x 8-bit fixed point input value.
 * @return Absolute value of input.
 */
int8 getAbsValI8(int8 x){
	if (x == ((int8)0x80)){
		x = (int8)0x7F;
	}else if (x < 0){
		x = -(x);
	}
	return (x);
}

/** @brief Calculation of 16-bit absolute value.
 *
 * out = |in|
 * 
 * @param x 16-bit fixed point input value.
 * @return Absolute value of input.
 */
int16 getAbsValI16(int16 x){
	if (x == ((int16)0x8000)){
		x = (int16)0x7FFF;
	}else if (x < 0){
		x = -(x);
	}
	return (x);
}

/** @brief Calculation of 32-bit absolute value.
 *
 * out = |in|
 * 
 * @param x 32-bit fixed point input value.
 * @return Absolute value of input.
 */
int32 getAbsValI32(int32 x){
	if (x == ((int32)0x80000000)){
		x = (int32)0x7FFFFFFF;
	}else if (x < 0){
		x = -(x);
	}
	return (x);
}

/** @brief Calculation of 64-bit absolute value.
 *
 * out = |in|
 * 
 * @param x 64-bit fixed point input value.
 * @return Absolute value of input.
 */
int64 getAbsValI64(int64 x){
	if (x == ((int64)0x8000000000000000)){
		x = (int64)0x7FFFFFFFFFFFFFFF;
	}else if (x < 0){
		x = -(x);
	}
	return (x);
}

/** @brief Calculation of single precision absolute value.
 *
 * out = |in|
 * 
 * @param x 32-bit floating point input value.
 * @return Absolute value of input.
 */
float32 getAbsValR32(float32 x){
	if (x < 0){
		x = -(x);
	}
	return (x);
}

/** @brief Calculation of double precision absolute value.
 *
 * out = |in|
 * 
 * @param x 64-bit floating point input value.
 * @return Absolute value of input.
 */
float64 getAbsValR64(float64 x){
	if (x < 0){
		x = -(x);
	}
	return (x);
}
