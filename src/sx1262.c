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

#include <stdio.h>
#include <stdint.h>
#include "sleep.h"
#include "sx1262.h"
#include "io.h"

void SX1262SetStandby()
{
    IO_Out32(OP_ADDR, 	0x00000180);
    usleep(1);
    IO_Out32(WR_ADDR, 	0x0);
    usleep(1000);
#ifdef _DEBUG_
    printf("set SX1262 to Standby mode\n\r");
#endif
}

void SX1262SetTx(uint32_t timeout)
{
    IO_Out32(OP_ADDR, 	0x00000383);
    usleep(1);
    IO_Out32(WR_ADDR, 	((timeout >> 16) & 0x000000FF));
    IO_Out32(WR_ADDR, 	((timeout >> 8)  & 0x000000FF));
    IO_Out32(WR_ADDR, 	(timeout         & 0x000000FF));
    usleep(1000);
#ifdef _DEBUG_
    printf("set SX1262 to Tx mode\n\r");
#endif
}

void SX1262SetRx(uint32_t timeout)
{
    IO_Out32(OP_ADDR, 	0x00000382);
    usleep(1);
    IO_Out32(WR_ADDR, 	((timeout >> 16) & 0x000000FF));
    IO_Out32(WR_ADDR, 	((timeout >> 8)  & 0x000000FF));
    IO_Out32(WR_ADDR, 	(timeout         & 0x000000FF));
    usleep(1000);
#ifdef _DEBUG_
    printf("set SX1262 to Rx mode\n\r");
#endif
}

uint32_t SX1262GetStatus()
{
    uint32_t status;
    IO_Out32(OP_ADDR, 0x000001C0);
    usleep(1000);
    status = IO_In32(RD_ADDR);
#ifdef _DEBUG_
    printf("SX1262 status is %02x\n\r", status);
#endif
    return status;
}

void SX1262SetPacketType()
{
    IO_Out32(OP_ADDR, 0x0000018A);
    usleep(1);
    IO_Out32(WR_ADDR, 0x01);
    usleep(1000);
#ifdef _DEBUG_
    printf("set SX1262 packet!\n\r");
#endif
}

uint32_t SX1262GetPacketType()
{
    uint32_t packetType;
    IO_Out32(OP_ADDR, 0x00000211);
    usleep(1000);
    packetType = IO_In32(RD_ADDR);
    packetType = IO_In32(RD_ADDR);
#ifdef _DEBUG_
    printf("SX1262 packet type is %02x\n\r", packetType);
#endif
    return packetType;
}

void SX1262SetBufferBaseAddress(uint32_t txBaseAddr, uint32_t rxBaseAddr)
{
    IO_Out32(OP_ADDR, 0x0000028F);
    usleep(1);
    IO_Out32(WR_ADDR, txBaseAddr);
    IO_Out32(WR_ADDR, rxBaseAddr);
    usleep(1000);
#ifdef _DEBUG_
    printf("Set SX1262 buffer base address!\n\r");
#endif
}

void SX1262WriteBuffer(uint32_t *wrData, uint32_t offsetAddr, uint32_t size)
{
    IO_Out32(OP_ADDR, (0x0000000E | (((size+1) & 0x000000FF) << 8)));
    usleep(1);
    IO_Out32(WR_ADDR, offsetAddr);
    uint32_t i;
    for(i=0; i<size; i++)
    {
        IO_Out32(WR_ADDR, *(wrData + i));
    }
    usleep(1000);
#ifdef _DEBUG_
    printf("Write SX1262 buffer!\n\r");
#endif
}

void SX1262ReadBuffer(uint32_t *rdData, uint32_t offsetAddr, uint32_t size)
{
    IO_Out32(OP_ADDR, (0x0000001E | (((size+2) & 0x000000FF) << 8) | ((offsetAddr & 0x000000FF) << 24)));
    usleep(1000);
    uint32_t i;
    IO_In32(RD_ADDR);
    IO_In32(RD_ADDR);
    for(i=0; i<size; i++)
    {
        *(rdData+i) = IO_In32(RD_ADDR);
#ifdef _DEBUG_
        printf("Buffer read %02x!\n\r", *(rdData+i));
#endif
    }
    usleep(1000);
}

uint32_t SX1262GetDeviceErrors()
{
    uint32_t errorCode;
    IO_Out32(OP_ADDR, 0x00000317);
    usleep(1000);
    errorCode = IO_In32(RD_ADDR);
    //printf("Error code %0x!\n\r", errorCode);
    errorCode = IO_In32(RD_ADDR);
    //printf("Error code %0x!\n\r", errorCode);
    errorCode = IO_In32(RD_ADDR);
#ifdef _DEBUG_
    printf("Error code %0x!\n\r", errorCode);
#endif
    return errorCode;
}

