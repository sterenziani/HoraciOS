#include <read.h>
#include <keyboard.h>
#include <naiveConsole.h>
#include <process.h>
#include <scheduler.h>

void read_buffer_handler(char* dest)
{
	mailbox_t input;
	if ((input = get_input(get_current_pid())) == NULL)
	{
		*dest = getFirstInBuffer();
	} else {
		read_message(input, 1, dest);
	}
}
