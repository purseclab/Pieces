#include <monitor.h>
#define MK_METHOD
RTMK_DATA
int goff;//Global offset across xcalls for shared memory
RTMK_CODE void* rtmkcpy(void * dest, void * src, int size);
RTMK_DATA
extern
int total_secs;

typedef void (*xfunction0)(void);
typedef int  (*ifunction0)(void);
typedef int  (*ifunction1i)(int);
typedef int  (*ifunction1p)(void *);
typedef void  (*xfunction1i)(int);
typedef void*  (*pfunction1i)(int);
typedef void * (*pfunction1p) (void *);
typedef void  (*xfunction1p)(void *);
typedef void  (*xfunction2pi)(void *, int);
typedef void  (*xfunction3pii)(void *, int, int);
typedef void* (*pfunction1i)(int);
typedef void* (*pfunction3iii)(int, int, int);
typedef void* (*pfunction0)();
typedef void  (*xfunction2ii)(int, int);
typedef void  (*xfunction2pp)(void *, void *);
typedef void  (*xfunction2ip)(int, void *);
typedef int  (*ifunction2pi)(void *, int);
typedef int	 (*ifunction2pd)(void *, unsigned long long);
typedef int  (*ifunction2pp)(void *, void *);
typedef int  (*ifunction2ppi)(void *, void *, int);
typedef int  (*ifunction3pii)(void *, int, int);
typedef int  (*ifunction2ppii)(void *, void *, int, int);
typedef int  (*ifunction2pppi)(void *, void *, void *, int);
typedef int  (*ifunction5iiipi)(int, int, int, void*, int);
typedef int  (*ifunction6ppipip)(void *, void *,int, void*,int, void*);
typedef int  (*ifunction6piiipp)(void *, int, int, int, void*, void *);

RTMK_CODE void xcall_arg0(int to, void * funcp) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int ret= switch_view(to, 1);
		xfunction0 func = (xfunction0)funcp;
		BNCHEND(bridge_time, handle);
		func();
		BNCHSTART(bridge_time, handle);
		switch_view(ret, 0);
		BNCHEND(bridge_time, handle);
}

RTMK_CODE void xcall_arg1i(int to, void * funcp, int arg0, int size) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int ret= switch_view(to, 1);
		xfunction1i func = (xfunction1i)funcp;
		BNCHEND(bridge_time, handle);
		func(arg0);
		BNCHSTART(exit_time, handle);
		switch_view(ret, 0);
		BNCHEND(exit_time, handle);
}

RTMK_CODE void * pcall_arg1p(int to, void * funcp, char * arg0, int size) {
		unsigned long long handle;
        BNCHSTART(bridge_time, handle);
        unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
        void * pt = rtmkcpy((&_shared_region) + goff, arg0, size);
        int ret= switch_view(to,1);
        pfunction1p func = (pfunction1p)funcp;
        if (size > 0)
                mem_used = ((unsigned long )pt + size - mem_used);
        else
                mem_used = 0;
        goff += mem_used;
        BNCHEND(bridge_time, handle);
        void * bret = func(pt);
        BNCHSTART(exit_time, handle);
        goff -= mem_used;
        rtmkcpy(arg0, (&_shared_region) + goff, size);
        switch_view(ret,0);
        BNCHEND(exit_time, handle);
		return bret;
}
RTMK_CODE void * pcall_arg0(int to, void * funcp) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int ret= switch_view(to,1);
		pfunction0 func = (pfunction0)funcp;
		BNCHEND(bridge_time, handle);
		void * bret = func();
		BNCHSTART(exit_time, handle);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return bret;
}

RTMK_CODE void *  pcall_arg3iii(int to, void * funcp, int arg0, int size, int arg1, int size1, int arg2, int size2) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int ret= switch_view(to,1);
		pfunction3iii func = (pfunction3iii)funcp;
		BNCHEND(bridge_time, handle);
		void * bret = func(arg0,arg1,arg2);
		BNCHSTART(exit_time, handle);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return bret;
}
#ifdef STACK_SWITCH
RTMK_DATA
char stack[19][512];
RTMK_DATA
void * ret_temp_stack;
#endif 

