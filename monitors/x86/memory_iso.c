#include <rtmk.h>
#define PROTO
#include "autogen_data.c"

#define KLEE
#ifdef KLEE
unsigned char * _shared_region;
#endif 
void init_mpu();
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
# if 1
RTMK_DATA
char shadow_stack[32];
#endif 

RTMK_DATA 
int sp;

RTMK_DATA
volatile unsigned long long bridge_time;

RTMK_DATA
volatile unsigned long long exit_time;

RTMK_DATA
int goff;//Global offset across xcalls for shared memory


RTMK_CODE
int switch_view(int to, int push);
void rtmk_init() {
}

int counter;
#define STACK_LEN
RTMK_DATA
int current;

RTMK_DATA
int last;

RTMK_CODE
int lastCompart() {
		return last;
}
unsigned char highest = 0;
RTMK_CODE
int switch_view(int to, int push) {

		/* Until kernel is init no need to update MPU */
		if (init == 0) { 
				last = current; current = to;
				return last;
		}
		if (push) {
				shadow_stack[sp++] = to;
				if (sp>highest)
						highest = sp;
		} else {
				int temp = shadow_stack[--sp]; // Current 
				if (current != temp) {
						//	while(1); // This is a BUG.
				}
				temp = shadow_stack[sp-1];
				if (temp != to) {
						/* Buffer overflow condition */
						//	while(1); 
				}
		}
		last = current;
		current = to;
		return last;
}

