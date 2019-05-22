#include <defs.h>
#include <non_sizable_stack.h>

void *peek(NonSizableStack stack){
	if (stack->position > 0){
		return stack->stack[stack->position - 1];
	}
	return NULL;
}

int push(NonSizableStack stack, void *to_push){
	if (stack->position >= SIZE){
		return 0;
	}
	stack->stack[stack->position] = to_push;
	(stack->position)++;
	return 1;
}

void *pop(NonSizableStack stack){
	if (stack->position == 0){
		return NULL;
	}
	(stack->position)--;
	return stack->stack[stack->position];
}

int is_empty(NonSizableStack stack){
	return stack->position == 0;
}

int count(NonSizableStack stack){
	return stack->position;
}
