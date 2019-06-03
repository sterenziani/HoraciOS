#include <stdlib.h>
#include <time.h>
#include <exception.h>
#include <memory.h>
#include <ipc.h>
#include <tests.h>
#include <taskmanager.h>
#include <prodcons_sem.h>
#include <philosophers.h>

#define BACKSPACE 14
#define BUFFER_SIZE 255

#define HELP_COMMAND "help"
#define CLOCK_COMMAND "clock"
#define DIGITAL_CLOCK_COMMAND "digitalclock"
#define MALLOC "malloc"
#define FREE "free"
#define MEM_CHECK "memcheck"
#define MUTEX_TEST "mutex_test"
#define MAILBOX_TEST "mailbox_test"
#define PRODCONS_SEM "prodcons"
#define KILL "kill"
#define PS "ps"
#define SCHEDULER_TEST "scheduler_test"
#define DUMMY "dummy"
#define NICE "nice"
#define TELL_POEM "poem"
#define FILTER_VOWELS "filter_vowels"
#define PIPE_TEST "pipe_test"
#define PHILOSOPHERS "philosophers"

int pipe_counter;


void invalid_command(char * s) {
	myPrintf("%s: command not found or lacking arguments\n",s);
	return;
}

void no_argument()
{
	myPrintf("You have not inserted an argument for that command\n");
}

void help() {
	myPrintf("\n");
	myPrintf(" *  %s - Prints current time\n\n", CLOCK_COMMAND);
	myPrintf(" *  %s - Shows a real-time clock with customizable colors\n\n", DIGITAL_CLOCK_COMMAND);
	myPrintf(" *  %s [int] - Reserves [int] bytes in memory\n\n", MALLOC);
	myPrintf(" *  %s [int] - Frees the block in address [int] (WARNING: For use with addresses 'malloc' suggests only)\n\n", FREE);
	myPrintf(" *  %s - Prints the current status of the memory bitmap and the size of each memory block\n\n", MEM_CHECK);
	myPrintf(" *  %s - Runs a test that uses mutexes to increase a variable without causing a race condition\n\n", MUTEX_TEST);
	myPrintf(" *  %s - Runs a test to prove mailboxes can block its readers until enough content is written\n\n", MAILBOX_TEST);
	myPrintf(" *  %s - Runs a simulation based on the producer-consumer problem using semaphores\n\n", PRODCONS_SEM);
	myPrintf(" *  %s [int] - Kills the process of PID [int]\n\n", KILL);
	myPrintf(" *  %s - Lists all processes running currently\n\n", PS);
	myPrintf(" *  %s - Tests scheduler by running three processes that only print a letter repeatedly\n\n", SCHEDULER_TEST);
	myPrintf(" *  %s - Sleeps for 10 seconds. Perfect for ps testing!\n\n", DUMMY);
	myPrintf(" *  %s [pid] [int] - Changes [pid]'s priority to [int]\n\n", NICE);
	myPrintf(" *  %s - Reads you a poem\n\n", TELL_POEM);
	myPrintf(" *  %s - Replaces all consonants with an asterisk\n\n", FILTER_VOWELS);
	myPrintf(" *  %s - Tests pipes through a producer-consumer problem problem\n\n", PIPE_TEST);
	myPrintf(" *  %s - Runs a simulation of the Philosophers Problem\n\n", PHILOSOPHERS);
	myPutchar('\n');
}

void zeroDivision() {
	int i = 0;
	i = 10/i;
	return;
}

void standard_command_handler(char* s, int background_flag, mailbox_t input, mailbox_t output)
{
	if(strcmp(s, HELP_COMMAND) == 0)
	{
		run("help", help, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, CLOCK_COMMAND) == 0)
	{
		run("watch", watch, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, DIGITAL_CLOCK_COMMAND) == 0)
	{
		if(background_flag == 1)
			myPrintf("This process requires user input and can not run on background\n");
		else
			run("digitalclock", digitalClock, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, MEM_CHECK) == 0)
	{
		run("printMemoryBitmap", printMemoryBitmap, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, MAILBOX_TEST) == 0)
	{
		run("mailbox_test", mailbox_test, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, MUTEX_TEST) == 0)
	{
		run("mutex_test", mutex_test, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, PRODCONS_SEM) == 0)
	{
		if(background_flag == 1)
			myPrintf("This process requires user input and can not run on background\n");
		else
			prodcons_restaurant();
		return;
	}
	if(strcmp(s, PS) == 0)
	{
		run("ps", list_all_processes, NULL, NULL, background_flag, 0, input, output);
		//myPrintf("hola");
		return;
	}
	if(strcmp(s, SCHEDULER_TEST) == 0)
	{
		run("scheduler_test", scheduler_test, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, DUMMY) == 0)
	{
		run("dummy", sleep_for_10_secs, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, TELL_POEM) == 0)
	{
		run("poem", tell_poem_test, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, FILTER_VOWELS) == 0)
	{
		run("filter_vowels", vowel_reader_test, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, PIPE_TEST) == 0)
	{
		run("pipe_test", pipe_test, NULL, NULL, background_flag, 0, input, output);
		return;
	}
	if(strcmp(s, PHILOSOPHERS) == 0)
	{
		if(background_flag == 1)
			myPrintf("This process requires user input and can not run on background\n");
		else
			prepareTable();
		return;
	}
	invalid_command(s);
	return;
}

