#include <stdio.h>
#include <stdint.h>
#include "utilities.h"
#include "sx1262.h"
#include "LoRaWan.h"
#include "io.h"

void LoRaWanInit()
{
    SX1262SetStandby();
    SX1262ClearDeviceErrors();

    //do the default setting
    uint32_t regVal[8];

    SX1262SetDIO3AsTCXOCtrl();
    SX1262CalibrateImage();
    SX1262Calibrate();
    SX1262SetDIO2AsRfSwitchCtrl();
    SX1262SetRegulatorMode();
    SX1262SetBufferBaseAddress(0,0);
    SX1262SetPaConfig();

    regVal[0] = 0x38;
    SX1262WriteRegister(regVal, 0x08e7, 1);
    SX1262SetTxParams();

}


/*#ifdef _DEBUG_
    printf("--Configure reg TxModulation--\n\r");
#endif
    SX1262ReadRegister(regVal, 0x0889, 1);
    regVal[0] = regVal[0] | 0x04;
    SX1262WriteRegister(regVal, 0x0889, 1);

#ifdef _DEBUG_
    printf("--Configure reg TxClampConfig--\n\r");
#endif
    SX1262ReadRegister(regVal, 0x08d8, 1);
    regVal[0] = regVal[0] | 0x1e;
    SX1262WriteRegister(regVal, 0x08d8, 1);

#ifdef _DEBUG_
    printf("--Configure reg SyncWord--\n\r");
#endif
    SX1262ReadRegister(regVal, 0x0740, 2);
    regVal[0] = 0x34;
    regVal[1] = 0x44;
    SX1262WriteRegister(regVal, 0x0740, 2);

#ifdef _DEBUG_
    printf("-----Complete Initialization-----\n\r");
#endif
}*/

void LoRaWanTxConfig()
{
    SX1262SetStandby();

    uint32_t regVal[8];
#ifdef _DEBUG_
    printf("--Configure reg IQ Polarity--\n\r");
#endif
/*
    SX1262ReadRegister(regVal, 0x0736, 1);
    regVal[0] = regVal[0] | 0x04;
    SX1262WriteRegister(regVal, 0x0736, 1);*/

    SX1262SetPacketType();

    regVal[0] = 0x34;
    regVal[1] = 0x44;
    SX1262WriteRegister(regVal, 0x0740, 1);
    SX1262WriteRegister(regVal+1, 0x0741, 1);

    SX1262SetModulationParams(true);
#ifdef _DEBUG_
    printf("-----Complete Configuration-----\n\r");
#endif
}

void LoRaWanTest()
{
    //uint8_t rn[4];
    //SX1262ReadRegister(rn, 0x0740, 2);
    //SX1262SetRx(0);
    //sleep(1);
    //SX1262ReadRegister(rn, 0x0819, 4);
    //SX1262ReadRegister(rn, 0x0740, 2);
    //SX1262ReadRegister(rn, 0x0819, 4);
    //SX1262SetStandby();
    //uint32_t rn = GenRand();
    SX1262GetIrqStatus();
    SX1262ClearIrqStatus();
    SX1262GetIrqStatus();
}

void LoRaWanJoin(uint8_t *appEui, uint8_t *devEui, uint8_t *key, uint16_t *devNonce)
{
    uint32_t mic;
    uint32_t i;
    uint8_t plaintext[19];
    uint32_t payload[23];
    uint32_t rn;

    //Tx setting
    SX1262SetStandby();
    SX1262SetRfFrequency(true);
    
    //prepare payload
    *plaintext = 0x00;
    for(i=0; i<8; i++)
    {
        *(plaintext+i+1) = *(appEui+7-i);
    }
    for(i=0; i<8; i++)
    {
        *(plaintext+i+9) = *(devEui+7-i);
    }
    rn = GenRand();
    *(plaintext+17) = rn & 0xff;
    *(plaintext+18) = (rn >> 8) & 0xff;
    *devNonce = (uint16_t) (rn & 0xffff);

    LoRaMacJoinComputeMic(plaintext, 19, key, &mic);

    for(i=0; i<19; i++)
    {
        *(payload+i) = (uint32_t)*(plaintext+i);
    }

    *(payload+19) = mic         & 0xff;
    *(payload+20) = (mic >> 8 ) & 0xff;
    *(payload+21) = (mic >> 16) & 0xff;
    *(payload+22) = (mic >> 24) & 0xff;

#ifdef _DEBUG_
    printf("Join message:");
    for(i=0; i<23; i++)
    {
        printf("%02x ", (*(payload+i) & 0xff));
    }
    printf("\n\r");
#endif

    //write payload and transmit
    SX1262WriteBuffer(payload, 0, 23);
    SX1262ReadBuffer(payload, 0, 23);
    SX1262SetPacketParams(23, true);

    SX1262ClearIrqStatus();
    SX1262SetDioIrqParams(true);

    SX1262SetTx(0);
    sleep(1);
#ifdef _DEBUG_
    printf("Tx setting done!\n\r");
    SX1262GetStatus();
    printf("-----Complete Join-----\n\r");
#endif
}

