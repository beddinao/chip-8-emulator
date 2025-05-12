#include <chip8.h> 

//// // /		INSTRUCTIONS

//	00E0:CLS
void	_00E0 (CHIP8* chip8_data) {
	memset(chip8_data->display, 0, sizeof(chip8_data->display));
}

//	00EE:RET
void	_00EE (CHIP8* chip8_data) {
	if (chip8_data->SP) {
		chip8_data->PC = chip8_data->stack[0];
		for (unsigned i = 0; i <= chip8_data->SP && i < STACK_DEPTH; i++) {
			if (i == chip8_data->SP||i == STACK_DEPTH - 1)
				chip8_data->stack[i] = 0;
			else	chip8_data->stack[i] = chip8_data->stack[i + 1];
		}
		chip8_data->SP -= 1;
	}
}

//	1NNN:jump addr
void	_1nnn (CHIP8* chip8_data) {
	chip8_data->PC = chip8_data->opcode & 0x0FFF;
}

//	2NNN:call addr
void	_2nnn (CHIP8* chip8_data) {
	if (chip8_data->SP < STACK_DEPTH) {
		for (unsigned i = STACK_DEPTH - 1; i; i--)
			chip8_data->stack[i] = chip8_data->stack[i - 1];
		chip8_data->SP += 1;
		chip8_data->stack[0] = chip8_data->PC;
		chip8_data->PC = chip8_data->opcode & 0x0FFF;
	}
}

//	3XKK:SE Vx, byte
void	_3xkk (CHIP8* chip8_data) {
	uint8_t	reg = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg < 16 && chip8_data->registers[reg] == (chip8_data->opcode & 0x00FF))
		chip8_data->PC += 2;
}

//	4XKK:SNE Vx, byte
void	_4xkk (CHIP8* chip8_data) {
	uint8_t	reg = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg < 16 && chip8_data->registers[reg] != (chip8_data->opcode & 0x00FF))
		chip8_data->PC += 2;
}

//	5XY0:SE Vx, Vy
void	_5xy0 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF
			&& chip8_data->registers[reg_x] == chip8_data->registers[reg_y])
		chip8_data->PC += 2;
}

//	6XKK:LD Vx, byte
void	_6xkk (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x <= 0xF)
		chip8_data->registers[reg_x] = chip8_data->opcode & 0x00FF;
}

//	7XKK:ADD Vx, byte
void	_7xkk (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x <= 0xF)
		chip8_data->registers[reg_x] += chip8_data->opcode & 0x00FF;
}

//	8XY0:LD Vx, Vy
void	_8xy0 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_y <= 0xF && reg_x <= 0xF)
		chip8_data->registers[reg_x] = chip8_data->registers[reg_y];
}

//	8XY1:OR Vx, Vy
void	_8xy1 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF)
		chip8_data->registers[reg_x] |= chip8_data->registers[reg_y];
}

//	8XY2: AND Vx, Vy
void	_8xy2 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF)
		chip8_data->registers[reg_x] &= chip8_data->registers[reg_y];
}

//	8XY3: XOR Vx, Vy
void	_8xy3 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF)
		chip8_data->registers[reg_x] ^= chip8_data->registers[reg_y];
}

//	8XY4: ADD Vx, Vy
void	_8xy4 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF) {
		uint16_t	sum = chip8_data->registers[reg_x] + chip8_data->registers[reg_y];
		chip8_data->registers[0xF] = sum > 0xFF ? 1 : 0;
		chip8_data->registers[reg_x] = sum & 0xFF;
	}
}

//	8XY5: SUB Vx, Vy
void	_8xy5 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF) {
		chip8_data->registers[0xF] = chip8_data->registers[reg_x] >= chip8_data->registers[reg_y] ? 1 : 0;
		chip8_data->registers[reg_x] = (chip8_data->registers[reg_x] - chip8_data->registers[reg_y]) & 0xFF;
	}
}

//	8XY6: SHR Vx {, Vy}
void	_8xy6 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF) {
		chip8_data->registers[reg_x] = chip8_data->registers[reg_y];
		chip8_data->registers[0xF] = chip8_data->registers[reg_x] & 0x1;
		chip8_data->registers[reg_x] >>= 1;
	}
}

