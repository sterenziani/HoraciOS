#include <stdlib.h>
#include <SysCallDispatcher.h>

void sleep(int extra_secs)
{
	if(extra_secs <= 0)
		return;
	int hour;
	int minute;
	int seconds;
	SysCallDispatcher(2, (uint64_t)&hour,(uint64_t)&minute,(uint64_t)&seconds, 0, 0);
	int current_time = hour*3600 + minute*60 + seconds;
	// maximo current_time es 86399
	int target_time = (current_time + extra_secs) % 86400;
	while(current_time != target_time)
	{
			SysCallDispatcher(2, (uint64_t)&hour,(uint64_t)&minute,(uint64_t)&seconds, 0, 0);
			current_time = hour*3600 + minute*60 + seconds;
	}
	return;
}

void watch() {
	int hour;
	int minute;
	int seconds;
	int aux;
	SysCallDispatcher(2, (uint64_t)&hour,(uint64_t)&minute,(uint64_t)&seconds, 0, 0);
	if(hour < 10) {
		myPutchar('0');
		myPutchar(hour+'0');
	} else {
		aux = hour%10;
		myPutchar((hour/10)+'0');
		myPutchar(aux+'0');
	}
	myPutchar(':');
	if(minute < 10) {
		myPutchar('0');
		myPutchar(minute+'0');
	} else {
		aux = minute%10;
		myPutchar((minute/10)+'0');
		myPutchar(aux+'0');
	}
	myPutchar(':');
	if(seconds < 10) {
		myPutchar('0');
		myPutchar(seconds+'0');
	} else {
		aux = seconds%10;
		myPutchar((seconds/10)+'0');
		myPutchar(aux+'0');
	}
	myPutchar('\n');
	return;
}

void digitalClock() {
	int hour;
	int minute;
	int seconds;
	int aux;
	int color = 1;
	char c;
	clearScreen();
	setCursor(screen_width()/2-200,screen_height()/2+100);
	myPrintf("Presione Barra espaciadora para cambiar el color\n");
	setCursor(screen_width()/2-120,screen_height()/2+120);
	myPrintf("Presione Enter para salir\n");
		while((c=myGetChar()) != '\n') {
			setCursor(20,240);
			SysCallDispatcher(2,(uint64_t)&hour,(uint64_t)&minute,(uint64_t)&seconds, 0, 0);
			if(c == ' ')
			{
				if(color == 5) {
					color = 1;
				} else {
					color++;
				}
				beep();
			}
			if(hour < 10) {
				myPutcharBig('0',color);
				myPutcharBig(hour+'0',color);
			} else {
				aux = hour%10;
				myPutcharBig((hour/10)+'0',color);
				myPutcharBig(aux+'0',color);
			}
			myPutcharBig(':',color);
			if(minute < 10) {
				myPutcharBig('0',color);
				myPutcharBig(minute+'0',color);
			} else {
				aux = minute%10;
				myPutcharBig((minute/10)+'0',color);
				myPutcharBig(aux+'0',color);
			}
			myPutcharBig(':',color);
			if(seconds < 10) {
				myPutcharBig('0',color);
				myPutcharBig(seconds+'0',color);
			} else {
				aux = seconds%10;
				myPutcharBig((seconds/10)+'0',color);
				myPutcharBig(aux+'0',color);
			}
		}
	clearScreen();
	setCursor(0, screen_height()-char_height());
	return;
}
