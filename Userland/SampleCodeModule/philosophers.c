#include <stdlib.h>
#include <ipc.h>
#include <taskmanager.h>
#include <philosophers.h>
#include <time.h>

process_id_t philosophers[MAX_PHILOSOPHERS] = {0};
mutex_t chopsticks[MAX_PHILOSOPHERS];

int philosophers_quantity = 1;
int philosophers_assigned = 0;
int philosophers_to_remove = 0;

int flag_of_first = 1;
int flag_of_second = 1;

mutex_t main_mutex;

void* argsp[2];

char* chopsticks_names[10] = {"chop1","chop2","chop3","chop4","chop5","chop6","chop7","chop8",
"chop9","chop10"};

int secret_number = 0;


/*Dado que no se puede comenzar el problema de los filosofos con un solo filosofo
se utilizan flags para esperar hasta que el usuario llame al segundo filosofo
para poder crear los dos procesos. El filosofo 1 se crea pensando que la
cantidad total es 2, para que actue correctamente.
*/

void prepareTable(){
	printInstructions();

	philosophers_quantity = 1;
	philosophers_assigned = 0;
	philosophers_to_remove = 0;

	for (int i = 0; i < MAX_PHILOSOPHERS; ++i) {
 		chopsticks[i] = mutex_create(chopsticks_names[i]);
 	}

 	main_mutex = mutex_create("main_mutex");

 	int flag_of_first = 1;
 	int flag_of_second = 1;

 	argsp[0] = main_mutex;
 	argsp[1] = chopsticks;

 	int c;
	while((c = myGetChar()) != QUIT) {
		switch(c) {
			case INC_PHILO:
			if(flag_of_first == 1){
				flag_of_first = 0;
				myPrintf("    Un filosofo solo no puede debatir, contrate a otro para que puedan comenzar!\n");
				break;
			}
			if(flag_of_second == 1){
				flag_of_second = 0;
				hirePhilosopher();
				sleep(1);
				mutex_lock(main_mutex);
				philosophers_quantity--;
				mutex_unlock(main_mutex);
				hirePhilosopher();
				break;
			}
			else
				hirePhilosopher();
				break;
			case DEC_PHILO:
				letGoPhilosopher();
				break;
			default:
				break;
			case SECRET:
				secretCommand();
				break;
		}
	}

	destroyAll();
	mutex_destroy(main_mutex);
	for (int i = 0; i < MAX_PHILOSOPHERS; ++i) {
 		mutex_destroy(chopsticks_names[i]);
 	}
}

void printInstructions() {
 	myPrintf("\nTodavia no contrato a un filosofo\n");
	myPrintf("Presione:\n");
  myPrintf("    %c para agregar a un filosofo\n", INC_PHILO);
	myPrintf("    %c para eliminar a un filosofo\n", DEC_PHILO);
	myPrintf("    %c para terminar la simulacion\n\n", QUIT);
}


/*
El filosofo calcula a que cubiertos (mutexes) debe tomar a su izquierda y derecha.
Los filosofos de philosopher_number par empiezan por la derecha
Los impares empiezan por la izquierda. De esta forma se evita de que dos filosofos
uno al lado del otro tomen un cubierto y luego peleen por el cubierto
que comparten, creando un deadlock
*/


