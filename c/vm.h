#ifndef VM_H
#define VM_H

#include "chunk.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip; // next instruction pointer
    Value stack[STACK_MAX];
    Value* stackTop; // next free slot in the stack
    Obj* objects;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
/**
 * Interprets a chunk of code.
 * @param chunk the chunk to interpret
 * @return the result of the interpretation
 */
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

#endif