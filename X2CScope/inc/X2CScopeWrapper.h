#ifndef __X2CSCOPEWRAPPER_H__
#define __X2CSCOPEWRAPPER_H__

#include "TableStruct.h"
#include "LNet.h"
#include "Services.h"
#include "BlockServicesX2C.h"
#include "Hardware.h"
#include "Target.h"
#include "X2C.h"
#include "VersionInfo.h"

/* generated X2C parameter identifier & block function table */
extern const tBlockFunctions blockFunctionTable[];
extern const tParameterTable parameterIdTable[];

/* X2C maintenance structure */
volatile tTableStruct TheTableStruct;
volatile tTableStruct *TableStruct = &TheTableStruct;

/* LNet protocol buffersize & node identifier */
#define LNET_BUFFERSIZE ((uint8)255)
#define LNET_NODEID ((uint8)1)

static tSerial interface;
static tLNet protocol;
static uint8 bufferLNet[LNET_BUFFERSIZE];

void X2CScope_Initialise(uint16_t scopeSize, const uint16_t appVersion, compilationDate_t compilationDate);
void X2CScope_Communicate();
void X2CScope_Update();

#endif