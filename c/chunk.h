#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"


 typedef enum {
    OP_RETURN,
 } OpCode;

/**
 * A chunk is a sequence of bytes that represents a program.
 * dynamic array of bytes
 */
 typedef struct {
   int count; //number of elements in the array
   int capacity; //number of elements the array can hold
   uint8_t* code;
 } Chunk;


 void initChunk(Chunk* chunk);
 void writeChunk(Chunk* chunk, uint8_t byte);
 void freeChunk(Chunk* chunk);

#endif