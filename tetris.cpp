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

// 22 rows 10 cols
std::array<std::array<int, 10>, 22> tiles;

int tickDuration = 500.0f;
int tickCurrent;
int tickLast;
int tickAccum;

// game data
const int BLOCK_WIDTH = 32;
const int BLOCK_HEIGHT = 32;

// tetrominos
std::vector<std::string> tetrominoNames = { "I", "O", "T", "S", "Z", "J", "L" };

struct Tetromino {
  Tetromino() {}

  Tetromino(
    std::vector<std::vector<std::vector<int>>> t_shapes,
    int t_row = 0,
    int t_col = 0
  ) : shapes(t_shapes), row(t_row), col(t_col) {
    shape = shapes[shapeIdx];
  }

  int col = 0;
  int row = 0;

  int x = BLOCK_WIDTH * col;
  int y = BLOCK_HEIGHT * row;

  int targetX = BLOCK_WIDTH * col;
  int targetY = BLOCK_HEIGHT * row;

  int shapeIdx = 0;
  std::vector<std::vector<int>> shape;
  std::vector<std::vector<std::vector<int>>> shapes;

};

Tetromino createTetromino(std::string name) {

  if (name == "I") {
    return Tetromino({

    { { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 1, 1, 1, 1 },
      { 0, 0, 0, 0 } },

    { { 0, 0, 1, 0 },
      { 0, 0, 1, 0 },
      { 0, 0, 1, 0 },
      { 0, 0, 1, 0 } }

    }, 0, 3);
  }

  if (name == "O") {
    return Tetromino({

    { { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 2, 2 },
      { 0, 0, 2, 2 } },


    }, 0, 3);
  }

  if (name == "T") {
    return Tetromino({
    { { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 3, 3, 3 },
      { 0, 0, 3, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 3, 0 },
      { 0, 3, 3, 0 },
      { 0, 0, 3, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 3, 0 },
      { 0, 3, 3, 3 },
      { 0, 0, 0, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 3, 0 },
      { 0, 0, 3, 3 },
      { 0, 0, 3, 0 } },

    }, 0, 3);
  }

  if (name == "S") {
    return Tetromino({

    { { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 0, 4, 4 },
      { 0, 4, 4, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 4, 0 },
      { 0, 0, 4, 4 },
      { 0, 0, 0, 4 } },

    }, 0, 3);
  }

  if (name == "Z") {
    return Tetromino({

    { { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 5, 5, 0 },
      { 0, 0, 5, 5 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 0, 5 },
      { 0, 0, 5, 5 },
      { 0, 0, 5, 0 } },

    }, 0, 2);
  }

  if (name == "J") {
    return Tetromino({
    { { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 6, 6, 6 },
      { 0, 0, 0, 6 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 6, 0 },
      { 0, 0, 6, 0 },
      { 0, 6, 6, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 6, 0, 0 },
      { 0, 6, 6, 6 },
      { 0, 0, 0, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 6, 6 },
      { 0, 0, 6, 0 },
      { 0, 0, 6, 0 } },

    }, 0, 3);
  }

  if (name == "L") {
    return Tetromino({
    { { 0, 0, 0, 0 },
      { 0, 0, 0, 0 },
      { 0, 7, 7, 7 },
      { 0, 7, 0, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 7, 7, 0 },
      { 0, 0, 7, 0 },
      { 0, 0, 7, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 0, 7 },
      { 0, 7, 7, 7 },
      { 0, 0, 0, 0 } },

    { { 0, 0, 0, 0 },
      { 0, 0, 7, 0 },
      { 0, 0, 7, 0 },
      { 0, 0, 7, 7 } },

    }, 0, 3);
  }

}

Tetromino tetromino;

