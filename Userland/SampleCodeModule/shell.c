#include <stdlib.h>
#include <time.h>
#include <exception.h>
#include <memory.h>
#include <ipc.h>
#include <tests.h>
#include <taskmanager.h>
#include <prodcons_sem.h>

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

mailbox_t mailbox;
mutex_t mutex;

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

	myPutchar('\n');
}

void zeroDivision() {
	int i = 0;
	i = 10/i;
	return;
}

void standard_command_handler(char* s, int background_flag)
{
	if(strcmp(s, HELP_COMMAND) == 0)
	{
		run("help", help, NULL, NULL, background_flag, 0);
		return;
	}
	if(strcmp(s, CLOCK_COMMAND) == 0)
	{
		run("watch", watch, NULL, NULL, background_flag, 0);
		return;
	}
	if(strcmp(s, DIGITAL_CLOCK_COMMAND) == 0)
	{
		if(background_flag == 1)
			myPrintf("This process requires user input and can not run on background\n");
		else
			run("digitalclock", digitalClock, NULL, NULL, background_flag, 0);
		return;
	}
	if(strcmp(s, MEM_CHECK) == 0)
	{
		run("printMemoryBitmap", printMemoryBitmap, NULL, NULL, background_flag, 0);
		return;
	}
	if(strcmp(s, MAILBOX_TEST) == 0)
	{
		run("mailbox_test", mailbox_test, NULL, NULL, background_flag, 0);
		return;
	}
	if(strcmp(s, MUTEX_TEST) == 0)
	{
		run("mutex_test", mutex_test, NULL, NULL, background_flag, 0);
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
		run("ps", list_all_processes, NULL, NULL, background_flag, 0);
		return;
	}
	if(strcmp(s, SCHEDULER_TEST) == 0)
	{
		run("scheduler_test", scheduler_test, NULL, NULL, background_flag, 0);
		return;
	}
	if(strcmp(s, DUMMY) == 0)
	{
		run("dummy", sleep_for_10_secs, NULL, NULL, background_flag, 0);
		return;
	}
	invalid_command(s);
	return;
}

void pipe_command_handler(char* app, char* target)
{
	myPrintf("Pipes have not been implemented yet! Come back when TP3 is finished!\n");
}

void background_command_handler(char* s)
{
	standard_command_handler(s, 1);
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
		standard_command_handler(command, 0);
}

void shell() {
	mailbox = NULL;
	mutex = NULL;
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
