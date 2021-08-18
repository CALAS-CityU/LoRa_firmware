#ifndef _LORAWAN_H_
#define _LORAWAN_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "LoRaMacCrypto.h"
#include "sx1262.h"
#include "glb_include.h"

void LoRaWanInit();
void LoRaWanJoin(uint8_t *appEui, uint8_t *devEui, uint8_t *key, uint16_t *devNonce);
void LoRaWanTest();
void LoRaWanTxConfig();
uint32_t GenRand();
void LoRaWanRxConfig();
void LoRaWanAcceptTest();
void LoRaWanAccept(uint8_t *key, uint8_t *decPld, uint8_t *nwkSKey, uint8_t *appSKey, uint16_t devNonce, uint8_t *appNonce, uint8_t *netID, uint32_t *devAddr);
void LoRaWanUp(bool confirm, uint32_t devAddr, uint8_t *appSKey, uint8_t *nwkSKey, uint16_t frameCnt, uint8_t length, uint8_t fPort, uint8_t *msg);
void LoRaWanDown(uint32_t devAddr, uint8_t *appSKey, uint16_t frameCnt, uint8_t* msg);
#endif
