
all:
	arm-none-eabi-gcc -O0 -mthumb -mcpu=cortex-m0 -c main.c -o main.o
	arm-none-eabi-ld -Map=main.map -Tlinker.ld main.o -o main.elf
	arm-none-eabi-readelf -a main.elf >main.debug    
	arm-none-eabi-objcopy -O binary main.elf main.bin    
	arm-none-eabi-nm main.elf >main.nm
	arm-none-eabi-objdump -d main.elf >main.disasm  
clean:
	rm -rf *.elf *.o *.debug *.bin *.nm *.map *.disasm