uint32_t GenRand()
{
    uint32_t rn;
    rn = IO_In32(TRNG_ADDR);
#ifdef _DEBUG_
    printf("Generate a random number: %08x\n\r", rn);
#endif
    return rn;
}

void LoRaWanRxConfig()
{
    SX1262SetStandby();
    SX1262SetRfFrequency(false);
    SX1262StopTimerOnPreamble();
    SX1262SetPacketType();
    SX1262SetModulationParams(false);
    SX1262SetPacketParams(128, false);
}

void LoRaWanAcceptTest()
{
    uint32_t payload[17];

    SX1262ClearIrqStatus();
    SX1262SetDioIrqParams(false);

    SX1262SetRx(0);
    sleep(6);

    SX1262SetStandby();
    SX1262ReadBuffer(payload, 0, 17);

    SX1262GetIrqStatus();
}

void LoRaWanAccept(uint8_t *appKey, uint8_t *decPld, uint8_t *nwkSKey, uint8_t *appSKey, uint16_t devNonce, uint8_t *appNonce, uint8_t *netID, uint32_t *devAddr)
{
    uint32_t payload[17];
    uint8_t  recPld[255];

    uint32_t i;
    uint32_t mic;

    SX1262ClearIrqStatus();
    SX1262SetDioIrqParams(false);

    SX1262SetRx(0);
    sleep(6);

    SX1262SetStandby();
    SX1262ReadBuffer(payload, 0, 17);

    SX1262GetIrqStatus();

    for(i=0; i<17; i++)
    {
        recPld[i] = (uint8_t)(payload[i] & 0xff);
    }

    *decPld = recPld[0];
    LoRaMacJoinDecrypt( recPld+1, 16, appKey, decPld+1);
#ifdef _DEBUG_
    printf("Decoded accept message: ");
    for(i=0; i<17; i++)
    {
        printf("%02x ", decPld[i]);
    }
    printf("\n\r");
#endif

    LoRaMacJoinComputeMic(decPld, 13, appKey, &mic);
#ifdef _DEBUG_
    printf("Compute join accept MIC: %08x\n\r", mic);
#endif

    *appNonce = decPld[1];
    *(appNonce+1) = decPld[2];
    *(appNonce+2) = decPld[3];

    *netID = decPld[4];
    *(netID+1) = decPld[5];
    *(netID+2) = decPld[6];

    *devAddr = ((uint32_t)decPld[7] & 0xff) | ((uint32_t)decPld[8] << 8) | ((uint32_t)decPld[9] << 16) | ((uint32_t)decPld[10] << 24);

#ifdef _DEBUG_
    printf("Get app nonce: %02x %02x %02x\n\r", *appNonce, *(appNonce+1), *(appNonce+2));
    printf("Get net ID: %02x %02x %02x\n\r", *netID, *(netID+1), *(netID+2));
    printf("Get dev addr: %08x\n\r", *devAddr);
#endif

    LoRaMacJoinComputeSKeys(appKey, decPld+1, devNonce, nwkSKey, appSKey);
#ifdef _DEBUG_
    printf("Generate network session key: ");
    for(i=0; i<16; i++)
    {
        printf("%02x ", *(nwkSKey+i));
    }
    printf("\n\r");
    printf("Generate application session key: ");
    for(i=0; i<16; i++)
    {
        printf("%02x ", *(appSKey+i));
    }
    printf("\n\r");
    printf("-----Complete Join Accept-----\n\r");
#endif
}

