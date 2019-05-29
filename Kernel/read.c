#include <read.h>
#include <keyboard.h>
#include <naiveConsole.h>

void read_buffer_handler(char* dest)
{
		*dest = getFirstInBuffer();
}
