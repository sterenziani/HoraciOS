#ifndef _ROUND_ROBIN_H
#define _ROUND_ROBIN_H

#include "process.h"

void queue_round_robin(void* process_context_block, int priority);
void dequeue_round_robin();
void* peek_round_robin();
void next_round_robin();
int amount_in_round_robin();

#endif