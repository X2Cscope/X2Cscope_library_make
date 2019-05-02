/** @file
 * @brief Common functions and defines.
 */ 
/* $LastChangedRevision: 994 $
 * $LastChangedDate:: 2016-08-10 18:47:26 +0200#$
 *
 * Copyright (c) 2015, Linz Center of Mechatronics GmbH (LCM) http://www.lcm.at/
 * All rights reserved.
 */
#ifndef COMMONFCTS_H
#define COMMONFCTS_H

#include "Target.h"

#define LOW(data)  ((data) & 0xFF)
#define HIGH(data) (((uint16)(data)) >> 8)

/* TODO deprecated */
#define NULL_BLK_FNC    {(uint16)0, (void (*)(void*))0, (void (*)(void*))0, \
	(uint8 (*)(void*, uint8[]))0, (uint8 (*)(void*, uint8[], uint8))0}
/* TODO deprecated */
#define NULL_PARAM      {(uint16)0, (void*)0}

/* Limitation */
#define LIMIT(data, limit) 								\
					do{									\
						if((data) > (limit)){         	\
                        	(data) = (limit);			\
                        }else{							\
                        	if((data) < -(limit)){   	\
                            	(data) = -(limit);		\
                            }							\
                        }								\
                    }while(0)
                        
/* Lookup table 8 bit (256+1 values) */
#define LOOKUP8(Table, Index, RetVal)						\
					do{										\
						(RetVal) = Table[(uint8)(Index)];	\
					}while(0)

/* Lookup table with 16 bit interpolation (256+1 values) */
#ifdef __XC16__
                    #define LOOKUP16(Table, Index, RetVal)								\
					do{																	\
						int16 tbl;														\
						int32 tmp;														\
						(RetVal)  = (Table)[(uint8)((uint16)(Index) >> 8)];				\
						tbl       = (Table)[(uint8)((uint16)(Index) >> 8) + 1];			\
						tmp       = __builtin_mulss((tbl-(RetVal)),((Index) & 0x00FF));	\
						(RetVal) += ((int16)(tmp >> 8));								\
					}while(0)
#else
#define LOOKUP16(Table, Index, RetVal)													\
					do{																	\
						int16 tbl;														\
						int32 tmp;														\
						(RetVal)  = (Table)[(uint8)((uint16)(Index) >> 8)];				\
						tbl       = (Table)[(uint8)((uint16)(Index) >> 8) + 1];			\
						tmp       = (int32)(tbl-(RetVal)) * (int32)((Index) & 0x00FF);	\
						(RetVal) += ((int16)(tmp >> 8));								\
					}while(0)
#endif

/* Lookup table with 32 bit interpolation (256+1 values) */
#define LOOKUP32(Table, Index, RetVal)														\
					do{																		\
						int32 tbl;															\
						int64 tmp;															\
						(RetVal)  = (Table)[(uint8)((uint32)(Index) >> 24)];				\
						tbl       = (Table)[(uint8)((uint32)(Index) >> 24) + 1];			\
						tmp  	  = (int64)(tbl-(RetVal)) * (int64)((Index) & 0x00FFFFFF);	\
						(RetVal) += ((int32)(tmp >> 24));									\
					}while(0)




/* abs(x) macros for compatibality reasons (will be removed in the future) */
#define ABS_I8		getAbsValI8
#define ABS_I16		getAbsValI16
#define ABS_I32		getAbsValI32
#define ABS_I64		getAbsValI64
#define ABS_R32		getAbsValR32
#define ABS_R64		getAbsValR64

#define MAX_SCOPE_CHANNELS ((uint8)8)
#define MAX_PROTOCOLS    ((uint8)2)


/****************/
/* DATA TYPES   */
/****************/
/** Block functions contain block identifier and functions pointers for update, init, load & save function */
typedef struct {
    uint16 iBlockID;
    void  (*pFUpdate)(void *pTCommon);
    void  (*pFInit)(void *pTCommon);
    uint8 (*pFLoad)(void *pTCommon, uint8 ucData[]);
    uint8 (*pFSave)(void *pTCommon, uint8 ucData[], uint8 ucFrameLength);
    void* (*pFGetAddress)(void* pTCommon, uint16 blockMember);
} tBlockFunctions;

/** Parameter table contains parameter identifier & block address */
typedef struct {
    uint16 uiParID;
    void *pAdr;
} tParameterTable;

/** Limit save function table contains parameter identifier and pointer to limit-save function */
typedef struct {
	uint16 uiParID;
	uint8 (*pFLimitSave)(void *pTCommon, uint8 ucData[], uint8 ucFrameLength);
} tLimitSaveFunctionTable;

typedef struct {
    void (*pU)(void*);
    void *pData;
} tStateMachine;

typedef enum {
	RECEIVE_ALLOWED, SEND_ALLOWED
} tCommState;

/* 'base class' interface */
typedef struct tInterface tInterface;
struct tInterface {
	void (*send)(tInterface* hwInterface, uint8 data);
	uint8 (*receive)(tInterface* hwInterface);
	uint8 (*isReceiveDataAvailable)(tInterface* hwInterface);
	uint8 (*isSendReady)(tInterface* hwInterface);
	uint8 (*getTxFifoFree)(tInterface* hwInterface);
	void (*flush)(tInterface* hwInterface);
};

