#ifndef MUSIC_H
#define MUSIC_H

#include <avr/pgmspace.h>

//melody 1 notes
#define A3 220.0
#define AS3 233.082
#define A4 440.0
#define AS4 466.164
#define B4 493.883
#define C4 261.626
#define CS4 277.183
#define C5 523.251
#define CS5 554.365
#define D4 293.665
#define DS4 311.127
#define DS3 155.563
#define E4 329.628
#define E5 659.255
#define F4 349.228
#define FS4 369.994
#define FS3 184.997
#define GS4 415.305
#define GS3 207.652

//additional melody 2 notes
#define GS5 830.609
#define A5 880.000
#define FS5 739.989

//null note
#define NX 0.0

// set macro for bpm
#define BPM 74

#define SNOTE 200
#define ENOTE 400
#define QNOTE 810
#define HNOTE 1620
#define WNOTE 3240

// converts input frequenct to TOP for ICR1 PWM period
unsigned long outputFreq(float freq);

// returns length of quarter note in miliseconds
unsigned short getQNoteLength(unsigned short bpm);


// 'Wet Hands' - C418   (designed for quater notes)
static PROGMEM const float melody1[] = {
    A3, CS4, A4, B4, CS5, B4, A4, E4, D4, FS4, CS5, E5, CS5, A4, NX, NX,
};

#define MELODY2_INTRO_SIZE 64

static PROGMEM const float melody2_intro[] = {
    // bar 1
    FS3, CS4, AS4, GS4, AS4, CS4, AS4, CS4, FS3, CS4, AS4, GS4, AS4, CS4, AS4, CS4,

    // bar 2
    FS3, DS4, B4, AS4, B4, DS4, B4, DS4, FS3, DS4, B4, AS4, B4, DS4, B4, DS4,

    // bar 3
    FS3, F4, B4, AS4, B4, F4, B4, F4, FS3, F4, B4, AS4, B4, F4, B4, F4,

    // bar4
    FS3, FS4, AS4, GS4, AS4, FS4, AS4, FS4, FS3, FS4, AS4, GS4, AS4, FS4, AS4, F4,
};

#define MELODY2_BRIDGE_SIZE 48

static PROGMEM const float melody2_bridge[] = {
    // bar 1
    FS3, DS4, AS4, GS4, AS4, FS4, F4, FS4, DS4, FS4, F4, FS4, AS3, CS4, C4, AS3,

    // bar 2
    C4, FS4, GS4, FS4, GS4, FS4, GS4, FS4, C4, FS4, GS4, FS4, GS4, FS4, GS4, FS4,

    //bar 3
    F4, GS4, CS5, C5, CS5, GS4, FS4, GS4, F4, GS4, FS4, GS4, CS4, F4, DS4, CS4
};

#define MELODY2_LOWBRIDGE_SIZE 48

static PROGMEM const float melody2_lowbridge[] = {
    //bar 1
    DS3, AS3, FS4, F4, FS4, AS4, FS4, AS3, DS3, AS3, FS4, F4, FS4, AS3, FS4, AS3,

    // bar 2
    DS3, C4, CS4, DS4, CS4, C4, AS3, GS3, FS4, F4, DS4, CS5, C5, AS4, GS4, FS4,

    // bar 3
    F4, DS4, CS4, CS5, GS4, CS5, F4, GS4, CS4, DS4, F4, GS4, FS4, F4, DS4, CS4
};

static PROGMEM const float melody2_dark[] = {
    //
};


#endif /* MUSIC_H */