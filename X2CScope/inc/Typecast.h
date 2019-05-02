/*
 * Typecast functions.
 * Provides functions to cast unsigned to signed datatypes with an memory data width greater than the data type.
 * For example: Cast uint8 to int8 on a target using memory with 16-bit data width
 *
 * $LastChangedRevision: 684 $
 *
 * Copyright (c) 2013, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef __TYPECAST_H__
#define __TYPECAST_H__

#if defined(__DATA_WIDTH_32BIT__)
	#define UINT8_TO_INT8(data)   ((int8)(((int16)(data) << 24) >> 24))
	#define UINT16_TO_INT16(data) ((int16)(((int32)(data) << 16) >> 16))
	#define UINT32_TO_INT32(data) ((int32)(data))
	#define UINT64_TO_INT64(data) ((int64)(data))

#elif defined(__DATA_WIDTH_16BIT__)
	#define UINT8_TO_INT8(data)   ((int8)(((int16)(data) << 8) >> 8))
	#define UINT16_TO_INT16(data) ((int16)(data))
	#define UINT32_TO_INT32(data) ((int32)(data))
	#define UINT64_TO_INT64(data) ((int64)(data))

#elif defined(__DATA_WIDTH_8BIT__)
	#define UINT8_TO_INT8(data)   ((int8)(data))
	#define UINT16_TO_INT16(data) ((int16)(data))
	#define UINT32_TO_INT32(data) ((int32)(data))
	#define UINT64_TO_INT64(data) ((int64)(data))

#elif defined(__MATLAB__) || defined(__SCILAB__)
	#define UINT8_TO_INT8(data)   ((int8)(data))
	#define UINT16_TO_INT16(data) ((int16)(data))
	#define UINT32_TO_INT32(data) ((int32)(data))
	#define UINT64_TO_INT64(data) ((int64)(data))

#else
	#error DATA WIDTH NOT DEFINED
#endif

#endif
