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

/**
 * This file shows example for X2Cscope_Init implementation.
 */
#include "X2CscopeComm.h"
#include "X2Cscope.h"

// SCOPE_SIZE is defined in X2Cscope.h, it is the size of the buffer that is sent to the host
int8_t X2CscopeArray[X2CSCOPE_BUFFER_SIZE]; 

// compalitionDate_t is defined in X2Cscope.h
// it can be read out by the Get Device Info X2Cscope service
compilationDate_t compilationDate = {__DATE__, __TIME__};

void X2Cscope_Init(void)
{
    X2Cscope_HookUARTFunctions(sendSerial, receiveSerial, isReceiveDataAvailable, isSendReady);
    X2Cscope_Initialise((void*)X2CscopeArray, X2CSCOPE_BUFFER_SIZE, X2CSCOPE_APP_VERSION, compilationDate);
}
