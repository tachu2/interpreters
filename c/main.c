#include "common.h"
#include "chunk.h"
#include "memory.h"
#include "debug.h"
#include "vm.h"
#include <stdio.h>

int main(int argc, const char* argv[]) {
    int arr[] = {1, 2, 3, 4, 5};
    int* ptr = &arr[2];
    printf("%d\n", ptr[-1]);
    printf("%d\n", ptr[1]);
    initVM();
    Chunk chunk;
    initChunk(&chunk);
    // int constant = addConstant(&chunk, 1.2);
    // writeChunk(&chunk, OP_CONSTANT, 123);
    // writeChunk(&chunk, constant, 123);

    // constant = addConstant(&chunk, 3.4);
    // writeChunk(&chunk, OP_CONSTANT, 123);
    // writeChunk(&chunk, constant, 123);
    // writeChunk(&chunk, OP_ADD, 123);

    // constant = addConstant(&chunk, 5.6);
    // writeChunk(&chunk, OP_CONSTANT, 123);
    // writeChunk(&chunk, constant, 123);
    // writeChunk(&chunk, OP_DIVIDE, 123);

    // writeChunk(&chunk, OP_NEGATE, 123);
    // writeChunk(&chunk, OP_RETURN, 123);

    int constant = addConstant(&chunk, 1);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    constant = addConstant(&chunk, 2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    constant = addConstant(&chunk, 3);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    writeChunk(&chunk, OP_MULTIPLY, 123);
    writeChunk(&chunk, OP_ADD, 123);

    constant = addConstant(&chunk, 4);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    constant = addConstant(&chunk, 5);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    writeChunk(&chunk, OP_NEGATE, 123);

    writeChunk(&chunk, OP_DIVIDE, 123);
    writeChunk(&chunk, OP_SUBTRACT, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    disassembleChunk(&chunk, "test chunk");
    interpret(&chunk);
    freeVM();
    freeChunk(&chunk);
    return 0;
}