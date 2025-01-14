// Test "kernel"; 32 bit protected mode
#include <stdint.h>

// Color text mode video memory
uint8_t *vidmem = (uint8_t *)0xB8000;

void halt_cpu() { __asm__ volatile("cli\nhlt\n" : : "a"(0xc0ffee)); }

void print_string(uint8_t x, uint8_t y, char *s) {
    // Assuming 80x25 text mode; 2 bytes per character
    uint8_t *p = &vidmem[(y*160) + (x*2)];
    while (*s) {
        *p++ = *s++;    // Character
        *p++ = 0x07;    // Colors
    }
}

// M A I N / entry point
[[gnu::section(".text.kmain")]]
void kmain() {
    print_string(0, 2, "32 bit protected mode hello!"); 
    halt_cpu();
}

