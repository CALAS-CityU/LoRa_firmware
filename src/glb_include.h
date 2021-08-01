#ifndef _GLB_INCLUDE_H_
#define _GLB_INCLUDE_H_

#include <stdint.h>

//typedef int bool;
//#define _DEBUG_

#define true 1
#define false 0

#define TRNG_ADDR 0x1a109000
#define OP_ADDR	0x1a108000
#define WR_ADDR	0x1a108010
#define RD_ADDR	0x1a108020

#define TIMER0_VAL_ADDR  0x1a103000
#define TIMER0_CTRL_ADDR 0x1a103004
#define TIMER0_CMP_ADDR  0x1a103008

#define TIMER1_VAL_ADDR  0x1a103010
#define TIMER1_CTRL_ADDR 0x1a103014
#define TIMER1_CMP_ADDR  0x1a103018

//#define TIMER_CLK ((uint32_t)50)
//#define TIMER_PRE ((uint32_t)5)
//#define TIMER_USEC ((uint32_t)(TIMER_CLK / TIMER_PRE))
//#define TIMER_MSEC (((uint32_t)(TIMER_CLK / TIMER_PRE)) * 1e3)
//#define TIMER_SEC  (((uint32_t)(TIMER_CLK / TIMER_PRE)) * 1e6)

#define TIMER_CLK 50
#define TIMER_PRE 4

//#define TIMER_USEC 10
#define TIMER_USEC ((uint32_t)(TIMER_CLK / (TIMER_PRE + 1)))
#define TIMER_MSEC (TIMER_USEC * 1000)
#define TIMER_SEC  (TIMER_USEC * 1000000)

#define printf sc_printf

#endif