//	8XY7:SUBN Vx, Vy
void	_8xy7 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF) {
		chip8_data->registers[0xF] = chip8_data->registers[reg_y] >= chip8_data->registers[reg_x] ? 1 : 0;
		chip8_data->registers[reg_x] = (chip8_data->registers[reg_y] - chip8_data->registers[reg_x]) & 0xFF;
	}

}

//	8XYE:SHL Vx {, Vy}
void	_8xyE (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF) {
		chip8_data->registers[reg_x] = chip8_data->registers[reg_y];
		chip8_data->registers[0xF] = (chip8_data->registers[reg_x] & 0x80) >> 7;
		chip8_data->registers[reg_x] <<= 1;
		chip8_data->registers[reg_x] &= 0xFF;
	}
}

//	9XY0: SNE Vx, Vy
void	_9xy0 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4;
	if (reg_x <= 0xF && reg_y <= 0xF
			&& chip8_data->registers[reg_x] != chip8_data->registers[reg_y])
		chip8_data->PC += 2;
}

//	ANNN: LD I, addr
void	_Annn (CHIP8* chip8_data) {
	chip8_data->IR = chip8_data->opcode & 0x0FFF;
}

//	BNNN: JP V0, addr
void	_Bnnn (CHIP8* chip8_data) {
	chip8_data->PC = (chip8_data->opcode & 0x0FFF) + chip8_data->registers[0];
}

//	CXKK:RND Vx, byte
void	_Cxkk (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x <= 0xF) {
		uint16_t	kk = chip8_data->opcode & 0x00FF;
		chip8_data->registers[reg_x] = kk & (rand() % 255);
	}
}

//	DXYN - DRW Vx, Vy, nibble
void	_Dxyn (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8,
		reg_y = (chip8_data->opcode & 0x00F0) >> 4,
		n_height = chip8_data->opcode & 0x000F;
	if (reg_x <= 0xF && reg_y <= 0xF) {
		unsigned x = chip8_data->registers[reg_x] % DIS_W;
		unsigned y = chip8_data->registers[reg_y] % DIS_H;
		uint8_t sprite_byte, sprite_pixel;
		uint32_t *display_pixel;
		chip8_data->registers[0xF] = 0;
		for (unsigned row = 0; row < n_height; row++) {
			sprite_byte = chip8_data->RAM[chip8_data->IR + row];
			for (unsigned col = 0; col < 8; col++) {
				if (x + col >= DIS_W || y + row >= DIS_H)
					continue;
				sprite_pixel = (sprite_byte >> (7 - col)) & 0x1;
				pthread_mutex_lock(&chip8_data->display_mutex);
				display_pixel = &chip8_data->display[(y + row) * DIS_W + (x + col)];
				if (*display_pixel == 1 && sprite_pixel == 1)
					chip8_data->registers[0xF] = 1;
				*display_pixel ^= sprite_pixel;
				pthread_mutex_unlock(&chip8_data->display_mutex);
			}
		}
		//render_display(chip8_data);
	}
}

//	EX9E:SKP Vx
void	_Ex9E (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	key = chip8_data->registers[ reg_x ];
	pthread_mutex_lock(&chip8_data->keys_mutex);
	if (chip8_data->keys[ key ]) {
		chip8_data->keys[ key ] = 0;
		chip8_data->PC += 2;
	}
	pthread_mutex_unlock(&chip8_data->keys_mutex);
}

//	EXA1:SKNP Vx
void	_ExA1 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint8_t	key = chip8_data->registers[reg_x];
	pthread_mutex_lock(&chip8_data->keys_mutex);
	if (chip8_data->keys[ key ]) 
		chip8_data->PC += 2;
	pthread_mutex_unlock(&chip8_data->keys_mutex);
}

//	FX07: LD Vx, DT
void	_Fx07 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x <= 0xF)
		chip8_data->registers[reg_x] = chip8_data->DT;
}

//	FX0A:LD Vx, K
void	_Fx0A (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	printf("ordered halt with key: %i\n", reg_x);
	if (reg_x <= 0xF) {
		chip8_data->halt = 1;
		chip8_data->key_register = reg_x;
		chip8_data->PC -= 2;
	}
}

//	FX15: LD DT, Vx
void	_Fx15 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x <= 0xF)
		chip8_data->DT = chip8_data->registers[reg_x];
}

