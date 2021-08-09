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
#include <stdbool.h>
#include "glb_include.h"
#include "sleep.h"
#include "sx1262.h"
#include "LoRaWan.h"
#include "ecdh.h"
#include "sc_print.h"

uint8_t devEui[8] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
uint8_t appEui[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
uint8_t appKey[16] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x0f, 0xed, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 };

uint8_t ecdh_pri_key[32] = {
    0x58, 0x4b, 0x65, 0x74, 0xaa, 0xf4, 0x9f, 0x7a, 
    0x6a, 0x78, 0x72, 0xe4, 0x9b, 0x8a, 0x31, 0xf1, 
    0xa9, 0x82, 0xbe, 0x27, 0xfd, 0x36, 0x6d, 0x84, 
    0xfe, 0xee, 0x1b, 0xeb, 0x30, 0xdf, 0x74, 0x7d};
uint8_t ecdh_pub_key[32];
uint8_t ecdh_shared_key[32];
bool ecdh_shared_key_exist = false;

uint8_t test_other_pub_key[32] = {
    0xb5, 0xbe, 0xa8, 0x23, 0xd9, 0xc9, 0xff, 0x57,
    0x60, 0x91, 0xc5, 0x4b, 0x7c, 0x59, 0x6c, 0x0a,
    0xe2, 0x96, 0x88, 0x4f, 0x0e, 0x15, 0x02, 0x90,
    0xe8, 0x84, 0x55, 0xd7, 0xfb, 0xa6, 0x12, 0x6f
};

void ecdh_packet_process(uint8_t *msg);
void test_ecdh(uint8_t *pri_key, uint8_t *other_pub);
void test_message(uint8_t *msg);

int main()
{
    
    uint32_t i;

    uint16_t devNonce;
    uint8_t nwkSKey[16];
    uint8_t appSKey[16];
    uint8_t plainRecPld[255];
    uint8_t appNonce[3];
    uint8_t netID[3];
    uint32_t devAddr;
    uint32_t frameCnt;
    sc_printf("Test Start\n\r");

    //uint32_t wrData[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    //uint32_t rdData[8];
    //SX1262WriteBuffer(wrData, 15, 8);
    //SX1262ReadBuffer(rdData, 14, 8);
    
    //SX1262SetStandby();
    //SX1262GetPacketType();
    //SX1262SetPacketType(1);
    //SX1262GetPacketType();
    
    LoRaWanInit();
    LoRaWanTxConfig();
    LoRaWanJoin(appEui, devEui, appKey, &devNonce);
    LoRaWanRxConfig();
    LoRaWanAccept(appKey, plainRecPld, nwkSKey, appSKey, devNonce, appNonce, netID, &devAddr);
    //sleep(10);
    //LoRaMacTxConfig();
    //LoRaMacUp(false, devAddr, appSKey, nwkSKey, 0, 0, 2);

    frameCnt = 0;
    //for(frameCnt = 0; frameCnt < 5; frameCnt ++)

    sc_printf("LoRaWAN Accept\n\r");
    // Use the hard-coded private key and pub key to test 
    generate_curve25519_pub_key(ecdh_pub_key, ecdh_pri_key);
    sc_printf("Key generation success\n\r");
    uint8_t other_pub_key_test[32];
    test_ecdh(ecdh_pri_key, other_pub_key_test);
    
    while(1)
    {
        sleep(15);
        LoRaWanTxConfig();
        LoRaWanUp(true, devAddr, appSKey, nwkSKey, frameCnt, 5, 2);
        LoRaWanRxConfig();

        uint8_t message_received[256];
        LoRaWanDown(message_received);
        test_message(message_received);
        frameCnt++;
        // if(!ecdh_shared_key_exist){
        //     // generated shared key
        //     ecdh_packet_process(message_received);
        // }else{
        //     // shared key generated by ecdh already exist
        //     // Need to further decrypt the message received with the shard ecdh key
        //     continue;
        // }
    }
    
    return 0;
}


void ecdh_packet_process(uint8_t *msg){
    /*******
    Format of the packet between server and device:
    | length (1 byte) | indicator (1 byte) | msg |
    *******/ 

    // packet offset 
    uint8_t front_offset = 9;
    uint8_t msg_length = msg[front_offset];

    if(msg[front_offset+1] == 0xff){
        uint8_t *other_pub_key = msg + front_offset + 2; 

        generate_curve25519_shared_key(ecdh_shared_key, ecdh_pri_key, other_pub_key);
        sc_printf("Server requst for ECDH, server public key is: \n\r");
        for(int i = 0; i < 32; ++i){
            sc_printf("%02hhX", *(other_pub_key+i));
        }
        sc_printf("\n\r");
        sc_printf("The shared secret key is: \n\r");
        for(int i = 0; i < 32; ++i){
            sc_printf("%02hhX", *(ecdh_shared_key+i));
        }
        sc_printf("\n\r");
        ecdh_shared_key_exist = true;
    }
}

void test_ecdh(uint8_t *pri_key, uint8_t *other_pub){
    uint8_t shared_key[32];
    generate_curve25519_shared_key(shared_key, pri_key, other_pub);
    sc_printf("ECDH test, server public key is: \n\r");
    for(int i = 0; i < 32; ++i){
        sc_printf("%02hhX", *(other_pub+i));
    }
    sc_printf("\n\r");
    sc_printf("The shared secret key is: \n\r");
    for(int i = 0; i < 32; ++i){
        sc_printf("%02hhX", *(shared_key+i));
    }
    sc_printf("\n\r");
}

void test_message(uint8_t *msg){
    uint8_t front_offset = 9;
    uint8_t msg_len = 8;
    sc_printf("The message received is: \n\r");
    for(int i = 0; i < msg_len; ++i){
        sc_printf("%02hhX", *(msg+i));
    }
    sc_printf("\n\r");
}