void LoRaWanUp(bool confirm, uint32_t devAddr, uint8_t *appSKey, uint8_t *nwkSKey, uint16_t frameCnt, uint8_t length, uint8_t fPort, uint8_t* msg)
{
    uint32_t mic;
    uint32_t i;
    uint8_t plaintext[255];
    uint8_t cryptedtext[255];
    uint32_t payload[255];

    if(confirm)
    {
        plaintext[0] = 0x80;
    }
    else
    {
        plaintext[0] = 0x40;
    }

    plaintext[1] = devAddr & 0xff;
    plaintext[2] = (devAddr >> 8)  & 0xff;
    plaintext[3] = (devAddr >> 16) & 0xff;
    plaintext[4] = (devAddr >> 24) & 0xff;

    if(confirm)
    {
        plaintext[5] = 0x00;//FCtrl
    }
    else
    {
        plaintext[5] = 0x00;//FCtrl
    }

    plaintext[6] = frameCnt & 0xff;
    plaintext[7] = (frameCnt >> 8) & 0xff;

    plaintext[8] = fPort;

    for(i=0; i<9; i++)
    {
        cryptedtext[i] = plaintext[i];
    }

    // for(i=0; i<length; i++)
    // {
    //     plaintext[9+i] = (frameCnt+i) & 0xff;
    // }


    for(i=0; i<length; i++)
    {
        plaintext[9+i] = msg[i];
    }

    if(length > 0)
    {
        LoRaMacPayloadEncrypt(plaintext+9, length, appSKey, devAddr, 0, (uint32_t) frameCnt, cryptedtext+9);
    }

    LoRaMacComputeMic(cryptedtext, length+9, nwkSKey, devAddr, 0, (uint32_t) frameCnt, &mic);

    for(i=0; i<9+length; i++)
    {
        payload[i] = (uint32_t) cryptedtext[i];
    }

    *(payload+9+length)   = mic         & 0xff;
    *(payload+9+length+1) = (mic >> 8 ) & 0xff;
    *(payload+9+length+2) = (mic >> 16) & 0xff;
    *(payload+9+length+3) = (mic >> 24) & 0xff;

    //sx1262 operation
    SX1262SetStandby();
    SX1262SetRfFrequency(true);

    //write payload and transmit
    SX1262WriteBuffer(payload, 0, length+13);
    SX1262ReadBuffer(payload, 0, length+13);
    SX1262SetPacketParams(length+13, true);

    SX1262ClearIrqStatus();
    SX1262SetDioIrqParams(true);

    SX1262SetTx(0);
    sleep(1);
#ifdef _DEBUG_
    SX1262GetStatus();
    if(confirm)
    {
        printf("-----Complete comfirmed uplink frame-----\n\r");
    }
    else
    {
        printf("-----Complete unconfirmed uplink frame-----\n\r");
    }
#endif
}

void LoRaWanDown(uint32_t devAddr, uint8_t *appSKey, uint16_t frameCnt, uint8_t* message)
{
    uint32_t payload[255];
    uint8_t  recPld[255];
    uint32_t i;

    SX1262ClearIrqStatus();
    SX1262SetDioIrqParams(false);

    SX1262SetRx(0);
    sleep(6);

    SX1262SetStandby();
    //SX1262ReadBuffer(payload, 0, 64);
    SX1262ReadBuffer(payload, 0, 30);
    SX1262GetIrqStatus();

    for(i=0; i<256; i++)
    {
        recPld[i] = (uint8_t)(payload[i] & 0xff);
        //printf("%02x", recPld[i]);
    }

    for(i = 0; i < 9; ++i){
        message[i] = recPld[i];
    }

    //uint8_t length = recPld[9];
    LoRaMacPayloadDecrypt(recPld+9, 247, appSKey, devAddr, 1, (uint32_t) frameCnt, message+9);

    // printf("----------Read downlink---------\n\r");
    // for(i=0; i<256; i++)
    // {
    //     printf("%02x", message[i]);
    // }
    // printf("\n\r--------------------------\n\r");
}
