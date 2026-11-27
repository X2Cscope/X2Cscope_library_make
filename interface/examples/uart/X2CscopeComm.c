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

#include "X2CscopeComm.h"

/* --------------------------------------------------------------------------
 * Adapt: UART driver include and interface instance name.
 * The MCC Melody UART driver exposes a uart_drv_interface_t struct instance
 * with function pointers Read, Write, IsRxReady, IsTxReady.
 * -------------------------------------------------------------------------- */
#include "../uart/uart1.h"                  /* adjust path to your driver   */
#define X2CScopeUart  uart1_drv_interface   /* adjust to your instance name */
/* -------------------------------------------------------------------------- */

/**
 * @brief Send one byte over UART.
 */
void X2CscopeComm_SendByte(uint8_t data)
{
    X2CScopeUart.Write(data);
}

/**
 * @brief Receive one byte from UART.
 * Only called when X2CscopeComm_IsByteAvailable() has returned non-zero.
 */
uint8_t X2CscopeComm_ReceiveByte(void)
{
    return X2CScopeUart.Read();
}

/**
 * @brief Check whether at least one received byte is waiting.
 * @return true if RX data available, false otherwise.
 */
bool X2CscopeComm_IsByteAvailable(void)
{
    return X2CScopeUart.IsRxReady();
}

/**
 * @brief Check whether the TX buffer can accept at least one more byte.
 * @return true if transmit is possible, false if TX buffer is full.
 */
bool X2CscopeComm_IsSendReady(void)
{
    return X2CScopeUart.IsTxReady();
}

/* X2CscopeComm_Flush() is not implemented for UART: each byte is sent by X2CscopeComm_SendByte(),
 * so the library's default (empty) X2CscopeComm_Flush() is used. */

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
