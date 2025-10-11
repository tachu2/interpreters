#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

/**
 * Reallocates memory.
 * @param pointer the pointer to the memory to reallocate
 * @param oldSize the old size of the memory (0 to allocate the memory)
 * @param newSize the new size of the memory (0 to free the memory)
 * @return the pointer to the reallocated memory
 */
void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif