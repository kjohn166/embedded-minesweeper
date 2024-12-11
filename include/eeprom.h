#include <avr/io.h>

void eepromWriteData(uint8_t address, uint8_t data);
uint8_t eepromReadData(uint8_t address);
void storeGame(uint8_t grid[10][10], uint8_t mineInfo[10][10], unsigned char mineCount, unsigned short time);
void loadGame(uint8_t grid[10][10], uint8_t mineInfo[10][10], unsigned char &mineCount, unsigned short &time);

// add a clear save function