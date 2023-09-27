RTMK_CODE 
unsigned int mon_lib_strlen(const char *str) {
		int i =0;
		while(*str++!= '\0') {
				i++;
		}
		return i;
}

RTMK_CODE
int mon_lib_memcmp(const void *m1, const void *m2, unsigned int n)
{
		const char *c1 = m1;
		const char *c2 = m2;

		if (!n) {
				return 0;
		}

		while ((--n > 0) && (*c1 == *c2)) {
				c1++;
				c2++;
		}

		return *c1 - *c2;
}

RTMK_CODE void _kill() {
		while(1);
}

RTMK_CODE void _getpid(){
		while(1);
}

RTMK_CODE void _read() {
		while(1);
}


RTMK_CODE  void _lseek() {
		while(1);
}
RTMK_CODE void _close() {
		while(1);
}

RTMK_CODE void _write() { while (1);}


RTMK_CODE void _fstat(){while (1);}
RTMK_CODE void _isatty(){while (1);}
