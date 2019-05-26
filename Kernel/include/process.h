#ifndef _PROCESS_H
#define _PROCESS_H

#include <binary_info.h>
#include <mailbox.h>

typedef unsigned int priority_t;

#define MAX_PRIORITY 9

typedef unsigned int process_id_t;

typedef struct process_stack {
	void *stack_base;
	void *current;
	int size;
} process_stack_t;

#define PROCESS_NAME_LENGTH 256

typedef struct process_context_block {
	//
	char name[PROCESS_NAME_LENGTH];
	//
	process_id_t id;

	priority_t priority;
	//
	process_state_t state;
	//
	process_stack_t stack;
	//
	process_type_t type;
	struct process_context_block * parent;
	mailbox_t input;
	mailbox_t output;
} process_context_block_t;

typedef process_context_block_t *ProcessContextBlock;

void *get_rsp(ProcessContextBlock process);
void set_rsp(ProcessContextBlock process, void *rsp);
int create_process(void **argv, void *entry_point, int *process_id, process_type_t type, char *name, int priority);
int doom_process(process_id_t pid);
int resume_process(process_id_t pid);
int mark_process_as_blocked(process_id_t pid);
int process_is_blocked(process_id_t pid);
process_state_t get_process_state(process_id_t pid);
int get_process_id(ProcessContextBlock process);
void get_ready_processes(binary_info_t *binary_info, int max, int *returned);
int change_priority(process_id_t pid, int priority);
int mark_process_as_finished(process_id_t pid);
int process_has_finished(process_id_t pid);

#endif
