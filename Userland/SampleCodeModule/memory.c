#include <memory.h>
#include <stdlib.h>
#include <SysCallDispatcher.h>

void* malloc(uint64_t bytes)
{
  void* address = 0;
  SysCallDispatcher(9, bytes, (uint64_t) &address, 0, 0, 0, 0);
  return address;
}

void free(void* address)
{
  SysCallDispatcher(10, (uint64_t)address, 0, 0, 0, 0, 0);
}

void printMemoryBitmap()
{
  SysCallDispatcher(87, 0, 0, 0, 0, 0, 0);
}

void user_malloc(uint64_t bytes)
{
  void* address = 0;
  SysCallDispatcher(9, bytes, (uint64_t) &address, 0, 0, 0, 0);
  if(address == NULL)
    myPrintf("\nCould not allocate memory! The requested size of memory is either too big or invalid.\n");
  else
  {
    myPrintf("\nBlock of memory has been created in address 0x");
    myPrintHex((uint64_t) address);
    myPrintf("\n");
    myPrintf("If you want to free this block of memory, run the command 'free %d'\n", (uint64_t)address);
  }
}

void user_free(uint64_t address)
{
  free((void*)address);
  myPrintf("The block of memory in address %d has been freed!\n", address);
}
