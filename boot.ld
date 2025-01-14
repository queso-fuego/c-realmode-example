ENTRY(boot)

PHDRS {
    text   PT_LOAD FLAGS(0x5);  /* RX */
    data   PT_LOAD FLAGS(0x6);  /* RW */
    rodata PT_LOAD FLAGS(0x4);  /* R */
}

SECTIONS {
    /* x86 loads bootsector to memory at 0x7C00 */
    .text 0x7C00 : {
        KEEP(*(.text.boot)); /* Put entry point at start of code */
        *(.text*);
    } :text

    .rodata : { *(.rodata*); }         :rodata
    .data   : { *(.data*); }           :data
    .bss    : { *(.bss*); *(COMMON); } :data

    /* Bytes 511 & 512 must be boot signature */ 
    .bootsig 0x7DFE : { SHORT(0xAA55); } 

    /DISCARD/ : { 
        *(.note*); 
        *(.comment); 
        *(.rel*); 
        /* Add more as needed depending on what compiler spits out */
    }
}