// game textures
std::vector<SDL_Texture*> blockTextures;
SDL_Texture* blockTexture = nullptr;
SDL_Texture* blockRedTexture = nullptr;
SDL_Texture* blockGreenTexture = nullptr;
SDL_Texture* blockYellowTexture = nullptr;
SDL_Texture* blockBlueTexture = nullptr;
SDL_Texture* blockLightBlueTexture = nullptr;
SDL_Texture* blockPinkTexture = nullptr;
SDL_Texture* blockBlackTexture = nullptr;
// game rects
SDL_Rect blockRect;
SDL_Rect boardRect = { 0, BLOCK_HEIGHT * 2, BLOCK_WIDTH * 10, BLOCK_HEIGHT * 20 };

bool initSDL();
SDL_Texture* loadTexture(const std::string& path);
bool loadMedia();
bool initGame();
void resetPlay();
void drawPlay();
void drawTetromino();
int random(int min, int max);
void generateTetromino();
bool canRotate(int rotationIdx);
bool canMoveLeft();
bool canMoveRight();
bool canMoveDown();
bool isRowComplete(const std::array<int, 10> row);
void removeRow(int rowIdx);
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
	blockRedTexture = loadTexture("assets/block-red.png");
	if(!blockRedTexture) {
		return false;
	}

  blockGreenTexture = loadTexture("assets/block-green.png");
	if(!blockGreenTexture) {
		return false;
	}

  blockYellowTexture = loadTexture("assets/block-yellow.jpg");
	if(!blockYellowTexture) {
		return false;
	}

  blockBlueTexture = loadTexture("assets/block-blue.jpg");
  if(!blockBlueTexture) {
    return false;
  }

  blockLightBlueTexture = loadTexture("assets/block-light-blue.jpg");
  if(!blockLightBlueTexture) {
    return false;
  }

  blockPinkTexture = loadTexture("assets/block-pink.jpg");
  if(!blockPinkTexture) {
    return false;
  }

  blockBlackTexture = loadTexture("assets/block-black.jpg");
  if(!blockBlackTexture) {
    return false;
  }


  blockTextures.push_back(nullptr);
  blockTextures.push_back(blockRedTexture);
  blockTextures.push_back(blockGreenTexture);
  blockTextures.push_back(blockYellowTexture);
  blockTextures.push_back(blockBlueTexture);
  blockTextures.push_back(blockLightBlueTexture);
  blockTextures.push_back(blockPinkTexture);
  blockTextures.push_back(blockBlackTexture);

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
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
  SDL_RenderFillRect(renderer, &boardRect);

  drawTetromino();

  // draw landed tetrominos
	for (auto row = 0; row < tiles.size(); row++) {
		for (auto col = 0; col < tiles[row].size(); col++) {
			if (tiles[row][col] != 0) {
				blockRect = { BLOCK_WIDTH * col, BLOCK_HEIGHT * row, BLOCK_WIDTH, BLOCK_HEIGHT };
				SDL_RenderCopy(renderer, blockTextures[tiles[row][col]], nullptr, &blockRect);
			}
		}
	}

	SDL_RenderPresent(renderer);
}

