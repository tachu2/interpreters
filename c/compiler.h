#ifndef COMPILER_H
#define COMPILER_H

#include "object.h"
#include "chunk.h"

bool compile(const char* source, Chunk *chunk);

#endif