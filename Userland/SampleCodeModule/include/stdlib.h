#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

//#define NULL (void*)0
#define ERROR -1
#define SUCCESS 0

void setCursor(int x, int y);
void beep();
int screen_width();
int screen_height();
int char_height();
int char_width();
void clearScreen();

char myGetChar();

void myPrintf(char * cadena,...);
void myPrintfBig(int color,char * cadena,...);
void myPutchar(int character);
void myPutcharBig(int character,int color);
void myPrintHex(uint64_t address);

void toString(int num,char* resp);
int strlen(char* str);
int strcmp(char string1[], char string2[]);
int myAtoi(char *p);
int isNumber(char* s);