//RTMK_CODE void* rtmkcpy(void * dest, void * src, int size);
RTMK_CODE void * pcall_arg1i(int to, void * funcp, int arg0, int size) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
#ifdef STACK_SWITCH
		int sp = 0;
		int comp = getCompartmentFromAddr(&sp);
#endif 
		int ret;
		pfunction1i func; 
		void * bret;
#ifdef STACK_SWITCH
		if (comp && comp!= to) {
				sp = ((unsigned)&bret -sizeof(bret));
				rtmkcpy(&stack[to][(sizeof(stack)/19) - (36)],((unsigned)&bret -sizeof(bret)), 40);
				/* Move to shared stack */
				//switch_stack();
				asm ("mov sp, %0\n\t"
								:
								: "r" (&stack[to][(sizeof(stack)/19) - 36]));
		}

#endif
		ret= switch_view(to,1);
		func = (pfunction1i)funcp;
		BNCHEND(bridge_time, handle);
		bret = func(arg0);
		BNCHSTART(exit_time, handle);
#ifdef STACK_SWITCH
		ret_temp_stack = bret;
#endif
		switch_view(ret,0);
#ifdef STACK_SWITCH
		if (comp && comp != to) {
				asm ("mov sp, %0\n\t"
								:
								: "r" (sp));
		}
		bret = ret_temp_stack;
#endif 
		BNCHEND(exit_time, handle);
		return bret;
}
RTMK_CODE void xcall_arg2ii(int to, void * funcp, int arg0, int size, int arg1, int size1) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int ret= switch_view(to,1);
		xfunction2ii func = (xfunction2ii)funcp;
		BNCHEND(bridge_time, handle);
		func(arg0, arg1);
		BNCHSTART(exit_time, handle);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
}

RTMK_CODE void xcall_arg2ip(int to, void * funcp, int arg0, int size, void * arg1, int size1) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * pt = rtmkcpy((&_shared_region) + goff, arg1, size1);
		int ret= switch_view(to,1);
		xfunction2ip func = (xfunction2ip)funcp;
		if (size1 > 0)
				mem_used = ((unsigned long )pt + size1 - mem_used);
		else 
				mem_used = 0;
		goff += mem_used;
		BNCHEND(bridge_time, handle);
		func(arg0, pt);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg1,  (&_shared_region) + goff, size1);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
}
#define DEBUG 
//__attribute__((always_inline))
RTMK_DATA
volatile unsigned int largeBuffers;
RTMK_DATA
volatile unsigned int possibleAtt;
RTMK_CODE void* rtmkcpy(void * dest, void * src, int size) {
		if (size<=0 || src == 0 || dest == 0) {
				return src;
		}
#ifdef DEBUG
		if ((size != -1)&&((unsigned long)src >= (unsigned long)0x40000000 || (unsigned long)src < (unsigned long)0x20000000)) {
				//Cmon bruh, can you even copy peripherals or code 
				while(1);
		}
		volatile unsigned int i =0;
		if (size <= 4) {
				possibleAtt++;
		}

		if  (size > 50) {
				largeBuffers++;
		}

#endif 
		// Typecast src and dest addresses to (char *)
		char *csrc = (char *)src;
		char *cdest = (char *)dest;

		// Copy contents of src[] to dest[]
		for (int i=0; i<size; i++)
				cdest[i] = csrc[i];

		//TODO: Investigate if this is necessary.
		//cdest[size] = 0;

		return dest;
}
RTMK_CODE void xcall_arg1p(int to, void * funcp, char * arg0, int size) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * pt = rtmkcpy((void *)((unsigned long) (&_shared_region) + goff), arg0, size);
		int ret= switch_view(to,1);
		xfunction1p func = (xfunction1p)funcp;
		if (size > 0)
				mem_used = ((unsigned long )pt + size - mem_used);
		else
				mem_used = 0;
		goff += mem_used;
		BNCHEND(bridge_time, handle);
		func(pt);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg0, (void *)((unsigned long) (&_shared_region) + goff), size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
}

