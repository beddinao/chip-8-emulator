#include <stdio.h>
#include <string.h>
#include <stdint.h>

int	main() {
	uint8_t		RAM[4096];
	uint16_t		stack[16];
	uint8_t		registers[16];
	uint16_t		IR;	// index register
	uint16_t		PC;	// program counter
	uint8_t		SP;	// stack pointer
	uint8_t		DT;	// delay timer
	uint8_t		ST;	// sound timer
	uint32_t		display[64*32];

	memset(&RAM, 0, sizeof(RAM));
	memset(&stack, 0, sizeof(stack));
	memset(&display, 0, sizeof(display));
	memset(&registers, 0, sizeof(registers));
}
