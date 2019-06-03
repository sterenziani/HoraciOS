#ifndef _PHILOSOPHERS_H
#define _PHILOSOPHERS_H


#define INC_PHILO 'w'
#define DEC_PHILO 's'
#define QUIT 'q'
#define MAX_PHILOSOPHERS 15


void prepareTable();

void printInstructions();

void * philosopher(void** args);

void hirePhilosopher();

void letGoPhilosopher();

void removePhilosopher();

void destroyAll();


#endif