void SX1262ClearDeviceErrors()
{
    IO_Out32(OP_ADDR, 0x00000207);
    usleep(1);
    IO_Out32(WR_ADDR, 0);
    IO_Out32(WR_ADDR, 0);
    usleep(1000);
#ifdef _DEBUG_
    printf("Clear device error!\n\r");
#endif
}

void SX1262SetRfFrequency(bool tx)
{
    IO_Out32(OP_ADDR, 0x00000486);
    usleep(1);
    if(tx)
    {
        IO_Out32(WR_ADDR, 0x39);
        IO_Out32(WR_ADDR, 0xb6);
        IO_Out32(WR_ADDR, 0x66);
        IO_Out32(WR_ADDR, 0x66);
#ifdef _DEBUG_
    printf("SetRfFrequency to 923.4MHz!\n\r");
#endif
    }
    else
    {
        IO_Out32(WR_ADDR, 0x39);
        IO_Out32(WR_ADDR, 0xb6);
        IO_Out32(WR_ADDR, 0x66);
        IO_Out32(WR_ADDR, 0x66);
#ifdef _DEBUG_
    printf("SetRfFrequency to 923.4MHz!\n\r");
#endif
    }
    usleep(1000);
}

void SX1262SetPaConfig()
{
    IO_Out32(OP_ADDR, 0x00000495);
    usleep(1);
    IO_Out32(WR_ADDR, 0x04);
    IO_Out32(WR_ADDR, 0x07);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x01);
    usleep(1000);
#ifdef _DEBUG_
    printf("Set PA: +22dBm!\n\r");
#endif
}

void SX1262SetTxParams()
{
    IO_Out32(OP_ADDR, 0x0000028e);
    usleep(1);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x04);
    usleep(1000);
#ifdef _DEBUG_
    printf("Set Tx parameters: +0dbm, 200u!\n\r");
#endif
}

void SX1262SetModulationParams(bool tx)
{
    IO_Out32(OP_ADDR, 0x0000088b);
    usleep(1);

    if(tx)
    {
        IO_Out32(WR_ADDR, 0x07);
#ifdef _DEBUG_
    printf("Set modulation parameters: SF7, BW_125kHz, CR_4_5, LDRO off!\n\r");
#endif
    }
    else
    {
        IO_Out32(WR_ADDR, 0x07);
#ifdef _DEBUG_
    printf("Set modulation parameters: SF7, BW_125kHz, CR_4_5, LDRO off!\n\r");
#endif
    }

    IO_Out32(WR_ADDR, 0x04);
    IO_Out32(WR_ADDR, 0x01);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    usleep(1000);
}

void SX1262SetPacketParams(uint32_t payloadLength, bool tx)
{
    IO_Out32(OP_ADDR, 0x0000098c);
    usleep(1);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x08);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, payloadLength);

    if(tx)
    {
        IO_Out32(WR_ADDR, 0x01);
        IO_Out32(WR_ADDR, 0x00);
#ifdef _DEBUG_
    printf("Set packet parameters: 8 preamble symbols, explicit header, payload length %d, CRC on, standard IQ!\n\r", payloadLength);
#endif
    }
    else
    {
        IO_Out32(WR_ADDR, 0x00);
        IO_Out32(WR_ADDR, 0x01);
#ifdef _DEBUG_
    printf("Set packet parameters: 4 preamble symbols, explicit header, payload length %d, CRC off, inverted IQ!\n\r", payloadLength);
#endif
    }
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    usleep(1000);

}

void SX1262WriteRegister(uint32_t *wrData, uint32_t offsetAddr, uint32_t size)
{
    IO_Out32(OP_ADDR, (0x0000000D | (((size+2) & 0x000000FF) << 8)));
    usleep(1);
    IO_Out32(WR_ADDR, ((offsetAddr >> 8) & 0xff));
    IO_Out32(WR_ADDR, (offsetAddr & 0xff));
    uint32_t i;
    for(i=0; i<size; i++)
    {
        IO_Out32(WR_ADDR, *(wrData + i));
#ifdef _DEBUG_
        printf("Reg write %02x @%04x!\n\r", *(wrData+i), (offsetAddr+i));
#endif
    }
    usleep(1000);

}