RTMK_CODE void xcall_arg2pi(int to, void * funcp, char * arg0, int size, int arg1, int size1) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * pt = rtmkcpy((&_shared_region) + goff, arg0, size);
		int ret= switch_view(to,1);
		xfunction2pi func = (xfunction2pi)funcp;
		if (size > 0)
				mem_used = ((unsigned long )pt + size - mem_used);
		else
				mem_used = 0;
		goff += mem_used;
		BNCHEND(bridge_time, handle);
		func(pt, arg1);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy((&_shared_region) + goff, arg0, size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
}


RTMK_CODE void xcall_arg3pii(int to, void * funcp, char * arg0, int size, int arg1, int size1, int arg2, int size2) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * pt = rtmkcpy((&_shared_region) + goff, arg0, size);
		int ret= switch_view(to,1);
		xfunction3pii func = (xfunction3pii)funcp;
		if (size > 0)
				mem_used = ((unsigned long )pt + size - mem_used);
		else
				mem_used = 0;
		goff += mem_used;
		BNCHEND(bridge_time, handle);
		func(pt, arg1, arg2);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg0, (&_shared_region) + goff, size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
}
RTMK_CODE int icall_arg3pii(int to, void * funcp, char * arg0, int size, int arg1, int size1, int arg2, int size2) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * pt = rtmkcpy((&_shared_region) + goff, arg0, size);
		int ret= switch_view(to,1);
		ifunction3pii func = (ifunction3pii)funcp;
		if (size > 0)
				mem_used = ((unsigned long )pt + size - mem_used);
		else
				mem_used = 0;
		goff += mem_used;
		BNCHEND(bridge_time, handle);
		int realRet = func(pt, arg1, arg2);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg0, (&_shared_region) + goff, size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return realRet;
}
RTMK_CODE void xcall_arg2pp(int to, void * funcp, char * arg0, int size, char * arg1, int size1) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void *pt  = rtmkcpy((&_shared_region) + goff, arg0, size);
		int offset = 0;
		if (size>0) 
				offset += size;
		void *pt1 = rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg1, size1);
		if (size1 > 0)
				mem_used = ((unsigned long )pt1 + size1 - mem_used);
		else {
				if (size > 0)
						mem_used = ((unsigned long )pt + size - mem_used);
				else 
						mem_used =0; // Both pointers are opaque 
		}

		goff += mem_used;
		int ret= switch_view(to,1);
		xfunction2pp func = (xfunction2pp)funcp;
		BNCHEND(bridge_time, handle);
		func(pt, pt1);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg1, (void *)((unsigned int)&_shared_region + offset), size1);
		rtmkcpy(arg0, (&_shared_region) + goff, size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
}

RTMK_CODE int icall_arg0(int to, void * funcp) {
		unsigned long long handle;
#ifdef MK_METHOD
		BNCHSTART(bridge_time, handle);
		int ret= switch_view(to,1);
		ifunction0 func = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = func();
		BNCHSTART(exit_time, handle);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
#elif defined(AC_METHOD)
		BNCHSTART(bridge_time, handle);
		int ret = switch_view_svc(to);
		BNCHEND(bridge_time, handle);
		int retReal = func();
		BNCHSTART(bridge_time, handle);
		switch_view_svc(ret);
		BNCHEND(bridge_time, handle);
		return retReal;
#endif
}

RTMK_CODE int icall_arg1i(int to, void * funcp, int arg0, int size) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int ret= switch_view(to,1);
		ifunction1i func = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = func(arg0);
		BNCHSTART(exit_time, handle);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}

RTMK_CODE int icall_arg1i_noid(void * func, int arg0, int size) {
		return icall_arg1i(getCompartmentFromAddr((unsigned int)func), func, arg0, size);
}

RTMK_CODE int icall_arg1p(int to, void * funcp, void * arg0, int size) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = 0;
		void * pt = rtmkcpy((void *)((unsigned long)(&_shared_region ) + goff), arg0, size);
		if (size > 0)
				mem_used = size;
		int ret= switch_view(to,1);
		/* TODO: How to copy? */
		ifunction1p func = funcp;	
		goff += mem_used;
		BNCHEND(bridge_time, handle);
		int retReal = func(pt);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg0, (void *)((unsigned long)(&_shared_region ) + goff), size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}
