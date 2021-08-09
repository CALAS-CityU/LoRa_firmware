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
#include "src/glb_include.h"
#include "src/sleep.h"
#include "src/sx1262.h"
#include "src/LoRaWan.h"
#include "src/ecdh.h"
#include "src/sc_print.h"

uint8_t devEui[8] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
uint8_t appEui[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
uint8_t appKey[16] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x0f, 0xed, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 };

uint8_t ecdh_pri_key[32];
uint8_t ecdh_pub_key[32];
uint8_t ecdh_shared_key[32];
bool ecdh_shared_key_exist = false;


// Hard-coded private for test
char pri_key_string[64] = "584B6574AAF49F7A6A7872E49B8A31F1A982BE27FD366D84FEEE1BEB30DF747D";
char other_pub_string[64] = "b5bea823d9c9ff576091c54b7c596c0ae296884f0e150290e88455d7fba6126f";

void string_to_byte_convert(char s[], uint8_t addressBytes[32]);
void ecdh_packet_process(uint8_t *msg);
void test_ecdh(uint8_t *pri_key, uint8_t *other_pub);
void test_message(uint8_t *msg);

int main()
{
    
    // uint32_t i;

    // uint16_t devNonce;
    // uint8_t nwkSKey[16];
    // uint8_t appSKey[16];
    // uint8_t plainRecPld[255];
    // uint8_t appNonce[3];
    // uint8_t netID[3];
    // uint32_t devAddr;
    // uint32_t frameCnt;
    sc_printf("Test Start\n");

    // //uint32_t wrData[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    // //uint32_t rdData[8];
    // //SX1262WriteBuffer(wrData, 15, 8);
    // //SX1262ReadBuffer(rdData, 14, 8);
    
    // //SX1262SetStandby();
    // //SX1262GetPacketType();
    // //SX1262SetPacketType(1);
    // //SX1262GetPacketType();
    
    // LoRaWanInit();
    // LoRaWanTxConfig();
    // LoRaWanJoin(appEui, devEui, appKey, &devNonce);
    // LoRaWanRxConfig();
    // LoRaWanAccept(appKey, plainRecPld, nwkSKey, appSKey, devNonce, appNonce, netID, &devAddr);
    // //sleep(10);
    // //LoRaMacTxConfig();
    // //LoRaMacUp(false, devAddr, appSKey, nwkSKey, 0, 0, 2);

    // frameCnt = 0;
    // //for(frameCnt = 0; frameCnt < 5; frameCnt ++)

    // sc_printf("LoRaWAN Accept\n");
    // // Use the hard-coded private key and pub key to test 
    // string_to_byte_convert(pri_key_string, ecdh_pri_key); 
    // generate_curve25519_pub_key(ecdh_pub_key, ecdh_pri_key);
    // uint8_t other_pub_key_test[32];
    // string_to_byte_convert(other_pub_string, other_pub_key_test);
    // test_ecdh(ecdh_pri_key, other_pub_key_test);

    // while(1)
    // {
    //     sleep(15);
    //     LoRaWanTxConfig();
    //     LoRaWanUp(true, devAddr, appSKey, nwkSKey, frameCnt, 5, 2);
    //     LoRaWanRxConfig();

    //     uint8_t message_received[256];
    //     LoRaWanDown(message_received);
    //     test_message(message_received);
    //     frameCnt++;
    //     // if(!ecdh_shared_key_exist){
    //     //     // generated shared key
    //     //     ecdh_packet_process(message_received);
    //     // }else{
    //     //     // shared key generated by ecdh already exist
    //     //     // Need to further decrypt the message received with the shard ecdh key
    //     //     continue;
    //     // }
    // }
    
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
        sc_printf("Server requst for ECDH, server public key is: \n");
        for(int i = 0; i < 32; ++i){
            sc_printf("%02hhX", *(other_pub_key+i));
        }
        sc_printf("\n");
        sc_printf("The shared secret key is: \n");
        for(int i = 0; i < 32; ++i){
            sc_printf("%02hhX", *(ecdh_shared_key+i));
        }
        sc_printf("\n");
        ecdh_shared_key_exist = true;
    }
}

void test_ecdh(uint8_t *pri_key, uint8_t *other_pub){
    uint8_t shared_key[32];
    generate_curve25519_shared_key(shared_key, pri_key, other_pub);
    sc_printf("ECDH test, server public key is: \n");
    for(int i = 0; i < 32; ++i){
        sc_printf("%02hhX", *(other_pub+i));
    }
    sc_printf("\n");
    sc_printf("The shared secret key is: \n");
    for(int i = 0; i < 32; ++i){
        sc_printf("%02hhX", *(shared_key+i));
    }
    sc_printf("\n");
}

void test_message(uint8_t *msg){
    uint8_t front_offset = 9;
    uint8_t msg_len = 8;
    sc_printf("The message received is: \n");
    for(int i = 0; i < msg_len; ++i){
        sc_printf("%02hhX", *(msg+i));
    }
    sc_printf("\n");
}

void string_to_byte_convert(char s[], uint8_t  addressBytes[32]) {
    sscanf(s, "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx", &addressBytes[0], 
    &addressBytes[1], &addressBytes[2], &addressBytes[3], &addressBytes[4],  \
    &addressBytes[5], &addressBytes[6], &addressBytes[7], &addressBytes[8],  \
    &addressBytes[9], &addressBytes[10], &addressBytes[11],&addressBytes[12],\
    &addressBytes[13], &addressBytes[14], &addressBytes[15], &addressBytes[16],
    &addressBytes[17], &addressBytes[18], &addressBytes[19], &addressBytes[20], 
    &addressBytes[21], &addressBytes[22], &addressBytes[23], &addressBytes[24], 
    &addressBytes[25], &addressBytes[26], &addressBytes[27], &addressBytes[28],
    &addressBytes[29], &addressBytes[30], &addressBytes[31]);
}