#include "sleep.h"
#include "glb_include.h"
#include "io.h"
#include <stdint.h>

void sleep(int second)
{
    uint32_t tEnd, tCur;
    IO_Out32(TIMER0_CMP_ADDR, 0xffffffff);
    IO_Out32(TIMER0_CTRL_ADDR, ((TIMER_PRE << 3) & 0x38) | 0x1);
    tEnd = TIMER_SEC * second;
    do
    {
         tCur = IO_In32(TIMER0_VAL_ADDR);
    } while (tCur < tEnd);
    return;
}

void msleep(int msecond)
{
    uint32_t tEnd, tCur;
    IO_Out32(TIMER0_CMP_ADDR, 0xffffffff);
    IO_Out32(TIMER0_CTRL_ADDR, ((TIMER_PRE << 3) & 0x38) | 0x1);
    tEnd = TIMER_MSEC * msecond;
    do
    {
         tCur = IO_In32(TIMER0_VAL_ADDR);
    } while (tCur < tEnd);
    return;
}

void usleep(int usecond)
{
    uint32_t tEnd, tCur;
    IO_Out32(TIMER0_CMP_ADDR, 0xffffffff);
    IO_Out32(TIMER0_CTRL_ADDR, ((TIMER_PRE << 3) & 0x38) | 0x1);
    tEnd = TIMER_USEC * usecond;
    do
    {
         tCur = IO_In32(TIMER0_VAL_ADDR);
    } while (tCur < tEnd);
    return;
}
