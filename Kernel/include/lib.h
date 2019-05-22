#ifndef LIB_H
#define LIB_H

#include <stddef.h>
#define ERROR -1
#define SUCCESS 0
#define FALSE 0
#define TRUE 1
#define PROCESS_QUEUE_SIZE 30
#define NAME_LENGTH 50

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);
char getSeconds(void);
char getMinutes(void);
char getHours(void);
char readKey(void);
void outb(uint64_t p1, uint64_t p2);
uint8_t inb(uint64_t p);
uint64_t _getRAX();
uint64_t _getRBX();
uint64_t _getRCX();
uint64_t _getRDX();
uint64_t _getRBP();
uint64_t _getRSP();
uint64_t _getRDI();
uint64_t _getRSI();
uint64_t _getR8();
uint64_t _getR9();
uint64_t _getR10();
uint64_t _getR11();
uint64_t _getR12();
uint64_t _getR13();
uint64_t _getR14();
uint64_t _getR15();
uint64_t _setRSP();
uint64_t _setRDI();
void beepon();
void beepoff();
int mutex_acquire(int* rdi);

#endif
