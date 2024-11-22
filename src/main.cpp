#include <avr/io.h>
#include <spiAVR.h>
#include <display_driver.h>
#include <graphics.h>
#include <serialATmega.h>
#include <irAVR.h>
#include <timerISR.h>
#include <music_driver.h>
#include <avr/pgmspace.h>

#define NUM_TASKS 3

// first number is column number, second number is row number in matrix
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
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

//global variables
selection currSelection;
unsigned long decodedVal;
unsigned char mineCount;
unsigned char game_state; // 0 = playing, 1 = win, 2 = lose

typedef struct _task{
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
}task;

//task periods
const unsigned long TIMER_PERIOD = 1000;
const unsigned long SELECT_PERIOD = 200;
const unsigned long BUZZER_PERIOD = ENOTE;
const unsigned long GCD_PERIOD = 100;

enum timer_states {timer_start, timer_update};
int timer_tick(int state);

enum select_states {select_start, select_wait};
int select_tick(int state);

enum Buzzer_states{Buzzer_Start, Buzzer_State1, Buzzer_State2};
int buzzer_tick(int);

_task tasks [] = {
    {timer_start, TIMER_PERIOD, TIMER_PERIOD, &timer_tick},
    {select_start, SELECT_PERIOD, SELECT_PERIOD, &select_tick},
    {Buzzer_Start, BUZZER_PERIOD, BUZZER_PERIOD, &buzzer_tick}
};


void TimerISR()
{
    TimerFlag = 1;
}

int main(void)
{
    DDRB = 0xFF;
    DDRD = 0xFF;

    PORTD = 0x00;
    
    //initialize SPI and display
    SPI_INIT();
    ST7735_init();
    IRinit(&DDRD, &PIND, 4);

    TimerSet(GCD_PERIOD);
    TimerOn();

    // buzzer init
    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);
    ICR1 = 15000;
    OCR1A = (unsigned)(ICR1 / 2);

    decodedVal = 0;
    currSelection = {0, 0};
    mineCount = 10;

    clearBG(GREY);
    drawGrid();
    drawMenuBox();
    drawOuterDetails();
    drawEmoji(3, 60, 7, YELLOW);
    drawEmoji(2, 60, 7, BLACK);
    updateMineCounter(mineCount);

    initMinesweeper(grid);
    highlightGrid(currSelection);


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

        while (!TimerFlag){}  // Wait for SM period
        TimerFlag = 0;

    }

    return 0;
}

int timer_tick(int state)
{
    static unsigned long i;

    switch(state)
    {
        case timer_start:
            state = timer_update;
            i = 0;
            break;

        case timer_update:
            state = timer_update;
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
                updateTimer(i);  
            }
            
            i++;
            if(i > 999) { i = 0; }
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
                else if (decodedVal == 0xFF22DD) //left
                {
                    if (currSelection.column - 1 >= 0 && currSelection.column - 1 < 10){
                        currSelection.column--;
                    }
                }
                else if (decodedVal == 0xFFC23D) //right
                {
                    if (currSelection.column + 1 >= 0 && currSelection.column + 1 < 10){
                        currSelection.column++;
                    }
                }
                else if (decodedVal == 0xFF02FD) //select
                {
                    floodfillReveal(grid, currSelection);
                    if(checkWin(grid)){
                        drawEmoji(3, 60, 7, YELLOW);
                        drawEmoji(0, 60, 7, BLACK);
                        game_state = 1;
                    }
                    if(checkLose(grid)){
                        drawEmoji(3, 60, 7, YELLOW);
                        drawEmoji(1, 60, 7, BLACK);
                        game_state = 2;
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
            
            if(game_state == 1)
            {
                unhighlightGrid(currSelection);
                flagAllMines(grid);
            }

            if(game_state == 2)
            {
                unhighlightGrid(currSelection);
                revealAllMines(grid);
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