void drawTetromino() {
  // draws a falling tetromino
  for (auto row = 0; row < tetromino.shape.size(); row++) {
    for (auto col = 0; col < tetromino.shape[row].size(); col++) {
      if (tetromino.shape[row][col] != 0) {
        SDL_Rect rect = {
          BLOCK_WIDTH * (col + tetromino.col),
          BLOCK_HEIGHT * (row + tetromino.row),
          BLOCK_WIDTH,
          BLOCK_HEIGHT
        };
        SDL_RenderCopy(renderer, blockTextures[tetromino.shape[row][col]], nullptr, &rect);
      }
    }
  }
  // std::cout << tetromino.y << std::endl;
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

bool canRotate(int rotationIdx) {
  const std::vector<std::vector<int>> rotation = tetromino.shapes[rotationIdx];
  for (auto row = 0; row < rotation.size(); row++) {
    for (auto col = 0; col < rotation[row].size(); col++) {
      if (tetromino.col + col < 0) {
        return false;
      }
      if (tetromino.col + col > 9) {
        return false;
      }
      if (rotation[row][col] != 0) {
        if (tiles[tetromino.row + row][tetromino.col + col] != 0) {
          return false;
        }
      }
    }
  }
  return true;
}

bool canMoveLeft() {
  for (auto row = 0; row < tetromino.shape.size(); row++) {
    for (auto col = 0; col < tetromino.shape[row].size(); col++) {
      if (tetromino.shape[row][col] != 0) {
        // check left board collision
        if (tetromino.col - 1 + col < 0) {
          return false;
        }
        // check left collsion with tetros
        if (tiles[tetromino.row + row][tetromino.col - 1 + col] != 0) {
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
      if (tetromino.shape[row][col] != 0) {
        // check right board collision
        if (tetromino.col + 1 + col > 9) {
          return false;
        }
        // check right collision with tetros
        if (tiles[tetromino.row + row][tetromino.col + 1 + col] != 0) {
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
      if (tetromino.shape[row][col] != 0) {
        // check bottom board collision
        if (tetromino.row + 1 + row > 21) {
          return false;
        }
        // check bottom collision with tetrominos
        if (tiles[tetromino.row + 1 + row][tetromino.col + col] != 0) {
          return false;
        }
      }
    }
  }
  return true;
}

bool isRowComplete(const std::array<int, 10> row) {
  for (auto col = 0; col < row.size(); col++) {
    if (row[col] == 0) {
      return false;
    }
  }

  return true;
}

void removeRow(int rowIdx) {
  for (auto col = 0; col < tiles[rowIdx].size(); col++) {
    tiles[rowIdx][col] = 0;
  }
}

void updateTetromino() {

  std::cout << tetromino.row << " : " << tetromino.col << std::endl;

  tickCurrent = SDL_GetTicks();
  tickAccum += tickCurrent - tickLast;
  if (tickAccum > tickDuration) {

    if (canMoveDown()) {
      tetromino.row += 1;
    }
    else {
      // copy falling tetro to landed
      for (auto row = 0; row < tetromino.shape.size(); row++) {
        for (auto col = 0; col < tetromino.shape[row].size(); col++) {
          if (tetromino.shape[row][col] != 0) {
            tiles[tetromino.row + row][tetromino.col + col] = tetromino.shape[row][col];
          }
        }
      }
      // clear lines
      for (auto rowIdx = 0; rowIdx < tiles.size(); rowIdx++) {
        if (isRowComplete(tiles[rowIdx])) {
          removeRow(rowIdx);
          SDL_Delay(500);
          // move all rows above the deleted one down by one col
          for (auto rowAboveIdx = rowIdx -1; rowAboveIdx >= 0; rowAboveIdx--) {
            for (auto col = 0; col < tiles[rowAboveIdx].size(); col++) {
              tiles[rowAboveIdx + 1][col] = tiles[rowAboveIdx][col];
            }
          }
        }
      }

      generateTetromino();
    }

    tickAccum -= tickDuration;
  }
  tickLast = SDL_GetTicks();

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

      if(event.type == SDL_KEYDOWN) {
        if(event.key.keysym.sym == SDLK_SPACE) {
          int rotationIdx = tetromino.shapeIdx;
          if(rotationIdx + 1 == tetromino.shapes.size()) {
            rotationIdx = 0;
          }
          else {
            rotationIdx += 1;
          }
          if (canRotate(rotationIdx)) {
            tetromino.shapeIdx = rotationIdx;
            tetromino.shape = tetromino.shapes[tetromino.shapeIdx];
          }

        }
      }

      if(event.type == SDL_KEYDOWN) {
        if(event.key.keysym.sym == SDLK_LEFT) {
          if (canMoveLeft()) {
            tetromino.col -= 1;
          }
        }
        if(event.key.keysym.sym == SDLK_RIGHT) {
          if (canMoveRight()) {
            tetromino.col += 1;
          }
        }
        if(event.key.keysym.sym == SDLK_DOWN) {
          tickDuration /= 2;
        }
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
