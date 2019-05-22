#include <stdint.h>
#define PAINT 1
#define CLOCK 2
#define READ 3
#define WRITE 4
#define SPEAKER 5
#define CURSOR 6
#define INFO 7
#define WRITE_BIG 8
#define MALLOC 9
#define FREE_MEM 10
#define WRITE_HEX 11
#define CREATE_PROCESS 12
#define KILL_PROCESS 13
#define GET_ALL_PROCESSES 14

#define PRINT_BITMAP 87
#define SEM_CREATE 88
#define SEM_DESTROY 89
#define SEM_WAIT 90
#define SEM_SIGNAL 91

#define MUTEX_CREATE 92
#define MUTEX_DESTROY 93
#define MUTEX_LOCK 94
#define MUTEX_UNLOCK 95

#define CREATE_MAILBOX 96
#define DESTROY_MAILBOX 97
#define MAIL_WRITE 98
#define MAIL_READ 99

void SysCallDispatcher(uint64_t id, uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t p5, uint64_t p6);
