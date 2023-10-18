#include<monitor.h>
RTMK_DATA
int init;
RTMK_DATA
int current;
RTMK_DATA
int last;
RTMK_DATA
int shadow_stack[100];
RTMK_DATA
int sp;
RTMK_DATA
int highest;

extern int arch_mm_switch_view(int, int);
extern int arch_mm_init();
RTMK_CODE
int switch_view(int to, int push) {
		last = current; current = to;
		if (init == 0) {
                return last;
        }
		if (push) {
                shadow_stack[sp++] = to;
                if (sp>highest)
                        highest = sp;
        } else {
                int temp = shadow_stack[--sp]; // Current
                if (current != temp) {
                        //  while(1); // This is a BUG.
                }
                temp = shadow_stack[sp-1];
                if (temp != to) {
                        /* Buffer overflow condition */
                        //  while(1);
                }
        }
        arch_mm_switch_view(to, push);
		return last;
}

int mon_init(){
		arch_mm_init();
		return 0;
}
