#define BUFFER_SIZE 257

#define LSHIFT_PRESS 42
#define RSHIFT_PRESS 54
#define LSHIFT_RELEASE 0xFFFFFFAA
#define RSHIFT_RELEASE 0xFFFFFFB6

#define CTRL_PRESS 29
#define ALT_PRESS 0
#define CTRL_RELEASE 0xFFFFFF9D
#define ALT_RELEASE 0xFFFFFFB8

#define UP_PRESS 72
#define RIGHT_PRESS 77
#define LEFT_PRESS 75
#define DOWN_PRESS 80
#define UP_RELEASE 0xFFFFFFC8
#define RIGHT_RELEASE 0xFFFFFFCD
#define LEFT_RELEASE 0xFFFFFFD0
#define DOWN_RELEASE 0xFFFFFFCB

#define ESC 1
#define BACKSPACE 14
#define TAB 15
#define ENTER 28
#define CAPSLOCK 58
#define F1 59
#define F2 60
#define F3 61
#define F4 62
#define F5 63
#define F6 64
#define F7 65
#define F8 66
#define F9 67
#define F10 68
#define F11 87
#define F12 88

#define INSERT 82
#define SUPR 83
#define INICIO 71
#define FIN 79
#define REPAG 73
#define AVPAG 81

#define NUM_ROW_START 2
#define NUM_ROW_END 13
#define Q_ROW_START 16
#define Q_ROW_END 27
#define A_ROW_START 30
#define A_ROW_END 40
#define Z_ROW_START 43
#define Z_ROW_END 53
#define ASTERISK 55
#define SPACEBAR 57
#define MINUS 74
#define PLUS 78
#define GREATER 86

void keypress_handler();
void refreshScreen();
char getFirstInBuffer();
void clearBuffer();
void shiftBufferLeft();
char getChar();