//	FX18: LD ST, Vx - set sound timer = Vx.
void	_Fx18 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x <= 0xF)
		chip8_data->ST = chip8_data->registers[reg_x];
}

//	FX1E: ADD I, Vx - set I = I + Vx.
void	_Fx1E (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x < 16) {
		chip8_data->IR += chip8_data->registers[reg_x];
		chip8_data->IR = chip8_data->IR & 0xFFF;
	}
}

//	FX29:LD F, Vx - set I = location of sprite for digit Vx.
void	_Fx29 (CHIP8* chip8_data) {
	uint8_t	vx = (chip8_data->opcode & 0x0F00) >> 8;
	if (vx <= 0xF)
		chip8_data->IR = vx * 5 + FONTS_START;
}

//	FX33:LD B, Vx - store BCD representation of Vx in memory location I, I+1, and I+2
void	_Fx33 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	if (reg_x <= 0xF && chip8_data->IR >= MEMORY_START
			&& chip8_data->IR + 2 < chip8_data->memory_occupied + MEMORY_START) {
		chip8_data->RAM[ chip8_data->IR ] = chip8_data->registers[reg_x] / 100;
		chip8_data->RAM[ chip8_data->IR + 1 ] = (chip8_data->registers[reg_x] / 10) % 10;
		chip8_data->RAM[ chip8_data->IR + 2 ] = chip8_data->registers[reg_x] % 10;
	}
}

//	FX55: LD [I], Vx 
void	_Fx55 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint16_t	IR = chip8_data->IR;
	if (IR >= MEMORY_START && IR + reg_x < chip8_data->memory_occupied + MEMORY_START && reg_x <= 0xF) {
		for (uint8_t reg = 0; reg <= reg_x; reg++)
			chip8_data->RAM[IR + reg] = chip8_data->registers[reg];
	}
}

//	FX65: Lx Vx, [I]
void	_Fx65 (CHIP8* chip8_data) {
	uint8_t	reg_x = (chip8_data->opcode & 0x0F00) >> 8;
	uint16_t	IR = chip8_data->IR;
	for (uint8_t reg = 0; reg <= reg_x && reg <= 0xF && IR < chip8_data->memory_occupied + MEMORY_START
			&& IR > MEMORY_START; reg++, IR++)
		chip8_data->registers[reg] = chip8_data->RAM[IR];
}

/*
   (uint16_t) opcode
   - (0-4)   opcode & 0xF000 |
   - (4-8)   opcode & 0x0F00 >> 8 |
   - (8-12)  opcode & 0x00F0 >> 4 |
   - (12-16) opcode & 0x000F
   */

// /// /	CYCLE