RTMK_CODE int icall_arg1p_noid(void * func, void * arg0, int size) {
		return icall_arg1p(getCompartmentFromAddr((unsigned int)func), func, arg0, size);
}
RTMK_CODE int getCompartmentFromAddr(unsigned int addr) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int ret;
		int compartment = 0;
		/* TODO: Binary search would be far superior here */
		for (int i =0; i< total_secs; i++) {
				if (addr > comp_info[i].start && addr < comp_info[i].end) {
						compartment = i;
						break;
				}
				if (addr > comp_info[i].dstart && addr < comp_info[i].dend) {
						compartment = i;
						break;
				}
		}
		BNCHEND(bridge_time, handle);
		return compartment;
}

RTMK_CODE int icall_arg0_noid(ifunction0 func) {
		return icall_arg0(getCompartmentFromAddr((unsigned int)func), func);
}

RTMK_CODE void xcall_arg0_noid(xfunction0 func) {
		xcall_arg0(getCompartmentFromAddr((unsigned int)func), func);
}

RTMK_CODE void xcall_arg1p_noid(char * function, void * arg0, int size) {
		xcall_arg1p(getCompartmentFromAddr((unsigned int)function), function, arg0, size);
}


RTMK_CODE int icall_arg2pi(int to, void * funcp, void * arg0, int size, int arg1, int size1) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * pt = rtmkcpy((&_shared_region) + goff, arg0, size);
		int ret= switch_view(to,1);
		ifunction2pi fun = funcp;
		if (size > 0)
				mem_used = ((unsigned long )pt + size - mem_used);
		else
				mem_used = 0;
		goff += mem_used;
		BNCHEND(bridge_time, handle);
		int retReal = fun(pt, arg1);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg0, (&_shared_region) + goff, size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}


RTMK_CODE int icall_arg2pd(int to, void * funcp, void * arg0, int size, long long arg1, long long size1) {
        unsigned long long handle;
        BNCHSTART(bridge_time, handle);
        unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
        void * pt = rtmkcpy((&_shared_region) + goff, arg0, size);
        int ret= switch_view(to,1);
        ifunction2pd fun = funcp;
        if (size > 0)
                mem_used = ((unsigned long )pt + size - mem_used);
        else
                mem_used = 0;
        goff += mem_used;
        BNCHEND(bridge_time, handle);
        int retReal = fun(pt, arg1);
        BNCHSTART(exit_time, handle);
		goff -= mem_used;
        rtmkcpy(arg0, (&_shared_region) + goff, size);
        switch_view(ret,0);
        BNCHEND(exit_time, handle);
        return retReal;
}

RTMK_CODE int icall_arg2pp(int to, void * funcp, void * arg0, int size, void * arg1, int size1) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void *pt  = rtmkcpy((void *)((unsigned long)(&_shared_region) + goff), arg0, size);
		int offset = goff;
		if (size>0)
				offset += size;
		void *pt1 = rtmkcpy((void *)((unsigned long)(&_shared_region) + offset), arg1, size1);
		if (size1 > 0)
				mem_used = ((unsigned long )pt1 + size1 - mem_used);
		else {
				if (size>0) {
						mem_used = ((unsigned long )pt + size - mem_used);
				}
				else {
						mem_used =0;
				}
		}
		goff += mem_used;
		int ret= switch_view(to,1);
		ifunction2pp fun = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = fun(pt,pt1);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg1, (void *)((unsigned long)(&_shared_region) + offset), size1);
        rtmkcpy(arg0, (void *)((unsigned long)(&_shared_region) + goff), size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}

