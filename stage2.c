// 2nd stage; 16 bit real mode, prekernel setup
#include <stdint.h>

void clear_screen() {
    // INT 0x10 AH = 0x07 Scroll down window 
    // BH = text attributes (bg/fg color) to clear to
    // CH/CL = upper left Y/X
    // DH/DL = lower right Y/X
    // 0-based indexing for screen size: 0x18 = 24, 0x4F = 79
    __asm__ volatile("int $0x10" : : "a"(0x0700), "b"(0x0700), "c"(0), "d"(0x184F));
}

void hide_cursor() {
    // INT 0x10 AH 0x01 set text mode cursor shape
    // CH = cursor top scan line and options; bit 7 = 0, 6-5 = blink (01 = invisible), 4-0 = line
    // CL = cursor bottom scan line in bits 4-0
    // 0x2607 = 0b0010_0110 0000_0111
    __asm__ volatile("int $0x10" : : "a"(0x0100), "c"(0x2607));
}

void set_cursor_position(uint8_t x, uint8_t y) {
    // INT 0x10 AH 0x02 set cursor position
    // BH = page number
    // DH = row, DL = column
    __asm__ volatile("int $0x10" : : "a"(0x0200), "b"(0), "d"(y << 8 | x));
}

void print_string(char *s) {
    while (*s) {
        // INT 0x10 AH = 0x0E teletype output 
        // AL = character to print
        // BH = page number, BL = fg color (if graphics mode)
        __asm__ volatile("int $0x10" : : "a"(0x0E00 | *s++) : "memory"); }
}

void load_gdt_and_jump_to_kernel() {
    typedef struct [[gnu::packed]] {
        uint16_t limit1;
        uint16_t base1;
        uint8_t  base2;
        uint8_t  access;
        uint8_t  flags_and_limit2;
        uint8_t  base3;
    } Descriptor;

    typedef struct [[gnu::packed]] {
        Descriptor null;    // Offset 0x00
        Descriptor code;    // Offset 0x08
        Descriptor data;    // Offset 0x10
    } Gdt;

    typedef struct [[gnu::packed]] {
        uint16_t size;
        uint32_t addr;
    } Gdtr;

    Gdt gdt = {
        { 0 },                                          // Null descriptor
        { 0xFFFF, 0x0000, 0x00, 0x9A, 0xCF, 0x00 },     // Code segment
        { 0xFFFF, 0x0000, 0x00, 0x92, 0xCF, 0x00 },     // Data segment
    };

    Gdtr gdtr = { sizeof gdt, (uint32_t)&gdt };

    // Load new GDT, enable protected mode, jump to kernel
    __asm__ volatile(
        "lgdt %[gdtr]\n"            // Load GDT

        "mov %%cr0, %%eax\n"        // Enable 32 bit protected mode
        "or $1, %%al\n"
        "mov %%eax, %%cr0\n"

        "jmp $0x08,$1f\n" // Far jump to set CS (code segment)

        ".code32\n"                 // Generate 32 bit code
        "1:\n"

        "mov $0x10, %%ax\n"         // Set rest of segment registers (data segment)
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"

        "mov $0x90000, %%esp\n"     // Set stack
        "jmp $0x08,$0x9000\n"       // Far jump to kernel

        ".code16gcc\n"              // Back to 16 bit code;
                                    //   compiled/assembled output can have code after this which
                                    //   needs to be 16 bit
        :
        : [gdtr]"g"(gdtr)
        : "memory"
    );
}

// Need memset for initializers/literals
void *memset(void *s, uint8_t c, uint32_t n) {
    void *rtn = s;
    uint8_t *p = s;
    while (n--) *p++ = c;
    return rtn;
}

// Need memcpy for initializers/literals
void *memcpy(void *dst, void *src, uint32_t n) {
    void *rtn = dst;
    uint8_t *x = dst, *y = src;
    while (n--) *x++ = *y++;
    return rtn;
}

// M A I N / entry point
[[gnu::section(".text.stage2")]] 
void stage2() {
    clear_screen();
    hide_cursor();
    set_cursor_position(0, 0);
    print_string("16 bit real mode hello!");

    load_gdt_and_jump_to_kernel();
}

