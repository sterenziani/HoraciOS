#include <mutex.h>
#include <process.h>
#define MAX_SEMS (MAX_MUTEXES/2)

typedef struct {
	int value;
	process_id_t lockedQueue[PROCESS_QUEUE_SIZE];
	int queue_start_index;
	int queue_free_index;
	mutex_t mutex;
}sem_struct_t;

typedef sem_struct_t * sem_t;

typedef struct{
  sem_t sems[MAX_SEMS];
  int free_index;
}sem_directory_t;

void init_sem_directory();
sem_t sem_create(char* name, int startValue);
void sem_wait(sem_t sem);
void sem_signal(sem_t sem);
void sem_destroy(sem_t sem);
void sem_wait_by_name(char* name);
void sem_signal_by_name(char* name);
void sem_destroy_by_name(char* name);