int is_output_command(char* command)
{
	if(strcmp(command, CLOCK_COMMAND) == 0)
		return 1;
	if(strcmp(command, HELP_COMMAND) == 0)
		return 1;
	if(strcmp(command, MEM_CHECK) == 0)
		return 1;
	if(strcmp(command, MUTEX_TEST) == 0)
		return 1;
	if(strcmp(command, MAILBOX_TEST) == 0)
		return 1;
	if(strcmp(command, PS) == 0)
		return 1;
	if(strcmp(command, SCHEDULER_TEST) == 0)
		return 1;
	if(strcmp(command, DUMMY) == 0)
		return 1;
	if(strcmp(command, TELL_POEM) == 0)
		return 1;
	return 0;
}

int is_input_command(char* command)
{
	if(strcmp(command, PRODCONS_SEM) == 0)
		return 1;
	if(strcmp(command, DIGITAL_CLOCK_COMMAND) == 0)
		return 1;
	if(strcmp(command, FILTER_VOWELS) == 0)
		return 1;
	return 0;
}

void pipe_command_handler(char* app, char* target)
{
	if(!is_output_command(app))
	{
		myPrintf("Left process is not allowed for use with pipes. Please replace it with a command that only outputs to the screen in shell");
		return;
	}
	if(!is_input_command(target))
	{
		myPrintf("Left process is not allowed for use with pipes. Please replace it with a command that only outputs to the screen in shell");
		return;
	}
	int number_length = 20;
	char pipe_number[number_length];
	char pipe_name[number_length+4];
	toString(pipe_counter, pipe_number);
	pipe_name[0] = 'p';
	pipe_name[1] = 'i';
	pipe_name[2] = 'p';
	pipe_name[3] = 'e';
	for (int i = 0; i < number_length; i++)
		pipe_name[4+i] = pipe_number[i];
	mailbox_t mailbox = create_mailbox(pipe_name);
	pipe_counter++;
	standard_command_handler(app, 1, NULL, mailbox);
	myPrintf("\n");
	standard_command_handler(target, 0, mailbox, NULL);
	myPrintf("Hola!\n");
	destroy_mailbox(mailbox);
	return;
}

void background_command_handler(char* s)
{
	standard_command_handler(s, 1, NULL, NULL);
}

void invalid_argument(char* command, char* target)
{
		myPrintf("%s is not a valid argument for command %s\n", target, command);
}

void argument_command_handler(char* command, char* target, char* arg)
{
	if(strcmp(command, MALLOC) == 0)
	{
		if(isNumber(target))
		{
			int bytes = myAtoi(target);
			user_malloc(bytes);
		}
		else
			invalid_argument(command, target);
		return;
	}
	if(strcmp(command, FREE) == 0)
	{
		if(isNumber(target))
		{
			int address = myAtoi(target);
			user_free(address);
		}
		else
			invalid_argument(command, target);
		return;
	}
	if(strcmp(command, KILL) == 0)
	{
		if(isNumber(target))
		{
			unsigned int pid = myAtoi(target);
			kill(pid);
		}
		else
			invalid_argument(command, target);
		return;
	}
	if(strcmp(command, NICE) == 0)
	{
		if(arg[0]!=0 && isNumber(target) && isNumber(arg))
		{
			unsigned int pid = myAtoi(target);
			unsigned int priority = myAtoi(arg);
			nice(pid, priority);
		}
		else
			no_argument();
		return;
	}
	invalid_command(command);
}

void interprete(char* string)
{
	char command[BUFFER_SIZE] = {0};
	char target[BUFFER_SIZE] = {0};
	char arg[BUFFER_SIZE] = {0};
	int j=0;
	while(string[j] != ' ' && string[j] != 0)
	{
		command[j] = string[j];
		j++;
	}
	if(string[j] == ' ')
	{
		j++;
		if(string[j] == '/' && string[j+1] == ' ')
		{
			j += 2;
			int k = 0;
			while(string[j] != 0)
			{
				target[k] = string[j];
				j++;
				k++;
			}
			pipe_command_handler(command, target);
		}
		else if(string[j] == '&' && string[j+1] == 0)
		{
			background_command_handler(command);
		}
		else
		{
			if(string[j] != 0)
			{
				int k = 0;
				while(string[j] != 0 && string[j] != ' ')
				{
					target[k] = string[j];
					j++;
					k++;
				}
				// Si hay algún segundo parametro, meterlo en arg
				if(string[j] == ' ')
				{
					k=0;
					j++;
					while (string[j] != 0)
					{
						arg[k] = string[j];
						j++;
						k++;
					}
				}
				argument_command_handler(command, target, arg);
			}
			else
				no_argument();
		}
	}
	else
		standard_command_handler(command, 0, NULL, NULL);
}

void shell() {
	pipe_counter = 0;
	setCursor(0, screen_height()-char_height());
	char c;
	int i=0;
	char string[BUFFER_SIZE]={0};
	while(1) {
		myPrintf("%s$: ","user");
		while(((c=myGetChar()) != '\n') && i<BUFFER_SIZE){
			if(c == BACKSPACE) {
				if(i != 0) {
					i--;
					setCursor(char_width()*(i+7),screen_height()-char_height());
					myPutchar(' ');
					setCursor(char_width()*(i+7),screen_height()-char_height());
				}
			} else if(c!=0) {
				string[i++]=c;
				myPutchar(c);
			}
		}
		string[i]=0;
		i=0;
		myPutchar('\n');
		interprete(string);
	}
}
