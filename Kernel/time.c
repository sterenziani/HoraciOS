#include <time.h>
#include <naiveConsole.h>
#include <keyboard.h>
#include <lib.h>
#include <stdint.h>

static unsigned long ticks = 0;
static unsigned long next_time = 1;
static int ticks_elapsed();
//static int seconds_elapsed();
static char translateTime(char value);

void timer_handler(){
	ticks++;
	if(seconds_elapsed() == next_time)
		next_time += 1;
}

void clock_handler(int* hours, int* minutes, int* seconds)
{
	*hours = currentHour();
	*minutes = currentMinute();
	*seconds = currentSecond();
}

static int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks_elapsed() / 18;
}

static char translateTime(char value)
{
	return (value/16)*10 + (value%16);
}

// Freezes for an amount of seconds
void timer_wait(int segs)
{
	int origin = seconds_elapsed();
	int aux;
	while((aux = seconds_elapsed()) <= (origin + segs))
	{
		printDec(aux);
		print("  vs  ");
		printDec(origin+segs);
		newLine();
	}
	newLine();
	print("END");
}

int currentHour()
{
	return translateTime(getHours());
}

int currentMinute()
{
	return translateTime(getMinutes());
}

int currentSecond()
{
	return translateTime(getSeconds());
}
