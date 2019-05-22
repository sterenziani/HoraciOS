#include <defs.h>
#include <priority_round_robin.h>

typedef struct node {
	void *element;
	int quantum;
	struct node * next;
}node_t;

typedef node_t *Node;

#define MAX_NODES 10

typedef struct {
	node_t node_buffer[MAX_NODES];
	int ptr;
} node_buffer_t;

typedef struct {
	int current_quantum;
	Node current;
	Node last;
	int nodes;
	node_buffer_t buffer;
} round_robin_t;



static round_robin_t round_robin = {-1, NULL, NULL, 0, {{{0, 1, 0}}, -1}};

static int calculate_quantum(int priority){
	return priority+1;
}


int amount_in_round_robin(){
	return round_robin.nodes;
}


void queue_round_robin(void* element, int priority){
	if (amount_in_round_robin() >= MAX_NODES)
	{
		return;
	}
	(round_robin.nodes)++;
	if (round_robin.current == NULL){
		round_robin.current_quantum = 0;
		round_robin.buffer.ptr = 0;
		round_robin.buffer.node_buffer[round_robin.buffer.ptr].element = element;
		round_robin.buffer.node_buffer[round_robin.buffer.ptr].quantum = calculate_quantum(priority);
		round_robin.buffer.node_buffer[round_robin.buffer.ptr].next = &round_robin.buffer.node_buffer[round_robin.buffer.ptr];
		round_robin.current = &round_robin.buffer.node_buffer[round_robin.buffer.ptr];
		round_robin.last = round_robin.current;
		return;
	}
	while(round_robin.buffer.node_buffer[round_robin.buffer.ptr].element != NULL){
		round_robin.buffer.ptr++;
	}
	round_robin.buffer.node_buffer[round_robin.buffer.ptr].element = element;
	round_robin.buffer.node_buffer[round_robin.buffer.ptr].quantum = calculate_quantum(priority);
	round_robin.buffer.node_buffer[round_robin.buffer.ptr].next = round_robin.current;
	round_robin.last->next = &round_robin.buffer.node_buffer[round_robin.buffer.ptr];
}

void dequeue_round_robin(){
	if (round_robin.current == NULL){
		return;
	} else if(round_robin.current == round_robin.last){
		round_robin.current = NULL;
		round_robin.last = NULL;
		(round_robin.nodes)--;
		return;
	}
	round_robin.last->next = round_robin.current->next;
	round_robin.current = round_robin.current->next;
	(round_robin.nodes)--;
}



void* peek_round_robin(){
	if (round_robin.current == NULL)
	{
		return round_robin.current;
	}
	return round_robin.current->element;
}

void next_round_robin(){
	if (round_robin.current == NULL){
		return;
	}
	round_robin.current_quantum++;
	if (round_robin.current_quantum >= round_robin.current->quantum)
	{
		//printf("Yay\n");
		round_robin.current_quantum = 0;
		round_robin.last = round_robin.current;
		round_robin.current = round_robin.current->next;
	}
}
