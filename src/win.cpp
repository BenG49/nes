#include <win.hpp>

Window::Window(int w, int h)
	: width(w), height(h)
{
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		printf("Error initializing SDL: %s\n", SDL_GetError());
	}

	win = SDL_CreateWindow(
		"nes",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height, 0);
	
	renderer = SDL_CreateRenderer(win, -1, 0);

	tex = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGB888,
		SDL_TEXTUREACCESS_STATIC,
		width, height);
	
	pixels = (uint32_t *)malloc(width * 4 * height);
}

void Window::set_px(int x, int y, uint32_t rgb) {
	if (x > width || y > height) {
		perror("Attempted to set out of bounds pixel\n");
		return;
	}

	pixels[y * width + x] = rgb;
}

void Window::render()  {
	SDL_RenderClear(renderer);
	SDL_UpdateTexture(tex, nullptr, pixels, width * 4);
	SDL_RenderCopy(renderer, tex, nullptr, nullptr);
	SDL_RenderPresent(renderer);

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				exit(0);
		}
	}
}
