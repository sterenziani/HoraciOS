#include <taskmanager.h>
#include <SysCallDispatcher.h>
#include <stdlib.h>
#include <binary_info.h>

int run(char *name, void *entry_point, void ** argv, int* pid, int process_type, int priority){
  int id;
  SysCallDispatcher(12, (uint64_t) entry_point, (uint64_t) argv, (uint64_t) &id, (uint64_t) name, process_type, priority);
  if (pid != NULL){
    *pid = id;
  }
  if (process_type == 1) {
    myPrintf("[%d]\n", id);
  }
  return id;
}

void kill(process_id_t pid)
{
  SysCallDispatcher(13, pid, 0, 0, 0, 0, 0);
}

#define PS_STRING "%s    "
#define PS_INT "%d    "

void list_all_processes()
{
  int process_count;
  char *type, *state;
  binary_info_t ready_processes[MAX_PROCESS_COUNT];
  SysCallDispatcher(14, (uint64_t) ready_processes, (uint64_t) MAX_PROCESS_COUNT, (uint64_t) &process_count, 0, 0, 0);
  myPrintf("\n%d Processes Currently Running\n(type;state;id;priority;name)\n\n", process_count);
  for (int i = 0; i < process_count; ++i)
  {
    switch(ready_processes[i].state){
      case READY:
        state = "READY  ";
        break;
      case BLOCKED:
        state = "BLOCKED";
        break;
      default:
        state = "UNKNOWN";
        break;
    }
    switch(ready_processes[i].type){
      case FOREGROUND:
        type = "FOREGROUND";
        break;
      case BACKGROUND:
        type = "BACKGROUND";
        break;
      default:
        type = "UNKNOWN   ";
        break;
    }
    myPrintf(PS_STRING, type);
    myPrintf(PS_STRING, state);
    myPrintf(PS_INT, ready_processes[i].id);
    myPrintf(PS_INT, ready_processes[i].priority);
    myPrintf("%s\n", ready_processes[i].name);
  }
}
