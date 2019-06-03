#include <mutex.h>
#include <lib.h>
#include <strings.h>
#include <memoryManager.h>
#include <naiveConsole.h>
#include <scheduler.h>

static mutex_directory_t mutex_directory;

void init_mutex_directory()
{
  mutex_directory.free_index = 0;
  for(int i = 0; i < MAX_MUTEXES; i++)
    mutex_directory.mutexes[i] = NULL;
}

int get_mutex_index(char* mutex)
{
  for (int i = 0; i < MAX_MUTEXES; i++)
  {
    if(mutex_directory.mutexes[i] != NULL && strcmp(mutex_directory.mutexes[i]->name, mutex) == 0)
      return i;
  }
  return -1;
}

mutex_t mutex_create(char* name)
{
  // Check if already exists
  int index = get_mutex_index(name);
  if(index != -1)
    return mutex_directory.mutexes[index];

  // Let's create one if allowed to
  if(mutex_directory.free_index == -1)
    return NULL;
  mutex_t m = malloc(sizeof(mutex_struct_t));
  if(m == NULL)
    return NULL;

  int length = strlen(name);
  if(length > NAME_LENGTH)
    length = NAME_LENGTH;
  for (int i = 0; i < length; i++)
    m->name[i] = name[i];
  m->name[length] = 0;

  m->value = 0;                                     // El mutex se inicializa en 0
  m->owner = -1;                                    // Aun no tiene dueño, asi que el PID del dueño es -1
  m->queue_start_index = 0;
  m->queue_free_index = 0;
  for (int i = 0; i < PROCESS_QUEUE_SIZE; i++)      // Aun no hay procesos en espera
    m->lockedQueue[i] = -1;
  mutex_directory.mutexes[mutex_directory.free_index] = m;

  // Actualizo el free_index
  int newly_created_index = mutex_directory.free_index;
  mutex_directory.free_index++;
  if(mutex_directory.free_index == MAX_MUTEXES)
    mutex_directory.free_index = 0;
  while(mutex_directory.mutexes[mutex_directory.free_index] != NULL && mutex_directory.free_index != newly_created_index)
  {
      mutex_directory.free_index++;
      if(mutex_directory.free_index >= MAX_MUTEXES)
        mutex_directory.free_index -= MAX_MUTEXES;
  }
  if(mutex_directory.free_index == newly_created_index)
    mutex_directory.free_index = -1;
  return m;
}

void mutex_lock(mutex_t mutex)
{
  if(mutex == NULL)
  {
    return;
  }
  // Si soy el dueño, ya tengo el lock
  if (mutex->owner == get_current_pid())
  {
    return;
  }
  // Si el mutex paso de 0 a 1, ahora soy su dueño
  if (mutex_acquire(&(mutex->value)) == 0)
  {
      mutex->owner = get_current_pid();
  }
  else
  {
    // Si el mutex ya estaba en 1, me meto en la cola y le pido al Scheduler que me bloquee
    process_id_t process = get_current_pid();
    if(mutex->lockedQueue[mutex->queue_free_index] != -1){
      print("ERROR! Too many processes waiting for mutex ");
      print(mutex->name);
      newLine();
    }
    mutex->lockedQueue[mutex->queue_free_index] = process;
    mutex->queue_free_index++;
    if(mutex->queue_free_index == PROCESS_QUEUE_SIZE)
      mutex->queue_free_index = 0;
    mark_process_as_blocked(process);
  }
}

void mutex_lock_by_name(char* name)
{
  int index = get_mutex_index(name);
  if(index == -1)
    return;
  mutex_t mutex = mutex_directory.mutexes[index];
  mutex_lock(mutex);
}

void mutex_unlock(mutex_t mutex)
{
  if(mutex == NULL)
    return;
  // Si no soy el dueño, no tengo derecho a desbloquear el mutex
  if (mutex->owner != get_current_pid())
  {
    return;
  }

  // Saco al primero de la cola, lo despierto y lo hago dueño
  if(mutex->lockedQueue[mutex->queue_start_index] != -1)
  {
    process_id_t process = mutex->lockedQueue[mutex->queue_start_index];
    mutex->lockedQueue[mutex->queue_start_index] = -1;
    mutex->queue_start_index++;
    if(mutex->queue_start_index == PROCESS_QUEUE_SIZE)
      mutex->queue_start_index = 0;
    mutex->owner = process;
    resume_process(process);
    return;
  }
  mutex->owner = -1;
  mutex->value = 0;
}

void mutex_unlock_by_name(char* name)
{
  int index = get_mutex_index(name);
  if(index == -1)
    return;
  mutex_t mutex = mutex_directory.mutexes[index];
  mutex_unlock(mutex);
}

void mutex_destroy(mutex_t mutex)
{
  mutex_destroy_by_name(mutex->name);
}

void mutex_destroy_by_name(char* name)
{
  int index = get_mutex_index(name);
  if(index == -1)
    return;
  mutex_t mutex = mutex_directory.mutexes[index];
  free_handler(mutex);
  mutex_directory.mutexes[index] = NULL;
  mutex_directory.free_index = index;
}
