/* Hardware and starter kit includes. */
//#include "arm_comm.h"
extern int jail(void );
extern int mon_init();
int foo() {
		volatile int i=0;
		while(i<0) i++;

		jail();
		return i;
}

void __libc_csu_fini() {
    // Implement the finalization logic here
    // You may need to finalize other global objects or resources here
}

void __libc_csu_init() {
}

int main(int argc, char *argv[])
{
	for( ;; ) {
			foo();
	}
}