RTMK_CODE int icall_arg3ppi(int to, void * funcp, void * arg0, int size, void * arg1, int size1, int arg2, int size2) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int offset = goff;
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * p0 =rtmkcpy((&_shared_region) +goff, arg0, size);
		if (size>0)
				offset += size;
		void * p1 =rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg1, size1);
		if (size1 > 0)
				mem_used = ((unsigned long )p1 + size1 - mem_used);
		else {
				if (size > 0) {
						mem_used = ((unsigned long )p0 +size - mem_used);
				} else {
						mem_used = 0;
				}
		}
		goff += mem_used;
		int ret= switch_view(to,1);
		ifunction2ppi fun = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = fun(p0,p1, arg2);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg1, (void *)((unsigned int)&_shared_region + offset), size1);
        rtmkcpy(arg0, (&_shared_region) + goff, size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}

RTMK_CODE int icall_arg4ppii(int to, void * funcp, void * arg0, int size, void * arg1, int size1, int arg2, int size2, int arg3, int size3) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void *pt  = rtmkcpy((&_shared_region) + goff, arg0, size);
		int offset = goff;
		if (size>0)
				offset += size;
		void *pt1 = rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg1, size1);

		if (size1 > 0)
				mem_used = ((unsigned long )pt1 + size1 - mem_used);
		else {
				if (size > 0) {
						mem_used = ((unsigned long )pt + size - mem_used);
				} else {
						mem_used = 0;
				}
		}
		goff += mem_used;
		int ret= switch_view(to,1);
		ifunction2ppii fun = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = fun(pt, pt1, arg2, arg3);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg1, (void *)((unsigned int)&_shared_region + offset), size1);
        rtmkcpy(arg0, (&_shared_region) + goff, size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}

RTMK_CODE int icall_arg4pppi(int to, void * funcp, void * arg0, int size, void * arg1, int size1, void * arg2, int size2, int arg3, int size3) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		int offset = goff;
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * p0 =rtmkcpy((&_shared_region) + goff, arg0, size);
		if (size>0)
				offset += size;
		void * p1 =rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg1, size1);
		if (size1>0)
				offset += size1;
		void * p2 =rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg2, size2);

		if (size2 > 0)
				mem_used = ((unsigned long )p2 + size2 - mem_used);
		else {
				if (size1>0) {
						mem_used = ((unsigned long )p1 + size1 - mem_used);
				} else if (size > 0) {
						mem_used = ((unsigned long )p0 + size - mem_used);
				}
				else {
						mem_used = 0;
				}
		}
		goff += mem_used;
		int ret= switch_view(to,1);
		ifunction2pppi fun = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = fun(p0, p1, p2, arg3);
		BNCHSTART(exit_time, handle);
		offset=goff;
		goff -= mem_used;
		switch_view(ret,0);
		if (size2 > 0) {
                offset = offset - size2;
                rtmkcpy(arg2, (void *)((unsigned int)&_shared_region + offset), size2);
        }
        if (size1 > 0) {
                offset = offset - size1;
                rtmkcpy(arg1, (void *)((unsigned int)&_shared_region + offset), size1);
        }
        if (size>0) {
                offset = offset - size;
                rtmkcpy(arg0, (void *)((unsigned int)&_shared_region + offset), size);
        }
		BNCHEND(exit_time, handle);
		return retReal;
}

