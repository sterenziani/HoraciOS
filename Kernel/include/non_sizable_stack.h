#ifndef _NON_SIZABLE_STACK_H
#define _NON_SIZABLE_STACK_H

#define SIZE 10

typedef struct {
	int one;
	void * stack[SIZE];
	int position;
} non_sizable_stack_t;

typedef non_sizable_stack_t *NonSizableStack;

void *peek(NonSizableStack stack);
int push(NonSizableStack stack, void *to_push);
void *pop(NonSizableStack stack);
int is_empty(NonSizableStack stack);
int count(NonSizableStack stack);

#endif