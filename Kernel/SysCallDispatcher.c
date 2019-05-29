#include <lib.h>
#include <time.h>
#include <read.h>
#include <write.h>
#include <naiveConsole.h>
#include <speaker.h>
#include <stdint.h>
#include <font.h>
#include <SysCallDispatcher.h>
#include <memoryManager.h>
#include <mailbox.h>
#include <mutex.h>
#include <sem.h>
#include <process.h>
#include <binary_info.h>

static void info_handler(int p1, int* p2);

/*
	1 =	PAINT PIXEL					p1 = x
													p2 = y
													p3 = color

	2 = GET CLOCK						Saves (H:M:S) in (p1:p2:p3) pointers

	3 = READ FROM KEYBOARD	p1 				Where to save next char in buffer

	4 = PRINT ON SCREEN			p1 = 0		Print message
													p1 = 1		Print error
													p1 = 2		New line
													p2 				String to print
													p3				How many chars to print

	5 = USE SPEAKER

	6 = CURSOR							p1 = x
													p2 = y

	7 = INFO								p1 = 0		SCREEN_WIDTH
													p1 = 1		SCREEN_HEIGHT
													p1 = 2		CHAR_WIDTH
													p1 = 3		CHAR_HEIGHT
													p1 = 4		CURSOR_X
													p1 = 5		CURSOR_Y
													p2				where to save result

	8 = BIG TEXT						p1 = 0		New line
													p1 = 1		White color
													p1 = 2		Red color
													p1 = 3		Blue color
													p1 = 4		Green color
													p1 = 5		Yellow color
													p2 				String to print
													p3				How many chars to print

	9 = MALLOC							p1 = Amount of bytes you need
													p2 = Pointer to store the start address of created memory

	10 = FREE_MEM						p1 = Page where process is allocated

	11 = PRINT ADDRESS			p1 = Hex value to print

	12 = CREATE PROCESS			p1 = (void*) entry point
													p2 = (void**) Args
													p3 = process_id_t
													p4 = (int*) return value

	13 = KILL PROCESS 			p1 = pid to kill

	87 = PRINT BITMAP

	88 = CREATE SEM 				p1 = Sem name
													p2 = int pointer containing initial value
													p3 = pointer to save sem

	89 = DESTROY SEM				p1 = Sem

	90 = SEM WAIT						p1 = pointer to sem

	91 = SEM SIGNAL					p1 = pointer to sem

	92 = CREATE MUTEX				p1 = Mutex name
													p2 = pointer to save mutex

	93 = DESTROY MUTEX			p1 = Mutex

	94 = MUTEX LOCK					p1 = Mutex

	95 = MUTEX UNLOCK				p1 = Mutex

	96 = CREATE MAILBOX			p1 = Mailbox name
													p2 = pointer to save mailbox

	97 = DESTROY_MAILBOX		p1 = Mailbox

	98 = WRITE TO MAILBOX		p1 = Mailbox
													p2 = Message
													p3 = int pointer for return value

	98 = READ MAILBOX				p1 = Mailbox
													p2 = Amount of bytes to read
													p3 = Buffer to copy read content to
													p4 = int pointer for return value
*/

int SysCallDispatcher(uint64_t id, uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t p5)
{
	switch (id)
	{
		case PAINT:
								putpixel((unsigned int)p1, (unsigned int)p2, (int)p3);
								break;
		case CLOCK:
								clock_handler((int*) p1, (int*) p2, (int*) p3);
								break;
		case READ:
								read_buffer_handler((char*) p1);
								break;
		case WRITE:
								write_handler(p1, (char*) p2, (int) p3);
								break;
		case SPEAKER:
								speaker_handler();
								break;
		case CURSOR:
								setCursor((unsigned int)p1, (unsigned int)p2);
								break;
		case INFO:
								info_handler((int) p1, (int*) p2);
								break;
		case WRITE_BIG:
								big_handler(p1, (char*)p2, (int)p3);
								break;
		case CREATE_PROCESS:
            					return create_process((void **) p2, (void *) p1, (header_t *) p3);
		case KILL_PROCESS:
								mark_process_as_finished((process_id_t) p1);
								break;
		case GET_ALL_PROCESSES:
								get_ready_processes((binary_info_t *) p1, (int) p2, (int*) p3);
								break;
		case BE_NICE:
								change_priority((process_id_t) p1, (int) p2);
								break;
		case MALLOC:
								malloc_handler(p1, (void**)p2);
								break;
		case FREE_MEM:
								free_handler((void*)p1);
								break;
		case WRITE_HEX:
								printHex(p1);
								break;
		case PRINT_BITMAP:
								print_bitmap();
								break;
		case SEM_CREATE:
								*((sem_t*)p3) = sem_create((char*) p1, (int)p2);
								break;
		case SEM_DESTROY:
								sem_destroy((sem_t)p1);
								break;
		case SEM_WAIT:
								sem_wait((sem_t) p1);
								break;
		case SEM_SIGNAL:
								sem_signal((sem_t) p1);
								break;
		case MUTEX_CREATE:
								*((mutex_t*)p2) = mutex_create((char*) p1);
								break;
		case MUTEX_DESTROY:
								mutex_destroy((mutex_t)p1);
								break;
		case MUTEX_LOCK:
								mutex_lock((mutex_t)p1);
								break;
		case MUTEX_UNLOCK:
								mutex_unlock((mutex_t)p1);
								break;
		case CREATE_MAILBOX:
								*((mailbox_t*)p2) = create_mailbox((char*) p1);
								break;
		case DESTROY_MAILBOX:
								destroy_mailbox((mailbox_t)p1);
								break;
		case MAIL_WRITE:
								*((int*)p3) = write_message((mailbox_t) p1, (char*) p2);
								break;
		case MAIL_READ:
								*((int*)p4) = read_message((mailbox_t) p1, (int) p2, (char*) p3);
								break;
	}
	return 0xC4F3;
}

static void info_handler(int p1, int* p2)
{
	switch(p1)
	{
		case 0:	*p2 = getScreenWidth();
						break;
		case 1:	*p2 = getScreenHeight();
						break;
		case 2: *p2 = getCharWidth();
						break;
		case 3:	*p2 = getCharHeight();
						break;
		case 4:	*p2 = getCursorX();
						break;
		case 5: *p2 = getCursorY();
						break;
	}
}
