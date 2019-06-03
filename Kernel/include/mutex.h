#ifndef MUTEX_H
#define MUTEX_H
#include <lib.h>
#include <process.h>

#define MAX_MUTEXES 50

typedef struct{
  char name[NAME_LENGTH + 1];
	int value;
	process_id_t owner;
	process_id_t lockedQueue[PROCESS_QUEUE_SIZE];
  int queue_start_index;
	int queue_free_index;
}mutex_struct_t;

typedef mutex_struct_t* mutex_t;

typedef struct{
  mutex_t mutexes[MAX_MUTEXES];
  int free_index;
}mutex_directory_t;

void init_mutex_directory();                                // Crea el mutex directory
mutex_t mutex_create(char* name);                           // Crea un mutex con ese nombre
void mutex_lock(mutex_t mutex);
void mutex_unlock(mutex_t mutex);
void mutex_lock_by_name(char* name);
void mutex_unlock_by_name(char* name);
void mutex_destroy(mutex_t mutex);
void mutex_destroy_by_name(char* name);

#endif