void SX1262ReadRegister(uint32_t *rdData, uint32_t offsetAddr, uint32_t size)
{
    IO_Out32(OP_ADDR, (0x0000001D | (((size+3) & 0x000000FF) << 8) | ((offsetAddr & 0x0000FFFF) << 16)));
    usleep(1000);
    uint32_t i;
    IO_In32(RD_ADDR);
    IO_In32(RD_ADDR);
    IO_In32(RD_ADDR);
    for(i=0; i<size; i++)
    {
        *(rdData+i) = IO_In32(RD_ADDR);
#ifdef _DEBUG_
        printf("Reg read %02x @%04x!\n\r", *(rdData+i), (offsetAddr+i));
#endif
    }
    usleep(1000);
}

void SX1262SetDioIrqParams(bool tx)
{
    IO_Out32(OP_ADDR, 0x00000808);
    usleep(1);
    if(tx)
    {
        IO_Out32(WR_ADDR, 0x00);
        IO_Out32(WR_ADDR, 0x01);
        IO_Out32(WR_ADDR, 0x00);
        IO_Out32(WR_ADDR, 0x01);
    }
    else
    {
        IO_Out32(WR_ADDR, 0x00);
        IO_Out32(WR_ADDR, 0x02);
        IO_Out32(WR_ADDR, 0x00);
        IO_Out32(WR_ADDR, 0x02);
    }
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
#ifdef _DEBUG_
    printf("Set interrupts!\n\r");
#endif
    usleep(1000);
}

void SX1262ClearIrqStatus()
{
    IO_Out32(OP_ADDR, 0x00000202);
    usleep(1);
    IO_Out32(WR_ADDR, 0xff);
    IO_Out32(WR_ADDR, 0xff);
#ifdef _DEBUG_
    printf("Clear all the interrupts!\n\r");
#endif
    usleep(1000);
}

uint32_t SX1262GetIrqStatus()
{
    uint32_t irqStatus;
    IO_Out32(OP_ADDR, 0x00000312);
    usleep(1000);
    IO_In32(RD_ADDR);
    irqStatus = (IO_In32(RD_ADDR) & 0xff) << 8;
    irqStatus = irqStatus | (IO_In32(RD_ADDR) & 0xff) ;
#ifdef _DEBUG_
    printf("Read interrupts: %04x!\n\r", irqStatus);
#endif
    return irqStatus;
}

void SX1262SetFs()
{
    IO_Out32(OP_ADDR, 	0x000000c1);
    usleep(100000);
#ifdef _DEBUG_
    printf("set SX1262 to FS mode\n\r");
#endif
}

void SX1262SetDIO2AsRfSwitchCtrl()
{
    IO_Out32(OP_ADDR, 0x0000019d);
    usleep(1);
    IO_Out32(WR_ADDR, 0x01);
#ifdef _DEBUG_
    printf("Set DIO2 as RfSwitchCtrl!\n\r");
#endif
    usleep(1000);
}

void SX1262SetDIO3AsTCXOCtrl()
{
    IO_Out32(OP_ADDR, 0x00000497);
    usleep(1);
    IO_Out32(WR_ADDR, 0x07);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x00);
    IO_Out32(WR_ADDR, 0x64);
#ifdef _DEBUG_
    printf("Set DIO3 as TXCOCtrl!\n\r");
#endif
    usleep(1000);
}

void SX1262SetRegulatorMode()
{
    IO_Out32(OP_ADDR, 0x00000196);
    usleep(1);
    IO_Out32(WR_ADDR, 0x01);
#ifdef _DEBUG_
    printf("Set Regulator Mode to DC_DC+LDO!\n\r");
#endif
    usleep(1000);
}

void SX1262Calibrate()
{
    IO_Out32(OP_ADDR, 0x00000189);
    usleep(1);
    IO_Out32(WR_ADDR, 0x7f);
#ifdef _DEBUG_
    printf("Enable calibration!\n\r");
#endif
    usleep(10000);
}

void SX1262CalibrateImage()
{
    IO_Out32(OP_ADDR, 0x00000298);
    usleep(1);
    IO_Out32(WR_ADDR, 0xe1);
    IO_Out32(WR_ADDR, 0xe9);
#ifdef _DEBUG_
    printf("Calibrate image!\n\r");
#endif
    usleep(1000);
}

void SX1262StopTimerOnPreamble()
{
    IO_Out32(OP_ADDR, 0x0000019f);
    usleep(1);
    IO_Out32(WR_ADDR, 0x00);
#ifdef _DEBUG_
    printf("Turn off stopping timer upon preamble!\n\r");
#endif
    usleep(1000);
}

void SX1262SetLoRaSymbNumTimeout()
{
    IO_Out32(OP_ADDR, 0x000001a0);
    usleep(1);
    IO_Out32(WR_ADDR, 0x00);
#ifdef _DEBUG_
    printf("Set Symbol number time out!\n\r");
#endif
    usleep(1000);
}
