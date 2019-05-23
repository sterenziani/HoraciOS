#include <non_sizable_stack.h>
#include <process.h>
#include <priority_round_robin.h>
#include <scheduler.h>
#include <libasm.h>
#include <defs.h>
#include <memoryManager.h>
#include <mutex.h>
#include <strings.h>
#include <binary_info.h>

#define MAX_PROCESS_ID MAX_PROCESS_COUNT-1
#define MAX_ARG_COUNT 7
#define PROCESS_INIT_STACK_SIZE 40960
#define VALID_PROCESS_ID(__pid) (__pid < MAX_PROCESS_COUNT)
#define INIT_PRIORITY 0
#define VALID_PRIORITY(__priority) (__priority >= 0 && __priority <= MAX_PRIORITY)

static process_context_block_t ready_processes[MAX_PROCESS_COUNT];
static int process_pointer = -1;
static int process_count = -1;

static void ready_processes_containers(){
	int i;
	char *stack_base;
	ProcessContextBlock process_context;
	for (i = 0; i < MAX_PROCESS_COUNT; ++i)
	{
		process_context = &(ready_processes[i]);
		stack_base = malloc(PROCESS_INIT_STACK_SIZE);
		process_context->id = i;
		process_context->stack.stack_base = stack_base + PROCESS_INIT_STACK_SIZE;
		process_context->stack.size = PROCESS_INIT_STACK_SIZE;
		process_context->state = FINISHED;
	}
}

static int count_process_args(void ** argv){
	if (argv == NULL){
		return 0;
	}
	int argc = 0;
	while(*argv != NULL && argc < MAX_ARG_COUNT){
		argc++;
		argv++;
	}
	return argc;
}

static int get_parent_process_id(process_id_t pid){
	if (!VALID_PROCESS_ID(pid))
	{
		return -2;
	}
	if(ready_processes[pid].parent == NULL){
		return -1;
	}
	return ready_processes[pid].parent->id;
}

int wrapper_process(void (*entry_point)(void**), void **argv, int argc, process_id_t pid){
	entry_point(argv);
	int parentid = get_parent_process_id(pid);
	if (parentid >= 0){
		resume_process(parentid);
	}
	mark_process_as_finished(pid);
	return 1;
}

int get_process_id(ProcessContextBlock process){
	if (process == NULL)
	{
		return -1;
	}
	return process->id;
}

int create_process(void **argv, void *entry_point, int *process_id, process_type_t type, char *name, int priority){
	if (name == NULL || !VALID_PRIORITY(priority)){
		if (process_id != NULL){
			*process_id = -1;
		}
    	return 0;
	}
	int ppid = get_current_pid();
	if(process_pointer == -1){
		process_pointer = 0;
		process_count = 0;
		ready_processes_containers();
	}

//// ENTER DANGER ZONE
	int reserved_process_pointer;
	mutex_t process_mutex = mutex_create("process_mutex");
	mutex_lock(process_mutex);
	if (process_count == MAX_PROCESS_COUNT){
		mutex_unlock(process_mutex);
		if (process_id != NULL){
			*process_id = -1;
		}
    	return 0;
  	}
	reserved_process_pointer = process_pointer;
	ready_processes[process_pointer].state = READY;
  	do{
    	process_pointer++;
   		if (process_pointer == MAX_PROCESS_COUNT){
     		process_pointer = 0;
		}
  	}while(ready_processes[process_pointer].state != FINISHED);
  	process_count++;
	mutex_unlock(process_mutex);
//// EXIT DANGER ZONE
	strncpy(ready_processes[reserved_process_pointer].name, name, PROCESS_NAME_LENGTH);

	if(process_id != NULL)
		*process_id = reserved_process_pointer;
	ready_processes[reserved_process_pointer].type = type;
	if(ppid == -1){
		ready_processes[reserved_process_pointer].parent = NULL;
	} else {
		ready_processes[reserved_process_pointer].parent = &ready_processes[ppid];
	}
	ready_processes[reserved_process_pointer].stack.current = _initialize_stack(argv, count_process_args(argv), ready_processes[reserved_process_pointer].stack.stack_base, (void *) wrapper_process, entry_point, reserved_process_pointer);
	ready_processes[reserved_process_pointer].priority = priority;
  	enqueue_process(&ready_processes[reserved_process_pointer]);
	if (type == FOREGROUND && ppid != -1 && scheduled_processes() > 1){
		mark_process_as_blocked(ppid);
	}
  	return 1;
}

static int change_process_state(process_id_t pid, process_state_t state){
  if (!VALID_PROCESS_ID(pid) || ready_processes[pid].state == FINISHED || ready_processes[pid].state == state){
    return 0;
  }
  ready_processes[pid].state = state;
  return 1;
}

int mark_process_as_finished(process_id_t pid){

    //DANGER ZONE process_count START
	mutex_t process_mutex = mutex_create("process_mutex");
    mutex_lock(process_mutex);
	ready_processes[pid].stack.current = ready_processes[pid].stack.stack_base;
    change_process_state(pid, FINISHED);
  	process_count--;
	mutex_unlock(process_mutex);
  	//DANGER ZONE process_count END

  	if (get_current_pid() == pid){
  		_force_irq00();
  	}
	return 1;
}

int mark_process_as_blocked(process_id_t pid){
  	change_process_state(pid, BLOCKED);
  	if(get_current_pid() == pid){
  		_force_irq00();
  	}
	return 1;
}

int resume_process(process_id_t pid){
  change_process_state(pid, READY);
	return 1;
}

process_state_t get_process_state(process_id_t pid){
	if (!VALID_PROCESS_ID(pid)){
    	return -1;
  	}
  	return ready_processes[pid].state;
}

static int process_has_state(process_id_t pid, process_state_t state){

  return ready_processes[pid].state == state;
}

int process_has_finished(process_id_t pid){
  return process_has_state(pid, FINISHED);
}

int process_is_blocked(process_id_t pid){
  return process_has_state(pid, BLOCKED);
}

void *get_rsp(ProcessContextBlock process){
	return process->stack.current;
}

void set_rsp(ProcessContextBlock process, void *rsp){
	process->stack.current = rsp;
}

int change_priority(process_id_t pid, int priority){
	if (!VALID_PROCESS_ID(pid) || !VALID_PRIORITY(priority)){
		return 0;
	}
	ready_processes[pid].priority = priority;
	return 1;
}

void get_ready_processes(binary_info_t *binary_info, int max, int *returned){
	int i, c = 0;
	for (i = 0; i < MAX_PROCESS_COUNT && c < max; ++i)
	{
		if (!process_has_finished(ready_processes[i].id))
		{
			binary_info[c].id = ready_processes[i].id;
			binary_info[c].state = ready_processes[i].state;
			strncpy(binary_info[c].name, ready_processes[i].name, PROCESS_NAME_LENGTH);
			binary_info[c].type = ready_processes[i].type;
			binary_info[c].priority = ready_processes[i].priority;
			c++;
		}

	}
	if (returned != NULL){
		*returned = c;
	}
}
