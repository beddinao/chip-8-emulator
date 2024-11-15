#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define	MEMORY_SIZE	0x1000 // 4096
#define	MEMORY_START	0x200  // 512
#define	MAX_PROGRAM_SIZE	0xE00  // 3584
#define	FONTS_START	0x50   // 80
#define	FONTS_SIZE	0x50   // 80
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
	unsigned		memory_occupied;
}	CHIP8;

int	load_to_memory(CHIP8* chip8_data, char *filename) {
	unsigned char buffer[MAX_PROGRAM_SIZE];
	unsigned chars_read;
	FILE *file = fopen(filename, "rb");
	if (!file) 
		return 0;

	memset(buffer, 0, sizeof(buffer));
	while ((chars_read = fread(buffer, 1, sizeof(buffer), file)) != 0) {
		if (chars_read + chip8_data->memory_occupied > MAX_PROGRAM_SIZE) {
			fclose(file);
			return 0;
		}
		memcpy(chip8_data->RAM + (MEMORY_START + chip8_data->memory_occupied), buffer, chars_read);
		chip8_data->memory_occupied += chars_read;
		memset(buffer, 0, sizeof(buffer));
	}
	fclose(file);

	if (!chip8_data->memory_occupied) 
		return 0;

	return 1;
}

void	load_fonts(CHIP8 *chip8_data) {
	uint8_t fonts[FONTS_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	};
	memcpy(chip8_data->RAM + FONTS_START, fonts, sizeof(fonts));
}

//// // /		INSTRUCTIONS

//	00E0:CLS - clear the display
void	_00E0 (CHIP8* chip8_data) {
	memset(chip8_data->display, 0, sizeof(chip8_data->display));
}

//	00EE:RET - return from a subroutine
void	_00EE (CHIP8* chip8_data) {};

//	1nnn:JP addr - jump to location nnn
void	_1nnn (CHIP8* chip8_data) {};

//	2nnn:CALL addr - call subroutine at nnn
void	_2nnn (CHIP8* chip8_data) {};

//	3xkk:SE Vx, byte - skip next instruction if Vx == kk
void	_3xkk (CHIP8* chip8_data) {};

//	4xkk: SNE Vx, byte - skip next instruction if Vx != kk
void	_4xkk (CHIP8* chip8_data) {};

//	5xy0: SE Vx, Vy - skip next instruction if Vx = Vy
void	_5xy0 (CHIP8* chip8_data) {};

//	6xkk: LD Vx, byte - put kk into register Vx
void	_6xkk (CHIP8* chip8_data) {};

//	7xkk: ADD Vx, byte - Add kk to the value of register Vx
void	_7xkk (CHIP8* chip8_data) {};

//	8xy0: LD Vx, Vy - store the value of register Vy in register Vx
void	_8xy0 (CHIP8* chip8_data) {};

//	8xy1: OR Vx, Vy - performs bitwise OR on the values of Vx and Vy,
//	then stores the result in Vx
void	_8xy1 (CHIP8* chip8_data) {};

//	8xy2: AND Vx, Vy - performs bitwise AND
void	_8xy2 (CHIP8* chip8_data) {};

//	8xy3: XOR Vx, Vy - performs bitwise XOR
void	_8xy3 (CHIP8* chip8_data) {};

//	8xy4: ADD Vx, Vy - The values of Vx and Vy are added together.
//	If the result is greater than 8 bits (i.e., > 255,) VF is set to 1,
//	otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx
void	_8xy4 (CHIP8* chip8_data) {};

//	8xy5: SUB Vx, Vy - If Vx > Vy, then VF is set to 1, otherwise 0.
//	Then Vy is subtracted from Vx, and the results stored in Vx.
void	_8xy5 (CHIP8* chip8_data) {};

//	8xy6: SHR Vx {, Vy} - If the least-significant bit of Vx is 1,
//	then VF is set to 1, otherwise 0. Then Vx is divided by 2.
void	_8xy6 (CHIP8* chip8_data) {};

//	8xy7: SUBN Vx, Vy - If Vy > Vx, then VF is set to 1, otherwise 0.
//	Then Vx is subtracted from Vy, and the results stored in Vx.
void	_8xy7 (CHIP8* chip8_data) {};

//	8xyE: SHL Vx {, Vy} - If the most-significant bit of Vx is 1,
//	then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
void	_8xyE (CHIP8* chip8_data) {};

int	main(int c, char **v)
{
	if (c != 2) {
		printf("usage: ./chip8 [program to execute]\n");
		return 1;
	}
	srand(time(0));
	
	CHIP8	*chip8_data = malloc(sizeof(CHIP8));
	if (!chip8_data) 
		return 1;

	/// / //		LOADING ROM
	if (!load_to_memory(chip8_data, v[1])) {
		printf("failed to load program to memory\n");
		free(chip8_data);
		return 1;
	}

	// /// /		LOADING FONTS
	load_fonts(chip8_data);

	/// // /		CICLE
	chip8_data->PC = *(chip8_data->RAM + MEMORY_START);
	

	free(chip8_data);
	return 0;
}
