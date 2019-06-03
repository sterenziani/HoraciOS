#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <SysCallDispatcher.h>
#include <memoryManager.h>

static memoryManager_t memoryManager;

uint64_t pow(uint32_t base, uint32_t exp)
{
	uint64_t result = 1;
	for (uint32_t i = 0; i < exp; i++) {
		result *= base;
	}
	return result;
}

uint32_t log2(uint32_t number)
{
	uint32_t power = 0;
	while(number - 1 > 0)
  {
		number /= 2;
		power++;
	}
	return power;
}

// Retorna la altura maxima del arbol dependiendo de la cantidad de memoria y el tamaño de pagina
uint32_t tree_depth()
{
  return log2(memoryManager.memoryAvailable) - log2(BLOCK_SIZE);
}

uint64_t amount_of_blocks()
{
  return memoryManager.memoryAvailable / BLOCK_SIZE;
}

uint64_t heap_size()
{
  uint32_t tree_height = tree_depth() + 1;
  return (1 << tree_height) - 1;    // 2^height - 1
}

void init_heap()
{
  uint64_t amount_of_nodes = heap_size();
  for (int i = 0; i < amount_of_nodes; i++)
  {
    memoryManager.pages[i] = EMPTY;
  }
}

void init_memoryManager(void * base_address, uint64_t bytes)
{
  if(bytes > MAX_MEMORY_BYTES)
    bytes = MAX_MEMORY_BYTES;
  memoryManager.memoryAvailable = bytes;
  uint64_t bytes_taken_by_arrays = (sizeof(uint8_t)) * heap_size();                    				//Necesitamos esta cantidad de bytes para el heap
  memoryManager.pages = base_address;
  memoryManager.maxOrder = tree_depth();
  memoryManager.memoryBaseAddress = (void *)((char *)base_address + bytes_taken_by_arrays);   // La memoria a administrar va a arrancar a partir del final del heap
  init_heap();
}

void print_bitmap()
{
  newLine();
	uint64_t amount_of_nodes = heap_size();
	int power = 0;
	int counter = 0;
  for (uint64_t i = 0; i < amount_of_nodes; i++)
	{
		if(counter == pow(2, power))
		{
			print(" --- Level ");
			printDec(power);
			print(" - Blocks of ");
			printDec((memoryManager.memoryAvailable / pow(2, power))/1024);
			print(" KB");
			newLine();
			counter = 0;
			power++;
		}
		switch (memoryManager.pages[i])
		{
			case EMPTY: printFormat("O", GREEN_COLOR, BLACK_COLOR);
									break;
			case USED:	printFormat("1", RED_COLOR, BLACK_COLOR);
									break;
			case SPLIT: printFormat("X", WHITE_COLOR, BLACK_COLOR);
									break;
		}
		counter++;
  }
	print(" --- Level ");
	printDec(power);
	print(" - Blocks of ");
			printDec((memoryManager.memoryAvailable / pow(2, power))/1024);
	print(" KB");
	newLine();
  newLine();
}

// Redondea los bytes a una potencia de 2
int amount_of_blocks_needed(uint64_t bytes)
{
	uint64_t multiple = BLOCK_SIZE;
  if (bytes > BLOCK_SIZE)
  {
    while (multiple <= memoryManager.memoryAvailable && bytes > multiple)
    {
      multiple *= 2;
    }
  }
  return multiple;
}

uint64_t calculateOffset(int pagesIndex, uint32_t order)
{
  uint64_t skippedPages = (pagesIndex + 1 - (1 << order)) << (memoryManager.maxOrder - order);
  return skippedPages * BLOCK_SIZE;
}

// Retorna el índice de su nodo hermano
uint32_t getBuddyIndex(uint32_t currentIndex)
{
  if (IS_LEFT_CHILD(currentIndex))
    return currentIndex + 1;
  else
    return currentIndex - 1;
}

