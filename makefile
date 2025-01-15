CC     = gcc
CFLAGS = -std=c23 -Wall -Wextra -Wpedantic \
		 -march=i386 \
		 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin \
		 -fno-asynchronous-unwind-tables \
		 -nostdlib -g \
		 -Wl,-nostdlib,-static,--omagic,--build-id=none

BINS_16 = boot stage2 
BINS_32 = kernel
DISK    = disk.raw
QEMU    = qemu-system-i386 -drive format=raw,file=$(DISK),media=disk,if=floppy

all: $(DISK)
	$(QEMU)	

debug: $(DISK)
	$(QEMU)	-S -s & gdb

$(BINS_16): %: %.c %.ld
	$(CC) $(CFLAGS) -m16 -Wl,-T$@.ld -o $@.elf $<
	objcopy -O binary --strip-debug $@.elf $@

$(BINS_32): %: %.c %.ld
	$(CC) $(CFLAGS) -m32 -Wl,-T$@.ld -o $@.elf $<
	objcopy -O binary --strip-debug $@.elf $@

$(DISK): $(BINS_16) $(BINS_32)
	cat $(BINS_16) > tmp16
	cat $(BINS_32) > tmp32
	dd if=/dev/zero of=$@ bs=1024 count=1440  	# For 1.44MB floppy
	dd if=tmp16     of=$@ bs=512  conv=notrunc
	dd if=tmp32     of=$@ bs=512  seek=$$(((0x9000-0x7C00)/512)) conv=notrunc # 0x9000 = kernel start
	rm -f tmp16 tmp32

clean:
	rm -f $(BINS_16) $(BINS_32) $(DISK) *.bc *.elf *.i *.map *.o *.s

