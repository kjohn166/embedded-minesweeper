#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <display_driver.h>

#define GAME_HEIGHT 100
#define GAME_WIDTH 100
#define MINECOUNT 10

struct selection {
    int column;
    int row;
};

struct pixelRegion{
    uint8_t xStart;
    uint8_t yStart;
    uint8_t xEnd;
    uint8_t yEnd;
};

// game graphics and functionality
void initMinesweeper(uint8_t grid[10][10]);
void initMineInfo(uint8_t grid[10][10], uint8_t mineInfo[10][10]);
void clearMinesweeper(uint8_t grid[10][10]);
void clearMineInfo(uint8_t mineInfo[10][10]);
uint8_t isInBounds(selection possibleGrid);
uint8_t getMinesAround(uint8_t grid[10][10], selection currGrid);
uint8_t getMineInfo(uint8_t mineInfo[10][10], selection currGrid);
pixelRegion getGridPixelRegion(selection currGrid);
pixelRegion getBoardPixelRegion();
pixelRegion getMenuPixelRegion();
void drawGrid();
void drawMenuBox();
void drawOuterDetails();
void drawRightNumBox();
void drawLeftNumBox();
void drawNum(uint8_t num, uint8_t x, uint8_t y, uint16_t color);
void drawChar(uint8_t index, uint8_t x, uint8_t y, uint16_t color);
void writeString(char* str, uint8_t x, uint8_t y, uint16_t color);
uint16_t getTextWidth(char* str);
void drawEmoji(uint8_t num, uint8_t x, uint8_t y, uint16_t color);
void drawCursor(uint8_t x, uint8_t y, uint8_t selection);
void updateTimer(uint16_t time);
void highlightGrid(selection currGrid);
void unhighlightGrid(selection prevGrid);
uint8_t isinBounds(selection possibleSelection);
void revealTile(uint8_t grid[10][10], uint8_t mineInfo[10][10], selection currGrid);
void flagTile(uint8_t grid[10][10], selection currGrid);
void unflagTile(uint8_t grid[10][10], selection currGrid);
void floodfillReveal(uint8_t grid[10][10], uint8_t mineInfo[10][10], selection currGrid);
void updateMineCounter(uint8_t count);
uint8_t checkWin(uint8_t grid[10][10]);
uint8_t checkLose(uint8_t grid[10][10]);
uint8_t revealAllMines(uint8_t grid[10][10], uint8_t mineInfo[10][10]);
uint8_t flagAllMines(uint8_t grid[10][10]);

// in-game win/loss modal graphics
void drawGameModal(uint8_t gameResult);

// start menu graphics
void drawStartMenu();

#endif /* GRAPHIC_H */