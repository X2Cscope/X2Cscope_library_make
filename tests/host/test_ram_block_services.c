/*
 * Host test for the LNet Get/Put RAM Block services (BlockServicesX2C.c).
 *
 * Runs the real service code on a 64-bit Linux PC (__GENERIC_X86__, 32-bit LNet addresses).
 * Target memory is mapped below 4 GB (MAP_32BIT) so its address fits into the 32-bit address field.
 *
 * Checks:
 *   - valid requests (8/16/32-bit, little endian),
 *   - malformed requests are answered with an error ID,
 *   - no write past the 255-byte LNet frame buffer (guard bytes),
 *   - no write to target memory for rejected Put RAM requests,
 *   - no crash (division by zero is caught as SIGFPE and reported as failure).
 *
 * Build and run (Linux, gcc):  see tests/host/run_host_tests.sh
 */
#define _GNU_SOURCE
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "Services.h"
#include "TableStruct.h"
#include "BlockServicesX2C.h"

#define LNET_BUFFER_SIZE 255u
#define GUARD_SIZE 16u
#define GUARD_BYTE 0xA5u
#define TARGET_SIZE 4096u

/* Not used by the RAM services, required by BlockServicesX2C.c */
volatile tTableStruct *TableStruct;

static uint8 frameMemory[LNET_BUFFER_SIZE + GUARD_SIZE];
static uint8 *target;
static tProtocol protocol;
static int responseSent;
static int failures;
static int checks;
static sigjmp_buf crashJump;

/* Stub of Services.c: same behaviour as the library function */
void sendError(tProtocol *p, uint8 errorId)
{
    p->ucFRAMESize = 2;
    p->ucFRAMEData[1] = errorId;
    p->pSnd_Enable(p);
}

static void sendEnable(tProtocol *p)
{
    (void)p;
    responseSent = 1;
}

static void onSigfpe(int sig)
{
    (void)sig;
    siglongjmp(crashJump, 1);
}

static void check(int condition, const char *test, const char *what)
{
    checks++;
    if (!condition)
    {
        failures++;
        printf("FAIL  %-48s %s\n", test, what);
    }
}

static int guardIntact(void)
{
    for (unsigned i = LNET_BUFFER_SIZE; i < LNET_BUFFER_SIZE + GUARD_SIZE; i++)
    {
        if (frameMemory[i] != GUARD_BYTE)
        {
            return 0;
        }
    }
    return 1;
}

/* Prepare a request: [SID][addr 4][...payload], frame size = 5 + payload length */
static void request(uint8 serviceId, const uint8 *payload, unsigned payloadLength, unsigned frameSize)
{
    uint32 addr = (uint32)(uintptr_t)target;
    memset(frameMemory, 0xEE, LNET_BUFFER_SIZE); /* stale content */
    memset(frameMemory + LNET_BUFFER_SIZE, GUARD_BYTE, GUARD_SIZE);
    frameMemory[0] = serviceId;
    frameMemory[1] = (uint8)addr;
    frameMemory[2] = (uint8)(addr >> 8);
    frameMemory[3] = (uint8)(addr >> 16);
    frameMemory[4] = (uint8)(addr >> 24);
    if (payloadLength > 0u)
    {
        memcpy(frameMemory + 5, payload, payloadLength);
    }
    protocol.ucFRAMESize = (uint8)frameSize;
    protocol.ucMaxCommSize = (uint8)LNET_BUFFER_SIZE;
    protocol.ucFRAMEData = frameMemory;
    protocol.pSnd_Enable = sendEnable;
    responseSent = 0;
}

/* Run a service; returns 0 if it crashed (SIGFPE) */
static int run(void (*service)(tProtocol *), const char *test)
{
    if (sigsetjmp(crashJump, 1) != 0)
    {
        check(0, test, "service crashed (SIGFPE, division by zero)");
        return 0;
    }
    service(&protocol);
    return 1;
}

static void fillTarget(void)
{
    for (unsigned i = 0; i < TARGET_SIZE; i++)
    {
        target[i] = (uint8)i;
    }
}

static int targetUnchanged(void)
{
    for (unsigned i = 0; i < TARGET_SIZE; i++)
    {
        if (target[i] != (uint8)i)
        {
            return 0;
        }
    }
    return 1;
}

/* ---------------------------------------------------------------- Get RAM Block */

static void getRam(const char *test, uint8 size, uint8 type, uint8 expectedError)
{
    const uint8 payload[2] = { size, type };
    request(0x09, payload, 2, 7);
    if (!run(getRAMBlock, test))
    {
        return;
    }
    check(responseSent, test, "no response sent");
    check(protocol.ucFRAMEData[1] == expectedError, test, "unexpected error ID");
    check(guardIntact(), test, "wrote past the LNet frame buffer");
    if (expectedError == ERRORSuccess)
    {
        check(protocol.ucFRAMESize == (uint8)(size + 2), test, "wrong response size");
        check(memcmp(protocol.ucFRAMEData + 2, target, size) == 0, test, "wrong data");
    }
    else
    {
        check(protocol.ucFRAMESize == 2, test, "error response must have 2 bytes");
    }
}

