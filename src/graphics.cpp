#include <graphics.h>
#include <stdlib.h>
#include <time.h>

void initMinesweeper(uint8_t grid[10][10])
{
    uint8_t randomNum1;
    uint8_t randomNum2;

    for(uint8_t i = 0; i < MINECOUNT; i++)
    {
        randomNum1 = rand() % 10;
        randomNum2 = rand() % 10;

        grid[randomNum1][randomNum2] = 3;
    }
}

uint8_t isInBounds(selection possibleGrid) 
{
    if(possibleGrid.column >= 0 && possibleGrid.column < 10 && possibleGrid.row >= 0 && possibleGrid.row < 10)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t getMinesAround(uint8_t grid[10][10], selection currGrid)
{
    uint8_t mineCount = 0;

    int8_t dx[] = {1, -1, 0, 0, 1, -1, 1, -1};
    int8_t dy[] = {0, 0, 1, -1, 1, 1, -1, -1};

    for (int i = 0; i < 8; i++)
    {
        int8_t newX = currGrid.column + dx[i];
        int8_t newY = currGrid.row + dy[i];
        selection newGrid = {newX, newY};

        if(isInBounds(newGrid))
        {
            if(grid[newX][newY] == 3)
            {
                mineCount++;
            }
        }
    }
    
    return mineCount;
}

pixelRegion getGridPixelRegion(selection currGrid)
{
    pixelRegion region;
    uint8_t xOffset = 14;
    uint8_t yOffset = 24;

    region.xStart = currGrid.column * 10 + xOffset;
    region.yStart = currGrid.row * 10 + yOffset;
    region.xEnd = region.xStart + 10;
    region.yEnd = region.yStart + 10;

    return region;
}

pixelRegion getBoardPixelRegion()
{
    pixelRegion region;
    region.xStart = 15;
    region.yStart = 25;
    region.xEnd = 114;
    region.yEnd = 124;

    return region;
}

pixelRegion getMenuPixelRegion()
{
    pixelRegion region;
    region.xStart = 15;
    region.yStart = 5;
    region.xEnd = 114;
    region.yEnd = 20;

    return region;
}

void drawGrid()
{

    for(uint8_t i = 0; i < 10; i++){
        for(uint8_t j = 0; j < 10; j++){
            selection currGrid = {j, i};
            pixelRegion region = getGridPixelRegion(currGrid);
            drawRectangleWireframe(region.xStart, region.yStart, region.xEnd, region.yEnd, BLACK);
            drawRectangle(region.xStart + 1, region.yStart + 1, region.xEnd - 1, region.yStart + 1, GREY_HIGHLIGHT);
            drawRectangle(region.xStart + 1, region.yStart + 1, region.xStart + 1, region.yEnd - 1, GREY_HIGHLIGHT);
            drawRectangle(region.xEnd - 1, region.yEnd - 1, region.xEnd - 1, region.yStart + 1, GREY_DETAIL_DARK);
            drawRectangle(region.xEnd - 1, region.yEnd - 1, region.xStart + 1, region.yEnd - 1, GREY_DETAIL_DARK);
        }
    }

    pixelRegion region = getBoardPixelRegion();

    drawRectangleWireframe(region.xStart - 1, region.yStart - 1, region.xEnd + 1, region.yEnd + 1, GREY_HIGHLIGHT);
    drawRectangle(region.xStart - 1, region.yStart - 1, region.xStart - 1, region.yEnd + 1, GREY_DETAIL_DARK);
    drawRectangle(region.xStart - 1, region.yStart - 1, region.xEnd + 1, region.yStart - 1, GREY_DETAIL_DARK);
}

void drawMenuBox()
{
    pixelRegion region = getMenuPixelRegion();
    drawRectangleWireframe(region.xStart, region.yStart, region.xEnd, region.yEnd, BLACK);

    drawLeftNumBox();
    drawRightNumBox();
}

void drawOuterDetails()
{
    drawRectangle(0, 0, 0, 127, GREY_HIGHLIGHT);
    drawRectangle(0, 0, 127, 0, GREY_HIGHLIGHT);
    drawRectangle(1, 1, 0, 127, GREY_HIGHLIGHT);
    drawRectangle(1, 1, 127, 0, GREY_HIGHLIGHT);
    drawRectangle(127, 127, 127, 0, GREY_DETAIL_DARK);
    drawRectangle(127, 127, 0, 127, GREY_DETAIL_DARK);
    drawRectangle(126, 126, 127, 0, GREY_DETAIL_DARK);
    drawRectangle(126, 126, 0, 127, GREY_DETAIL_DARK);

    pixelRegion region = getMenuPixelRegion();
    drawRectangleWireframe(region.xStart - 1, region.yStart - 1, region.xEnd + 1, region.yEnd + 1, GREY_HIGHLIGHT);
    drawRectangle(region.xStart - 1, region.yStart - 1, region.xStart - 1, region.yEnd + 1, GREY_DETAIL_DARK);
    drawRectangle(region.xStart - 1, region.yStart - 1, region.xEnd + 1, region.yStart - 1, GREY_DETAIL_DARK);
}

void drawLeftNumBox()
{
    pixelRegion region = {21, 9, 42, 16};
    drawRectangle(region.xStart, region.yStart, region.xEnd, region.yEnd, BLACK);
}

void drawRightNumBox()
{
    pixelRegion region = {87, 9, 108, 16};
    drawRectangle(region.xStart, region.yStart, region.xEnd, region.yEnd, BLACK);
}

void drawNum(uint8_t num, uint8_t x, uint8_t y, uint16_t color)
{
    pixelRegion region = {x, y, x + NUM_WIDTH, y + NUM_HEIGHT};

    for(uint8_t i = 0; i < NUM_HEIGHT; i++)
    {
        uint8_t currByte = pgm_read_byte(&(bitmap[num][i]));

        for(uint8_t j = 0; j < NUM_WIDTH; j++)
        {

            if((currByte << j) & 0x20)
            {
                drawPixel(region.xStart + j, region.yStart + i, color);
            }
        }
    }
}

void drawEmoji(uint8_t num, uint8_t x, uint8_t y, uint16_t color)
{
    pixelRegion region = {x, y, x + EMOJI_WIDTH, y + EMOJI_HEIGHT};

    for(uint8_t i = 0; i < EMOJI_HEIGHT; i++)
    {
        uint16_t currByte = pgm_read_word(&(emojimap[num][i]));

        for(uint8_t j = 0; j < EMOJI_HEIGHT; j++)
        {

            if((currByte << j) & 0x0400)
            {
                drawPixel(region.xStart + j, region.yStart + i, color);
            }
        }
    }
}

// optimize to only update screen of changed digits
void updateTimer(uint16_t time)
{
    drawRightNumBox();
    drawNum(time / 100, 88, 10, RED);
    drawNum((time % 100) / 10, 95, 10, RED);
    drawNum(time % 10, 102, 10, RED);
}

void highlightGrid(selection currGrid)
{
    pixelRegion region = getGridPixelRegion(currGrid);
    drawRectangleWireframe(region.xStart, region.yStart, region.xEnd, region.yEnd, YELLOW);
}

void unhighlightGrid(selection prevGrid)
{
    pixelRegion region = getGridPixelRegion(prevGrid);
    drawRectangleWireframe(region.xStart, region.yStart, region.xEnd, region.yEnd, BLACK);
}

uint8_t isinBounds(selection possibleSelection)
{
    if (possibleSelection.column >= 0 && possibleSelection.column < 10 && possibleSelection.row >= 0 && possibleSelection.row < 10){
        return 1;
    }
    else{
        return 0;
    }
}

void revealTile(uint8_t grid[10][10], selection currGrid)
{
    pixelRegion region = getGridPixelRegion(currGrid);

    if(grid[currGrid.column][currGrid.row] == 0)
    {
        drawRectangleWireframe(region.xStart + 1, region.yStart + 1, region.xEnd - 1, region.yEnd - 1, GREY);
        if (getMinesAround(grid, currGrid) != 0)
        {
            drawNum(getMinesAround(grid, currGrid), region.xStart + 3, region.yStart + 3, RED);
        }
        grid[currGrid.column][currGrid.row] = 1;
    }
    else if(grid[currGrid.column][currGrid.row] == 3)
    {
        drawNum(10, region.xStart + 3, region.yStart + 3, BLACK);
        grid[currGrid.column][currGrid.row] = 4;
    }
}

void flagTile(uint8_t grid[10][10], selection currGrid)
{
    pixelRegion region = getGridPixelRegion(currGrid);
    drawNum(11, region.xStart + 3, region.yStart + 3, ORANGE);
}

void unflagTile(uint8_t grid[10][10], selection currGrid)
{
    pixelRegion region = getGridPixelRegion(currGrid);
    drawRectangle(region.xStart + 2, region.yStart + 2, region.xEnd - 2, region.yEnd - 2, GREY);
}


// FIXME: floodfillReveal is not working properly in some cases (jumping)
void floodfillReveal(uint8_t grid[10][10], selection currGrid)
{
    int8_t dx[] = {1, -1, 0, 0, 1, -1, 1, -1};
    int8_t dy[] = {0, 0, 1, -1, 1, 1, -1, -1};

    selection queue[100];
    uint8_t inQueue = 0;
    selection startSelection = currGrid;

    queue[inQueue++] = startSelection;
    if(grid[startSelection.column][startSelection.row] == 3 || grid[startSelection.column][startSelection.row] == 2) {
        revealTile(grid, startSelection);
        return;
    }
    else if(grid[startSelection.column][startSelection.row] == 1 || grid[startSelection.column][startSelection.row] == 4) {
        return;
    }
    else {
        revealTile(grid, startSelection);
    }
    

    // i loops 16 times due to performance issues when using inQueue :/, works the same tho
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            int8_t newX = queue[i].column + dx[j];
            if(newX < 0 || newX >= 10)
            {
                continue;
            }

            int8_t newY = queue[i].row + dy[j];
            if(newY < 0 || newY >= 10)
            {
                continue;
            }

            selection newSelection = {newX, newY};

            if(getMinesAround(grid, newSelection) == 0 && grid[newX][newY] == 0)
            {
                revealTile(grid, newSelection);
                queue[inQueue++] = newSelection;
            }
            else {
                if(grid[newX][newY] == 0)
                {
                    revealTile(grid, newSelection);
                }
            }
        }
    }
}

