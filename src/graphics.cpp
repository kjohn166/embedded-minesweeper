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

void initMineInfo(uint8_t grid[10][10], uint8_t mineInfo[10][10])
{
    for(uint8_t i = 0; i < 10; i++)
    {
        for(uint8_t j = 0; j < 10; j++)
        {
            mineInfo[i][j] = getMinesAround(grid, {i, j});
        }
    }
}

void clearMinesweeper(uint8_t grid[10][10])
{
    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            grid[i][j] = 0;
        }
    }
}

void clearMineInfo(uint8_t mineInfo[10][10])
{
    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            mineInfo[i][j] = 0;
        }
    }
}

uint8_t getMineInfo(uint8_t mineInfo[10][10], selection currGrid)
{
    return mineInfo[currGrid.column][currGrid.row];
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

void drawChar(uint8_t index, uint8_t x, uint8_t y, uint16_t color)
{
    pixelRegion region = {x, y, x + FONT_WIDTH, y + FONT_HEIGHT};

    for(uint8_t i = 0; i < FONT_HEIGHT; i++)
    {
        uint8_t currByte = pgm_read_byte(&(fontmap[index][i]));

        for(uint8_t j = 0; j < FONT_WIDTH; j++)
        {

            if((currByte << j) & 0x20)
            {
                drawPixel(region.xStart + j, region.yStart + i, color);
            }
        }
    }
}

void writeString(char* str, uint8_t x, uint8_t y, uint16_t color)
{
    uint8_t i = 0;
    while(str[i] != '\0')
    {
        if (str[i] != ' ' && str[i] != '!') {
            drawChar(str[i] - 'A', x + i * 7, y, color);
        }
        else if (str[i] == '!') {
            drawChar(26, x - 1 + i * 7, y, color);
        }
        i++;
    }
}

uint16_t getTextWidth(char* str) {
    uint16_t width = 0;
    uint8_t i = 0;
    while(str[i] != '\0')
    {
        width += 7;
        i++;
    }
    return width;
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

void revealTile(uint8_t grid[10][10], uint8_t mineInfo[10][10], selection currGrid)
{
    pixelRegion region = getGridPixelRegion(currGrid);

    if(grid[currGrid.column][currGrid.row] == 0)
    {
        drawRectangleWireframe(region.xStart + 1, region.yStart + 1, region.xEnd - 1, region.yEnd - 1, GREY);
        if (getMineInfo(mineInfo, currGrid) != 0)
        {
            drawNum(getMineInfo(mineInfo, currGrid), region.xStart + 3, region.yStart + 3, RED);
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

void floodfillReveal(uint8_t grid[10][10], uint8_t mineGrid[10][10], selection currGrid)
{
    int8_t dx[] = {1, -1, 0, 0};
    int8_t dy[] = {0, 0, 1, -1};

    selection queue[100];
    uint8_t inQueue = 0;
    selection startSelection = currGrid;

    queue[inQueue++] = startSelection;

    if(grid[startSelection.column][startSelection.row] == 3 || grid[startSelection.column][startSelection.row] == 2) {
        revealTile(grid, mineGrid, startSelection);
        return;
    }
    else if(grid[startSelection.column][startSelection.row] == 1 || grid[startSelection.column][startSelection.row] == 4) {
        return;
    }
    else {
        revealTile(grid, mineGrid, startSelection);
    }
    
    // main loop
    for(int i = 0; i < inQueue; i++)
    {
        for(int j = 0; j < 4; j++)
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

            if(mineGrid[newX][newY] == 0 && grid[newX][newY] == 0)
            {
                revealTile(grid, mineGrid, newSelection);
                queue[inQueue++] = newSelection;
            }
            else {
                if(grid[newX][newY] == 0)
                {
                    revealTile(grid, mineGrid, newSelection);
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


uint8_t revealAllMines(uint8_t grid[10][10], uint8_t mineInfo[10][10])
{
    for(uint8_t i = 0; i < 10; i++)
    {
        for(uint8_t j = 0; j < 10; j++)
        {
            if(grid[j][i] == 3)
            {
                revealTile(grid, mineInfo, {j, i});
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

void drawStartMenu() {
    pixelRegion region = {(SCREEN_WIDTH / 2) - ((BOMB_WIDTH - 1) / 2), (SCREEN_HEIGHT / 2) - ((BOMB_HEIGHT - 1) / 2), (SCREEN_WIDTH / 2) - ((BOMB_WIDTH - 1) / 2) + BOMB_WIDTH - 1, (SCREEN_HEIGHT / 2) - ((BOMB_HEIGHT - 1) / 2) + BOMB_HEIGHT - 1};

    for(uint16_t i = 0; i < BOMB_HEIGHT; i++)
    {

        for(uint16_t j = 0; j < BOMB_WIDTH; j++)
        {
            uint16_t currColor = pgm_read_word(&(bomb[i][j]));
            drawPixel(region.xStart + j, region.yStart + i, currColor);
        }
    }

    // draw title
    writeString("MINESWEEPER", (SCREEN_WIDTH / 2) - (getTextWidth("MINESWEEPER") / 2), 10, BLACK);

    // draw border around bomb
    drawRectangleWireframe(region.xStart + 1, region.yStart + 1, region.xEnd - 1, region.yEnd - 1, BLACK);

    // draw border around screen
    drawRectangle(0, 0, 0, 127, GREY_HIGHLIGHT);
    drawRectangle(0, 0, 127, 0, GREY_HIGHLIGHT);
    drawRectangle(1, 1, 0, 127, GREY_HIGHLIGHT);
    drawRectangle(1, 1, 127, 0, GREY_HIGHLIGHT);
    drawRectangle(127, 127, 127, 0, GREY_DETAIL_DARK);
    drawRectangle(127, 127, 0, 127, GREY_DETAIL_DARK);
    drawRectangle(126, 126, 127, 0, GREY_DETAIL_DARK);
    drawRectangle(126, 126, 0, 127, GREY_DETAIL_DARK);

    region = getMenuPixelRegion();
    drawRectangleWireframe(region.xStart, region.yStart, region.xEnd, region.yEnd, BLACK);
    drawOuterDetails();

    //draw two buttons at bottom of screen for start
    pixelRegion startButton = {6, 104, 59, 123};
    uint8_t startButtonWidth = (abs((startButton.xStart + 2) - (startButton.xEnd + 2))) / 2;
    uint8_t startButtonHeight = (abs((startButton.yStart + 2) - (startButton.yEnd + 2))) / 2;

    pixelRegion placeHolderButton = {69, 104, 123, 123};
    uint8_t placeHolderButtonWidth = (abs((placeHolderButton.xStart + 2) - (placeHolderButton.xEnd + 2))) / 2;
    uint8_t placeHolderButtonHeight = (abs((placeHolderButton.yStart + 2) - (placeHolderButton.yEnd + 2))) / 2;

    uint8_t buttonOffset = 3;

    drawRectangleWireframe(startButton.xStart, startButton.yStart, startButton.xEnd, startButton.yEnd, BLACK);
    drawRectangleWireframe(startButton.xStart + 1, startButton.yStart + 1, startButton.xEnd - 1, startButton.yEnd - 1, GREY_DETAIL_DARK);
    drawRectangle(startButton.xStart + 1, startButton.yStart + 1, startButton.xStart + 1, startButton.yEnd - 2, GREY_HIGHLIGHT);
    drawRectangle(startButton.xStart + 1, startButton.yStart + 1, startButton.xEnd - 2, startButton.yStart + 1, GREY_HIGHLIGHT);
    writeString("START", (startButtonWidth / 2) + buttonOffset, (startButtonHeight / 2) + startButton.yStart + 2, BLACK);

    drawRectangleWireframe(placeHolderButton.xStart, placeHolderButton.yStart, placeHolderButton.xEnd, placeHolderButton.yEnd, BLACK);
    drawRectangleWireframe(placeHolderButton.xStart + 1, placeHolderButton.yStart + 1, placeHolderButton.xEnd - 1, placeHolderButton.yEnd - 1, GREY_DETAIL_DARK);
    drawRectangle(placeHolderButton.xStart + 1, placeHolderButton.yStart + 1, placeHolderButton.xStart + 1, placeHolderButton.yEnd - 2, GREY_HIGHLIGHT);
    drawRectangle(placeHolderButton.xStart + 1, placeHolderButton.yStart + 1, placeHolderButton.xEnd - 2, placeHolderButton.yStart + 1, GREY_HIGHLIGHT);
    writeString("TEMP", (placeHolderButtonWidth / 2) + placeHolderButton.xStart, (placeHolderButtonHeight / 2) + placeHolderButton.yStart + 2, BLACK);
}

void drawCursor(uint8_t x, uint8_t y, uint8_t selection)
{
    for(uint16_t i = 0; i < CURSOR_HEIGHT; i++)
    {

        for(uint16_t j = 0; j < CURSOR_WIDTH; j++)
        {
            uint16_t currColor = pgm_read_word(&(cursormap[i][j]));
            drawPixel(x + j, y + i, currColor);
        }
    }
}

void drawGameModal(uint8_t gameResult) {
    const int8_t modalWidth = (SCREEN_WIDTH / 2) + 30;
    const int8_t modalHeight = (SCREEN_HEIGHT / 2) + 5;
    pixelRegion modalRegion = {(SCREEN_WIDTH / 2) - (modalWidth / 2), (SCREEN_HEIGHT / 2) - (modalHeight / 2), (SCREEN_WIDTH / 2) - (modalWidth / 2) + modalWidth, (SCREEN_HEIGHT / 4) - (modalHeight / 4) + modalHeight};

    // draw modal bounding box
    drawRectangleWireframe(modalRegion.xStart, modalRegion.yStart, modalRegion.xEnd, modalRegion.yEnd, BLACK);
    drawRectangle(modalRegion.xStart + 1, modalRegion.yStart + 1, modalRegion.xEnd - 1, modalRegion.yEnd - 1, GREY);
     
    // draw modal box details
    drawRectangleWireframe(modalRegion.xStart + 1, modalRegion.yStart + 1, modalRegion.xEnd - 1, modalRegion.yEnd - 1, GREY_DETAIL_DARK);
    drawRectangle(modalRegion.xStart + 1, modalRegion.yStart + 1, modalRegion.xStart + 1, modalRegion.yEnd - 2, GREY_HIGHLIGHT);
    drawRectangle(modalRegion.xStart + 1, modalRegion.yStart + 1, modalRegion.xEnd - 2, modalRegion.yStart + 1, GREY_HIGHLIGHT);

    // draw appropriate text
    pixelRegion textRegion = {(modalRegion.xEnd + modalRegion.xStart) / 2 - (getTextWidth("YOU LOSE!") / 2), modalRegion.yStart + 8, modalRegion.xEnd - 2, modalRegion.yEnd - 8};

    if (gameResult == 1) { // game win
        writeString("YOU WIN!", textRegion.xStart, textRegion.yStart, BLACK);
    }
    else if (gameResult == 2) { // game lose
        writeString("YOU LOSE!", textRegion.xStart, textRegion.yStart, BLACK);
    }

    // draw bounding boxes for modal selection buttons
    pixelRegion leftButtonRegion = {modalRegion.xStart + 3, (modalRegion.yStart + 3) + int((modalRegion.yEnd - modalRegion.yStart) / 1.5) - 4, ((modalRegion.xEnd - modalRegion.xStart) / 2) + modalRegion.xStart, modalRegion.yEnd - 3};
    pixelRegion rightButtonRegion = {((modalRegion.xEnd - modalRegion.xStart) / 2) + modalRegion.xStart, (modalRegion.yStart + 3) + int((modalRegion.yEnd - modalRegion.yStart) / 1.5) - 4, modalRegion.xEnd - 3, modalRegion.yEnd - 3};

    drawRectangleWireframe(leftButtonRegion.xStart, leftButtonRegion.yStart, leftButtonRegion.xEnd, leftButtonRegion.yEnd, BLACK);
    drawRectangleWireframe(rightButtonRegion.xStart, rightButtonRegion.yStart, rightButtonRegion.xEnd, rightButtonRegion.yEnd, BLACK);

    // draw details for left button
    drawRectangleWireframe(leftButtonRegion.xStart + 1, leftButtonRegion.yStart + 1, leftButtonRegion.xEnd - 1, leftButtonRegion.yEnd - 1, GREY_DETAIL_DARK);
    drawRectangle(leftButtonRegion.xStart + 1, leftButtonRegion.yStart + 1, leftButtonRegion.xStart + 1, leftButtonRegion.yEnd - 2, GREY_HIGHLIGHT);
    drawRectangle(leftButtonRegion.xStart + 1, leftButtonRegion.yStart + 1, leftButtonRegion.xEnd - 2, leftButtonRegion.yStart + 1, GREY_HIGHLIGHT);

    // draw details for right button
    drawRectangleWireframe(rightButtonRegion.xStart + 1, rightButtonRegion.yStart + 1, rightButtonRegion.xEnd - 1, rightButtonRegion.yEnd - 1, GREY_DETAIL_DARK);
    drawRectangle(rightButtonRegion.xStart + 1, rightButtonRegion.yStart + 1, rightButtonRegion.xStart + 1, rightButtonRegion.yEnd - 2, GREY_HIGHLIGHT);
    drawRectangle(rightButtonRegion.xStart + 1, rightButtonRegion.yStart + 1, rightButtonRegion.xEnd - 2, rightButtonRegion.yStart + 1, GREY_HIGHLIGHT);

    // draw text for left button
    writeString("RESET", ((leftButtonRegion.xEnd - leftButtonRegion.xStart) / 2 + leftButtonRegion.xStart) - (getTextWidth("RESET") / 2), leftButtonRegion.yStart + 5, BLACK);

    // draw text for right button
    writeString("EXIT", ((rightButtonRegion.xEnd - rightButtonRegion.xStart) / 2 + rightButtonRegion.xStart) - (getTextWidth("EXIT") / 2), rightButtonRegion.yStart + 5, BLACK);
}





