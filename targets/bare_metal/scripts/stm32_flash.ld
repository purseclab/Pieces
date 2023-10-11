/*
*****************************************************************************
**
**  File        : stm32_flash.ld
**
**  Abstract    : Linker script for STM32F407VG Device with
**                1024KByte FLASH, 192KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Environment : Atollic TrueSTUDIO(R)
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
/*_estack = 0x20020000; */    /* end of 128K RAM on AHB bus*/

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
  STACK_RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 4K
  RAM (xrw)       : ORIGIN = 0x20001400, LENGTH = 512K /* 187K for all boards, for eval board we can increase */
  MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}
_estack = 0x20001400;
_shared_region = 0x20000000;

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
	*(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
    _exit = .;
  } >FLASH

  .rtmkcode :
  {
	. = ALIGN(32);
	_srtmkcode = .;
    *(.rtmkcode*)
    . = ALIGN(32);
	_ertmkcode = .;
  }
  .csection0 : 
  {
_scsection0 = .;
	  . = ALIGN(4);
	  *(.csection0)
	  . = ALIGN(4);
_ecsection0 = .;
  } > FLASH 
  .csection1 : 
  {
_scsection1 = .;
	  . = ALIGN(4);
	  *(.csection1)
	  . = ALIGN(4);
_ecsection1 = .;
  } > FLASH 
  .csection2 : 
  {
_scsection2 = .;
	  . = ALIGN(4);
	  *(.csection2)
	  . = ALIGN(4);
_ecsection2 = .;
  } > FLASH 
  .csection3 : 
  {
_scsection3 = .;
	  . = ALIGN(4);
	  *(.csection3)
	  . = ALIGN(4);
_ecsection3 = .;
  } > FLASH 
  .csection4 : 
  {
_scsection4 = .;
	  . = ALIGN(4);
	  *(.csection4)
	  . = ALIGN(4);
_ecsection4 = .;
  } > FLASH 
  .csection5 : 
  {
_scsection5 = .;
	  . = ALIGN(4);
	  *(.csection5)
	  . = ALIGN(4);
_ecsection5 = .;
  } > FLASH 
  .csection6 : 
  {
_scsection6 = .;
	  . = ALIGN(4);
	  *(.csection6)
	  . = ALIGN(4);
_ecsection6 = .;
  } > FLASH 
  .csection7 : 
  {
_scsection7 = .;
	  . = ALIGN(4);
	  *(.csection7)
	  . = ALIGN(4);
_ecsection7 = .;
  } > FLASH 
  .csection8 : 
  {
_scsection8 = .;
	  . = ALIGN(4);
	  *(.csection8)
	  . = ALIGN(4);
_ecsection8 = .;
  } > FLASH 
  .csection9 : 
  {
_scsection9 = .;
	  . = ALIGN(4);
	  *(.csection9)
	  . = ALIGN(4);
_ecsection9 = .;
  } > FLASH 
  .csection10 : 
  {
_scsection10 = .;
	  . = ALIGN(4);
	  *(.csection10)
	  . = ALIGN(4);
_ecsection10 = .;
  } > FLASH 
  .csection11 : 
  {
_scsection11 = .;
	  . = ALIGN(4);
	  *(.csection11)
	  . = ALIGN(4);
_ecsection11 = .;
  } > FLASH 
  .csection12 : 
  {
_scsection12 = .;
	  . = ALIGN(4);
	  *(.csection12)
	  . = ALIGN(4);
_ecsection12 = .;
  } > FLASH 
  .csection13 : 
  {
_scsection13 = .;
	  . = ALIGN(4);
	  *(.csection13)
	  . = ALIGN(4);
_ecsection13 = .;
  } > FLASH 
  .csection14 : 
  {
_scsection14 = .;
	  . = ALIGN(4);
	  *(.csection14)
	  . = ALIGN(4);
_ecsection14 = .;
  } > FLASH 
  .csection15 : 
  {
_scsection15 = .;
	  . = ALIGN(4);
	  *(.csection15)
	  . = ALIGN(4);
_ecsection15 = .;
  } > FLASH 
  .csection16 : 
  {
_scsection16 = .;
	  . = ALIGN(4);
	  *(.csection16)
	  . = ALIGN(4);
_ecsection16 = .;
  } > FLASH 
  .csection17 : 
  {
_scsection17 = .;
	  . = ALIGN(4);
	  *(.csection17)
	  . = ALIGN(4);
_ecsection17 = .;
  } > FLASH 
  .csection18 : 
  {
_scsection18 = .;
	  . = ALIGN(4);
	  *(.csection18)
	  . = ALIGN(4);
_ecsection18 = .;
  } > FLASH 
  .csection19 : 
  {
_scsection19 = .;
	  . = ALIGN(4);
	  *(.csection19)
	  . = ALIGN(4);
_ecsection19 = .;
  } > FLASH 

   .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM : {
    __exidx_start = .;
      *(.ARM.exidx*)
      __exidx_end = .;
    } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  _sisdata = .;
  .shared : AT ( _sisdata )
  {
          . = ALIGN(4);
          _ssdata = .;
          *(.shared_data*);
          . = ALIGN(4);
          _eshared = .;


  } >STACK_RAM
  _shared_region =  _eshared;

  _sidata = _sisdata +SIZEOF(.shared);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : AT ( _sidata)
  {
    . = ALIGN(32);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(32);
    _edata = .;        /* define a global symbol at data end */
  } >RAM
  compartLMA = SIZEOF(.data);
  .rtmkdata : AT (_sidata + compartLMA)
  {
      . = ALIGN(32);
      _srtmkdata = .;
      sstart = .;
      *(.rtmkdata*)
      . = ALIGN(32);
      _ertmkdata = .;
      _edata = .;
      send = .;
  }  > RAM
  compartLMA = compartLMA + SIZEOF(.rtmkdata);
  .kernsyms : AT (_sidata + compartLMA)
  {
      . = ALIGN(32);
      _skernsyms = .;
      *(.kernsyms*)
      . = ALIGN(32);
      _ekernsyms = .;
  }  > RAM
  .osection0 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection0 = .;
	  *(.osection0)
	  . = ALIGN(4);
	  _eosection0 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection0); 
  .osection1 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection1 = .;
	  *(.osection1)
	  . = ALIGN(4);
	  _eosection1 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection1); 
  .osection2 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection2 = .;
	  *(.osection2)
	  . = ALIGN(4);
	  _eosection2 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection2); 
  .osection3 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection3 = .;
	  *(.osection3)
	  . = ALIGN(4);
	  _eosection3 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection3); 
  .osection4 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection4 = .;
	  *(.osection4)
	  . = ALIGN(4);
	  _eosection4 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection4); 
  .osection5 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection5 = .;
	  *(.osection5)
	  . = ALIGN(4);
	  _eosection5 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection5); 
  .osection6 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection6 = .;
	  *(.osection6)
	  . = ALIGN(4);
	  _eosection6 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection6); 
  .osection7 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection7 = .;
	  *(.osection7)
	  . = ALIGN(4);
	  _eosection7 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection7); 
  .osection8 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection8 = .;
	  *(.osection8)
	  . = ALIGN(4);
	  _eosection8 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection8); 
  .osection9 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection9 = .;
	  *(.osection9)
	  . = ALIGN(4);
	  _eosection9 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection9); 
  .osection10 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection10 = .;
	  *(.osection10)
	  . = ALIGN(4);
	  _eosection10 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection10); 
  .osection11 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection11 = .;
	  *(.osection11)
	  . = ALIGN(4);
	  _eosection11 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection11); 
  .osection12 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection12 = .;
	  *(.osection12)
	  . = ALIGN(4);
	  _eosection12 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection12); 
  .osection13 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection13 = .;
	  *(.osection13)
	  . = ALIGN(4);
	  _eosection13 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection13); 
  .osection14 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection14 = .;
	  *(.osection14)
	  . = ALIGN(4);
	  _eosection14 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection14); 
  .osection15 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection15 = .;
	  *(.osection15)
	  . = ALIGN(4);
	  _eosection15 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection15); 
  .osection16 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection16 = .;
	  *(.osection16)
	  . = ALIGN(4);
	  _eosection16 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection16); 
  .osection17 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection17 = .;
	  *(.osection17)
	  . = ALIGN(4);
	  _eosection17 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection17); 
  .osection18 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection18 = .;
	  *(.osection18)
	  . = ALIGN(4);
	  _eosection18 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection18); 
  .osection19 : AT ( _sidata  + compartLMA)
  {
	  . = ALIGN(4);
	  _sosection19 = .;
	  *(.osection19)
	  . = ALIGN(4);
	  _eosection19 = .;
  }  > RAM 
compartLMA = compartLMA + SIZEOF(.osection19); 
  _edata = .;
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left 
  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM
  */

  /* MEMORY_bank1 section, code must be located here explicitly            */
  /* Example: extern int foo(void) __attribute__ ((section (".mb1text"))); */
  .memory_b1_text :
  {
    *(.mb1text)        /* .mb1text sections (code) */
    *(.mb1text*)       /* .mb1text* sections (code)  */
    *(.mb1rodata)      /* read-only data (constants) */
    *(.mb1rodata*)
  } >MEMORY_B1

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
