#ifndef _BINARY_INFO_H
#define _BINARY_INFO_H

#define PROCESS_NAME_LENGTH 256
#define MAX_PROCESS_COUNT 50

typedef enum {
	FOREGROUND = 0,
	BACKGROUND = 1
} process_type_t;

typedef enum {
	READY = 0,
	BLOCKED,
	FINISHED
} process_state_t;

typedef struct
{
	char name[PROCESS_NAME_LENGTH];
	int id;
	process_state_t state;
	process_type_t type;
	int priority;
} binary_info_t;

#endif
