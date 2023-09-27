#include <monitor.h>
#include "autogen_data.c"

RTMK_CODE
int arch_mm_switch_view(int to, int push) {
		return 0;
}



RTMK_CODE
void arch_mm_init() {
        arch_mm_switch_view(BOOT_COMPARTMENT, PUSH);
}
