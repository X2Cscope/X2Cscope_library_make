/*
 * Copyright (c) [2012-2020] Microchip Technology Inc.
 * All rights reserved.
 *
 * This file is part of X2Cscope firmware implementation.
 *
 * X2CscopeComm.c — CAN example (MCC-generated CAN driver, dsPIC33C/dsPIC33A)
 *
 * Implements the X2Cscope communication callbacks over classical CAN 2.0.
 *
 * Protocol framing:
 *   X2Cscope uses a byte-stream protocol (LNet). CAN carries fixed-size frames,
 *   so bytes are accumulated in a small TX buffer and sent as 8-byte CAN frames.
 *   sendSerial() fires a frame automatically every 8 bytes. flushSerial() sends
 *   any remaining bytes at the end of each LNet frame as a partial frame.
 *   Received CAN frames are pushed byte-by-byte into a software ring buffer;
 *   receiveSerial() and isReceiveDataAvailable() drain it one byte at a time.
 *
 * Integration checklist:
 *   1. Copy X2CscopeComm.c and X2CscopeComm.h into your project's X2Cscope folder.
 *   2. Add both files to your build system.
 *   3. Adjust the CAN driver include path below.
 *   4. Adjust X2CSCOPE_CAN_TX_ID / X2CSCOPE_CAN_RX_ID to your network.
 *   5. Configure the CAN peripheral RX filter/mask via MCC to accept frames
 *      with X2CSCOPE_CAN_RX_ID.
 *   6. Call X2CscopeComm_Communicate() from your main/idle loop (NOT
 *      X2Cscope_Communicate() directly).
 *   7. Call X2Cscope_Update() from a fixed-rate task or timer ISR.
 */

#include <stdint.h>
#include "X2CscopeComm.h"

/* --------------------------------------------------------------------------
 * Adapt: CAN driver include path and CAN message IDs.
 * -------------------------------------------------------------------------- */
#include "can/can1.h"       /* adjust path to your MCC-generated CAN driver */
#include "can/can_types.h"  /* CAN_MSG_OBJ, CAN_TX_FIFO_STATUS, etc.        */

#define X2CSCOPE_CAN_TX_ID  (0x100U)    /* device → PC  (X2Cscope tool RX) */
#define X2CSCOPE_CAN_RX_ID  (0x110U)    /* PC → device  (X2Cscope tool TX) */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * RX ring buffer.
 * Filled by X2CscopeComm_Communicate() from incoming CAN frames.
 * Drained one byte at a time by receiveSerial().
 * Size must be a power of 2 (RING_SZ = 2^RING_BITS).
 * ------------------------------------------------------------------------- */
#define RING_BITS   5u
#define RING_SZ     (1u << RING_BITS)   /* 32 bytes */
#define RING_MASK   (RING_SZ - 1u)

static volatile uint8_t rx_buf[RING_SZ];
static volatile uint8_t rx_head;        /* write index */
static volatile uint8_t rx_tail;        /* read  index */

/* -------------------------------------------------------------------------
 * TX accumulation buffer.
 * Bytes are collected here; a CAN frame is sent every 8 bytes.
 * flushSerial() sends any remaining bytes as a shorter frame.
 * ------------------------------------------------------------------------- */
#define CAN_MAX_DLC  8u

static uint8_t tx_buf[CAN_MAX_DLC];
static uint8_t tx_idx;

/* -------------------------------------------------------------------------
 * Internal helpers
 * ------------------------------------------------------------------------- */
static void ringbuf_reset(void)
{
    rx_head = 0u;
    rx_tail = 0u;
}

static bool ringbuf_empty(void)
{
    return (rx_head == rx_tail);
}

static void ringbuf_push(uint8_t byte)
{
    rx_buf[rx_head & RING_MASK] = byte;
    rx_head++;
}

static uint8_t ringbuf_pop(void)
{
    uint8_t byte = rx_buf[rx_tail & RING_MASK];
    rx_tail++;
    return byte;
}

