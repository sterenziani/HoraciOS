#include <stdint.h>

void* malloc(uint64_t bytes);
void free(void* address);
void printMemoryBitmap();
void user_malloc(uint64_t bytes);
void user_free(uint64_t address);
