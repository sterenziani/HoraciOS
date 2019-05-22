#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <process.h>

void enqueue_process(ProcessContextBlock process);
void true_enqueue_process();
void *short_term_scheduling(void *rsp);
int get_current_pid();
int scheduled_processes();

#endif
