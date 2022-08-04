#pragma once

#include <SDL2/SDL.h>

struct Window {
	SDL_Window *win;
	SDL_Renderer *renderer;
	SDL_Texture *tex;

	uint32_t *pixels;

	int width, height;

	Window(int w, int h);

	void set_px(int x, int y, uint32_t rgb);

	void render();
};
