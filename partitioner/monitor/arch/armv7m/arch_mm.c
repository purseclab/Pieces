#include <monitor.h>
#include <arch/armv7m/arch.h>


//This should come from core:
extern unsigned int init;
extern unsigned int last;
extern unsigned int current;


//This comes from autogen:
#define BOOT_COMPARTMENT 0
#define KLEE
#ifdef KLEE
unsigned char * _shared_region;
#endif 
RTMK_DATA
int selectedFunction = 0;
RTMK_DATA
int arg = 0;
RTMK_DATA
int sret = 0;
RTMK_CODE
void SVC_Handler_Main( unsigned int *svc_args );
extern unsigned char * _shared_region;
RTMK_CODE void* rtmkcpy(void * dest, void * src, int size);

RTMK_DATA
volatile unsigned long long bridge_time;

RTMK_DATA
volatile unsigned long long exit_time;


RTMK_CODE
int switch_view(int to, int push);


RTMK_CODE 
void _init_mpu() {
		volatile unsigned int type = MPU->TYPE;

		__ISB();
		__DSB();
		/* Disable MPU */
		MPU->CTRL = 0;

		/* RTMK Code */
		MPU->RNR = 0;
		/* Configure region 0 to cover 512KB Flash (Normal, Non-Shared, Executable, Read-only) */
		MPU->RBAR = (code_base & MPU_RBAR_ADDR_Msk) | MPU_RBAR_VALID_Msk | 0;
		MPU->RASR = NORMAL_OUTER_INNER_WRITE_THROUGH_NON_SHAREABLE | code_size | RO_Msk | MPU_RASR_ENABLE_Msk;


		/* RTMK Data */
		MPU->RNR = 1;
		/* Configure region 1 to cover CPU 32KB SRAM (Normal, Non-Shared, Executable, Full Access) */
		MPU->RBAR = (data_base & MPU_RBAR_ADDR_Msk) | MPU_RBAR_VALID_Msk |  1;
		MPU->RASR = NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE|MPU_RASR_XN_Msk |P_RW_U_NA_Msk| data_size | MPU_RASR_ENABLE_Msk;


		/* Shared region */
		MPU->RNR = 2;
		/* Configure region 1 to cover CPU 32KB SRAM (Normal, Non-Shared, Executable, Full Access) */
		MPU->RBAR = (((unsigned int)_shared_region) & MPU_RBAR_ADDR_Msk) | MPU_RBAR_VALID_Msk |  2;
		MPU->RASR = NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE|MPU_RASR_XN_Msk |P_RW_U_NA_Msk| REGION_1K | MPU_RASR_ENABLE_Msk;

		//   MPU->RASR = REGION_Enabled |DEVICE_NON_SHAREABLE | REGION_16K | FULL_ACCESS;
		MPU->RNR = 4;
		/* Configure region 4 to cover 512KB APB Peripherials (Device, Non-Shared, Full Access Device, Full Access) */
		MPU->RBAR = 0x40000000 | MPU_RBAR_VALID_Msk | 4;
		MPU->RASR = MPU_RASR_ENABLE_Msk | DEVICE_NON_SHAREABLE | REGION_2M | FULL_ACCESS ;
		/* Configure region 5 to cover 2MB AHB Peripherials (Device, Non-Shared, Full Access Device, Full Access) */
		MPU->RNR = 5;
		MPU->RBAR = 0x50000000 | MPU_RBAR_VALID_Msk | 5;
		MPU->RASR = MPU_RASR_ENABLE_Msk | DEVICE_NON_SHAREABLE | REGION_2M | FULL_ACCESS;
		/* Enable MPU */
		MPU->CTRL = MPU_CTRL_ENABLE_Msk;

		__ISB();
		__DSB();
}
RTMK_CODE
int arch_mm_switch_view(int to, int push) {
        __DSB();
        /* Disable MPU */
        MPU->CTRL = 0;

        MPU->RNR = 7;
        /* Configure region 1 to cover CPU 32KB SRAM (Normal, Non-Shared, Executable, Full Access) */
        MPU->RBAR = (comp_info[to].start & MPU_RBAR_ADDR_Msk) | MPU_RBAR_VALID_Msk |  7;
        MPU->RASR = NORMAL_OUTER_INNER_WRITE_THROUGH_NON_SHAREABLE | comp_info[to].size | RO_Msk | MPU_RASR_ENABLE_Msk;


        MPU->RNR = 6;
        /* Configure region 1 to cover CPU 32KB SRAM (Normal, Non-Shared, Executable, Full Access) */
        MPU->RBAR = (comp_info[to].dstart & MPU_RBAR_ADDR_Msk) | MPU_RBAR_VALID_Msk |  6;
        MPU->RASR = NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE|MPU_RASR_XN_Msk |P_RW_U_NA_Msk| comp_info[to].dsize | MPU_RASR_ENABLE_Msk;

        MPU->RNR = 5;
        /* Configure region 1 to cover CPU 32KB SRAM (Normal, Non-Shared, Executable, Full Access) */
        MPU->RBAR = (comp_info[to].devstart & MPU_RBAR_ADDR_Msk) | MPU_RBAR_VALID_Msk |  5;
        MPU->RASR = MPU_RASR_ENABLE_Msk | DEVICE_NON_SHAREABLE | comp_info[to].devsize | MPU_RASR_ENABLE_Msk;

		/* Enable MPU */
        MPU->CTRL = MPU_CTRL_ENABLE_Msk;

        __ISB();
        __DSB();

		return 0;
}



RTMK_CODE
void arch_mm_init() {
        _init_mpu();
        init++;
        arch_mm_switch_view(BOOT_COMPARTMENT, PUSH);
}
