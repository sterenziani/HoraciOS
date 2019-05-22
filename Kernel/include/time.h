#ifndef _TIME_H_
#define _TIME_H_

int seconds_elapsed();
void timer_handler();
void clock_handler(int* hours, int* minutes, int* seconds);
void timer_wait(int segs);
int currentHour();
int currentMinute();
int currentSecond();

#endif
