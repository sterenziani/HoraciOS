#ifndef _PRODCONS_H
#define _PRODCONS_H

#define QUIT 'q'
#define INC_CHEF 'w'
#define DEC_CHEF 'e'
#define INC_WAITER 'r'
#define DEC_WAITER 't'
#define MAX_PRODCONS 10
#define TRAYS_QTY 20
#define EMPTY_SPACE '-'

int prodcons_restaurant();
void printMenu();
void printTrays();
void callChef();
void * chef();
void restChef();
void chefSuicide();
void callWaiter();
void * waiter();
void restWaiter();
void waiterSuicide();
void terminateAll();

#endif
