#include <naiveConsole.h>
#include <font.h>
#include <process.h>
#include <scheduler.h>
#define BIG_FONT_SIZE 15

enum FD1{STDOUT=0, STDERR, NEWLINE};
enum FD2{NEWBIG=0, WHITE, RED, BLUE, GREEN, YELLOW};

void write_handler(uint64_t fd, char * buffer, int strlen)
{
	char cadena[strlen+1];
	int i=0;
	for (i = 0; i < strlen; ++i)
		cadena[i] = buffer[i];
	cadena[i]= 0;
	mailbox_t output;
	if ((output = get_output(get_current_pid())) == NULL){
		switch(fd){
			case STDOUT:	print(cadena);
							break;
			case STDERR:  	printFormat(cadena, RED_COLOR, BACKGROUND_COLOR);
							break;
			case NEWLINE:	newLine();
							break;
		}
	} else {
		write_message(output, cadena);
	}	
}

void big_handler(uint64_t fd, char* buffer, int strlen)
{
	char cadena[strlen+1];
	int i=0;
	for (i = 0; i < strlen; ++i)
		cadena[i] = buffer[i];
	cadena[i]= 0;
	switch(fd)
	{
		case NEWBIG:			newBigLine(BIG_FONT_SIZE);
											break;
		case WHITE:  			printBigFormat(cadena, WHITE_COLOR, BACKGROUND_COLOR, BIG_FONT_SIZE);
											break;
		case RED:  				printBigFormat(cadena, RED_COLOR, BACKGROUND_COLOR, BIG_FONT_SIZE);
											break;
		case BLUE:  			printBigFormat(cadena, BLUE_COLOR, BACKGROUND_COLOR, BIG_FONT_SIZE);
											break;
		case GREEN:  			printBigFormat(cadena, GREEN_COLOR, BACKGROUND_COLOR, BIG_FONT_SIZE);
											break;
		case YELLOW:  		printBigFormat(cadena, YELLOW_COLOR, BACKGROUND_COLOR, BIG_FONT_SIZE);
											break;
	}
}
