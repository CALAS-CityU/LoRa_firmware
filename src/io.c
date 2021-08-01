#include <stdint.h>
#include "io.h"

uint32_t IO_In32(uint32_t addr)
{
    volatile uint32_t *localAddr = (uint32_t *) addr;
    return *localAddr;
}

void IO_Out32(uint32_t addr, uint32_t val)
{
    volatile uint32_t *localAddr = (uint32_t *) addr;
    *localAddr = val;
}


