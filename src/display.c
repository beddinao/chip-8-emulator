#include "chip8.h"

// //// /		DRAWING UTILS

void	draw_background(WIN *window, uint32_t color) {
	/*SDL_SetRenderDrawColor(window->renderer,
			(color >> 24) & 0xFF,
			(color >> 16) & 0xFF,
			(color >> 8) & 0xFF,
			0xFF);*/

	SDL_SetRenderDrawColor(window->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(window->renderer);
}

unsigned	__calc_new_range(unsigned old_value,
		unsigned old_min, unsigned old_max,
		unsigned new_min, unsigned new_max) {
	if (old_value == old_min)
		return(new_min);
	return (((old_value - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min;
}

void	render_display(void *p) {
	CHIP8* chip8_data = (CHIP8*)p;
	unsigned scale_x, scale_y;
	uint32_t color;
	SDL_Event event;

	while (1) {
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT: close_hook(chip8_data); break;
				case SDL_EVENT_KEY_DOWN: key_hook(chip8_data, &event, 1); break;
				case SDL_EVENT_KEY_UP: key_hook(chip8_data, &event, 0); break;
				default: break;
			}
		}

		pthread_mutex_lock(&chip8_data->state_mutex);
		if (chip8_data->emu_state) {
			pthread_mutex_unlock(&chip8_data->state_mutex);
			close_hook(chip8_data);
		}
		pthread_mutex_unlock(&chip8_data->state_mutex);

		scale_x = chip8_data->window->width / DIS_W;
		scale_y = chip8_data->window->height / DIS_H;

		/*for (unsigned y = 0; y < DIS_H; y++)
			for (unsigned x = 0; x < DIS_W; x++) {
				pthread_mutex_lock(&chip8_data->display_mutex);
				color = chip8_data->display[y * DIS_W + x] ? 0xFFFFFF : 0x000000;
				pthread_mutex_unlock(&chip8_data->display_mutex);
				for (unsigned sy = 0; sy < scale_y; sy++)
					for (unsigned sx = 0; sx < scale_x; sx++)
						mlx_put_pixel(chip8_data->window->mlx_img, x*scale_x+sx, y*scale_y+sy, color<<8|0xFF);
			}
		*/
	}
}


int	init_window(CHIP8 *chip8_data, char *ROM) {
	WIN *window = chip8_data->window;
	SDL_Window *win = NULL;

	window->height = DEF_HEIGHT;
	window->width = DEF_WIDTH;
	
	if (!SDL_Init(SDL_INIT_EVENTS))
		return 0;

	win = SDL_CreateWindow(ROM, window->width, window->height, SDL_WINDOW_RESIZABLE);
	
	if (!win || !(window->renderer = SDL_CreateRenderer(win, NULL))) {
		if (win) SDL_DestroyWindow(win);
		return 0;
	}

	SDL_SetWindowMinimumSize(win, MIN_WIDTH, MIN_HEIGHT);

	window->win = win;

	draw_background(window, 0xFFFFFFFF);
	draw_background(win, 0xFFFFFFFF);
	return 1;
}
