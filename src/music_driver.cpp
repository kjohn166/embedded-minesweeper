#include <music_driver.h>

unsigned long outputFreq(float freq)
{
  return (unsigned long)(16000000 / (8 * freq));
}

unsigned short getQNoteLength(unsigned short bpm)
{
  return (unsigned short)((60 / bpm) * 1000);
}