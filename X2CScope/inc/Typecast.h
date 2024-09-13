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

 * Typecast functions.
 * Provides functions to cast unsigned to signed datatypes with an memory data width greater than the data type.
 * For example: Cast uint8 to int8 on a target using memory with 16-bit data width
 *
 * $LastChangedRevision: 684 $
 * $LastChangedDate:: 2024-09-13 13:00:00 +0200#$
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
