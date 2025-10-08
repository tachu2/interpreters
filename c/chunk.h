#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"
#include "value.h"

 typedef enum {
  OP_CONSTANT,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_RETURN,
 } OpCode;

/**
 * A chunk is a sequence of bytes that represents a program.
 * dynamic array of bytes
 */
 typedef struct {
   int count; //number of elements in the array
   int capacity; //number of elements the array can hold
   uint8_t* code; // code of the program
   int* lines; // line numbers for each bytecode
   ValueArray constants; // constant pool(定数プール)
 } Chunk;

 void initChunk(Chunk* chunk);
 void writeChunk(Chunk* chunk, uint8_t byte, int line);
 void freeChunk(Chunk* chunk);
 /**
  * Adds a constant to the constant pool.
  * @param chunk the chunk to add the constant to
  * @param value the constant to add
  * @return the index of the constant in the constant pool
  */
 int addConstant(Chunk* chunk, Value value);

#endif