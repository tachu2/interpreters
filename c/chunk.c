#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    initValueArray(&(chunk->constants));
    chunk->lines.runs = NULL;
    chunk->lines.count = 0;
    chunk->lines.capacity = 0;
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(LineRun, chunk->lines.runs, chunk->lines.capacity);
    freeValueArray(&(chunk->constants));
    initChunk(chunk);
}

int findLine(Chunk* chunk, int instructionOffset) {
    for (int i = 0; i < chunk->lines.count; i++) {
        if (instructionOffset < chunk->lines.runs[i].count) {
            return chunk->lines.runs[i].line;
        }
        i += chunk->lines.runs[i].count;
    }
    return 0;
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
    if (chunk->lines.count > 0 &&
        chunk->lines.runs[chunk->lines.count - 1].line == line) {
      chunk->lines.runs[chunk->lines.count - 1].count++;
    } else {
        if (chunk->lines.capacity < chunk->lines.count + 1) {
            int oldCapacity = chunk->lines.capacity;
            chunk->lines.capacity = GROW_CAPACITY(oldCapacity);
            chunk->lines.runs = GROW_ARRAY(LineRun, chunk->lines.runs, oldCapacity, chunk->lines.capacity);
        }
        chunk->lines.runs[chunk->lines.count].line = line;
        chunk->lines.runs[chunk->lines.count].count = 1;
        chunk->lines.count++;
    }
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&(chunk->constants), value);
    return chunk->constants.count - 1;
}