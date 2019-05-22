#include <ipc.h>
#include <stdlib.h>
#include <SysCallDispatcher.h>

mailbox_t create_mailbox(char* name)
{
  mailbox_t mailbox;
  SysCallDispatcher(96, (uint64_t)name, (uint64_t)&mailbox, 0, 0, 0, 0);
  return mailbox;
}

int write_message(mailbox_t mailbox, char* msg)
{
  int resp;
  resp = ERROR;
  SysCallDispatcher(98, (uint64_t)mailbox, (uint64_t)msg, (uint64_t)&resp, 0, 0, 0);
  return resp;
}

int read_message(mailbox_t mailbox, int bytes, char* dest)
{
  int resp;
  resp = ERROR;
  SysCallDispatcher(99, (uint64_t)mailbox, (uint64_t)bytes, (uint64_t)dest, (uint64_t)&resp, 0, 0);
  return resp;
}

void destroy_mailbox(mailbox_t mailbox)
{
  SysCallDispatcher(97, (uint64_t)mailbox, 0, 0, 0, 0, 0);
}

mutex_t mutex_create(char* name)
{
  mutex_t mutex;
  SysCallDispatcher(92, (uint64_t)name, (uint64_t)&mutex, 0, 0, 0, 0);
  return mutex;
}

void mutex_lock(mutex_t mutex)
{
  SysCallDispatcher(94, (uint64_t)mutex, 0, 0, 0, 0, 0);
}

void mutex_unlock(mutex_t mutex)
{
  SysCallDispatcher(95, (uint64_t)mutex, 0, 0, 0, 0, 0);
}

void mutex_destroy(mutex_t mutex)
{
  SysCallDispatcher(93, (uint64_t)mutex, 0, 0, 0, 0, 0);
}

sem_t sem_create(char* name, int initial_value)
{
  sem_t sem;
  int value = initial_value;
  SysCallDispatcher(88, (uint64_t)name, (uint64_t)value, (uint64_t)&sem, 0, 0, 0);
  return sem;
}

void sem_wait(sem_t sem)
{
  SysCallDispatcher(90, (uint64_t)sem, 0, 0, 0, 0, 0);
}

void sem_signal(sem_t sem)
{
  SysCallDispatcher(91, (uint64_t)sem, 0, 0, 0, 0, 0);
}

void sem_destroy(sem_t sem)
{
  SysCallDispatcher(89, (uint64_t)sem, 0, 0, 0, 0, 0);
}
