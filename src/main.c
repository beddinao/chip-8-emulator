#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "MLX42.h"

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
	uint16_t		opcode;
	uint16_t		memory_occupied;
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
void	_00EE (CHIP8* chip8_data) {
	printf("00EE\n");
	if (chip8_data->SP) {
		chip8_data->PC = chip8_data->stack[0];
		for (unsigned i = 0; i <= chip8_data->SP && i < STACK_DEPTH; i++) {
			if (i == chip8_data->SP || i == STACK_DEPTH - 1)
				chip8_data->stack[i] = 0;
			else	chip8_data->stack[i] = chip8_data->stack[i + 1];
		}
		chip8_data->SP -= 1;
	}
}

//	1nnn:JP addr - jump to location nnn
void	_1nnn (CHIP8* chip8_data) {
	printf("1nnn\n");
	chip8_data->PC = chip8_data->opcode & 0x0FFF;
}

//	2nnn:CALL addr - call subroutine at nnn
void	_2nnn (CHIP8* chip8_data) {
	printf("2nnn\n");
	if (chip8_data->SP < STACK_DEPTH) {
		for (unsigned i = STACK_DEPTH - 1; i; i--)
			chip8_data->stack[i] = chip8_data->stack[i - 1];
		chip8_data->SP += 1;
		chip8_data->stack[0] = chip8_data->PC;
		chip8_data->PC = chip8_data->opcode & 0x0FFF;
	}
}

//	3xkk:SE Vx, byte - skip next instruction if Vx == kk
void	_3xkk (CHIP8* chip8_data) {
	printf("3xkk\n");
	uint8_t	reg = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg < 16 && chip8_data->registers[reg] == (chip8_data->opcode & 0x00FF))
		chip8_data->PC += 2;
}

//	4xkk: SNE Vx, byte - skip next instruction if Vx != kk
void	_4xkk (CHIP8* chip8_data) {
	printf("4xkk\n");
	uint8_t	reg = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg < 16 && chip8_data->registers[reg] != (chip8_data->opcode & 0x00FF))
		chip8_data->PC += 2;
}

//	5xy0: SE Vx, Vy - skip next instruction if Vx = Vy
void	_5xy0 (CHIP8* chip8_data) {
	printf("5xy0\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x < 16 && reg_y < 16
		&& chip8_data->registers[reg_x] == chip8_data->registers[reg_y])
		chip8_data->PC += 2;
}

