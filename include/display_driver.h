#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define PIN_DC PORTB0
#define PIN_RST PORTD7

#define DC_COMMAND PORTB &= ~(1 << PIN_DC)
#define DC_DATA PORTB |= (1 << PIN_DC)
#define CS_LOW PORTB &= ~(1 << PIN_SS)
#define CS_HIGH PORTB |= (1 << PIN_SS)
#define RST_LOW PORTB &= ~(1 << PIN_RST)
#define RST_HIGH PORTB |= (1 << PIN_RST)

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C

//color definitions
#define BLACK 0x0000
#define BLUE 0xFF00
#define RED 0x00FF
#define GREEN 0x07E0
#define YELLOW 0x077F
#define ORANGE 0x049F
#define GREY 0x6b6b
#define GREY_DETAIL_DARK 0x2124
#define GREY_HIGHLIGHT 0xEF7D

#define ColStart 2
#define RowStart 3

#define Width 127
#define Height 127

void ST7735_init();
void sendCommand(uint8_t cmd);
void sendData(uint8_t data);
char getHighByte16(uint16_t data);
char getLowByte16(uint16_t data);

void setMappingWindow(uint8_t xStart, uint8_t yStart, uint8_t xEnd, uint8_t yEnd);
void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawRectangle(uint8_t xStart, uint8_t yStart, uint8_t xEnd, uint8_t yEnd, uint16_t color);
void drawRectangleWireframe(uint8_t xStart, uint8_t yStart, uint8_t xEnd, uint8_t yEnd, uint16_t color);
void clearBG(uint16_t color);



#define NUM_WIDTH 6
#define NUM_HEIGHT 6

// 6x6 bitmapping for numbers 0-9
const PROGMEM uint8_t bitmap[12][NUM_HEIGHT] = {
    {0b011100, 0b100010, 0b100010, 0b100010, 0b100010, 0b011100}, // 0
    {0b001100, 0b011100, 0b001100, 0b001100, 0b001100, 0b011110}, // 1 
    {0b011100, 0b110010, 0b000010, 0b011100, 0b110000, 0b111110}, // 2
    {0b011100, 0b110010, 0b000100, 0b000110, 0b110010, 0b011100}, // 3
    {0b000100, 0b001100, 0b010100, 0b111110, 0b000100, 0b000100}, // 4
    {0b111110, 0b100000, 0b100000, 0b111100, 0b000010, 0b111100}, // 5
    {0b011110, 0b100000, 0b100000, 0b111100, 0b100010, 0b011100}, // 6
    {0b111110, 0b000010, 0b000100, 0b001000, 0b001000, 0b001000}, // 7
    {0b011100, 0b100010, 0b011100, 0b011100, 0b100010, 0b011100}, // 8
    {0b011100, 0b100010, 0b011110, 0b000010, 0b000010, 0b011100}, // 9
    {0b101101, 0b011110, 0b111111, 0b111111, 0b011110, 0b101101}, // Mine
    {0b001110, 0b011110, 0b111010, 0b000010, 0b000010, 0b001111}, // flag
};

#define EMOJI_WIDTH 11
#define EMOJI_HEIGHT 11

const PROGMEM uint16_t emojimap[4][EMOJI_HEIGHT] = {
    {0x0000, 0x0000, 0x0000, 0b00111111100, 0b01111111110, 0b10110001101, 0x0000, 0b00100000100, 0b00011111000, 0x0000, 0x0000}, // sunglasses
    {0x0000, 0x0000, 0x0000, 0b00101010100, 0b00010001000, 0b00101010100, 0x0000, 0b00001110000, 0b00010001000, 0x0000, 0x0000}, // dead
    {0x0000, 0x0000, 0x0000, 0b00110011000, 0b00110011000, 0x0000, 0x0000, 0b00100001000, 0b00011110000, 0x0000, 0x0000}, // smile
    {0b00011111000, 0b00111111100, 0b01111111110, 0b11111111111, 0b11111111111, 0b11111111111, 0b11111111111, 0b11111111111, 0b01111111110, 0b00111111100, 0b00011111000}, // base
};


#endif /* DISPLAY_DRIVER_H */