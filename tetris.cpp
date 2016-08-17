#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <random>
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
bool hasLanded = false;
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
// piet
struct Line {

  Line() {}

  Line(int t_x1, int t_y1, int t_x2, int t_y2) {
    if (t_x1 <= t_x2) {
      x1 = t_x1;
      x2 = t_x2;
    }
    else {
      x1 = t_x2;
      x2 = t_x1;
    }

    if (t_y1 <= t_y2) {
      y1 = t_y1;
      y2 = t_y2;
    }
    else {
      y1 = t_y2;
      y2 = t_y1;
    }
  }

  int x1, y1, x2, y2;
};

std::vector<SDL_Point> takedPoints;

std::vector<Line> guides;
std::vector<Line> generateGuides();
void drawGuides();

std::vector<Line> lines;
std::vector<Line> generateLines();
void drawLines();

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
  guides = generateGuides();
  lines = generateLines();
  generateTetromino();
}


// 8 tiles of padding left and right, 10 tiles for the board
// backround starts at 96 x
// background starts at 64 y
// tile x size 320 10 rows
// tile y size 640 20 rows
const int BOARD_LEFT = 352;
const int BOARD_TOP = 64;
const int BOARD_RIGHT = 672;
const int BOARD_BOTTOM = 640;

const int PICTURE_LEFT = 96;
const int PICTURE_TOP = 32;
const int PICTURE_RIGHT = 928;
const int PICTURE_BOTTOM = 736;

std::vector<Line> generateGuides() {

  std::vector<int> coordsX;
  for (auto i = 0; i < 10; i++) {
    coordsX.push_back(i * BLOCK_WIDTH + BOARD_LEFT);
  }

  std::vector<int> coordsY;
  for (auto i = 0; i < 20; i++) {
    coordsY.push_back(i * BLOCK_HEIGHT + BOARD_TOP);
  }

  int horGuideX1 = BOARD_LEFT;
  int horGuideY1 = coordsY[random(0, coordsY.size())];
  int horGuideX2 = BOARD_RIGHT;
  int horGuideY2 = horGuideY1;

  int verGuideX1 = coordsX[random(0, coordsX.size())];
  int verGuideY1 = BOARD_TOP;
  int verGuideX2 = verGuideX1;
  int verGuideY2 = BOARD_BOTTOM;

  std::vector<Line> guides;
  guides.push_back(Line(horGuideX1, horGuideY1, horGuideX2, horGuideY2));
  guides.push_back(Line(verGuideX1, verGuideY1, verGuideX2, verGuideY2));

  takedPoints.push_back({ horGuideX1, horGuideY1 });
  takedPoints.push_back({ horGuideX2, horGuideY2 });

  takedPoints.push_back({ verGuideX1, verGuideY1 });
  takedPoints.push_back({ verGuideX2, verGuideY2 });

  return guides;

}

std::vector<Line> getHorLines() {
  std::vector<Line> horLines;

  Line line;
  for (auto i = 0; i < lines.size(); i++) {
    line = lines[i];
    if (line.y1 == line.y2) {
      horLines.push_back(line);
    }
  }

  return horLines;
}


std::vector<Line> getVerLines() {
  std::vector<Line> verLines;

  Line line;
  for (auto i = 0; i < lines.size(); i++) {
    line = lines[i];
    if (line.x1 == line.x2) {
      verLines.push_back(line);
    }
  }

  return verLines;
}

std::vector<int> getXIntercepts(int y) {
  std::vector<int> xs;

  std::vector<Line> verLines = getVerLines();
  Line line;
  for (auto i = 0; i < verLines.size(); i++) {
    line = verLines[i];
    if (line.y1 <= y && line.y2 >= y) {
      xs.push_back(line.x1);
    }
  }

  return xs;
}

