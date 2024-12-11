#include <avr/io.h>
#include <spiAVR.h>
#include <display_driver.h>
#include <graphics.h>
#include <serialATmega.h>
#include <irAVR.h>
#include <timerISR.h>
#include <music_driver.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <eeprom.h>

#define NUM_TASKS 6

// uninitialized game grid
uint8_t grid[10][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// data grid used to show mine count around each tile
uint8_t mineInfo [10][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

//global variables
selection currSelection;
unsigned long decodedVal;
unsigned char mineCount;
int8_t game_state; // 0 = playing, 1 = win, 2 = lose, 3 = pause, 5 = start modal
unsigned char menuSelection; // 0 = start, 2 = temp
unsigned char modalSelection; // 0 = reset, 1 = exit / save
unsigned short timer;

typedef struct _task{
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
}task;

//task periods
const unsigned long TIMER_PERIOD = 1000;
const unsigned long SELECT_PERIOD = 200;
const unsigned long MENU_GRAPHICS_HANDLER_PERIOD = 500;
const unsigned long MODAL_GRAPHICS_HANDLER_PERIOD = 500;
const unsigned long BUZZER_PERIOD = SNOTE;
const unsigned long GAME_EVENT_PERIOD = 1000;
const unsigned long GCD_PERIOD = 100;

enum timer_states {timer_start, timer_update};
int timer_tick(int);

enum select_states {select_start, select_wait};
int select_tick(int);

enum menu_graphics_handler_states {menu_graphic_handler_start, menu_graphic_handler_wait};
int menu_graphics_handler_tick(int);

enum modal_graphics_handler_states {modal_graphics_handler_start, modal_graphics_handler_wait};
int modal_graphics_handler_tick(int);

enum Buzzer_states{Buzzer_Start, Buzzer_State1, Buzzer_State2};
int buzzer_tick(int);

enum game_event_states {game_start, game_reset, game_playing, game_win, game_lose, game_pause, game_win_loss_modal};
int game_event_tick(int);

_task tasks [] = {
    {timer_start, TIMER_PERIOD, TIMER_PERIOD, &timer_tick},
    {select_start, SELECT_PERIOD, SELECT_PERIOD, &select_tick},
    {menu_graphic_handler_start, MENU_GRAPHICS_HANDLER_PERIOD, MENU_GRAPHICS_HANDLER_PERIOD, &menu_graphics_handler_tick},
    {modal_graphics_handler_start, MODAL_GRAPHICS_HANDLER_PERIOD, MODAL_GRAPHICS_HANDLER_PERIOD, &modal_graphics_handler_tick},
    {Buzzer_Start, BUZZER_PERIOD, BUZZER_PERIOD, &buzzer_tick},
    {game_start, GAME_EVENT_PERIOD, GAME_EVENT_PERIOD, &game_event_tick},
};

void TimerISR()
{
    TimerFlag = 1;
}

int main(void)
{
    // initialize ports
    DDRB = 0xFF;
    DDRD = 0xFF;

    PORTD = 0x00;
    
    // initialize SPI and display
    SPI_INIT();
    ST7735_init();
    IRinit(&DDRD, &PIND, 4);

    // init timer
    TimerSet(GCD_PERIOD);
    TimerOn();

    // buzzer init
    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);
    ICR1 = 15000;
    OCR1A = (unsigned)(ICR1 / 2);

    // init globals
    decodedVal = 0;
    currSelection = {0, 0};
    mineCount = 10;
    game_state = 5;
    menuSelection = 0;
    modalSelection = 0;
    timer = 0;

    // init game screen
    clearBG(GREY);
    drawStartMenu();

    // init seed using eeprom pseudo-random number at address 0x00
    uint8_t seed = eepromReadData(0x00);
    eepromWriteData(0x00, ++seed);
    srand(seed);

    // main loop
    while(1) {
        for (unsigned int i = 0; i < NUM_TASKS; i++)
        {
            if(tasks[i].elapsedTime >= tasks[i].period)
            {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
            }
            tasks[i].elapsedTime += GCD_PERIOD;
        }

        while (!TimerFlag){}
        TimerFlag = 0;
    }

    return 0;
}

int timer_tick(int state)
{

    switch(state)
    {
        case timer_start:
            if(game_state == 5) {
                state = timer_start;
            }
            else if (game_state == 0) {
                state = timer_update;
                timer = 0;
            }
            break;

        case timer_update:
            if(game_state == 0) {
                state = timer_update;
            }
            else if (game_state == 1 || game_state == 2) {
                state = timer_start;
                timer = 0;
            }
            break;

        default:
            break;
    }

    switch(state)
    {
        case timer_start:
            break;

        case timer_update:
            if (game_state == 0) {
                updateTimer(timer);  
            }
            
            timer++;
            if(timer > 999) { timer = 0; }
            break;

        default:
            break;
    }

    return state;
}

int select_tick(int state)
{
    static decode_results results;
    static selection prevSelection;

    switch(state)
    {
        case select_start:
                if(!IRdecode(&results))
                {
                    state = select_start;
                }
                else
                {
                    state = select_wait;
                    IRresume();
                }
            break;

        case select_wait:
            state = select_start;
            decodedVal = results.value;
            prevSelection = currSelection;

            if(game_state == 0){
                if(decodedVal == 0xFF629D) //up
                {
                    if (currSelection.row - 1 >= 0 && currSelection.row - 1 < 10){
                        currSelection.row--;
                    }
                }
                else if (decodedVal == 0xFFA857) //down
                {
                    if (currSelection.row + 1 >= 0 && currSelection.row + 1 < 10){
                        currSelection.row++;
                    }
                }
                else if (decodedVal == 0xFFE21D) //flag
                {
                    if(grid[currSelection.column][currSelection.row] == 2)
                    {
                        unflagTile(grid, currSelection);
                        grid[currSelection.column][currSelection.row] = 0;
                        mineCount++;
                        updateMineCounter(mineCount);
                    }
                    else{
                        flagTile(grid, currSelection);
                        grid[currSelection.column][currSelection.row] = 2;
                        mineCount--;
                        updateMineCounter(mineCount);
                    }
                }
                unhighlightGrid(prevSelection);
                highlightGrid(currSelection);
            }

            // handling for all inputs which are used for different graphic menus
            if (decodedVal == 0xFF02FD) //select
            {
                if(game_state == 0) {
                    floodfillReveal(grid, mineInfo, currSelection);
                    if(checkWin(grid)){
                        drawEmoji(3, 60, 7, YELLOW);
                        drawEmoji(0, 60, 7, BLACK);
                        modalSelection = 0;
                        game_state = 1;
                    }
                    if(checkLose(grid)){
                        drawEmoji(3, 60, 7, YELLOW);
                        drawEmoji(1, 60, 7, BLACK);
                        modalSelection = 0;
                        game_state = 2;
                    }
                }
                else if (game_state == 2 || game_state == 1) {
                    if(modalSelection == 0) {
                        clearBG(GREY);
                        currSelection = {0, 0};
                        mineCount = 10;
                        game_state = 0;
                        timer = 0;
                    }
                    else if(modalSelection == 1) {
                        clearBG(GREY);
                        game_state = 5;
                    }
                }
                else if (game_state == 3) {
                    if(modalSelection == 0) {
                        clearBG(GREY);
                        currSelection = {0, 0};
                        mineCount = 10;
                        timer = 0;
                        game_state = 0;
                    }
                    else if(modalSelection == 1) {
                        clearBG(GREY);
                        game_state = 5;
                    }
                }
                else if (game_state == 5) {
                    if (menuSelection == 0) { // start
                        clearBG(GREY);
                        currSelection = {0, 0};
                        mineCount = 10;
                        timer = 0;
                        game_state = 0;
                    }
                    else if (menuSelection == 1) { // temp

                    }
                }
            }

            if (decodedVal == 0xFF22DD) //left
            {
                if (game_state == 0) {
                    if (currSelection.column - 1 >= 0 && currSelection.column - 1 < 10){
                        currSelection.column--;
                    }
                    unhighlightGrid(prevSelection);
                    highlightGrid(currSelection);
                }
                else if (game_state == 3 || game_state == 2 || game_state == 1) {
                    modalSelection = 0;
                    drawRectangle(75, 45, 91, 62, GREY);
                }
                else if (game_state == 5) {
                    drawRectangle(90, 86, 106, 102, GREY); // draw over old cursor position
                    menuSelection = 0;
                }
            }

            if (decodedVal == 0xFFC23D) //right
            {
                if (game_state == 0) {
                    if(currSelection.column + 1 >= 0 && currSelection.column + 1 < 10){
                        currSelection.column++;
                    }
                    unhighlightGrid(prevSelection);
                    highlightGrid(currSelection);
                }
                else if (game_state == 3 || game_state == 2 || game_state == 1) {
                    modalSelection = 1;
                    drawRectangle(31, 45, 47, 62, GREY);
                }
                else if (game_state == 5) {
                    drawRectangle(20, 86, 36, 102, GREY);  // draw over old cursor position
                    menuSelection = 1;
                }
            }

            if (decodedVal == 0xFFA25D) { // pause
                if(game_state == 0) {
                    modalSelection = 0;
                    game_state = 3;
                }
                else if(game_state == 3) {
                    modalSelection = 2; // no selection
                    game_state = 0;
                }
            }

            decodedVal = 0;
            results.value = 0;
            break;

        default:
            break;
    }

    switch(state)
    {
        case select_start:
            break;

        case select_wait:
            break;

        default:
            break;
    }

    return state;
}

int menu_graphics_handler_tick(int state)
{
    static uint8_t offset = 0;

    switch(state)
    {
        case menu_graphic_handler_start:
            if(game_state != 5) {
                state = menu_graphic_handler_wait;
            }
            else if ( game_state == 5) {
                state = menu_graphic_handler_start;
                if (menuSelection == 0) {
                    if (offset) {
                        drawRectangle(20, 86, 32, 86, GREY);
                    }
                    drawCursor(20, 86 + offset, 0);
                    if(!offset) {
                        drawRectangle(29, 102, 30, 102, GREY);
                    }
                }
                else if (menuSelection == 1) {
                    if (offset) {
                        drawRectangle(90, 86, 102, 86, GREY);
                    }
                    drawCursor(90, 86 + offset, 0);
                    if(!offset) {
                        drawRectangle(99, 102, 100, 102, GREY);
                    }
                }
                offset = !offset;
            }
            break;

        case menu_graphic_handler_wait:
            if(game_state == 5) {
                state = menu_graphic_handler_start;
            }
            else {
                state = menu_graphic_handler_wait;
            }
            break;

        default:
            break;
    }

    switch(state)
    {
        case menu_graphic_handler_start:
            break;

        case menu_graphic_handler_wait:
            break;

        default:
            break;
    }

    return state;
}

int modal_graphics_handler_tick(int state)
{
    static uint8_t offset = 0;

    switch(state)
    {
        case modal_graphics_handler_start:
            if(game_state != 1 && game_state != 2 && game_state != 3) {
                state = modal_graphics_handler_wait;
            }
            else {
                state = modal_graphics_handler_start;

                if (modalSelection == 0) {
                    if (offset) {
                        drawRectangle(31, 46, 43, 46, GREY);
                    }
                    drawCursor(31, 46 + offset, 0);
                    if(!offset) {
                        drawRectangle(40, 62, 41, 62, GREY);
                    }
                }
                else if (modalSelection == 1) {
                    if (offset) {
                        drawRectangle(75, 46, 87, 46, GREY);
                    }
                    drawCursor(75, 46 + offset, 0);
                    if(!offset) {
                        drawRectangle(84, 62, 85, 62, GREY);
                    }
                }
                offset = !offset;

            }
            break;

        case modal_graphics_handler_wait:
            if(game_state == 2 || game_state == 1 || game_state == 3) {
                state = modal_graphics_handler_start;
            }
            else {
                state = modal_graphics_handler_wait;
            }
            break;

        default:
            break;
    }

    switch(state)
    {
        case modal_graphics_handler_start:
            break;

        case modal_graphics_handler_wait:
            break;

        default:
            break;
    }

    return state;
}

int buzzer_tick(int state)
{
  static unsigned long i;
  static unsigned short noteArrSize;
  static float currNote;

  switch(state)
  {
    case Buzzer_Start:
      state = Buzzer_State1;
      noteArrSize = MELODY2_INTRO_SIZE;
      i = 0;
      break;

    case Buzzer_State1:
        if(i >= noteArrSize) { 
            state = Buzzer_State2;
            noteArrSize = MELODY2_BRIDGE_SIZE;
            i = 0;
        } else {
            state = Buzzer_State1;
        }
      break;

    case Buzzer_State2:
        if(i >= noteArrSize) { 
            state = Buzzer_State1;
            noteArrSize = MELODY2_INTRO_SIZE;
            i = 0;
        } else {
            state = Buzzer_State2;
        }
      break;

    default:
      break;
  }

  switch(state)
  {
    case Buzzer_Start:
      break;

    case Buzzer_State1:
        currNote = pgm_read_float(&(melody2_intro[i]));
        if (currNote == NX){
            OCR1A = ICR1;
        }
        else{
        ICR1 = outputFreq(currNote);
        OCR1A = (unsigned long)(ICR1 / 2);
        }

        i++;
      break;

    case Buzzer_State2:
        currNote = pgm_read_float(&(melody2_bridge[i]));
        if (currNote == NX){
            OCR1A = ICR1;
        }
        else{
        ICR1 = outputFreq(currNote);
        OCR1A = (unsigned long)(ICR1 / 2);
        }

        i++;
      break;

    default:
      break;
    }   

    return state;
}

int game_event_tick(int state)
{
    switch(state)
    {
        case game_start:
            if (game_state == 5) {
                state = game_start;
            }
            else if (game_state == 0) {
                if(menuSelection == 0) {
                    state = game_reset;
                }
                else if(menuSelection == 1) {
                    // game load button functionality for start menu
                }
            }
            break;

        case game_reset:
            state = game_playing;

            // init game data
            clearMinesweeper(grid);
            clearMineInfo(mineInfo);
            initMinesweeper(grid);
            initMineInfo(grid, mineInfo);

            // init game screen
            drawGrid();
            drawMenuBox();
            drawOuterDetails();
            drawEmoji(3, 60, 7, YELLOW);
            drawEmoji(2, 60, 7, BLACK);
            updateMineCounter(mineCount);
            highlightGrid(currSelection);
            break;

        case game_playing:
            if (game_state == 0) {
                state = game_playing;
            }
            else if (game_state == 1 || game_state == 2) {
                state = game_win_loss_modal;
                if(game_state == 1)
                {
                    unhighlightGrid(currSelection);
                    flagAllMines(grid);
                }
                if(game_state == 2)
                {
                    unhighlightGrid(currSelection);
                    revealAllMines(grid, mineInfo);
                }

                drawGameModal(game_state);
            }
            else if (game_state == 3) {
                state = game_pause;
                drawPauseMenu();
            }
            break;

        case game_pause:
            if (game_state == 5) {
                state = game_start;
                drawStartMenu();
            }
            else if (game_state == 0 && modalSelection == 0) {
                state = game_reset;
                clearModal();
            }
            else if (game_state == 0 && modalSelection == 1) {
                state = game_start;
                drawStartMenu();
            }
            else if (game_state == 0 && modalSelection == 2) {
                state = game_playing;
                clearModal();
                drawCells(grid, mineInfo);
                highlightGrid(currSelection);
            }
            else {
                state = game_pause;
            }
            break;

        case game_win_loss_modal:
            if (game_state == 5) {
                state = game_start;
                drawStartMenu();
            }
            else if (game_state == 0) {
                state = game_playing;

                // init game data
                clearMinesweeper(grid);
                clearMineInfo(mineInfo);
                initMinesweeper(grid);
                initMineInfo(grid, mineInfo);

                // init game screen
                drawGrid();
                drawMenuBox();
                drawOuterDetails();
                drawEmoji(3, 60, 7, YELLOW);
                drawEmoji(2, 60, 7, BLACK);
                updateMineCounter(mineCount);
                highlightGrid(currSelection);
            }
            else {
                state = game_win_loss_modal;
            }
            break;

        default:
            break;
    }

    switch(state)
    {
        case game_start:
            break;

        case game_playing:
            break;

        case game_win:
            break;

        case game_lose:
            break;

        case game_win_loss_modal:
            break;

        default:
            break;
    }

    return state;
}






