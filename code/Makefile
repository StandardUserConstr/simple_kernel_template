all:
	nasm boot.asm -f bin -o program_files/boot.bin
	nasm kernel_entry.asm -f elf32 -o kernel_entry.o
	gcc -m32 -c kernel_main.c -o kernel_main.o 
	ld -m i386pe -o kernel.o -Ttext 0x1000 kernel_entry.o kernel_main.o
	objcopy -O binary -j .text kernel.o program_files/kernel.bin
	type program_files\boot.bin program_files\kernel.bin > program_files/drive.bin
	del /f /q *.o *.bin