void updateMineCounter(uint8_t count)
{
    drawLeftNumBox();
    if (count / 10 == 1)
    {
        drawNum(count / 10, 29, 10, RED);
    }
    drawNum(count % 10, 36, 10, RED);
}

uint8_t checkWin(uint8_t grid[10][10])
{
    uint8_t count = 0;
    for(uint8_t i = 0; i < 10; i++)
    {
        for(uint8_t j = 0; j < 10; j++)
        {
            if(grid[i][j] == 1)
            {
                count++;
            }
        }
    }

    if(count == 90)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t checkLose(uint8_t grid[10][10])
{
    for(uint8_t i = 0; i < 10; i++)
    {
        for(uint8_t j = 0; j < 10; j++)
        {
            if(grid[j][i] == 4)
            {
                return 1;
            }
        }
    }
    return 0;
}


uint8_t revealAllMines(uint8_t grid[10][10])
{
    for(uint8_t i = 0; i < 10; i++)
    {
        for(uint8_t j = 0; j < 10; j++)
        {
            if(grid[j][i] == 3)
            {
                revealTile(grid, {j, i});
            }
        }
    }
}

uint8_t flagAllMines(uint8_t grid[10][10])
{
    for(uint8_t i = 0; i < 10; i++)
    {
        for(uint8_t j = 0; j < 10; j++)
        {
            if(grid[j][i] == 3)
            {
                pixelRegion region = getGridPixelRegion({j, i});
                drawRectangle(region.xStart + 2, region.yStart + 2, region.xEnd - 2, region.yEnd - 2, GREY);
                flagTile(grid, {j, i});
            }
        }
    }
}







