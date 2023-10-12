/* Hardware and starter kit includes. */
//#include "arm_comm.h"
extern void jail();
int foo() {
		volatile int i=0;
		while(i<0) i++;

		jail();
		return i;
}
int main(int argc, char *argv[])
{
//	rtmk_init();
	for( ;; ) {
			foo();
	}
}