void * philosopher(void** argsp) {
	mutex_lock(main_mutex);
	int philosopher_number = philosophers_assigned;
	mutex_t main_mutex = argsp[0];
  int right = ((philosopher_number) % philosophers_quantity);
	int left = ((philosophers_quantity + philosopher_number) - 1) % philosophers_quantity;
	mutex_unlock(main_mutex);

	mutex_t * chopsticks;
	chopsticks = argsp[1];

	myPrintf("    Ha llegado el filosofo %d\n", philosopher_number);
	while (1){
		mutex_lock(main_mutex);
		if (philosophers_to_remove > 0)
		{
				removePhilosopher();
		}
		right = ((philosopher_number) % philosophers_quantity);
		left = ((philosophers_quantity + philosopher_number) - 1) % philosophers_quantity;
		myPrintf("Philosopher %d - ", philosopher_number);
		myPrintf("L = %d   ", left);
		myPrintf(" R = %d", right);
		myPrintf("  Quantity = %d", philosophers_quantity);
		myPrintf("  Assigned = %d\n", philosophers_assigned);
		mutex_unlock(main_mutex);

		if (philosopher_number % 2) {
			myPrintf("El filosofo %d esta esperando para tomar el cubierto ", philosopher_number);
			myPrintf("%d\n",right);
			mutex_lock(chopsticks[right]);
			myPrintf("El filosofo %d pudo tomar el cubierto ", philosopher_number);
			myPrintf("%d\n",right);

			myPrintf("El filosofo %d esta esperando para tomar el cubierto ", philosopher_number);
			myPrintf("%d\n",left);
			mutex_lock(chopsticks[left]);
			myPrintf("El filosofo %d pudo tomar el cubierto ", philosopher_number);
			myPrintf("%d\n",left);
		}
		else {
			myPrintf("El filosofo %d esta esperando para tomar el cubierto ", philosopher_number);
			myPrintf("%d\n",left);
			mutex_lock(chopsticks[left]);
			myPrintf("El filosofo %d pudo tomar el cubierto ", philosopher_number);
			myPrintf("%d\n",left);

			myPrintf("El filosofo %d esta esperando para tomar el cubierto ", philosopher_number);
			myPrintf("%d\n",right);
			mutex_lock(chopsticks[right]);
			myPrintf("El filosofo %d pudo tomar el cubierto ", philosopher_number);
			myPrintf("%d\n",right);
		}

		myPrintf("El filosofo %d esta comiendo en la mesa\n", philosopher_number);

		mutex_lock(main_mutex);
		myPrintf("El filosofo %d termino de comer\n", philosopher_number);
		mutex_unlock(chopsticks[right]);
		mutex_unlock(chopsticks[left]);
		mutex_unlock(main_mutex);
		}
	return 0;
}


void hirePhilosopher() {
 	mutex_lock(main_mutex);

	if(philosophers_quantity >= MAX_PHILOSOPHERS) {
		myPrintf("    No hay filosofos disponibles para contratar\n");
		mutex_unlock(main_mutex);
		return;
	}

	int pid = 0;
	philosophers_quantity++;
	philosophers_assigned++;
	run("philosopher", &philosopher, argsp, &pid, 1, 0, NULL, NULL);
	philosophers[philosophers_assigned - 1] = pid;
	mutex_unlock(main_mutex);
}


void letGoPhilosopher() {
	mutex_lock(main_mutex);

 	if(philosophers_quantity <= 2) {
		myPrintf("    Necesitas al menos dos filosofos en tu casa\n!");
		mutex_unlock(main_mutex);
		return;
	}

	philosophers_to_remove ++;
	mutex_unlock(main_mutex);
}


void removePhilosopher() {
  philosophers_quantity--;
  philosophers_to_remove--;
	philosophers_assigned--;
  process_id_t fired_philospher = philosophers[philosophers_quantity];
  myPrintf("    El filosofo %d se fue\n", philosophers_quantity+1);
  kill(fired_philospher);
	mutex_unlock(main_mutex);
}


void destroyAll() {
 	mutex_lock(main_mutex);

	for (int i = 0; i < philosophers_quantity; i++) {
		kill(philosophers[i]);
	}

 	mutex_unlock(main_mutex);
}


void secretCommand(){
	char* secrets[10] = {"\nYo solo se que no se nada\n",
	"\nNo es lo que te ocurre, si no como reaccionas lo que importa\n",
	"\nNuestra envidia siempre dura mas que la felicidad de quien envidiamos\n",
	"\nHace falta una vida para aprender a vivir\n",
	"\nLos grandes resultados requieren grandes ambiciones\n",
	"\nCuanto mas conozco a la gente, mas quiero a mi perro\n",
	"\nEl hombre es la medida de todas las cosas\n",
	"\nLas raices de la educacion son amargas, pero sus frutos son dulces\n",
	"\nSomos mas sinceros cuando estamos iracundos que cuando estamos tranquilos\n",
	"\nEl insulto deshonra a quien lo infiere, no a quien lo recibe\n"};
	myPrintf(secrets[secret_number]);
	secret_number++;
	if(secret_number>9){
		secret_number = 0;
	}
}
