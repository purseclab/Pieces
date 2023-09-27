#include <monitor.h>

/*********
  Locals
  ********/
RTMK_DATA 
static int counter;
RTMK_CODE
void _test_debug_mon() {
		/* Good testing script because gdb won't work with debugmon hack */
		counter++;
        switch (counter % 4) {
                case 3: //11
                        GPIOD->ODR ^= (1 << 14);
                        //GPIOD->ODR ^= (1 << 15);
                        break;
                case 2: //10
                        GPIOD->ODR ^= (1 << 15);
                        GPIOD->ODR ^= (1 << 14);
                        break;
                case 1: //01
                        GPIOD->ODR ^= (1 << 14);
                        //GPIOD->ODR ^= (1 << 15);
                        break;
                case 0:
                        GPIOD->ODR ^= (1 << 14);
                        GPIOD->ODR ^= (1 << 15);
                        break;
				default:
                        break;

        }
}

RTMK_DATA static int debugmon_hits = 0; 
RTMK_CODE
void arch_exceptions_debugmon_handler(sContextStateFrame *frame) {
		debugmon_hits++;
		arch_error_fatal();
		__asm("isb");
}


void arch_exceptions_init() {
		/* Setup debugmoni and other arch specific setup for exceptions */
		/* TODO: Copy code from old working copy */
}
