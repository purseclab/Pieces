#ifndef __ARCH_H
#define __ARCH_H
#define CONFIG_CPU_CORTEX_M4 
#include <stdint.h>
#include "../Device/ST/STM32F4xx/Include/stm32f4xx.h"
#include "arm_mpu_v7m.h"
extern unsigned char * _shared_region;

typedef struct __attribute__((packed)) ContextStateFrame {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t return_address;
  uint32_t xpsr;
} sContextStateFrame;


#define DWT_CTRL        (*(volatile unsigned int *)0xE0001000)
#define DWT_COMP0       (*(volatile unsigned int *)0xE0001020)
#define DWT_MASK0       (*(volatile unsigned int *)0xE0001024)

#define DWT_COMP1       (*(volatile unsigned int *)0xE0001030)
#define DWT_MASK1       (*(volatile unsigned int *)0xE0001034)
#define DWT_FUNCTION1   (*(volatile unsigned int *)0xE0001038)

#define DWT_FUNCTION0   (*(volatile unsigned int *)0xE0001028)
#define DEMCR           (*(volatile unsigned int *)0xE000EDFC)
#define DHCSR           (*(volatile unsigned int *)0xE000EDF0)
#define DFSR            (*(volatile unsigned int *)0xE000ED30)
//DEBUG RANGE: 0xE000EDF0 - 0xE000EEFF
//MPU RANGE: 0xE000ED90 - 0xE000EDEF
#define MON_EN          (1<<16)
#define C_DEBUGEN       (1<<0)
#define TRCENA          (1<<24)
#define MON_PEND        (1<<17)

RTMK_DATA
extern  SEC_INFO  comp_info[];
extern int code_base;
extern int code_size;
extern int data_base;
extern int data_size;

RTMK_CODE
int lastCompart();

RTMK_CODE
void rtmk_init();


RTMK_CODE
int switch_view(int to, int push);
RTMK_CODE unsigned long long curr_stamp();

RTMK_CODE int getCompartmentFromAddr(unsigned int addr);
#ifdef NORTMK
#define RTMK_TRAMPOLINE_ISR(func,suffix)\
void func(void)
#else 
#define RTMK_TRAMPOLINE_ISR(func,suffix) 							  \
__attribute__((section(".rtmkcode")))								  \
void func##suffix();												  \
void func(void){													  \
		int ret =switch_view(getCompartmentFromAddr(func##suffix),1); \
        func##suffix();												  \
        switch_view(ret, 0); 										  \
}																	  \
void func##suffix()										
#endif 
extern int bnch_init;
#endif 
