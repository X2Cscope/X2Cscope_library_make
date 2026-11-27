/*
 * Host test for the optional application hooks X2CscopeComm_Flush() and X2CscopeComm_PostInit().
 *
 * Linked like an application: interface/X2Cscope.c (X2Cscope_Init(), unchanged) plus the library as a
 * static archive, so the weak defaults must be found in the archive. Built twice (see run_host_tests.sh):
 *   without -DAPP_HOOKS: the application implements only the four required functions; the library's
 *                        defaults are used (the program links and Get Device Info is answered).
 *   with    -DAPP_HOOKS: the application implements both hooks; they replace the defaults
 *                        (PostInit called once by X2Cscope_Init(), Flush called for each response frame).
 */
#include <stdio.h>
#include <string.h>

#include "X2CscopeComm.h"
#include "X2Cscope.h"

/* ---------------------------------------------------------------- in-memory transport */

static uint8_t rxBuf[512];
static unsigned rxHead, rxTail;
static uint8_t txBuf[512];
static unsigned txLen;

void X2CscopeComm_SendByte(uint8_t data) { if (txLen < sizeof(txBuf)) { txBuf[txLen++] = data; } }
uint8_t X2CscopeComm_ReceiveByte(void) { return (rxTail < rxHead) ? rxBuf[rxTail++] : 0u; }
bool X2CscopeComm_IsByteAvailable(void) { return rxTail < rxHead; }
bool X2CscopeComm_IsSendReady(void) { return true; }

#ifdef APP_HOOKS
static int flushCalls, postInitCalls;
void X2CscopeComm_Flush(void) { flushCalls++; }
void X2CscopeComm_PostInit(void) { postInitCalls++; }
#endif

/* ---------------------------------------------------------------- helpers */

static int failures, checks;

static void check(int condition, const char *test, const char *what)
{
    checks++;
    if (!condition)
    {
        failures++;
        printf("FAIL  %-44s %s\n", test, what);
    }
}

/* Send one request frame (raw bytes as on the wire) and let X2Cscope process it. */
static void request(const uint8_t *frame, unsigned len)
{
    rxHead = rxTail = 0;
    txLen = 0;
    memcpy(rxBuf, frame, len);
    rxHead = len;
    for (int i = 0; i < 20; i++)
    {
        X2Cscope_Communicate();
    }
}

/* Get Device Info response: SYN, SIZE 0x2E (46, no fill byte), NODE, Service ID 0x00, Error ID 0x00, ... */
static int isDeviceInfoResponse(void)
{
    return txLen >= 50u && txBuf[0] == 0x55 && txBuf[1] == 0x2E && txBuf[3] == 0x00 && txBuf[4] == 0x00;
}

static const uint8_t DEVICE_INFO[] = { 0x55, 0x01, 0x01, 0x00, 0x57 };

/* ---------------------------------------------------------------- main */

int main(void)
{
    X2Cscope_Init();    /* interface/X2Cscope.c: X2Cscope_InitialiseEx() + X2CscopeComm_PostInit() */

#ifdef APP_HOOKS
    check(postInitCalls == 1, "application PostInit", "not called once by X2Cscope_Init()");
#endif

    request(DEVICE_INFO, sizeof(DEVICE_INFO));
    check(isDeviceInfoResponse(), "device info", "no valid response");

#ifdef APP_HOOKS
    check(flushCalls == 1, "application Flush", "not called once for the response frame");
    request(DEVICE_INFO, sizeof(DEVICE_INFO));
    check(flushCalls == 2, "application Flush", "not called for the second response frame");
#endif

    printf("%s: %d of %d checks failed (%s)\n", failures ? "FAILED" : "PASSED", failures, checks,
#ifdef APP_HOOKS
           "application hooks"
#else
           "library defaults"
#endif
    );
    return failures ? 1 : 0;
}
