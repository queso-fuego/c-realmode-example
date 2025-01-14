ENTRY(stage2)

PHDRS {
    text   PT_LOAD FLAGS(0x5);  /* RX */
    data   PT_LOAD FLAGS(0x6);  /* RW */
    rodata PT_LOAD FLAGS(0x4);  /* R */
}

SECTIONS {
    /* Put right after boot sector */
    .text 0x7E00 : {
        KEEP(*(.text.stage2)); 
        *(.text*);
    } :text

    .rodata : { *(.rodata*); }         :data
    .data   : { *(.data*); }           :data
    .bss    : { *(.bss*); *(COMMON); } :data

    /* Pad out size to next sector boundary */
    .value ALIGN(512)-1 : { BYTE(0); }

    /DISCARD/ : { 
        *(.note*); 
        *(.comment); 
        *(.rel*); 
        /* Add more as needed depending on what compiler spits out */
    }
}