//	6xkk: LD Vx, byte - put kk into register Vx
void	_6xkk (CHIP8* chip8_data) {
	printf("6xkk\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x < 16)
		chip8_data->registers[reg_x] = chip8_data->opcode & 0x00FF;
}

//	7xkk: ADD Vx, byte - Add kk to the value of register Vx
void	_7xkk (CHIP8* chip8_data) {
	printf("7xkk\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x < 16)
		chip8_data->registers[reg_x] += chip8_data->opcode & 0x00FF;
}

//	8xy0: LD Vx, Vy - store the value of register Vy in register Vx
void	_8xy0 (CHIP8* chip8_data) {
	printf("8xy0\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_y < 16 && reg_x < 16)
		chip8_data->registers[reg_x] = chip8_data->registers[reg_y];
}

//	8xy1: OR Vx, Vy - performs bitwise OR on the values of Vx and Vy,
//	then stores the result in Vx
void	_8xy1 (CHIP8* chip8_data) {
	printf("8xy1\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x < 16 && reg_y < 16)
		chip8_data->registers[reg_x] |= chip8_data->registers[reg_y];
}

//	8xy2: AND Vx, Vy - performs bitwise AND
void	_8xy2 (CHIP8* chip8_data) {
	printf("8xy2\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x < 16 && reg_y < 16)
		chip8_data->registers[reg_x] &= chip8_data->registers[reg_y];
}

//	8xy3: XOR Vx, Vy - performs bitwise XOR
void	_8xy3 (CHIP8* chip8_data) {
	printf("8xy3\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x < 16 && reg_y < 16)
		chip8_data->registers[reg_x] ^= chip8_data->registers[reg_y];
}

//	8xy4: ADD Vx, Vy - The values of Vx and Vy are added together.
//	If the result is greater than 8 bits (i.e., > 255,) VF is set to 1,
//	otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx
void	_8xy4 (CHIP8* chip8_data) {
	printf("8xy4\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x < 16 && reg_y < 16) {
		uint16_t	sum = chip8_data->registers[reg_x] + chip8_data->registers[reg_y];
		chip8_data->registers[15] = sum > 0xFF ? 1 : 0;
		chip8_data->registers[reg_x] = sum & 0xFF;
	}
}

//	8xy5: SUB Vx, Vy - If Vx > Vy, then VF is set to 1, otherwise 0.
//	Then Vy is subtracted from Vx, and the results stored in Vx.
void	_8xy5 (CHIP8* chip8_data) {
	printf("8xy5\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x < 16 && reg_y < 16) {

	}
}

//	8xy6: SHR Vx {, Vy} - If the least-significant bit of Vx is 1,
//	then VF is set to 1, otherwise 0. Then Vx is divided by 2.
void	_8xy6 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("8xy6\n");
}

//	8xy7: SUBN Vx, Vy - If Vy > Vx, then VF is set to 1, otherwise 0.
//	Then Vx is subtracted from Vy, and the results stored in Vx.
void	_8xy7 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("8xy7\n");
}

//	8xyE: SHL Vx {, Vy} - If the most-significant bit of Vx is 1,
//	then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
void	_8xyE (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("8xyE\n");
}

//	9xy0: SNE Vx, Vy - the values of  Vx and Vy are compared,
//	and if they are not equal, the PC in increased by 2
void	_9xy0 (CHIP8* chip8_data) {
	printf("9xy0\n");
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x < 16 && reg_y < 16
		&& chip8_data->registers[reg_x] != chip8_data->registers[reg_y])
		chip8_data->PC += 2;
}

//	Annn: LD I, addr - the value of register I is set to nnn
void	_Annn (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Annn\n");
}

//	Bnnn: JP V0, addr - the PC is set to nnn plus the value of V0
void	_Bnnn (CHIP8* chip8_data) {
	printf("Bnnn\n");
	chip8_data->PC = (chip8_data->opcode & 0x0FFF) + chip8_data->registers[0];
}

//	Cxkk: RND Vx, byte - generates a random number from 0 to 255,
//	which is then ANDed with the value kk. The results are stored in Vx
void	_Cxkk (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Cxkk\n");
}

//	Dxyn: DRW Vx, Vy, nibble - Display n-byte sprite starting at
//	memory location I at (Vx, Vy), set VF = collision
void	_Dxyn (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Dxyn\n");
}

//	Ex9E: SKP Vx - Skip next instruction if key with the value of Vx is pressed.
//	Checks the keyboard, and if the key corresponding to the value of Vx
//	is currently in the down position, PC is increased by 2.
void	_Ex9E (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Ex9E\n");
}

//	ExA1: SKNP Vx - skip next instruction if key of the value is not pressed.
void	_ExA1 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("ExA1\n");
}

//	Fx07: LD Vx, DT - set Vx = delay timer value
//	the value of DT is placed into Vx
void	_Fx07 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx07\n");
}

//	Fx0A: LD Vx, K - wait for a key pres, store the value of the key in Vx.
//	all execution stops until a key is pressed, then the value of that key
//	is stored in Vx
void	_Fx0A (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx0A\n");
}

//	Fx15: LD DT, Vx - set delay timer = Vx.
//	DT is set equal to the value of Vx
void	_Fx15 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx15\n");
}

//	Fx18: LD ST, Vx - set sound timer = Vx.
//	ST is set equal to the value of Vx
void	_Fx18 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx18\n");
}

//	Fx1E: ADD I, Vx - set I = I + Vx.
//	the values of I and Vx are added,
//	and the result are stored in I
void	_Fx1E (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx1E\n");
}

//	Fx29: LD F, Vx - set I = location of sprite for digit Vx.
//	the value of I is set to the location for the hex sprite corresponding
//	to the value of Vx.
void	_Fx29 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx29\n");
}

//	Fx33: LD B, Vx - store BCD representation of Vx in memory location I, I+1, and I+2
//	takes the decimal value of Vx, and places the hundreds digit in memory at location in I,
//	the tens digit at location I+1, and the ones digit at location I+2.
void	_Fx33 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx33\n");
}

//	Fx55: LD [I], Vx - store registers V0 through Vx in memory starting at location I.
//	copied the values of registers V0 through Vx into memory, starting at the address in I.
void	_Fx55 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx55\n");
}

