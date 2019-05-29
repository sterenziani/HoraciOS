#include <tests.h>
#include <memory.h>
#include <stdlib.h>
#include <ipc.h>
#include <time.h>
#include <taskmanager.h>
#include <myScanf.h>

#define SLEEP_TIME 4
#define SCREAMS 5000

void vowel_reader_test()
{
  int c;
  while((c=myGetChar()) != '\n')
  {
    if(c == 'a' || c == 'A' || c == 'e' || c == 'E' || c == 'i' || c == 'I' || c == 'o' || c == 'O' || c == 'u' || c == 'U' || c == ' ')
      myPutchar(c);
    else if(c == 0)
      ;
    else
      myPutchar(' ');
  }
  myPutchar('\n');
}

void tell_poem_test()
{
  myPrintf("There was an old man from Peru who dreamed he was eating his shoe. When he woke in a fright in the dark of the night he found it was perfectly true\n");
}

void increment_variable_with_mutex(void** args)
{
  mutex_t var_mutex = args[0];
  mutex_t sync_mutex = args[1];
  unsigned int* p = (unsigned int*)args[2];
  if(sync_mutex != NULL)
    mutex_lock(sync_mutex);
  for (int i = 0; i < 100000; i++)
  {
    mutex_lock(var_mutex);
    *p += 1;
    mutex_unlock(var_mutex);
  }
  if(sync_mutex != NULL)
  {
    mutex_unlock(sync_mutex);
    myPrintf("Process A finished!\n");
  }
}

void mutex_test()
{
  mutex_t mutex1 = mutex_create("mutex_test_1");
  mutex_t mutex2 = mutex_create("mutex_test_2");
  if(mutex1 == NULL || mutex2 == NULL)
  {
    myPrintf("Not enough memory to run this!");
    return;
  }
  unsigned int p = 0;
  myPrintf("We will try to run two processes, each of which will increment the same variable 100000 times.\nWe expect the result to be 200000 and see the processes alternate in increasing the variable.\n\n");
  void* argsB[3] = {mutex1, mutex2, (void*)&p};
  void* argsA[3] = {mutex1, NULL, (void*)&p};
  int pid;

  // Let's go!
  run("increment variable with mutex", increment_variable_with_mutex, argsA, &pid, 1, 0, NULL, NULL);
  increment_variable_with_mutex(argsB);
  myPrintf("Process B finished!\n");
  mutex_lock(mutex2);
  myPrintf("Variable was incremented 100000 times by each process and its value is now %d!\n\n", p);
  mutex_unlock(mutex2);

  // Let's leave
  mutex_destroy(mutex1);
  mutex_destroy(mutex2);
  return;
}

void writer(void** args)
{
  mailbox_t mailbox = args[0];
  sleep(SLEEP_TIME*3);
  write_message(mailbox, "This is a message"); // 17 bytes
  sleep(SLEEP_TIME);
  write_message(mailbox, "A message for the Earthlings"); // 28 bytes
  sleep(SLEEP_TIME);
  write_message(mailbox, "Are you reading this?"); // 21 bytes
  sleep(SLEEP_TIME);
  write_message(mailbox, "Cool, hear us out"); // 17 bytes
  sleep(SLEEP_TIME);
  write_message(mailbox, "Us aliens have something to say..."); // 34 bytes
  sleep(SLEEP_TIME);
  write_message(mailbox, "Hello World :) "); // 14 bytes
}

void mailbox_test()
{
  mailbox_t mailbox = create_mailbox("mailbox_test");
  if(mailbox == NULL)
  {
    myPrintf("Not enough memory to run this!");
    return;
  }
  myPrintf("We're awaiting messages coming from aliens.\nWe expect our reader process to block itself until there is something to read\nLet's see if we can establish communication...\n\n");
  int pid;
  void* args[1] = {mailbox};
  run("writer", writer, args, &pid, 1, 0, NULL, NULL);

  char cadena[500];
  read_message(mailbox, 17, cadena);
  myPrintf("   %s\n", cadena);
  read_message(mailbox, 28, cadena);
  myPrintf("   %s\n", cadena);
  read_message(mailbox, 21, cadena);
  myPrintf("   %s\n", cadena);
  read_message(mailbox, 17, cadena);
  myPrintf("   %s\n", cadena);
  read_message(mailbox, 34, cadena);
  myPrintf("   %s\n", cadena);
  read_message(mailbox, 14, cadena);
  myPrintf("   %s\n", cadena);
  sleep(SLEEP_TIME/2);
  myPrintf("\n---End of communication---\n");
  destroy_mailbox(mailbox);
  return;
}

void screamA()
{
  mutex_t mutex1 = mutex_create("sch_mutex1");
  mutex_lock(mutex1);
  for (int i = 0; i < 5000; i++)
  {
    myPrintf("A");
  }
  mutex_unlock(mutex1);
}

void screamB()
{
  mutex_t mutex2 = mutex_create("sch_mutex2");
  mutex_lock(mutex2);
  for (int i = 0; i < 5000; i++)
  {
    myPrintf("B");
  }
  mutex_unlock(mutex2);
}

void screamC()
{
  mutex_t mutex3 = mutex_create("sch_mutex3");
  mutex_lock(mutex3);
  for (int i = 0; i < 5000; i++)
  {
    myPrintf("C");
  }
  mutex_unlock(mutex3);
}

void scheduler_test()
{
  mutex_t mutex1 = mutex_create("sch_mutex1");
  mutex_t mutex2 = mutex_create("sch_mutex2");
  mutex_t mutex3 = mutex_create("sch_mutex3");
  run("screamA", &screamA, NULL, NULL, 1, 2, NULL, NULL);
  run("screamB", &screamB, NULL, NULL, 1, 0, NULL, NULL);
  run("screamC", &screamC, NULL, NULL, 1, 5, NULL, NULL);
  sleep(5);
  mutex_lock(mutex1);
  mutex_lock(mutex2);
  mutex_lock(mutex3);
  myPrintf("\nFin\n\n");
  mutex_unlock(mutex1);
  mutex_unlock(mutex2);
  mutex_unlock(mutex3);
  mutex_destroy(mutex1);
  mutex_destroy(mutex2);
  mutex_destroy(mutex3);
}

void sleep_for_10_secs()
{
  sleep(10);
}
