#include <prodcons_sem.h>
#include <stdlib.h>
#include <ipc.h>
#include <taskmanager.h>

sem_t full;
sem_t empty;
mutex_t mutexA;
void* args[3];
int plates = 0;
process_id_t waiters[MAX_PRODCONS] = {0};
process_id_t chefs[MAX_PRODCONS] = {0};
int waitersQty = 0, chefsQty = 0;
int waitersToDie = 0, chefsToDie = 0;

int prodcons_restaurant() {
  printMenu();
  mutexA = mutex_create("prodConsMutex");
	full = sem_create("fullSem", TRAYS_QTY);
  empty = sem_create("emptySem", 0);
  plates = 0;
  waitersQty = 0;
  chefsQty = 0;
  waitersToDie = 0;
  chefsToDie = 0;
  args[0] = mutexA;
  args[1] = full;
  args[2] = empty;
	printTrays();

	int c;
	while((c = myGetChar()) != QUIT) {
		switch(c) {
			case INC_CHEF:
				callChef();
				break;
			case DEC_CHEF:
				restChef();
				break;
			case INC_WAITER:
				callWaiter();
				break;
			case DEC_WAITER:
				restWaiter();
				break;
			default:
				break;
		}
	}
	terminateAll();
  mutex_destroy(mutexA);
  sem_destroy(full);
  sem_destroy(empty);
	myPrintf("La simulacion termino\n");
	return 0;
}

void printMenu() {
  myPrintf("\nEl restaurante Prodcons tiene un maximo de 20 platos\n");
	myPrintf("Hay 10 chefs y 10 camareros que inicialmente estan descansando\n\n", MAX_PRODCONS, MAX_PRODCONS);
	myPrintf("Inserte:\n");
  myPrintf("    %c para llamar a un chef\n", INC_CHEF);
	myPrintf("    %c para poner a descansar a un chef\n", DEC_CHEF);
	myPrintf("    %c para llamar a un camarero\n", INC_WAITER);
	myPrintf("    %c para poner a descansar a un camarero\n", DEC_WAITER);
	myPrintf("    %c para terminar la simulacion\n\n\n", QUIT);
}

void printTrays() {
	myPrintf("Platos = ");
	for (int i = 0; i < plates; i ++) {
    myPrintf("X");
  }
  for (int i = plates; i < TRAYS_QTY; i ++) {
    myPrintf("_");
  }
	myPrintf("\n\n\n");
}

void callChef() {

  mutex_lock(mutexA);
	if(chefsQty >= MAX_PRODCONS) {
		myPrintf("No hay mas chefs descansando\n\n\n");
		mutex_unlock(mutexA);
		return;
	}
  int pid = 0;
  run("chef", &chef, args, &pid, 1, 0, NULL, NULL);
	chefs[chefsQty++] = pid;
	mutex_unlock(mutexA);
}

void * chef(void** args) {
	myPrintf("Soy el chef %d que se pone a trabajar!\n\n\n", chefsQty);
  mutex_t mutexA = args[0];
  sem_t full = args[1];
  sem_t empty = args[2];
	while(1) {
		sem_wait(full);
		mutex_lock(mutexA);
    if (chefsToDie > 0) {
      sem_signal(full);
      chefSuicide();
    }
    plates ++;
    sem_signal(empty);

		printTrays();

		mutex_unlock(mutexA);
	}
	return 0;
}

void restChef() {
	mutex_lock(mutexA);
  if(chefsQty == 0) {
		myPrintf("No hay chefs trabajando\n\n\n");
		mutex_unlock(mutexA);
		return;
	}
  chefsToDie ++;
  mutex_unlock(mutexA);
}

void chefSuicide() {
  chefsQty--;
  chefsToDie--;
  process_id_t chefToDie = chefs[chefsQty];
  mutex_unlock(mutexA);
  myPrintf("El chef %d se fue a descansar\n\n\n", chefsQty);
  kill(chefToDie);
}

void callWaiter() {
	mutex_lock(mutexA);
	if(waitersQty >= MAX_PRODCONS) {
		myPrintf("No hay mas camareros descansando\n\n\n");
		mutex_unlock(mutexA);
		return;
	}
  int pid;
  run("waiter", &waiter, args, &pid, 1, 0, NULL, NULL);
	waiters[waitersQty++] = pid;
	mutex_unlock(mutexA);
}

void * waiter(void** args) {
	myPrintf("Soy el camarero %d que se pone a trabajar!\n\n\n", waitersQty);
  mutex_t mutexA = args[0];
  sem_t full = args[1];
  sem_t empty = args[2];
	while(1) {
		sem_wait(empty);
		mutex_lock(mutexA);
    if (waitersToDie > 0) {
      sem_signal(empty);
      waiterSuicide();
    }
		plates --;
    sem_signal(full);
		printTrays();
		mutex_unlock(mutexA);
	}
}

void restWaiter() {
	mutex_lock(mutexA);
	if(waitersQty == 0) {
		myPrintf("No hay camareros trabajando\n\n\n");
		mutex_unlock(mutexA);
		return;
	}
  waitersToDie ++;
  mutex_unlock(mutexA);
}

void waiterSuicide() {
  waitersQty--;
  waitersToDie--;
  process_id_t waiterToDie = waiters[waitersQty];
  mutex_unlock(mutexA);
  myPrintf("El camarero %d se fue a descansar\n\n\n", waitersQty);
  kill(waiterToDie);
}

void terminateAll() {
  mutex_lock(mutexA);
	for (int i = 0; i < chefsQty; i++) {
			kill(chefs[i]);
	}
  for (int i = 0; i < waitersQty; i++) {
      kill(waiters[i]);
  }
  mutex_unlock(mutexA);
}
