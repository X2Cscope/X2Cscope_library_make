/*
 * Host test for the device reset through the LNet Reboot service (0x19), through the whole library:
 * X2Cscope_InitialiseEx() + X2Cscope_Communicate() with an in-memory transport.
 *
 * Built twice (see run_host_tests.sh):
 *   without -DAPP_RESET: the application has no X2CscopeComm_ResetDevice(); the library's weak
 *                        default answers 0x21 Service not available.
 *   with    -DAPP_RESET: the application implements X2CscopeComm_ResetDevice():
 *                        cannot reset -> 0x22 Invalid DSP state; accepted -> no response frame,
 *                        and the next request is still answered (receiver ready again).
 * Both: Reboot request with service data -> 0x14 Format error.
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
void X2CscopeComm_Flush(void) { }
void X2CscopeComm_PostInit(void) { }

#ifdef APP_RESET
static bool acceptReset;
static int resetCalls;
bool X2CscopeComm_ResetDevice(void)
{
    resetCalls++;
    return acceptReset;    /* a real application resets the device here */
}
#endif

/* ---------------------------------------------------------------- helpers */

static int failures, checks;

static void check(int condition, const char *test, const char *what)
{
    checks++;
    if (!condition)
    {
        failures++;
        printf("FAIL  %-44s %s\n      sent %u bytes:", test, what, txLen);
        for (unsigned i = 0; i < txLen && i < 24u; i++)
        {
            printf(" %02X", txBuf[i]);
        }
        printf("\n");
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

/* Error response: SYN, SIZE 0x02 + fill byte, NODE, Service ID, Error ID, checksum */
static int isErrorResponse(uint8_t sid, uint8_t error)
{
    uint8_t sum = (uint8_t)(0x55 + 0x02 + 0x01 + sid + error);
    if (sum == 0x55 || sum == 0x02) { sum = (uint8_t)~sum; }
    return txLen == 7 && txBuf[0] == 0x55 && txBuf[1] == 0x02 && txBuf[2] == 0x00 && txBuf[3] == 0x01
           && txBuf[4] == sid && txBuf[5] == error && txBuf[6] == sum;
}

/* Get Device Info response: SYN, SIZE 0x2E (46, no fill byte), NODE, Service ID 0x00, Error ID 0x00, ... */
static int isDeviceInfoResponse(void)
{
    return txLen >= 50u && txBuf[0] == 0x55 && txBuf[1] == 0x2E && txBuf[3] == 0x00 && txBuf[4] == 0x00;
}

/* LNet Reboot service request, without and with (invalid) service data */
static const uint8_t RESET_REQUEST[] = { 0x55, 0x01, 0x01, 0x19, 0x70 };
static const uint8_t RESET_REQUEST_WITH_DATA[] = { 0x55, 0x02, 0x00, 0x01, 0x19, 0x00, 0x71 };
static const uint8_t DEVICE_INFO[] = { 0x55, 0x01, 0x01, 0x00, 0x57 };

/* ---------------------------------------------------------------- main */

int8_t X2CscopeArray[256];
const compilationDate_t compilationDate = { "Jan 01 2026", "12:00:00" };

int main(void)
{
    X2Cscope_Config_t config = X2CSCOPE_CONFIG_INIT(
        X2CscopeComm_SendByte, X2CscopeComm_ReceiveByte, X2CscopeComm_IsByteAvailable,
        X2CscopeComm_IsSendReady, X2CscopeComm_Flush,
        (void*)X2CscopeArray, sizeof(X2CscopeArray), 0x0001, compilationDate);
    X2Cscope_InitialiseEx(&config);

    request(DEVICE_INFO, sizeof(DEVICE_INFO));
    check(isDeviceInfoResponse(), "device info before reset", "no valid response");

    request(RESET_REQUEST_WITH_DATA, sizeof(RESET_REQUEST_WITH_DATA));
    check(isErrorResponse(0x19, 0x14), "reset with service data", "expected 0x14 Format error");

#ifndef APP_RESET
    request(RESET_REQUEST, sizeof(RESET_REQUEST));
    check(isErrorResponse(0x19, 0x21), "reset, no application hook", "expected 0x21 Service not available");
#else
    acceptReset = false;
    request(RESET_REQUEST, sizeof(RESET_REQUEST));
    check(resetCalls == 1, "reset not possible", "hook not called once");
    check(isErrorResponse(0x19, 0x22), "reset not possible", "expected 0x22 Invalid DSP state");

    acceptReset = true;
    request(RESET_REQUEST, sizeof(RESET_REQUEST));
    check(resetCalls == 2, "reset accepted", "hook not called");
    check(txLen == 0, "reset accepted", "a response frame was sent (LNet v5: none on success)");

    /* the receiver must be ready for the next frame (deferred reset in the application) */
    request(DEVICE_INFO, sizeof(DEVICE_INFO));
    check(isDeviceInfoResponse(), "device info after accepted reset",
          "no valid response: receiver stuck after a service without response");
#endif

    printf("%s: %d of %d checks failed (%s)\n", failures ? "FAILED" : "PASSED", failures, checks,
#ifdef APP_RESET
           "with application hook"
#else
           "library default"
#endif
    );
    return failures ? 1 : 0;
}
