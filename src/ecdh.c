#include <stdint.h>
#include <stdlib.h>
#include "ecdh.h"

// base point for curve25519
static const uint8_t basepoint[32] = {9};

extern int 
curve25519_donna(uint8_t *pub_key, const uint8_t *pri_key, const uint8_t *base);

void generate_curve25519_pri_key(uint8_t *priv_key){
    // Generate random number 
    // Naive implementation, need further improvement to ensure security
    for(int i = 0; i < 32; ++i){
        priv_key[i] = rand() % 256;
    }
}

int generate_curve25519_pub_key(uint8_t *pub_key, const uint8_t *pri_key){
    return curve25519_donna(pub_key, pri_key, basepoint);
}

int generate_curve25519_shared_key(uint8_t *shared_key, const uint8_t *pri_key, const uint8_t *other_pub){
    return curve25519_donna(shared_key, pri_key, other_pub);
}

// void ecdh_process(uint8_t *raw_msg, uint8_t *pri_key, uint8_t *pub_key, uint8_t *shared_key){
//     /*******
    
//     Format of the packet between server and device:
//     | length (2 byte) | indicator (1 byte) | msg |
//     *******/ 
//     int front_offset = 9, back_offset = 4;

// }

