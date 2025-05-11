#ifndef CHIP8_H
# define CHIP8_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <SDL3/SDL.h> 

#define	MEMORY_SIZE	0x1000 // 4096
#define	MEMORY_START	0x200  // 512
#define	MAX_PROGRAM_SIZE	0xE00  // 3584
#define	FONTS_START	0x50
#define	FONTS_SIZE	0x50
#define	STACK_DEPTH	16
#define	DIS_H		32
#define	DIS_W		64
#define	DEF_HEIGHT	0x300
#define	DEF_WIDTH		0x640
#define	MIN_HEIGHT	0x190  // 400
#define	MIN_WIDTH		0x3E8  // 1000
#define	CYCLES_PER_TIMER	0xFFFFF
#define	KEY_PRESS_CYCLES	0x50
#define	TIME_DIFF		0x7D0

typedef	struct {
	SDL_Window	*win;
	SDL_Renderer	*renderer;

	unsigned		width;
	unsigned		height;
}	WIN;

typedef	struct chip8 {
	uint8_t		RAM[MEMORY_SIZE];	// 0x200 - 512
	uint16_t		stack[STACK_DEPTH];
	uint8_t		registers[16];	// V0 - VF
	uint16_t		IR;		// index register
	uint16_t		PC;		// program counter
	uint8_t		SP;		// stack pointer
	uint8_t		DT;		// delay timer
	uint8_t		ST;		// sound timer
	uint32_t		CT;		// cycles timer
	uint32_t		display[DIS_H*DIS_W];
	///////////////
	void		(*_0_7_set[7])(struct chip8*);
	void		(*_8s_set[9])(struct chip8*);
	void		(*_9_D_set[5])(struct chip8*);
	void		(*_Es_set[2])(struct chip8*);
	void		(*_Fs_set[9])(struct chip8*);
	///////////////
	uint8_t		keys[16];
	uint8_t		key_register;
	uint8_t		halt;
	///////////////
	WIN		*window;
	uint16_t		opcode;
	uint16_t		memory_occupied;
	//////////////
	uint8_t		emu_state;
	pthread_t		worker;
	pthread_mutex_t	display_mutex;
	pthread_mutex_t	keys_mutex;
	pthread_mutex_t	state_mutex;
}	CHIP8;

int	init_window(CHIP8*, char*);
void	render_display(void*);

/* hooks.c */
void	key_hook(CHIP8*, SDL_Event*, int);
void	close_hook(void*);

#endif
