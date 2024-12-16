
all:
	arm-none-eabi-as -mthumb -mcpu=cortex-m0 boot_up.s -o boot_up.o
	arm-none-eabi-ld -Map=boot_up.map -Tlinker.ld  boot_up.o -o boot_up.elf
	arm-none-eabi-readelf -a boot_up.elf >boot_up.debug	
	arm-none-eabi-objcopy -O binary boot_up.elf boot_up.bin	
	arm-none-eabi-nm boot_up.elf >boot_up.nm

clean:
	rm -rf *.elf *.o *.debug *.bin *.nm *.map