// Devuelve el índice del próximo nodo en DFS
int next_possible_index(int pagesIndex, uint32_t* currentOrder, uint64_t* currentBlockSize)
{
	// Si estoy en un hijo izquierdo, paso al hijo derecho. En el heap, son consecutivos.
  if (IS_LEFT_CHILD(pagesIndex))
  {
    pagesIndex++;
  }
  else
  {
		// Soy un hijo derecho. No puedo incrementar para hallar al próximo nodo. Necesito ir subiendo por el árbol.
    int done = FALSE;
    while (pagesIndex >= 0 && !done)
    {
        (*currentOrder)--;                        // Subo de nivel
        (*currentBlockSize) *= 2;                 // Ajusto el tamaño al de este nivel
        pagesIndex = PARENT_INDEX(pagesIndex);    // Ajusto el indice al de este nivel

        if (IS_LEFT_CHILD(pagesIndex))            // Si es un hijo izquierdo me muevo al de la derecha, tío del original
        {                                         // Si es el hijo derecho, en el proximo loop me subirá al nivel superior de nuevo
            pagesIndex++;
            done = TRUE;
        }
    }
  }
  return pagesIndex;
}

// Antes de llamar a esta funcion recursiva debería haber marcado el indice que le paso como usado
// Esta función actualiza el estado de los nodos superiores al que le paso, marcándolos como USED o SPLIT según corresponda
void updateUsedNodesStatus(int pagesIndex)
{
	// Si no soy la raíz y mi hermano también está usado, marco a nuestro padre como usado y hago recursión para nuestro padre.
  if (pagesIndex > 0 && memoryManager.pages[getBuddyIndex(pagesIndex)] == USED)
  {
    pagesIndex = PARENT_INDEX(pagesIndex);
    if (pagesIndex >= 0) {
      memoryManager.pages[pagesIndex] = USED;
    }
    updateUsedNodesStatus(pagesIndex);
  }
  else
  {
		// Mi hermano no está usado. Mientras yo no sea la raíz, marco a todos mis antepasados como SPLIT
    while (pagesIndex > 0)
    {
      pagesIndex = PARENT_INDEX(pagesIndex);
      memoryManager.pages[pagesIndex] = SPLIT;
    }
  }
}

// Finds an offset starting form memoryBaseAddress where that many bytes can be allocated
uint64_t first_suitable_offset(uint64_t bytes_to_reserve)
{
  int pagesIndex = 0;
  uint32_t currentOrder = 0;
  uint64_t currentBlockSize = memoryManager.memoryAvailable;
  uint64_t neededBlockSize = amount_of_blocks_needed(bytes_to_reserve); // Redondeo para ver cuántos bloques necesito reservar (2^n)
  uint8_t memoryAllocated = FALSE;

  while (pagesIndex >= 0 && !memoryAllocated)
  {
    if (memoryManager.pages[pagesIndex] != USED)
    {
			// Si este nodo no está ocupado y necesito todo su tamaño:
      if (neededBlockSize == currentBlockSize)
      {
          if (memoryManager.pages[pagesIndex] == EMPTY)
          {
						// Si el nodo está libre, lo marco como usado
            memoryManager.pages[pagesIndex] = USED;
            updateUsedNodesStatus(pagesIndex);
            memoryAllocated = TRUE;
          }
          else
          {
						// Este nodo tiene algo de contenido. Pero como necesito todo el espacio, no me sirve.
						// Busco el próximo índice para probar otra vez en el próximo loop.
            pagesIndex = next_possible_index(pagesIndex, &currentOrder, &currentBlockSize);
            if (pagesIndex < 0)
              return ERROR;
          }
      }
      else
      {
				// Necesito menos memoria que la que ofrece este nodo. Bajo al próximo nivel. Si el hijo izquierdo no está usado, voy a él. Sino a la derecha
        currentOrder++;
        currentBlockSize /= 2;
        if (memoryManager.pages[LEFT_CHILD_INDEX(pagesIndex)] != USED)
          pagesIndex = LEFT_CHILD_INDEX(pagesIndex);
        else
          pagesIndex = RIGHT_CHILD_INDEX(pagesIndex);
      }
    }
    else
    {
			// El nodo está usado. Busco el próximo índice para probar otra vez en el próximo loop.
      pagesIndex = next_possible_index(pagesIndex, &currentOrder, &currentBlockSize);
      if (pagesIndex < 0)
        return ERROR;
    }
  }
  if(memoryAllocated)
    return calculateOffset(pagesIndex, currentOrder);
  else
    return ERROR;
}

void* malloc(uint64_t bytes_to_reserve)
{
  // If there's no way to reserve this much, return NULL
  if (bytes_to_reserve <= 0 || bytes_to_reserve > memoryManager.memoryAvailable)
    return NULL;

  uint64_t offset = first_suitable_offset(bytes_to_reserve);
  if (offset == ERROR)
    return NULL;
  return (void *)((uint64_t)memoryManager.memoryBaseAddress + offset);
}

