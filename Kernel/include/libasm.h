#ifndef _LIBASM_H
#define _LIBASM_H

#include <process.h>

void *_initialize_stack(void **argv, int argc, void *stack, void *wrapper_process, void *entry_point, process_id_t pid);

void _force_irq00(void);

#endif