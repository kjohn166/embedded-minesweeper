#include "display_driver.h"
#include "spiAVR.h"

void ST7735_init()
{
    CS_LOW;
    sendCommand(0x11);
    _delay_ms(10);
    sendCommand(0x36);  // set MADCTL
    sendData(0xC0);     // MY = 1, MX = 1
    sendCommand(0x26) ; // set default gamma
    sendData(0x04);    // 2.2 gamma curve
    sendCommand(0x3A);  // set color mode
    sendData(0x05);     // 16-bit color
    sendData(0x14);     // RGB
    sendCommand(0x29);  // Display on
    CS_HIGH;
}

void sendCommand(uint8_t cmd)
{
    CS_LOW;
    DC_COMMAND;
    SPI_SEND(cmd);
    CS_HIGH;
}

void sendData(uint8_t data)
{
    CS_LOW;
    DC_DATA;
    SPI_SEND(data);
    CS_HIGH;
}

// used to take upper byte of 16 bit input for sending color data
char getHighByte16(uint16_t data)
{
    return (char)(data >> 8);
}

// used to take lower byte of 16 bit input for sending color data
char getLowByte16(uint16_t data)
{
    return (char)(data & 0xFF);
}

// set an area of the display for writing to, using reverse addressing so (0,0) is top left, (127,127) is bottom right
void setMappingWindow(uint8_t xStart, uint8_t yStart, uint8_t xEnd, uint8_t yEnd)
{
    sendCommand(ST7735_CASET);
    sendData(0x00);
    sendData(xStart + ColStart);
    sendData(0x00);
    sendData(xEnd + ColStart);

    sendCommand(ST7735_RASET);
    sendData(0x00);
    sendData(yStart + RowStart);
    sendData(0x00);
    sendData(yEnd + RowStart);

    sendCommand(ST7735_RAMWR);
}


// draw a pixel at a point (x,y)
void drawPixel(uint8_t x, uint8_t y, uint16_t color)
{
    setMappingWindow(x, y, 0, 0);
    sendData(getHighByte16(color));
    sendData(getLowByte16(color));
}

// draw a rectanle with top left corner at (x1,y1) and bottom right corner at (x2,y2)
void drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
    if(x1 > x2) {
        x1 ^= x2;
        x2 ^= x1;
        x1 ^= x2;
    }
    if(y1 > y2) {
        y1 ^= y2;
        y2 ^= y1;
        y1 ^= y2;
    }

    setMappingWindow(x1, y1, x2, y2);
    for(uint8_t i = x1; i < x2 + 1; i++) {
        for(uint8_t j = y1; j < y2 + 1; j++) {
            sendData(getHighByte16(color));
            sendData(getLowByte16(color));
        }
    }
}
   

void drawRectangleWireframe(uint8_t xStart, uint8_t yStart, uint8_t xEnd, uint8_t yEnd, uint16_t color)
{
    drawRectangle(xStart, yStart, xEnd, yStart, color);
    drawRectangle(xStart, yStart, xStart, yEnd, color);
    drawRectangle(xEnd, yStart, xEnd, yEnd, color);
    drawRectangle(xStart, yEnd, xEnd, yEnd, color);
}


void clearBG(uint16_t color)
{
    drawRectangle(0, 0, 127, 127, color);
}

