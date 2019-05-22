#include <sem.h>
#include <lib.h>
#include <strings.h>
#include <memoryManager.h>
#include <naiveConsole.h>
#include <scheduler.h>

static sem_directory_t sem_directory;

void init_sem_directory()
{
  sem_directory.free_index = 0;
  for(int i = 0; i < MAX_SEMS; i++)
    sem_directory.sems[i] = NULL;
}

int get_sem_index(char* sem)
{
  for (int i = 0; i < MAX_SEMS; i++)
  {
    if(sem_directory.sems[i] != NULL && strcmp(sem_directory.sems[i]->mutex->name, sem) == 0)
      return i;
  }
  return -1;
}

sem_t sem_create(char* name, int startValue)
{
  print("Creating ");
  print(name);
  print(" with value ");
  printDec(startValue);
  newLine();

  // Check if already exists
  int index = get_sem_index(name);
  if(index != -1)
    return sem_directory.sems[index];

  // Let's create one if allowed to
  if(sem_directory.free_index == -1)
    return NULL;
  sem_t sem = malloc(sizeof(sem_struct_t));
  if(sem == NULL)
    return NULL;

  sem->value = startValue;
  sem->queue_start_index = 0;
  sem->queue_free_index = 0;
  for (int i = 0; i < PROCESS_QUEUE_SIZE; i++)      // Aun no hay procesos en espera
    sem->lockedQueue[i] = -1;
  sem->mutex = mutex_create(name);
  sem_directory.sems[sem_directory.free_index] = sem;

  // Actualizo el free_index
  int newly_created_index = sem_directory.free_index;
  sem_directory.free_index++;
  if(sem_directory.free_index == MAX_SEMS)
    sem_directory.free_index = 0;
  while(sem_directory.sems[sem_directory.free_index] != NULL && sem_directory.free_index != newly_created_index)
  {
      sem_directory.free_index++;
      if(sem_directory.free_index >= MAX_SEMS)
        sem_directory.free_index -= MAX_SEMS;
  }
  if(sem_directory.free_index == newly_created_index)
    sem_directory.free_index = -1;
  return sem;
}

void sem_wait(sem_t sem)
{
  if(sem == NULL)
    return;

  mutex_lock(sem->mutex);
  if (sem->value == 0)
  {
    process_id_t process = get_current_pid();
    if(sem->lockedQueue[sem->queue_free_index] != -1)
      print("ERROR! Too many processes waiting for semaphore!");
    sem->lockedQueue[sem->queue_free_index] = process;
    sem->queue_free_index++;
    if(sem->queue_free_index == PROCESS_QUEUE_SIZE)
      sem->queue_free_index = 0;
    mutex_unlock(sem->mutex);
    mark_process_as_blocked(process);
  }
  else
  {
    sem->value--;
    mutex_unlock(sem->mutex);
  }
}

void sem_wait_by_name(char* name)
{
  int index = get_sem_index(name);
  if(index == -1)
    return;
  sem_t sem = sem_directory.sems[index];
  sem_wait(sem);
}

void sem_signal(sem_t sem)
{
  if(sem == NULL)
    return;
  mutex_lock(sem->mutex);
  if(sem->lockedQueue[sem->queue_start_index] != -1)
  {
      process_id_t process = sem->lockedQueue[sem->queue_start_index];
      sem->lockedQueue[sem->queue_start_index] = -1;
      sem->queue_start_index++;
      if(sem->queue_start_index == PROCESS_QUEUE_SIZE)
        sem->queue_start_index = 0;

      resume_process(process);
      mutex_unlock(sem->mutex);
      return;
  }
  sem->value++;
  mutex_unlock(sem->mutex);
}

void sem_signal_by_name(char* name)
{
  int index = get_sem_index(name);
  if(index == -1)
    return;
  sem_t sem = sem_directory.sems[index];
  sem_signal(sem);
}

void sem_destroy(sem_t sem)
{
  sem_destroy_by_name(sem->mutex->name);
}

void sem_destroy_by_name(char* name)
{
  int index = get_sem_index(name);
  if(index == -1)
    return;
  sem_t sem = sem_directory.sems[index];
  mutex_destroy(sem->mutex);
  free_handler(sem);
  sem_directory.sems[index] = NULL;
  sem_directory.free_index = index;
}
