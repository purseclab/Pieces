ENTRY(_main)
_Min_Heap_Size = 0x200 ;    /* required amount of heap  */
_Min_Stack_Size = 0x400 ;   /* required amount of stack */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x8000000,   LENGTH = 1024K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K

}

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
				KEEP (*(.init))
    KEEP (*(.fini))
 		*(.text)
 		*(.text.*)
		*(.rodata)
		*(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
		_sidata = .;
	} >FLASH

	/* Initialized data will initially be loaded in FLASH at the end of the .text section. */
    .data : AT (_sidata) 
    {
		_sdata = .;
		*(.data)		/* Initialized data */
		_edata = .;
	} >RAM

	
	 /* Constant data into "FLASH" Rom type memory */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH
  

	.ARM : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >FLASH

	.bss : {
		_sbss = .;
		__bss_start__ = .;
		*(.bss)         /* Zero-filled run time allocate data memory */
		_ebss = .;
		__bss_end__ = .;
	} >RAM

	._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    PROVIDE ( __end__ = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM
   
	/* Initialized data sections into "RAM" Ram type memory */
  .data :
  {
    . = ALIGN(4);
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */

  } >RAM AT> FLASH
    _estack = ORIGIN(RAM) + LENGTH(RAM);
 }  
