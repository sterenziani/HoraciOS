#include <stdint.h>

#define EMPTY 0
#define USED 1
#define SPLIT 2
#define BLOCK_SIZE 4096
#define MAX_MEMORY_BYTES (4096*1024)

#define PARENT_INDEX(currentIndex) ((currentIndex + 1) / 2 - 1)
#define LEFT_CHILD_INDEX(currentIndex) (currentIndex * 2 + 1)
#define RIGHT_CHILD_INDEX(currentIndex) (currentIndex * 2 + 2)
#define IS_LEFT_CHILD(currentIndex) (currentIndex % 2 == 1)
#define IS_RIGHT_CHILD(currentIndex) (currentIndex % 2 == 0)

typedef struct {
  uint64_t memoryAvailable;
  uint8_t* pages;
  uint32_t maxOrder;
  void * memoryBaseAddress;
} memoryManager_t;

void init_memoryManager(void * base_address, uint64_t bytes);
void* malloc(uint64_t bytes_to_reserve);
void* malloc_handler(uint64_t bytes_to_reserve, void** address);
void free_handler(void* p);
void print_bitmap();
