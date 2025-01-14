// Boot sector; 16 bit real mode
#include <stdint.h>

uint8_t drive;  // Drive number from DL on boot 

void read_disk_sectors(uint16_t c, uint8_t h, uint8_t s, uint8_t sects, uint16_t buf) {
    // INT 0x13 AH 0x02 read disk sectors to memory
    // AL    = # of sectors to read (1+)
    // CH    = low 8 bits of cylinder
    // CL    = bit 7-6 = high 2 bits of cylidner, bit 5-0 = sector 1-63 
    // DH    = head number
    // DL    = drive number (hard disk has bit 7 set)
    // ES:BX = buffer
    __asm__ volatile(
        "int $0x13\n" 
        : 
        : "a"(0x0200 | sects), 
          "b"(buf), 
          "c"((c & 0xFF) << 8 | ((c >> 8) & 0b11) << 6 | s), 
          "d"(h << 8 | drive)
        : "memory"
    );
}

// M A I N
void main() {
    // Read 8KB into memory right after boot sector for stage2 and kernel;
    //   adjust for actual amount as needed (e.g. 10 sectors + sizeof kernel & others)
    read_disk_sectors(0, 0, 2, 16, 0x7E00);

    void (*stage2)() = (void (*)())0x7E00;
    stage2();
}

// Entry point
[[gnu::naked, gnu::section(".text.boot")]] 
void boot() {
    __asm__ volatile(
        "cli\n"                 // Clear interrupts
        "cld\n"                 // Clear direction flag

        "xor %%ax, %%ax\n"      // Initialize stack 
        "mov %%ax, %%ss\n"
        "mov $0x7C00, %%sp\n"

        "mov %%ax, %%ds\n"      // Initialize data segment
        "mov %%ax, %%es\n"      // Initialize extra segment

        "mov %%dl, %[drive]\n"  // Store drive number
        "jmp main"              // Jump to main function

        : [drive]"=m"(drive)
        : 
        : "memory"
    );
}

