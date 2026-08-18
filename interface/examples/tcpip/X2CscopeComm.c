/*
 * Copyright (c) [2012-2020] Microchip Technology Inc.
 * All rights reserved.
 *
 * This file is part of X2Cscope firmware implementation.
 *
 * X2CscopeComm.c — TCP/IP (lwIP) example
 *
 * Implements the X2Cscope communication callbacks over a TCP/IP connection
 * using an lwIP TCP server. The X2Cscope PC tool connects as a TCP client.
 *
 * Protocol framing:
 *   X2Cscope uses a byte-stream protocol (LNet). TCP carries it natively —
 *   no CAN-style framing is needed. TX bytes are accumulated per LNet frame
 *   and sent as a single TCP segment by flushSerial(), minimising TCP overhead
 *   and preventing Nagle-algorithm delays. Received TCP data is pushed
 *   byte-by-byte into a circular buffer by the TCP receive callback and drained
 *   by receiveSerial().
 *
 * Integration checklist:
 *   1. Copy X2CscopeComm.c and X2CscopeComm.h into your project's X2Cscope folder.
 *   2. Add both files to your build system.
 *   3. Adjust the TCP server include path below.
 *   4. Adjust X2CSCOPE_TCP_PORT to your network configuration.
 *   5. Ensure lwIP and the network interface are initialised before
 *      X2Cscope_Init() is called.
 *   6. Call X2Cscope_Communicate() from your main/idle loop.
 *   7. Call X2Cscope_Update() from a fixed-rate task or timer ISR.
 */

#include <stdint.h>
#include "X2CscopeComm.h"

/* --------------------------------------------------------------------------
 * Adapt: TCP server include path and listening port.
 * The tcp_server.h / tcp_server.c driver used here is the one from the
 * dsPIC33AK512_T1S reference project. Adapt the TCP_Server type and API
 * calls to your own lwIP TCP server wrapper if different.
 * -------------------------------------------------------------------------- */
#include "lwip/tcp_server.h"    /* adjust path to your TCP server driver    */

#define X2CSCOPE_TCP_PORT  (12666U)     /* port the device listens on       */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * RX circular buffer.
 * Filled by the TCP receive callback; drained by receiveSerial().
 * Size must be a power of 2.
 * ------------------------------------------------------------------------- */
#define TCP_RX_BUFSIZE  256u

typedef struct {
    uint8_t           buf[TCP_RX_BUFSIZE];
    volatile uint16_t head;     /* write index (TCP receive callback) */
    volatile uint16_t tail;     /* read  index (receiveSerial)        */
} RxCircularBuffer;

static RxCircularBuffer rx_buf = {0};

/* -------------------------------------------------------------------------
 * TX accumulation buffer.
 * Bytes are collected here and sent as a single TCP segment by flushSerial().
 * 516 bytes covers the maximum LNet frame (255 payload + overhead).
 * ------------------------------------------------------------------------- */
#define TCP_TX_BUFSIZE  516u

static uint8_t  tx_buf[TCP_TX_BUFSIZE];
static uint16_t tx_head = 0u;

/* -------------------------------------------------------------------------
 * TCP server instance
 * ------------------------------------------------------------------------- */
static void receiveTcpData(void *cfg, const uint8_t *data, uint16_t len);

static TCP_Server x2cscope_tcp_server = {
    .pcb     = NULL,
    .port    = X2CSCOPE_TCP_PORT,
    .recv_fn = receiveTcpData
};

/* -------------------------------------------------------------------------
 * TCP receive callback — called by the TCP server driver on every RX event.
 * Pushes all received bytes into the RX circular buffer.
 * ------------------------------------------------------------------------- */
static void receiveTcpData(void *cfg, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    (void)cfg;

    for (i = 0u; i < len; i++)
    {
        uint16_t next = (rx_buf.head + 1u) % TCP_RX_BUFSIZE;
        if (next != rx_buf.tail)    /* silently drop on overflow */
        {
            rx_buf.buf[rx_buf.head] = data[i];
            rx_buf.head = next;
        }
    }
}

/* -------------------------------------------------------------------------
 * X2Cscope communication callbacks
 * ------------------------------------------------------------------------- */

/**
 * @brief Accumulate one byte into the TX buffer.
 * The buffer is sent as a single TCP segment by flushSerial().
 */
void sendSerial(uint8_t data)
{
    if (tx_head < TCP_TX_BUFSIZE)
    {
        tx_buf[tx_head++] = data;
    }
}

/**
 * @brief Return one byte from the RX circular buffer.
 * Only called when isReceiveDataAvailable() has returned non-zero.
 */
uint8_t receiveSerial(void)
{
    if (rx_buf.head == rx_buf.tail) { return 0u; }
    uint8_t data = rx_buf.buf[rx_buf.tail];
    rx_buf.tail  = (rx_buf.tail + 1u) % TCP_RX_BUFSIZE;
    return data;
}

/**
 * @brief Check whether at least one received byte is waiting.
 * @return true if RX data available, false otherwise.
 */
bool isReceiveDataAvailable(void)
{
    return (rx_buf.head != rx_buf.tail);
}

/**
 * @brief TCP is always ready from the firmware side — TX is buffered.
 * @return true always.
 */
bool isSendReady(void)
{
    return true;
}

/**
 * @brief Send the accumulated TX buffer as a single TCP segment.
 * Called by the X2Cscope library at the end of each LNet frame.
 * Sending one segment per LNet frame avoids Nagle-algorithm delays.
 */
void flushSerial(void)
{
    tcp_server_send(&x2cscope_tcp_server, tx_buf, tx_head);
    tx_head = 0u;
}

/**
 * @brief Comm-layer post-init, called from X2Cscope_Init() after
 * X2Cscope_InitialiseEx(). Starts the TCP server so the X2Cscope PC tool
 * can connect.
 */
void X2CscopeComm_PostInit(void)
{
    tcp_server_start(&x2cscope_tcp_server);
}

/* End of File */
