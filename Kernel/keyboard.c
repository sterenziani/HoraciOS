#include <keyboard.h>
#include <naiveConsole.h>
#include <lib.h>

static char shift = 0;
char buffer[BUFFER_SIZE] = {0};
static int buffer_index = 0;
static int isCharacter(int key);
static void bufferCursorLeft();

static unsigned char unshifted_keys[] = {
0, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE,
TAB, 'q', 'w','e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', ENTER, 0,
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '~', '{', 0, 0, '}',
'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0, '*', 0, ' ', CAPSLOCK,
F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 0, 0,
INICIO, UP_PRESS, REPAG,'-', LEFT_PRESS, '5', RIGHT_PRESS, '+', FIN, DOWN_PRESS, AVPAG, INSERT, SUPR, 0, 0, '<', F11, F12};

static unsigned char shifted_keys[] = {
0, ESC, '!', '\"', '#', '$', '%', '&', '/', '(', ')', '=', '?', ' ', BACKSPACE,
9, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', ENTER, 0,
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\\', '[', ' ', 0, ']',
'Z', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0, '*', 0, ' ', CAPSLOCK,
F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 0, 0,
INICIO, UP_PRESS, REPAG, '-', LEFT_PRESS, 0, RIGHT_PRESS, '+', FIN, DOWN_PRESS, AVPAG, INSERT, SUPR, 0, 0, '>', F11, F12};

//moves all elements starting from index to the right
static void shiftBufferRight(int index)
{
	if(index == BUFFER_SIZE)
		return;
	shiftBufferRight(index+1);
	buffer[index] = buffer[index-1];
	return;
}

void shiftBufferLeft()
{
	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = buffer[i+1];
	buffer[BUFFER_SIZE-1] = 0;
	buffer_index = 0;
}

static void addtoBuffer(char character)
{
	//if we're going outside the buffer, delete the start of the buffer to make space
	// buffer[BUFFER_SIZE-1] should always be 0 because we want it to be a string
	if(buffer[BUFFER_SIZE-2] != 0)
	{
		shiftBufferLeft();
		bufferCursorLeft();
	}
	shiftBufferRight(buffer_index);
	buffer[buffer_index] = character;
	buffer_index++;
}

char getFirstInBuffer()
{
	char c = buffer[0];
	shiftBufferLeft();
	return c;
}

void clearBuffer()
{
	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;
	buffer_index = 0;
}

void keypress_handler()
{
	unsigned int key = readKey();
	if (key == LSHIFT_PRESS || key == RSHIFT_PRESS)
		shift = 1;
	else if(key == CAPSLOCK)
		shift = !shift;
	else if (key == LSHIFT_RELEASE || key == RSHIFT_RELEASE)
		shift = 0;
	else if (isCharacter(key))
		{
			switch(shift)
			{
				case 0:	addtoBuffer(unshifted_keys[key]);
								break;
				case 1:	addtoBuffer(shifted_keys[key]);
								break;
			}

		}
		else
		{
			switch(key)
			{
				case ENTER:				addtoBuffer('\n');
													break;
			}
	}
}

static void bufferCursorLeft()
{
	if (buffer_index > 0)
		buffer_index--;
}

//checks if key can be printed
static int isCharacter(int key)
{
	return ((key >= NUM_ROW_START && key <= NUM_ROW_END) || (key >= Q_ROW_START && key <= Q_ROW_END) || (key >= A_ROW_START && key <= A_ROW_END)
						|| (key >= Z_ROW_START && key <= Z_ROW_END)	|| (key == ASTERISK) ||(key == SPACEBAR) || (key == MINUS) || (key == PLUS)
						|| (key == GREATER) || (key == BACKSPACE));
}
