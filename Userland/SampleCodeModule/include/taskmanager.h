#ifndef _TASK_MANAGER_H
#define _TASK_MANAGER_H
#include <ipc.h>

typedef unsigned int process_id_t;
int run(char *name, void *entry_point, void ** argv, int* pid, int process_type, int priority, mailbox_t input, mailbox_t output);
void kill(process_id_t pid);
void list_all_processes();
void nice(process_id_t pid, int new_priority);
void reset_io();

#endif
