/* This file is derived from am X2C generated file, modified for use with the X2Cscope Plugin */
/* This file is part of X2C. http://www.mechatronic-simulation.org/                                                   */

#include "X2C.h"

/**********************************************************************************************************************/
/**                                              Global Variables Block                                              **/
/**********************************************************************************************************************/
SCOPE_MAIN                    TScope;

/* Block function table                                                                                               */
#define END_BLOCKFUNCTIONS { (uint16)0, (void (*)(void*))0, (void (*)(void*))0, \
    (uint8 (*)(void*, uint8[]))0, (uint8 (*)(void*, uint8[], uint8))0, (void* (*)(void*, uint16))0 }

const tBlockFunctions blockFunctionTable[] = {
    FUNCTIONS,
    END_BLOCKFUNCTIONS
};

/* Parameter identifier table                                                                                         */
#define END_PARAMETERTABLE { (uint16)0, (void*)0 }

const tParameterTable parameterIdTable[] = {
    PARAMETER_TABLE,
    END_PARAMETERTABLE
};


void X2C_Init(void* scopeArray, uint16 scopeSize)
{
    Scope_Main_Init(&TScope, scopeArray, scopeSize);
}

void X2C_Update(void)
{
    Scope_Main_Update(&TScope);
}