/* 'base class' protocol */
typedef struct tProtocol tProtocol;
struct tProtocol {
    void (*pCommunicate)(tProtocol* protocol);
    void (*pSnd_Enable)(tProtocol* protocol);
    
    uint8 ucFRAMESize;  
    uint8 ucMaxCommSize;
    uint8* ucFRAMEData;
    
    void* pServiceTable;

	tInterface* hwInterface;
};

/** Scope states */
typedef enum {
	SCOPE_IDLE=0, SCOPE_WAITTRG_OFFLINE=1, SCOPE_SAMPLE_OFFLINE=2, SCOPE_SAMPLE_ONLINE=4, \
	SCOPE_WAITTRGPOS_ONLINE=5, SCOPE_WAITTRGNEG_ONLINE=6, SCOPE_TRG_SAMPLE_OFFLINE=7, \
	SCOPE_WAIT_TRG_NEG_DELAY=8
} tScopeState;

typedef enum {
	EDGE_FALLING=0, EDGE_RISING=1
} tTrgEdgeType;


/** Scope block structure */
typedef struct {
	uint16 ID;								/** Scope block identifier */
	void *channelAddr[MAX_SCOPE_CHANNELS];	/** Pointer to each channel sample location (location of data to sample) */
	uint8 dataSize[MAX_SCOPE_CHANNELS];		/** Data type size for each channel */
	void *arrayAddr;						/** Pointer to data array (where all sample data is stored) */
	uint32 arraySize;						/** RAM array size */
	uint64 trgLevel;						/** Trigger level */
	uint64 trgLastValue;					/** Last value in trigger mode */
	void *trgAddr;							/** Pointer to trigger source (location of trigger source) */
	uint8 trgDataType;						/** Trigger data type */
	tScopeState state;						/** Sampling state */
	uint32 offlinePtr;						/** Pointer to current value in data array */
	uint8 dataSizeTotal;					/** Total data type size of all channels */
	uint8 noChannels;						/** Number of selected channels */
	uint16 stf;								/** Sample time factor */
	uint16 stfCnt;							/** Sample time factor counter */
	uint16 timestamp;						/** Time stamp (only used in OSM) */
	uint8 txFrameSize;						/** OSM frame size (only used in OSM) */
	int32 trgDelay;							/** Trigger delay in offline array steps */
	int32 trgEventPos;	 					/** Trigger event position (offline array index) */
	uint8 trgCountReached;					/** Indicates if minimum amount of pre/post trigger values have been sampled */
	uint32 trgCount;						/** Absolute value of trgDelay, for comparison if trigger delay values have been sampled */
	uint32 maxUsedLength;					/** Maximum used length of data array */
	tTrgEdgeType trgEdge;					/** Trigger edge (rising, falling) */
} SCOPE_MAIN;

/** DSP states */
typedef enum {
	MONITOR_STATE=0,		/**< DSP executes monitor program */
	PRG_LOADED_STATE=1,		/**< DSP executes application program */
	IDLE_STATE=2,			/**< DSP is idle */
	INIT_STATE=3,			/**< Initialization of application program */
	RUN_STATE_POWER_OFF=4,	/**< DSP executes control task, power electronics is disabled */
    RUN_STATE_POWER_ON=5	/**< DSP executes control task, power electronics is enabled */
} tDSPState;

typedef struct {
	void* dummy;
} tDynamicCodeData;

/** @brief TableStruct
 * 
 * Consists of:
 * - parameter table
 * - block function table
 * - limit-save function table
 * - DSP state
 * - event type
 * - event ID
 * - current protocol count
 * - pointer to protocol array
 * - frame program version
 * - pointer to frame program compilation date & time string
 * - pointer to Scope block
 * - pointer to dynamic code data
 * .
 */
typedef struct tTableStruct tTableStruct;
struct tTableStruct {
    const tParameterTable* TParamTable;
    const tBlockFunctions* TFncTable;
	const tLimitSaveFunctionTable* TLimitSaveFncTable;

	/* TODO: maybe struct 'DspStatus' and only add pointer here ? */
	tDSPState DSPState;
	uint16 eventType;
	uint32 eventId;

    uint16 protocolCount;
    tProtocol* protocols[MAX_PROTOCOLS];
    
    uint16 framePrgVersion;
    uint8* framePrgCompDateTime;

	SCOPE_MAIN* piScope;

	tDynamicCodeData* dynamicCodeData;
};


/* public prototypes */
void Common_Init(void* pTCommon);
void Common_Update(void* pTCommon);
int16 Common_Load(const void* pTCommon, uint8 ucData[]);
int16 Common_Save(void* pTCommon, const uint8 ucData[], uint8 ucFrameLength);
void* Common_GetAddress(const void* pTCommon, uint16 elementId);

int8 getAbsValI8(int8 x);
int16 getAbsValI16(int16 x);
int32 getAbsValI32(int32 x);
int64 getAbsValI64(int64 x);
float32 getAbsValR32(float32 x);
float64 getAbsValR64(float64 x);

#endif