static void can_transmit_frame(uint8_t len)
{
    CAN_MSG_OBJ msg;
    uint8_t i;
    uint8_t *p = (uint8_t*)&msg;

    for (i = 0u; i < (uint8_t)sizeof(msg); i++) { p[i] = 0u; }

    msg.msgId            = X2CSCOPE_CAN_TX_ID;
    msg.field.formatType = CAN_2_0_FORMAT;
    msg.field.brs        = CAN_NON_BRS_MODE;
    msg.field.frameType  = CAN_FRAME_DATA;
    msg.field.idType     = CAN_FRAME_STD;
    msg.field.dlc        = len;
    msg.data             = tx_buf;

    (void)CAN1_Transmit(CAN1_TXQ, &msg);

    for (i = 0u; i < CAN_MAX_DLC; i++) { tx_buf[i] = 0xFFu; }
}

/* -------------------------------------------------------------------------
 * X2Cscope communication callbacks
 * ------------------------------------------------------------------------- */

/**
 * @brief Accumulate one byte into the TX buffer.
 * A full 8-byte CAN frame is sent automatically every 8 bytes.
 */
void sendSerial(uint8_t data)
{
    tx_buf[tx_idx & (CAN_MAX_DLC - 1u)] = data;

    if ((tx_idx & (CAN_MAX_DLC - 1u)) == (CAN_MAX_DLC - 1u))
    {
        can_transmit_frame(CAN_MAX_DLC);
    }

    tx_idx++;
}

/**
 * @brief Return one byte from the RX ring buffer.
 * Only called when isReceiveDataAvailable() has returned non-zero.
 */
uint8_t receiveSerial(void)
{
    return ringbuf_pop();
}

/**
 * @brief Check whether at least one byte is waiting in the RX ring buffer.
 * @return true if RX data available, false otherwise.
 */
bool isReceiveDataAvailable(void)
{
    return !ringbuf_empty();
}

/**
 * @brief Check whether the CAN TX queue can accept a new frame.
 * @return true if TX is possible, false if TX queue is full.
 */
bool isSendReady(void)
{
    return (CAN_TX_FIFO_AVAILABLE ==
            (CAN1_TransmitFIFOStatusGet(CAN1_TXQ) & CAN_TX_FIFO_AVAILABLE));
}

/**
 * @brief Send any remaining bytes as a partial CAN frame.
 * Called by the X2Cscope library at the end of each LNet frame.
 */
void flushSerial(void)
{
    uint8_t remainder = tx_idx & (CAN_MAX_DLC - 1u);

    if (remainder != 0u)
    {
        can_transmit_frame(remainder);
    }

    tx_idx = 0u;
}

/**
 * @brief Poll CAN RX FIFO and drive the X2Cscope protocol engine.
 *
 * Call this from your main/idle loop. It drains all pending CAN frames from
 * the hardware FIFO into the RX ring buffer, then calls X2Cscope_Communicate()
 * to process queued protocol bytes.
 *
 * Do NOT call X2Cscope_Communicate() directly in a CAN application — always
 * call X2CscopeComm_Communicate() so incoming frames are drained first.
 */
void X2CscopeComm_Communicate(void)
{
    CAN_MSG_OBJ rx_msg;
    uint8_t i;

    while (CAN1_ReceivedMessageCountGet() != 0u)
    {
        if ((bool)CAN1_Receive(&rx_msg))
        {
            for (i = 0u; i < (uint8_t)rx_msg.field.dlc; i++)
            {
                ringbuf_push(rx_msg.data[i]);
            }
        }
    }

    X2Cscope_Communicate();
}

/**
 * @brief Comm-layer post-init, called from X2Cscope_Init() after
 * X2Cscope_InitialiseEx(). Resets the RX ring buffer and TX index.
 */
void X2CscopeComm_PostInit(void)
{
    ringbuf_reset();
    tx_idx = 0u;
}

/* End of File */