void	*instruction_cycle(void *p) {
	uint16_t	instruction, valid_instruction;
	CHIP8 *chip8_data = (CHIP8*)p;
	chip8_data->PC = MEMORY_START;
	struct timeval f_time, s_time;
	memset(&f_time, 0, sizeof(f_time));
	memset(&s_time, 0, sizeof(s_time));

	gettimeofday(&f_time, NULL);
	while (1) {
		pthread_mutex_lock(&chip8_data->state_mutex);
		if (chip8_data->emu_state) {
			pthread_mutex_unlock(&chip8_data->state_mutex);
			return 0;
		}
		pthread_mutex_unlock(&chip8_data->state_mutex);

		gettimeofday(&s_time, NULL);
		if (abs((int)s_time.tv_usec - (int)f_time.tv_usec) > TIME_DIFF)
			gettimeofday(&f_time, NULL);
		else	continue;

		chip8_data->DT -= (chip8_data->DT ? 1 : 0);
		chip8_data->ST -= (chip8_data->ST ? 1 : 0);

		valid_instruction = 1;
		pthread_mutex_lock(&chip8_data->keys_mutex);
		for (unsigned i = 0; i <= 0xF; i++)
			if (chip8_data->keys[i]) {
				if (chip8_data->halt) {
					chip8_data->registers[chip8_data->key_register] = i;
					chip8_data->keys[i] = 0;
					chip8_data->halt = 0;
					chip8_data->PC += 2;
					valid_instruction = 0;
					break;
				}
				else	chip8_data->keys[i]--;
			}
		pthread_mutex_unlock(&chip8_data->keys_mutex);

		if (chip8_data->halt || !valid_instruction)
			continue;

		chip8_data->opcode = (chip8_data->RAM[ chip8_data->PC ] << 8 | chip8_data->RAM[ chip8_data->PC + 1 ]);
		chip8_data->PC += 2;

		instruction = chip8_data->opcode & 0xF000;

		if (instruction == 0x0000)
			switch (chip8_data->opcode & 0x00FF) {
				case 0x00E0: _00E0(chip8_data); break;
				case 0x00EE: _00EE(chip8_data); break;
				default: valid_instruction = 0;
			}
		else if (instruction >= 0x1000 && instruction <= 0x7000)
			chip8_data->_0_7_set[(instruction >> 0xC) - 0x1](chip8_data);
		else if (instruction == 0x8000) {
			uint8_t n = chip8_data->opcode & 0x000F;
			if (n <= 0x7 || n == 0xE)
				chip8_data->_8s_set[n](chip8_data);
			else	valid_instruction = 0;
		}
		else if (instruction >= 0x9000 && instruction <= 0xD000)
			chip8_data->_9_D_set[(instruction >> 0xC) - 0x9](chip8_data);
		else if (instruction == 0xE000)
			switch (chip8_data->opcode & 0x00FF) {
				case 0x009E: chip8_data->_Es_set[0](chip8_data); break;
				case 0x00A1: chip8_data->_Es_set[1](chip8_data); break;
				default: valid_instruction = 0;
			}
		else if (instruction == 0xF000)
			switch (chip8_data->opcode & 0x00FF) {
				case 0x0007: chip8_data->_Fs_set[0](chip8_data); break;
				case 0x000A: chip8_data->_Fs_set[1](chip8_data); break;
				case 0x0015: chip8_data->_Fs_set[2](chip8_data); break;
				case 0x0018: chip8_data->_Fs_set[3](chip8_data); break;
				case 0x001E: chip8_data->_Fs_set[4](chip8_data); break;
				case 0x0029: chip8_data->_Fs_set[5](chip8_data); break;
				case 0x0033: chip8_data->_Fs_set[6](chip8_data); break;
				case 0x0055: chip8_data->_Fs_set[7](chip8_data); break;
				case 0x0065: chip8_data->_Fs_set[8](chip8_data); break;
				default: valid_instruction = 0;
			}
		else	valid_instruction = 0;

		if (!valid_instruction
				|| chip8_data->PC > MEMORY_SIZE
				|| chip8_data->PC > chip8_data->memory_occupied + MEMORY_START
				|| chip8_data->PC < MEMORY_START) {
			pthread_mutex_lock(&chip8_data->state_mutex);
			chip8_data->emu_state = 1;
			pthread_mutex_unlock(&chip8_data->state_mutex);
			break;
		}
	}
	return 0;
}

void	load_instructions(CHIP8* chip8_data) {
	chip8_data->_0_7_set[0] = _1nnn;
	chip8_data->_0_7_set[1] = _2nnn;
	chip8_data->_0_7_set[2] = _3xkk;
	chip8_data->_0_7_set[3] = _4xkk;
	chip8_data->_0_7_set[4] = _5xy0;
	chip8_data->_0_7_set[5] = _6xkk;
	chip8_data->_0_7_set[6] = _7xkk;
	chip8_data->_8s_set[0] = _8xy0;
	chip8_data->_8s_set[1] = _8xy1;
	chip8_data->_8s_set[2] = _8xy2;
	chip8_data->_8s_set[3] = _8xy3;
	chip8_data->_8s_set[4] = _8xy4;
	chip8_data->_8s_set[5] = _8xy5;
	chip8_data->_8s_set[6] = _8xy6;
	chip8_data->_8s_set[7] = _8xy7;
	chip8_data->_8s_set[8] = _8xyE;
	chip8_data->_9_D_set[0] = _9xy0;
	chip8_data->_9_D_set[1] = _Annn;
	chip8_data->_9_D_set[2] = _Bnnn;
	chip8_data->_9_D_set[3] = _Cxkk;
	chip8_data->_9_D_set[4] = _Dxyn;
	chip8_data->_Es_set[0] = _Ex9E;
	chip8_data->_Es_set[1] = _ExA1;
	chip8_data->_Fs_set[0] = _Fx07;
	chip8_data->_Fs_set[1] = _Fx0A;
	chip8_data->_Fs_set[2] = _Fx15;
	chip8_data->_Fs_set[3] = _Fx18;
	chip8_data->_Fs_set[4] = _Fx1E;
	chip8_data->_Fs_set[5] = _Fx29;
	chip8_data->_Fs_set[6] = _Fx33;
	chip8_data->_Fs_set[7] = _Fx55;
	chip8_data->_Fs_set[8] = _Fx65;
}

