#include <iostream>
#include <string>
#include <vector>
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

// 16 rows 10 cols
std::array<std::array<int, 10>, 16> tiles;

// game data
const int BLOCK_WIDTH = 48;
const int BLOCK_HEIGHT = 48;

// tetrominos

int tetrominoFallSpeed = 2;

std::vector<std::string> tetrominoNames = { "I", "O", "T", "S", "Z", "J", "L" };

struct Tetromino {
  Tetromino() {}

  Tetromino(std::array<std::array<int, 4>, 2> t_shape) : shape(t_shape) {}

  int col = 4;
  int row = 0;

  int x = BLOCK_WIDTH * col;
  int y = 0;

  int targetY = BLOCK_HEIGHT;
  int targetX = col * BLOCK_WIDTH;

  bool hasLanded = false;

  std::array<std::array<int, 4>, 2> shape;

  bool isMoving = false;
};

Tetromino createTetromino(std::string name) {

  if (name == "I") {
    return Tetromino({
      0, 0, 0, 0,
      1, 1, 1, 1
    });
  }

  if (name == "O") {
    return Tetromino({
      0, 1, 1, 0,
      0, 1, 1, 0
    });
  }

  if (name == "T") {
    return Tetromino({
      0, 1, 0, 0,
      1, 1, 1, 0
    });
  }

  if (name == "S") {
    return Tetromino({
      0, 1, 1, 0,
      1, 1, 0, 0
    });
  }

  if (name == "Z") {
    return Tetromino({
      1, 1, 0, 0,
      0, 1, 1, 0
    });
  }

  if (name == "J") {
    return Tetromino({
      1, 0, 0, 0,
      1, 1, 1, 0
    });
  }

  if (name == "L") {
    return Tetromino({
      0, 0, 1, 0,
      1, 1, 1, 0
    });
  }
}

Tetromino tetromino;

// game textures
SDL_Texture* blockTexture = nullptr;

// game rects
SDL_Rect blockRect;

bool initSDL();
SDL_Texture* loadTexture(const std::string& path);
bool loadMedia();
bool initGame();
void resetPlay();
void drawPlay();
void drawTetromino();
int random(int min, int max);
void generateTetromino();
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

  resetPlay();

  return true;
}

void resetPlay() {
  generateTetromino();
}

void drawPlay() {
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  drawTetromino();

  // draw landed tetrominos
	for (auto row = 0; row < tiles.size(); row++) {
		for (auto col = 0; col < tiles[row].size(); col++) {
			if (tiles[row][col] == 1) {
				blockRect = { BLOCK_WIDTH * col, BLOCK_HEIGHT * row, BLOCK_WIDTH, BLOCK_HEIGHT };
				SDL_RenderCopy(renderer, blockTexture, nullptr, &blockRect);
			}
		}
	}

	SDL_RenderPresent(renderer);
}

void drawTetromino() {
  // draws a falling tetromino
  for (auto row = 0; row < tetromino.shape.size(); row++) {
    for (auto col = 0; col < tetromino.shape[row].size(); col++) {
      if (tetromino.shape[row][col] == 1) {
        SDL_Rect rect = {
          tetromino.x + BLOCK_WIDTH * col,
          tetromino.y + BLOCK_HEIGHT * row,
          BLOCK_WIDTH,
          BLOCK_HEIGHT
        };
        SDL_RenderCopy(renderer, blockTexture, nullptr, &rect);
      }
    }
  }
}

// range : [min, max) max no inclusive
int random(int min, int max) {
  static bool first = true;
  if (first) {
	  //seeding for the first time only!
    srand(time(NULL));
    first = false;
  }
  return min + rand() % (max - min);
}

void generateTetromino() {
  const std::string tetrominoName = tetrominoNames[random(0, tetrominoNames.size())];
  tetromino = createTetromino(tetrominoName);
}

bool canMoveLeft() {
  for (auto row = 0; row < tetromino.shape.size(); row++) {
    for (auto col = 0; col < tetromino.shape[row].size(); col++) {
      if (tetromino.shape[row][col] == 1) {
        // check left board collision
        if (tetromino.col - 1 + col < 0) {
          return false;
        }
        // check left collsion with tetros
        if (tiles[tetromino.row + row][tetromino.col - 1 + col] == 1) {
          return false;
        }
      }
    }
  }
  return true;
}

bool canMoveRight() {
  for (auto row = 0; row < tetromino.shape.size(); row++) {
    for (auto col = 0; col < tetromino.shape[row].size(); col++) {
      if (tetromino.shape[row][col] == 1) {
        // check bottom board collision
        if (tetromino.col + 1 + col > 9) {
          return false;
        }
        // check right collision with tetros
        if (tiles[tetromino.row + row][tetromino.col + 1 + col] == 1) {
          return false;
        }
      }
    }
  }
  return true;
}

bool canMoveDown() {
  for (auto row = 0; row < tetromino.shape.size(); row++) {
    for (auto col = 0; col < tetromino.shape[row].size(); col++) {
      if (tetromino.shape[row][col] == 1) {
        // check bottom board collision
        if (tetromino.row + 1 + row > 15) {
          return false;
        }
        // check bottom collision with tetros
        if (tiles[tetromino.row + 1 + row][tetromino.col + col] == 1) {
          return false;
        }
      }
    }
  }
  return true;
}

void updateTetromino() {


  Uint8 *keys = (Uint8*)SDL_GetKeyboardState(NULL);
  if(keys[SDL_SCANCODE_DOWN]) {}

  if(keys[SDL_SCANCODE_LEFT]) {
    if (!tetromino.isMoving  && canMoveLeft()) {
      tetromino.col -= 1;
    }
    // tetro is moving right
    if (tetromino.targetX > tetromino.x) {
      tetromino.col -= 1;
    }
    tetromino.targetX = tetromino.col * BLOCK_WIDTH;
    tetromino.isMoving = true;
  }

  if(keys[SDL_SCANCODE_RIGHT]) {
    if (!tetromino.isMoving && canMoveRight()) {
      tetromino.col += 1;
    }
    // tetro is moving left
    if (tetromino.targetX < tetromino.x) {
      tetromino.col += 1;
    }

    tetromino.targetX = tetromino.col * BLOCK_WIDTH;
    tetromino.isMoving = true;
  }


  if (tetromino.isMoving) {
    if (tetromino.x == tetromino.targetX) {
      tetromino.isMoving = false;
    }
    if (tetromino.targetX < tetromino.x) {
      tetromino.x -= 2;
    }
    if (tetromino.targetX > tetromino.x) {
      tetromino.x += 2;
    }
  }

  if (tetromino.y == tetromino.targetY) {
    if (canMoveDown()) {
      tetromino.row += 1;
      tetromino.targetY = tetromino.row * BLOCK_HEIGHT;
    }
    else {
      for (auto row = 0; row < tetromino.shape.size(); row++) {
        for (auto col = 0; col < tetromino.shape[row].size(); col++) {
          if (tetromino.shape[row][col] == 1) {
            tiles[tetromino.row + row][tetromino.col + col] = 1;
          }
        }
      }
      generateTetromino();
    }
  }

  if (tetromino.y < tetromino.targetY) {
    tetromino.y += 2;
  }

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
