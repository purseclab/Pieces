#ifndef __RTMK_H
#define __RTMK_H
#include <stdint.h>

#define RTMK_CODE   __attribute__((section(".rtmkcode"))) extern
#define DRIVER_CODE __attribute__((section(".driver"))) static inline 
#define RTMK_CODE_INTERNAL __attribute__((section(".rtmkcode")))  static inline
#define INIT       __attribute__((section(".init")))
#define RTMK_DATA  __attribute__((section(".rtmkdata")))
#define OPAQUE	    __attribute__ ((xcallsize("opaque")))
#define STRING		__attribute__ ((xcallsize("string")))
#define SHARED_DATA __attribute__((section(".shared_data")))
#define UTILITY 	__attribute__((xcallsize("utility")))
#define KERNEL		__attribute__((section(".kernel")))
#define KERNEL_THREAD __attribute__((section(".kernelthread")))
#define OWNER(ownerlist...) __attribute((section("olist:" #ownerlist )))
#define PINNED(PID)	__attribute((section("pinned:" #PID )))
#define CLONE		__attribute((section("clone")))
#define SECRET		__attribute((section("secret")))
extern unsigned char * _shared_region;

typedef struct {
        int start;	//Start of code section
        int size;	//Size of code section
        int dstart;	//Start of data section
        int dsize;	//Size of data section
		int end;	//End of code
		int dend;	//End of data
		int devstart;	//Start of dev region
		int devsize;	//Size of dev region
		int devend;	//End of dev region
} SEC_INFO;

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

#define PUSH 1
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
#ifdef MICROBNCH_DEBUG
static inline void BNCHSTART(unsigned long long counter, unsigned long long handle) \
{										\
	if (bnch_init)handle = curr_stamp();				\
}										\

static inline void  BNCHEND(unsigned long long counter,unsigned long long handle) 			
{												\
		if (bnch_init) {unsigned long long temp= curr_stamp();		\
		if (temp > handle) {counter = counter + (temp - handle);}}	\
}
#elif defined(MICROBNCH)
#define BNCHSTART(counter, handle) \
{                                       \
    if (bnch_init)handle = curr_stamp();                \
}                                       \

#define BNCHEND(counter,handle)             \
{                                               \
        if (bnch_init) {unsigned long long temp= curr_stamp();      \
        if (temp > handle) {counter = counter + (temp - handle);}  else while(1);}\
}
#else
#define BNCHSTART(counter, handle)
#define BNCHEND(counter,handle)
#endif /* MICROBNCH */
#endif 