void* malloc_handler(uint64_t bytes_to_reserve, void** address)
{
  *address = malloc(bytes_to_reserve);
  return *address;
}

void updateEmptyNodesStatus(int pagesIndex)
{
	// Si no soy la raíz y mi hermano también está libre, marco a nuestro padre como libre y hago recursión para nuestro padre.
  if (pagesIndex > 0 && memoryManager.pages[getBuddyIndex(pagesIndex)] == EMPTY)
  {
    pagesIndex = PARENT_INDEX(pagesIndex);
    if (pagesIndex >= 0)
      memoryManager.pages[pagesIndex] = EMPTY;
    updateEmptyNodesStatus(pagesIndex);
  }
  else
  {
		// Mi hermano no está libre. Mientras yo no sea la raíz, marco a todos mis antepasados como SPLIT
    while (pagesIndex > 0)
    {
      pagesIndex = PARENT_INDEX(pagesIndex);
      memoryManager.pages[pagesIndex] = SPLIT;
    }
  }
}

// No estoy seguro de que funcione 100% bien
void free_handler(void* p)
{
	if(p == NULL)
    return;
	uint64_t offset = (char*)p - (char*)memoryManager.memoryBaseAddress;
  if(offset % BLOCK_SIZE != 0)
  {
    return; // No hago nada porque esa direccion no es el inicio de un bloque
  }

	// bitmap_index es la posición que ocuparía en un bitmap como la implementación anterior
	uint64_t bitmap_index = offset / BLOCK_SIZE;

	// pages_skipped_in_branch es el mínimo índice de bitmap que pueden tener las páginas accedibles desde esta rama
	uint32_t pages_skipped_in_branch = 0;

	// Nos preparamos para explorar el árbol desde la raíz
  uint64_t viewable_pages = amount_of_blocks();
  uint32_t pagesIndex = 0;
  uint64_t amount_of_nodes = heap_size();
  uint32_t leftChildIndex, rightChildIndex;

  if (bitmap_index >= amount_of_blocks())
    return;

	// Mientras estemos observando un nodo dentro del heap
  while (pagesIndex < amount_of_nodes)
  {
    leftChildIndex = LEFT_CHILD_INDEX(pagesIndex);
    rightChildIndex = RIGHT_CHILD_INDEX(pagesIndex);

		// Si este índice estaba USED, lo libero porque alguno de sus hijos será liberado, convirtiéndolo en EMPTY o SPLIT
    if(memoryManager.pages[pagesIndex] == USED)
    {
      memoryManager.pages[pagesIndex] = EMPTY;

			// Si no tengo hijos, soy una hoja a la que acaban de liberar. Vamos a emprolijar el rastro de EMPTYs que dejamos hasta llegar aca
			if(leftChildIndex >= amount_of_nodes)
			{
				updateEmptyNodesStatus(pagesIndex);
				return;
			}
      // Si sus hijos existen y están libres, significa que la dirección ocupaba varios bloques de memoria. Emprolijo el rastro y retorno
      if (leftChildIndex < amount_of_nodes && rightChildIndex < amount_of_nodes && memoryManager.pages[leftChildIndex] == EMPTY && memoryManager.pages[rightChildIndex] == EMPTY)
			{
				updateEmptyNodesStatus(pagesIndex);
				return;
			}
    }

		// Me preparo para bajar al próximo nivel
    viewable_pages /= 2;
		//Si bitmap index es menor que el mínimo al que podría acceder si fuese por la derecha, me voy por la rama izquierda. Si es mayor, por la derecha
    if (bitmap_index < pages_skipped_in_branch + viewable_pages)
      pagesIndex = leftChildIndex;
    else
    {
      pages_skipped_in_branch += viewable_pages;
      pagesIndex = RIGHT_CHILD_INDEX(pagesIndex);
    }
  }
}

/*
viewable_pages = 4
bitmap_index = 0
pagesSkippedAtBranch = 0

														M
								M												0
					0						M						0						0
			 0		 0		 M		 0		 0		 0		 0		 0
			0	0		0	0		0	X		0	0		0	0		0	0		0	0		0	0

*/
