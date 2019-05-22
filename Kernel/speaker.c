#include <speaker.h>
#include <stdint.h>
#include <lib.h>
#include <time.h>

void speaker_handler()
{
  beep();
}

void beep()
{
  beepon();
  for (int i = 0; i < 40000; i++) {
    ;
  }
  beepoff();
}
