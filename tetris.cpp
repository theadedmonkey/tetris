#include <iostream>
#include <string>
#include <array>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_WIDTH_HALF = SCREEN_WIDTH / 2;

const int SCREEN_HEIGHT = 768;
const int SCREEN_HEIGHT_HALF = SCREEN_HEIGHT / 2;

// FPS constants
const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

// window and renderer
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// 10 cols 16 rows
std::array<std::array<int, 10>, 16> tiles;

std::array<std::array<int, 2>, 2> square = { 1, 1,
                                             1, 1 };


// game data
const int BLOCK_WIDTH = 32;
const int BLOCK_HEIGHT = 32;

// game textures
SDL_Texture* blockTexture = nullptr;

// game rects
SDL_Rect blockRect;

bool initSDL();
SDL_Texture* loadTexture(const std::string& path);
bool loadMedia();
bool initGame();
void drawPlay();
void updateTetromino();

bool initSDL() {
	if(SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) {
	  std::cout << " Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}

  if(TTF_Init() == -1) {
    std::cout << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
    return false;
  }

  window = SDL_CreateWindow(
		"Tetris",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
  if(window == NULL) {
    std::cout << "Failed to create window with error: " << SDL_GetError() << std::endl;
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == NULL) {
    std::cout << "Failed to create renderer with error: " << SDL_GetError() << std::endl;
    return false;
  }

	return true;
}

SDL_Texture* loadTexture(const std::string &path) {
  SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
  if (texture == nullptr) {
    std::cout << "Failed to load texture " << path << " error : " << SDL_GetError() << std::endl;
    return nullptr;
  }
  return texture;
}

bool loadMedia() {
	blockTexture = loadTexture("assets/block.png");
	if(!blockTexture) {
		return false;
	}
  return true;
}

bool initGame() {
  if(!initSDL()) {
    return false;
  }

  if(!loadMedia()) {
    return false;
  }

  // resetPlay();

  return true;
}

void drawPlay() {
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	for (auto col = 0; col < tiles.size(); col++) {
		for (auto row = 0; row < tiles[col].size(); row++) {
			if (tiles[col][row] == 1) {
				blockRect = { BLOCK_WIDTH * col, BLOCK_HEIGHT * row, BLOCK_WIDTH, BLOCK_HEIGHT };
				SDL_RenderCopy(renderer, blockTexture, nullptr, &blockRect);
			}
		}
	}

	SDL_RenderPresent(renderer);
}

void updateTetromino() {
  Uint8 *keys = (Uint8*)SDL_GetKeyboardState(NULL);

  if(keys[SDL_SCANCODE_DOWN]) {

  }

  if(keys[SDL_SCANCODE_LEFT]) {

  }

  if(keys[SDL_SCANCODE_RIGHT]) {

  }

	tiles[5][8] = 1;
	tiles[6][8] = 1;
	tiles[5][9] = 1;
	tiles[6][9] = 1;
}

int main( int argc, char* args[] ) {
  if(!initGame()) {
    return 0;
  }

	SDL_Event event;
  bool isRunning = true;

  while(isRunning) {
    long int oldTime = SDL_GetTicks();

    while(SDL_PollEvent(&event) != 0) {
			// close the game
			if(event.type == SDL_QUIT) {
				isRunning = false;
			}
    }

		updateTetromino();
		drawPlay();

    int frameTime = SDL_GetTicks() - oldTime;
    if(frameTime < DELAY_TIME) {
      SDL_Delay(DELAY_TIME - frameTime);
    }
  }

}
