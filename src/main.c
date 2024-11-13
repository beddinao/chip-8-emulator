#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#define	MEMORY_SIZE	0x1000 // 4096
#define	MEMORY_START	0x200  // 512
#define	MAX_PROGRAM_SIZE	0xE00  // 3584
#define	STACK_DEPTH	16
#define	DISPLAY_HEIGHT	32
#define	DISPLAY_WIDTH	64

typedef	struct {
	uint8_t		RAM[MEMORY_SIZE];	// 0x200 - 512
	uint16_t		stack[STACK_DEPTH];
	uint8_t		registers[16];	// V0 - VF
	uint16_t		IR;		// index register
	uint16_t		PC;		// program counter
	uint8_t		SP;		// stack pointer
	uint8_t		DT;		// delay timer
	uint8_t		ST;		// sound timer
	uint32_t		display[DISPLAY_HEIGHT*DISPLAY_WIDTH];
	///////////////
	unsigned int	memory_occupied;
	//////////////	INSRUCTIONS	//////////////
	/////////////	FONTS		//////////////
}	CHIP8;

int	main(int c, char **v)
{
	if (c != 2) {
		printf("usage: ./chip8 [program to execute]\n");
		return 1;
	}

	FILE	*file = fopen(v[1], "rb");

	if (!file) {
		printf("unvalid file path\n");
		return 1;
	}

	CHIP8	*chip8_data = malloc(sizeof(CHIP8));
	if (!chip8_data) {
		printf("malloc failure\n");
		fclose(file);
		return 1;
	}
	memset(chip8_data, 0, sizeof(CHIP8));

	unsigned char buffer[MAX_PROGRAM_SIZE];
	memset(buffer, 0, sizeof(buffer));
	int chars_read = fread(buffer, sizeof(buffer), 1, file);
	if (!chars_read) {
		printf("%s\n", feof(file) ? "end of file" : ferror(file) ? "file error" : "wtf");
		printf("failed to load program into memory\n");
		free(chip8_data);
		fclose(file);
		return 1;
	}
	memcpy(chip8_data->RAM + MEMORY_START, buffer, chars_read);
	chip8_data->memory_occupied = chars_read;

	for (int i = 0; i < chars_read; i++)
		printf("%u ", buffer[i]);

	fclose(file);
	free(chip8_data);
	return 0;
}