//	Fx65: Lx Vx, [I] - read registers V0 through Vx from memory starting at location I
//	reads the values from memory starting at location I into registers V0 through Vx.
void	_Fx65 (CHIP8* chip8_data) {
	(void)chip8_data;
	printf("Fx65\n");
}

/*
   (uint16_t) opcode
   - (0-4)   opcode & 0xF000 |
   - (4-8)   opcode & 0x0F00 >> 8 |
   - (8-12)  opcode & 0x00F0 >> 4 |
   - (12-16) opcode & 0x000F
*/

// /// /	CYCLE
void	instruction_cycle(CHIP8* chip8_data) {
	//	instruction tables
	void	(*_0_7_set[7]) (CHIP8*) = { _1nnn, _2nnn, _3xkk, _4xkk, _5xy0, _6xkk, _7xkk };
	void	(*_8s_set[9]) (CHIP8*) = { _8xy0, _8xy1, _8xy2, _8xy3, _8xy4, _8xy5, _8xy6, _8xy7, _8xyE };
	void	(*_9_D_set[5]) (CHIP8*) = { _9xy0, _Annn, _Bnnn, _Cxkk, _Dxyn };
	void	(*_Es_set[2]) (CHIP8*) = { _Ex9E, _ExA1 };
	void	(*_Fs_set[9]) (CHIP8*) = { _Fx07, _Fx0A, _Fx15, _Fx18, _Fx1E, _Fx29, _Fx33, _Fx55, _Fx65};

	chip8_data->PC = *(chip8_data->RAM + MEMORY_START);
	uint16_t	instruction, valid_instruction;

	while (1) {
		chip8_data->opcode = (chip8_data->RAM[ chip8_data->PC ] << 8 | chip8_data->RAM[ chip8_data->PC + 1 ]);
		chip8_data->PC += 2;
		valid_instruction = 1;
		
		instruction = chip8_data->opcode & 0xF000;

		if (instruction > 0x0000 && instruction < 0x8000)
			_0_7_set[(instruction - 0x1000) / 0x1000](chip8_data);

		else if (instruction > 0x8000 && instruction < 0xE000)
			_9_D_set[(instruction - 0x9000) / 0x1000](chip8_data);

		else if (instruction == 0xF000)
			switch (chip8_data->opcode & 0x000F) {
				case 0x7:	_Fs_set[0](chip8_data); break;
				case 0xA: _Fs_set[1](chip8_data); break;
				case 0x5: _Fs_set[2](chip8_data); break;
				case 0x8: _Fs_set[3](chip8_data); break;
				case 0xE: _Fs_set[4](chip8_data); break;
				case 0x9: _Fs_set[5](chip8_data); break;
				case 0x3: _Fs_set[6](chip8_data); break;
				default: valid_instruction = 0;
			}
		else {
			switch (instruction) {
				case 0x0000:
					if (!(chip8_data->opcode & 0x000F))
						_00E0(chip8_data);
					else if ((chip8_data->opcode & 0x000F) == 0xE)
						_00EE(chip8_data);
					break;
				case 0x8000:
					if ((chip8_data->opcode & 0x000F) == 0xE || (chip8_data->opcode & 0x000F) < 0x8)
						_8s_set[chip8_data->opcode & 0x000F](chip8_data);
					break;
				case 0xE000:
					if ((chip8_data->opcode & 0x000F) == 0xE)
						_Es_set[0](chip8_data);
					else if ((chip8_data->opcode & 0x000F) == 0x1)
						_Es_set[1](chip8_data);
					break;
				default:
					valid_instruction = 0;
			}
		}
		if (!valid_instruction
			|| chip8_data->PC >= *(chip8_data->RAM + MEMORY_START + chip8_data->memory_occupied)
			|| chip8_data->PC < *(chip8_data->RAM + MEMORY_START))
			break;
	}
}

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
	memset(chip8_data, 0, sizeof(CHIP8));

	/// / //		LOADING ROM
	if (!load_to_memory(chip8_data, v[1])) {
		printf("failed to load program to memory\n");
		free(chip8_data);
		return 1;
	}

	// /// /		LOADING FONTS
	load_fonts(chip8_data);

	/// // /		CYCLE
	instruction_cycle(chip8_data);

	free(chip8_data);
	return 0;
}