RTMK_CODE int icall_arg6ppipip(int to, void * funcp, void * arg0, int size, void * arg1, int size1, int arg2, int size2, void * arg3, int size3, 
				int arg4, int size4, void * arg5, int size5) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		int offset = goff;
		void * p0 =rtmkcpy((&_shared_region) + goff, arg0, size);
		unsigned long offset_ptr = (unsigned long) (&_shared_region) + goff;
		unsigned long offset_size = 0;
		if (size>0) {
				offset += size;
				offset_ptr = (unsigned long) p0;
				offset_size = size;
		}
		void * p1 =rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg1, size1);
		if (size1>0) {
				offset += size1;
				offset_ptr = (unsigned long)p1;
				offset_size = size1;
		}
		void * p2 =rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg3, size3);
		if (size3>0) {
				offset += size3;
				offset_ptr = (unsigned long)p2;
				offset_size = size3;
		}
		void * p3 =rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg5, size5);

		if (size5 > 0)
				mem_used = ((unsigned long )p3 + size5 - mem_used);
		else 
				mem_used = ((unsigned long )offset_ptr +offset_size - mem_used);
		goff += mem_used;
		int ret= switch_view(to,1);
		ifunction6ppipip fun = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = fun(p0, p1, arg2, p2, arg4, p3);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		if (size5 > 0) {
                offset = offset - size5;
                rtmkcpy(arg5, (void *)((unsigned int)&_shared_region + offset), size5);
        }
        if (size3 > 0) {
                offset = offset - size4;
                rtmkcpy(arg3, (void *)((unsigned int)&_shared_region + offset), size3);
        }
		if (size1>0) {
                offset = offset - size;
                rtmkcpy(arg1, (void *)((unsigned int)&_shared_region + offset), size1);
        }
        if (size>0) {
                offset = offset - size;
                rtmkcpy(arg0, (void *)((unsigned int)&_shared_region + offset), size);
        }
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}
RTMK_CODE int icall_arg6piiipp(int to, void * funcp, void * arg0, int size, int arg1, int size1, int arg2, int size2, int arg3, int size3,
				void* arg4, int size4, void * arg5, int size5) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		int offset = goff;
		unsigned long offset_ptr = (unsigned long) (&_shared_region) + goff;
		unsigned long offset_size = 0;
		void * p0 =rtmkcpy((&_shared_region) + goff, arg0, size);
		if (size>0) {
				offset += size;
				offset_ptr = (unsigned long)p0;
				offset_size = size;
		}
		void * p4 =rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg4, size4);
		if (size4>0) {
				offset += size4;
				offset_ptr = (unsigned long)p4;
				offset_size = size4;
		}
		void * p5 =rtmkcpy((void *)((unsigned int)&_shared_region + offset), arg5, size5);
		if (size5 > 0)
				mem_used = ((unsigned long )p5 + size5 - mem_used);
		else
				mem_used = ((unsigned long )offset_ptr + offset_size - mem_used);
		goff += mem_used;
		int ret= switch_view(to,1);
		ifunction6piiipp fun = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = fun(p0, arg1, arg2, arg3, p4, p5);
		BNCHSTART(exit_time, handle);
		if (size5 > 0) {
				offset = offset - size5;
				rtmkcpy(arg5, (void *)((unsigned int)&_shared_region + offset), size5);
		}
		if (size4 > 0) {
				offset = offset - size4;
				rtmkcpy(arg4, (void *)((unsigned int)&_shared_region + offset), size4);
		}
		if (size>0) {
				offset = offset - size;
				rtmkcpy(arg0, (void *)((unsigned int)&_shared_region + offset), size);
		}
		goff -= mem_used;
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}
RTMK_CODE int icall_arg5iiipi(int to, void * funcp, int arg0, int size, int arg1, int size1, int arg2, int size2, void * arg3, int size3,
				int arg4, int size4) {
		unsigned long long handle;
		BNCHSTART(bridge_time, handle);
		unsigned long mem_used = (unsigned long) (&_shared_region) + goff;
		void * p2 =rtmkcpy((&_shared_region) + goff, arg3, size3);
		if (size3 > 0)
				mem_used = ((unsigned long )p2 + size3 - mem_used);
		else
				mem_used = ((unsigned long )p2 - mem_used);
		goff += mem_used;
		int ret= switch_view(to,1);
		ifunction5iiipi fun = funcp;
		BNCHEND(bridge_time, handle);
		int retReal = fun(arg0, arg1, arg2, p2, arg4);
		BNCHSTART(exit_time, handle);
		goff -= mem_used;
		rtmkcpy(arg3, (&_shared_region) + goff, size);
		switch_view(ret,0);
		BNCHEND(exit_time, handle);
		return retReal;
}

#if 0
RTMK_CODE 
unsigned int mystrlen(const char *str) {
		int i =0;
		while(*str++!= '\0') {
				i++;
		}
		return i;
}

		RTMK_CODE
int mymemcmp(const void *m1, const void *m2, unsigned int n)
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
#endif 

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



/* 
   typedef struct {
   int start;
   int size;
   int dstart;
   int dsize;
   } SEC_INFO; */
