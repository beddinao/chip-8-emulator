#include "chip8.h"

// //// /		DRAWING UTILS

void	draw_background(WIN *window, uint32_t color) {
	SDL_SetRenderDrawColor(window->renderer,
			(color >> 24) & 0xFF,
			(color >> 16) & 0xFF,
			(color >> 8) & 0xFF,
			color & 0xFF);

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

	//while (1) {
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

		for (unsigned y = 0; y < DIS_H; y++)
			for (unsigned x = 0; x < DIS_W; x++) {
				pthread_mutex_lock(&chip8_data->display_mutex);
				color = chip8_data->display[y * DIS_W + x] ? 0xFFFFFFFF : 0x000000FF;
				pthread_mutex_unlock(&chip8_data->display_mutex);

				SDL_SetRenderDrawColor(chip8_data->window->renderer,
						(color >> 24) & 0xFF,
						(color >> 16) & 0xFF,
						(color >> 8) & 0xFF,
						0xFF);
				
				SDL_FPoint points[scale_x*scale_y];

				unsigned In = 0;

				for (unsigned sy = 0; sy < scale_y; sy++)
					for (unsigned sx = 0; sx < scale_x; sx++) {
						points[In].x = x*scale_x+sx;
						points[In++].y = y*scale_y+sy;
					}

				SDL_RenderPoints(chip8_data->window->renderer, points, In);
			}
	
		SDL_RenderPresent(chip8_data->window->renderer);	

	//}
}


int	init_window(CHIP8 *chip8_data, char *ROM) {
	chip8_data->window->height = DEF_HEIGHT;
	chip8_data->window->width = DEF_WIDTH;
	if (!SDL_Init(SDL_INIT_EVENTS))
		return 0;
	chip8_data->window->win = SDL_CreateWindow(ROM, chip8_data->window->width, chip8_data->window->height, SDL_WINDOW_RESIZABLE);
	if (!chip8_data->window->win || !(chip8_data->window->renderer = SDL_CreateRenderer(chip8_data->window->win, NULL))) {
		if (chip8_data->window->win) SDL_DestroyWindow(chip8_data->window->win);
		SDL_Quit();
		return 0;
	}
	SDL_SetWindowMinimumSize(chip8_data->window->win, MIN_WIDTH, MIN_HEIGHT);
	//draw_background(chip8_data->window, 0xFFFFFFFF);
	draw_background(chip8_data->window, 0x0000FFFF);
	SDL_RenderPresent(chip8_data->window->renderer);
	sleep(5);
	return 1;
}
