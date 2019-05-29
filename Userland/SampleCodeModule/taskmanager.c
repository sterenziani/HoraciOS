#include <taskmanager.h>
#include <SysCallDispatcher.h>
#include <stdlib.h>
#include <binary_info.h>

typedef struct {
  char *name;
  process_type_t type;
  int priority;
  mailbox_t input;
  mailbox_t output;
} header_t;

static header_t new_header(char *name, int process_type, int priority, mailbox_t input, mailbox_t output){
  header_t header;
  header.name = name;
  header.type = process_type;
  header.priority = priority;
  header.input = input;
  header.output = output;
  return header;
}

int run(char *name, void *entry_point, void ** argv, int* pid, int process_type, int priority, mailbox_t input, mailbox_t output){
  int id;
  header_t process_header = new_header(name, process_type, priority, input, output);
  id = SysCallDispatcher(12, (uint64_t) entry_point, (uint64_t) argv, (uint64_t) &process_header, 0, 0);
  if (pid != NULL){
    *pid = id;
  }
  if (process_type == 1) {
    myPrintf("[%d]\n", id);
  }
  return id;
}

void nice(process_id_t pid, int new_priority){
  SysCallDispatcher(15, (uint64_t) pid, (uint64_t) new_priority, 0, 0, 0);
}

void kill(process_id_t pid)
{
  SysCallDispatcher(13, (uint64_t)pid, 0, 0, 0, 0);
}

#define PS_STRING "%s    "
#define PS_INT "%d    "

void list_all_processes()
{
  int process_count;
  char *type, *state;
  binary_info_t ready_processes[MAX_PROCESS_COUNT];
  SysCallDispatcher(14, (uint64_t) ready_processes, (uint64_t) MAX_PROCESS_COUNT, (uint64_t) &process_count, 0, 0);
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