std::vector<int> getYIntercepts(int x) {
  std::vector<int> ys;

  std::vector<Line> horLines = getHorLines();
  Line line;
  for (auto i = 0; i < horLines.size(); i++) {
    line = horLines[i];
    if (line.x1 <= x && line.x2 >= x) {
      ys.push_back(line.y1);
    }
  }

  return ys;
}

std::vector<Line> generateLines() {

  std::vector<int> coordsX;
  for (auto i = 0; i < 26; i++) {
    coordsX.push_back(i * BLOCK_WIDTH + PICTURE_LEFT);
  }

  std::vector<int> coordsY;
  for (auto i = 0; i < 22; i++) {
    coordsY.push_back(i * BLOCK_HEIGHT + PICTURE_TOP);
  }

  lines.push_back(Line(PICTURE_LEFT, PICTURE_TOP, PICTURE_RIGHT, PICTURE_TOP));
  lines.push_back(Line(PICTURE_RIGHT, PICTURE_TOP, PICTURE_RIGHT, PICTURE_BOTTOM));
  lines.push_back(Line(PICTURE_RIGHT, PICTURE_BOTTOM, PICTURE_LEFT, PICTURE_BOTTOM));
  lines.push_back(Line(PICTURE_LEFT, PICTURE_BOTTOM, PICTURE_LEFT, PICTURE_TOP));

  std::vector<int> xIntercepts;
  std::vector<int> yIntercepts;

  int align = 0;
  int x1, y1, x2, y2;
  int splitsCount = 40;
  while (splitsCount > 0) {
    align = random(0, 2);
    // std::cout << align << std::endl;
    // align = 0;

    if (align == 0) {
      y1 = coordsY[random(0, coordsY.size())];
      y2 = y1;

      xIntercepts = getXIntercepts(y1);
      auto engine = std::default_random_engine{};
      std::shuffle(std::begin(xIntercepts), std::end(xIntercepts), engine);

      x1 = xIntercepts[0];
      x2 = xIntercepts[1];

    }

    if (align == 1) {
      x1 = coordsX[random(0, coordsX.size())];
      x2 = x1;

      yIntercepts = getYIntercepts(x1);
      auto engine = std::default_random_engine{};
      std::shuffle(std::begin(yIntercepts), std::end(yIntercepts), engine);

      y1 = yIntercepts[0];
      y2 = yIntercepts[1];

    }

    lines.push_back(Line(x1, y1, x2, y2));
    --splitsCount;

  }

  return lines;

}

void drawGuides() {
  SDL_SetRenderDrawColor(renderer, 255, 000, 000, 255);

  Line guide;
  for (auto i = 0; i < guides.size(); i++) {
    guide = guides[i];
    SDL_RenderDrawLine(renderer, guide.x1, guide.y1, guide.x2, guide.y2);
  }
}

void drawLines() {
  SDL_SetRenderDrawColor(renderer, 000, 000, 000, 255);

  Line line;
  for (auto i = 0; i < lines.size(); i++) {
    line = lines[i];
    SDL_RenderDrawLine(renderer, line.x1, line.y1, line.x2, line.y2);
  }
}

void drawPlay() {
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  // SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
  // SDL_RenderFillRect(renderer, &boardRect);

  // drawTetromino();

  // draw landed tetrominos
  /*
	for (auto row = 0; row < tiles.size(); row++) {
		for (auto col = 0; col < tiles[row].size(); col++) {
			if (tiles[row][col] != 0) {
				blockRect = { BLOCK_WIDTH * col, BLOCK_HEIGHT * row, BLOCK_WIDTH, BLOCK_HEIGHT };
				SDL_RenderCopy(renderer, blockTextures[tiles[row][col]], nullptr, &blockRect);
			}
		}
	}
  */

  drawLines();
  drawGuides();
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

  // std::cout << tetromino.row << " : " << tetromino.col << std::endl;

  tickCurrent = SDL_GetTicks();
  tickAccum += tickCurrent - tickLast;
  if (tickAccum > tickDuration) {

    if (canMoveDown()) {
      hasLanded = false;
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
      hasLanded = true;
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
