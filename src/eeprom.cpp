#include "eeprom.h"

void eepromWriteData(uint8_t address, uint8_t data) {
    while(EECR & (1 << EEPE));
    EEAR = address;
    EEDR = data;
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
}

uint8_t eepromReadData(uint8_t address) {
    EEAR = address;
    EECR |= (1 << EERE);
    return EEDR;
}

void storeGame(uint8_t grid[10][10], uint8_t mineInfo[10][10], unsigned char mineCount, unsigned short time)
{
    uint8_t currAddress = 0x01;
    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            eepromWriteData(currAddress, grid[i][j]);
            currAddress++;
        }
    }

    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            eepromWriteData(currAddress, mineInfo[i][j]);
            currAddress++;
        }
    }

    eepromWriteData(currAddress, mineCount);
    currAddress++;
    eepromWriteData(currAddress, time & 0xFF);
    currAddress++;
    eepromWriteData(currAddress, (time >> 8) & 0xFF);
}

void loadGame(uint8_t grid[10][10], uint8_t mineInfo[10][10], unsigned char &mineCount, unsigned short &time)
{
    uint8_t currAddress = 0x01;
    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            grid[i][j] = eepromReadData(currAddress);
            currAddress++;
        }
    }

    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            mineInfo[i][j] = eepromReadData(currAddress);
            currAddress++;
        }
    }

    mineCount = eepromReadData(currAddress);
    currAddress++;
    time = eepromReadData(currAddress);
    currAddress++;
    time |= (uint16_t)(eepromReadData(currAddress)) << 8;
}