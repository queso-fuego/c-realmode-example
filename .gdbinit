target remote localhost:1234
add-symbol-file boot.elf
add-symbol-file stage2.elf
add-symbol-file kernel.elf
break boot
break stage2
break kmain
tui layout split
continue

