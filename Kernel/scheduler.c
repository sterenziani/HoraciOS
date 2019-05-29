#include <process.h>
#include <priority_round_robin.h>
#include <non_sizable_stack.h>
#include <defs.h>

static non_sizable_stack_t to_queue = {1, {0}, 0};

void enqueue_process(ProcessContextBlock process){
	void *prev = NULL;
	if(!is_empty(&to_queue)){
		prev = pop(&to_queue);
	}
	push(&to_queue, (void *) process);
	if (prev != NULL)
	{
		push(&to_queue, prev);
	}
}

void true_enqueue_process(){
	ProcessContextBlock queuee;
	while((queuee = (ProcessContextBlock) pop(&to_queue)) != NULL){
		queue_round_robin(queuee, queuee->priority);
	}
}

int scheduled_processes(){
	return amount_in_round_robin() + count(&to_queue);
}

static void *create_ghost_process(){
	header_t header = {"shell", FOREGROUND, 1, NULL, NULL};
	create_process(NULL, (void *) 0x400000, &header);
	true_enqueue_process();
	return get_rsp(peek_round_robin());
}

void *short_term_scheduling(void *rsp){
	true_enqueue_process();
	int finished_process;
	ProcessContextBlock current_process = peek_round_robin();
	if (current_process == NULL){
		return create_ghost_process();
	}

	//From here there is at least 1 ready process of unknown state

	current_process = peek_round_robin();
	if (!(finished_process = process_has_finished(get_process_id(current_process)))){
		set_rsp(current_process, rsp);
	}
	/*else {
		while(finished_process = process_has_finished(get_process_id(current_process))){
			dequeue_round_robin();
			current_process = peek_round_robin();
			if (current_process == NULL){
				return create_ghost_process();
			}
			return get_process_id(current_process);
		}
	}*/

	//From here there is at least 1 ready process who has not finished and it is pointing to

	next_round_robin();
	int process_state;
	do{
    	current_process = peek_round_robin();
    	if (current_process == NULL){
			return create_ghost_process();
		}
    	process_state = get_process_state(get_process_id(current_process));
    	switch(process_state){
    		case BLOCKED:
    			next_round_robin();
    			break;
			case FINISHED:
				dequeue_round_robin();
    			break;
    	}
  	} while(process_state == BLOCKED || process_state == FINISHED);
	return get_rsp(current_process);
}

int get_current_pid()
{
	ProcessContextBlock current = (ProcessContextBlock) peek_round_robin();
	if (current == NULL)
	{
		return -1;
	}
	return current->id;
}
