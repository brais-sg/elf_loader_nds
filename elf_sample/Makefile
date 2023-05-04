CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld

TARGET=elf_module.elf

.PHONY: all

all: main.c
	$(CC) -o $(TARGET) main.c -O2 -nostdlib -pie -fPIC -mcpu=arm946e-s -marm -shared

clean:
	rm -rf *.elf *.so $(TARGET)