static void testGetRam(void)
{
    fillTarget();
    getRam("get: 8-bit, 4 bytes", 4, DATATYPE_8BIT, ERRORSuccess);
    getRam("get: 16-bit, 6 bytes", 6, DATATYPE_16BIT, ERRORSuccess);
    getRam("get: 32-bit, 8 bytes", 8, DATATYPE_32BIT, ERRORSuccess);
    getRam("get: 8-bit, 253 bytes (maximum)", 253, DATATYPE_8BIT, ERRORSuccess);
    getRam("get: 32-bit, 252 bytes (maximum for 32-bit)", 252, DATATYPE_32BIT, ERRORSuccess);
    getRam("get: 8-bit, 254 bytes (too large)", 254, DATATYPE_8BIT, ERRORSizeTooLarge);
    getRam("get: 32-bit, 253 bytes (not a multiple of 4)", 253, DATATYPE_32BIT, ERRORFormat);
    getRam("get: 16-bit, 253 bytes (not a multiple of 2)", 253, DATATYPE_16BIT, ERRORFormat);
    getRam("get: 32-bit, 6 bytes (not a multiple of 4)", 6, DATATYPE_32BIT, ERRORFormat);
    getRam("get: data type 0", 4, 0, ERRORFormat);
    getRam("get: data type 3", 4, 3, ERRORFormat);
    getRam("get: data type 8 (64-bit, not supported)", 8, DATATYPE_64BIT, ERRORFormat);

    /* request without size and type bytes: SID + address only */
    {
        const char *test = "get: request too short";
        request(0x09, NULL, 0, 5);
        frameMemory[5] = 8;               /* stale bytes that look like a request */
        frameMemory[6] = DATATYPE_8BIT;
        if (run(getRAMBlock, test))
        {
            check(protocol.ucFRAMEData[1] == ERRORFormat, test, "stale bytes were used as size/type");
        }
    }
}

/* ---------------------------------------------------------------- Put RAM Block */

static void putRam(const char *test, uint8 type, const uint8 *data, unsigned length, uint8 expectedError)
{
    uint8 payload[LNET_BUFFER_SIZE];
    payload[0] = type;
    memcpy(payload + 1, data, length);
    fillTarget();
    request(0x0A, payload, 1 + length, 6 + length);
    if (!run(putRAMBlock, test))
    {
        return;
    }
    check(responseSent, test, "no response sent");
    check(protocol.ucFRAMEData[1] == expectedError, test, "unexpected error ID");
    check(protocol.ucFRAMESize == 2, test, "response must have 2 bytes");
    check(guardIntact(), test, "wrote past the LNet frame buffer");
    if (expectedError == ERRORSuccess)
    {
        check(memcmp(target, data, length) == 0, test, "target memory not written");
        check(target[length] == (uint8)length, test, "wrote past the requested length");
    }
    else
    {
        check(targetUnchanged(), test, "target memory changed although rejected");
    }
}

static void testPutRam(void)
{
    const uint8 data[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };

    putRam("put: 8-bit, 3 bytes", DATATYPE_8BIT, data, 3, ERRORSuccess);
    putRam("put: 16-bit, 4 bytes", DATATYPE_16BIT, data, 4, ERRORSuccess);
    putRam("put: 32-bit, 8 bytes", DATATYPE_32BIT, data, 8, ERRORSuccess);
    putRam("put: 16-bit, 3 bytes (not a multiple of 2)", DATATYPE_16BIT, data, 3, ERRORFormat);
    putRam("put: 32-bit, 6 bytes (not a multiple of 4)", DATATYPE_32BIT, data, 6, ERRORFormat);
    putRam("put: data type 0", 0, data, 4, ERRORFormat);
    putRam("put: data type 3", 3, data, 3, ERRORFormat);
    putRam("put: data type 8 (64-bit, not supported)", DATATYPE_64BIT, data, 8, ERRORFormat);

    /* request without type byte: SID + address only -> size must not underflow */
    {
        const char *test = "put: request too short (no type byte)";
        fillTarget();
        request(0x0A, NULL, 0, 5);
        frameMemory[5] = DATATYPE_8BIT;   /* stale bytes that look like a type */
        if (run(putRAMBlock, test))
        {
            check(protocol.ucFRAMEData[1] == ERRORFormat, test, "unexpected error ID");
            check(targetUnchanged(), test, "size underflow: target memory overwritten");
        }
    }
}

int main(void)
{
    target = mmap(NULL, TARGET_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
    if (target == MAP_FAILED)
    {
        perror("mmap");
        return 2;
    }
    signal(SIGFPE, onSigfpe);

    testGetRam();
    testPutRam();

    printf("%s: %d of %d checks failed\n", failures ? "FAILED" : "PASSED", failures, checks);
    return failures ? 1 : 0;
}
