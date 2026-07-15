/*
 * Copyright (c) [2012-2020] Microchip Technology Inc.
 * All rights reserved.
 *
 * This file is part of X2Cscope firmware implementation.
 *
 * X2CscopeComm.c — UART example
 *
 * Implements the X2Cscope communication callbacks using an MCC Melody UART
 * driver (uart_drv_interface_t). Adapt the include and instance name below
 * to match your project's generated driver.
 *
 * Integration checklist:
 *   1. Copy X2CscopeComm.c and X2CscopeComm.h into your project's X2Cscope folder.
 *   2. Add both files to your build system (MPLAB X project or CMakeLists.txt).
 *   3. Adjust the UART driver include path and X2CScopeUart instance name below.
 *   4. Ensure the UART peripheral is initialised before X2Cscope_Init() is called.
 *   5. Call X2Cscope_Communicate() from your main/idle loop.
 *   6. Call X2Cscope_Update() from a fixed-rate task or timer ISR.
 */

#include <xc.h>
#include "X2CscopeComm.h"

/* --------------------------------------------------------------------------
 * Adapt: UART driver include and interface instance name.
 * The MCC Melody UART driver exposes a uart_drv_interface_t struct instance
 * with function pointers Read, Write, IsRxReady, IsTxReady.
 * -------------------------------------------------------------------------- */
#include "uart/uart1.h"                     /* adjust path to your driver   */
#define X2CScopeUart  uart1_drv_interface   /* adjust to your instance name */
/* -------------------------------------------------------------------------- */

/**
 * @brief Send one byte over UART.
 */
void sendSerial(uint8_t data)
{
    X2CScopeUart.Write(data);
}

/**
 * @brief Receive one byte from UART.
 * Only called when isReceiveDataAvailable() has returned non-zero.
 */
uint8_t receiveSerial(void)
{
    return X2CScopeUart.Read();
}

/**
 * @brief Check whether at least one received byte is waiting.
 * @return Non-zero if RX data available, zero otherwise.
 */
uint8_t isReceiveDataAvailable(void)
{
    return (uint8_t)X2CScopeUart.IsRxReady();
}

/**
 * @brief Check whether the TX buffer can accept at least one more byte.
 * @return Non-zero if transmit is possible, zero if TX buffer is full.
 */
uint8_t isSendReady(void)
{
    return (uint8_t)X2CScopeUart.IsTxReady();
}

/**
 * @brief Flush the transmit buffer.
 * For a direct byte-at-a-time UART the hardware sends each byte immediately
 * as it is written — no explicit flush is needed. Leave this function empty
 * unless your implementation uses a software TX FIFO requiring an explicit
 * commit step.
 */
void flushSerial(void)
{
    /* Nothing to do — UART transmits each byte immediately. */
}

/**
 * @brief Comm-layer post-init, called from X2Cscope_Init() after
 * X2Cscope_InitialiseEx(). Enable the UART TX interrupt here if using
 * interrupt-driven transmission. Leave empty if not needed.
 */
void X2CscopeComm_PostInit(void)
{
    /* Example: enable UART TX interrupt for interrupt-driven TX.
     * IEC0bits.U1TXIE = 1;
     */
}

/* End of File */
