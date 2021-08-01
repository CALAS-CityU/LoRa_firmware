#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

uint32_t IO_In32(uint32_t addr);
void IO_Out32(uint32_t addr, uint32_t val);

#endif
