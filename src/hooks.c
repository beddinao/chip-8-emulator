#include <chip8.h>

/// / ////	HOOKS

void	close_hook(void *p) {
	CHIP8 *chip8_data = (CHIP8*)p;
	WIN* window = chip8_data->window;

	pthread_mutex_lock(&chip8_data->state_mutex);
	chip8_data->emu_state = 1;
	pthread_mutex_unlock(&chip8_data->state_mutex);
	pthread_join(chip8_data->worker, NULL);

	pthread_mutex_destroy(&chip8_data->display_mutex);
	pthread_mutex_destroy(&chip8_data->state_mutex);
	pthread_mutex_destroy(&chip8_data->keys_mutex);

	SDL_DestroyRenderer(window->renderer);
	SDL_DestroyWindow(window->win);
	SDL_Quit();

	free(chip8_data->window);
	free(chip8_data);

	exit(0);
}

void	key_hook(CHIP8* chip8_data, SDL_Event *event, int press) {
	SDL_Keycode key = event->key.key;

	pthread_mutex_lock(&chip8_data->keys_mutex);
	if (key == SDLK_ESCAPE) {
		pthread_mutex_unlock(&chip8_data->keys_mutex);
		close_hook(chip8_data);
	}
	else if (key >= SDLK_0 && key <= SDLK_9)
		chip8_data->keys[key - SDLK_0] = KEY_PRESS_CYCLES;
	else if (key >= SDLK_KP_1 && key <= SDLK_KP_0) {
		uint8_t i;
		switch (key) {
			case SDLK_KP_1: i = 7; break;
			case SDLK_KP_2: i = 8; break;
			case SDLK_KP_3: i = 9; break;
			case SDLK_KP_7: i = 1; break;
			case SDLK_KP_8: i = 2; break;
			case SDLK_KP_9: i = 3; break;
			default: i = key - SDLK_KP_0;
		}
		chip8_data->keys[ i ] = KEY_PRESS_CYCLES;
	}
	else if (key >= SDLK_A && key <= SDLK_F)
		chip8_data->keys[(key - SDLK_A) + 10] = KEY_PRESS_CYCLES;
	/*else if (key >= MLX_KEY_KP_DECIMAL && key <= MLX_KEY_KP_EQUAL) {
		uint8_t i;
		switch (keydata.key) {
			case MLX_KEY_KP_DECIMAL: i = 0xA; break;
			case MLX_KEY_KP_ENTER: i = 0xB; break;
			case MLX_KEY_KP_ADD: i = 0xC; break;
			case MLX_KEY_KP_SUBTRACT: i = 0xD; break;
			case MLX_KEY_KP_MULTIPLY: i = 0xE; break;
			case MLX_KEY_KP_DIVIDE: i = 0xF; break;
			default: i = 0xF;
		}
		chip8_data->keys[ i ] = KEY_PRESS_CYCLES;
	}*/
	pthread_mutex_unlock(&chip8_data->keys_mutex);
}

/*
void	resize_hook(int width, int height, void *p) {
	CHIP8 *chip8_data = (CHIP8*)p;
	unsigned	valid = 0;

	if (height > MIN_HEIGHT) {
		chip8_data->window->height = height;
		valid = 1;
	}
	if (width > MIN_WIDTH) {
		chip8_data->window->width = width;
		valid = 1;
	}

	if (valid) {
		if (!mlx_resize_image(chip8_data->window->mlx_img,
				chip8_data->window->width,
				chip8_data->window->height))
			close_hook(p);
		render_display(chip8_data);
	}
}
*/

