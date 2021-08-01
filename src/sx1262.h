/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#ifndef _SX1262_H_
#define _SX1262_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "sleep.h"
#include "glb_include.h"

//#define _DEBUG_

void SX1262SetStandby();
void SX1262SetTx(uint32_t timeout);
void SX1262SetRx(uint32_t timeout);

uint32_t SX1262GetStatus();
void SX1262SetPacketType();
uint32_t SX1262GetPacketType();
void SX1262SetBufferBaseAddress(uint32_t txBaseAddr, uint32_t rxBaseAddr);
void SX1262WriteBuffer(uint32_t *wrData, uint32_t offsetAddr, uint32_t size);
void SX1262ReadBuffer(uint32_t *rdData, uint32_t offsetAddr, uint32_t size);
uint32_t SX1262GetDeviceErrors();
void SX1262ClearDeviceErrors();
void SX1262SetRfFrequency(bool tx);
void SX1262SetPaConfig();
void SX1262SetTxParams();
void SX1262SetModulationParams(bool tx);
void SX1262SetPacketParams(uint32_t payloadLength, bool tx);
void SX1262WriteRegister(uint32_t *wrData, uint32_t offsetAddr, uint32_t size);
void SX1262ReadRegister(uint32_t *rdData, uint32_t offsetAddr, uint32_t size);
void SX1262SetDioIrqParams(bool tx);
void SX1262ClearIrqStatus();
uint32_t SX1262GetIrqStatus();
void SX1262SetFs();
void SX1262SetDIO2AsRfSwitchCtrl();
void SX1262SetDIO3AsTCXOCtrl();
void SX1262SetRegulatorMode();
void SX1262Calibrate();
void SX1262CalibrateImage();
void SX1262StopTimerOnPreamble();
void SX1262SetLoRaSymbNumTimeout();
#endif
