#ifndef _ECDH_H_
#define _ECDH_H_

#include <stdint.h>
#include "sc_print.h"

// ECDH related 
void generate_curve25519_pri_key(uint8_t *priv_key);
int generate_curve25519_pub_key(uint8_t *pub_key, const uint8_t *pri_key);
int generate_curve25519_shared_key(uint8_t *shared_key, const uint8_t *pri_key, const uint8_t *other_pub);

// Message processing 
// void ecdh_process(uint8_t *raw_msg, uint8_t *pri_key, uint8_t *pub_key, uint8_t *shared_key);


#endif