CHIP8* chip8_data = NULL;

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

void	exec_clr() {
	printf("exec_clr exec\n");
}

void	exec_ldp(uint8_t *program, unsigned size) {
	printf("exec_ldp exec\n");
	if (!size || size > MAX_PROGRAM_SIZE)
		printf("cant load program to memory\n");

	memcpy(chip8_data->RAM + MEMORY_START, program, size);
	chip8_data->memory_occupied = size;
	chip8_data->PC = MEMORY_START;
	printf("program loaded successfully\n");
}

int	main(int c, char **v)
{
	srand(time(0));

	chip8_data = malloc(sizeof(CHIP8));
	if (!chip8_data) 
		return 1;
	memset(chip8_data, 0, sizeof(CHIP8));

	/// / //		LOADING ROM
	/*if (!load_to_memory(chip8_data, "./programs/roms/IBM_logo.ch8")) {
	  printf("failed to load program to memory\n");
	  free(chip8_data);
	  return 1;
	  }*/

	unsigned char chip_8_emulator_programs_IBM_logo_ch8[] = {
		0x00, 0xe0, 0xa2, 0x2a, 0x60, 0x0c, 0x61, 0x08, 0xd0, 0x1f, 0x70, 0x09,
		0xa2, 0x39, 0xd0, 0x1f, 0xa2, 0x48, 0x70, 0x08, 0xd0, 0x1f, 0x70, 0x04,
		0xa2, 0x57, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x66, 0xd0, 0x1f, 0x70, 0x08,
		0xa2, 0x75, 0xd0, 0x1f, 0x12, 0x28, 0xff, 0x00, 0xff, 0x00, 0x3c, 0x00,
		0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
		0x00, 0x38, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x38, 0x00, 0xff, 0x00, 0xff,
		0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x80, 0x00, 0xe0, 0x00,
		0xe0, 0x00, 0x80, 0xf8, 0x00, 0xfc, 0x00, 0x3e, 0x00, 0x3f, 0x00, 0x3b,
		0x00, 0x39, 0x00, 0xf8, 0x00, 0xf8, 0x03, 0x00, 0x07, 0x00, 0x0f, 0x00,
		0xbf, 0x00, 0xfb, 0x00, 0xf3, 0x00, 0xe3, 0x00, 0x43, 0xe0, 0x00, 0xe0,
		0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xe0, 0x00, 0xe0
	};
	unsigned int chip_8_emulator_programs_IBM_logo_ch8_len = 132;

	memcpy(chip8_data->RAM + MEMORY_START, chip_8_emulator_programs_IBM_logo_ch8, chip_8_emulator_programs_IBM_logo_ch8_len);
	chip8_data->memory_occupied = chip_8_emulator_programs_IBM_logo_ch8_len;

	// /// /		LOADING FONTS
	load_fonts(chip8_data);

	/// / //		INIT DISPLAY
	chip8_data->window = malloc(sizeof(WIN));
	if (!chip8_data->window || !init_window(chip8_data, "chip-8-emu")) {
		printf("failed to initialize SDL window\n");
		if (chip8_data->window) free(chip8_data->window);
		free(chip8_data);
		return 1;
	}

	/// /// /		LOADING INSTRUCTIONS
	load_instructions(chip8_data);

	/// / //		CYCLE
	pthread_mutex_init(&chip8_data->display_mutex, NULL);
	pthread_mutex_init(&chip8_data->state_mutex, NULL);
	pthread_mutex_init(&chip8_data->keys_mutex, NULL);
	pthread_create(&chip8_data->worker, NULL, instruction_cycle, chip8_data);

	emscripten_set_main_loop_arg(render_display, chip8_data, 0, 1);
}
