#include "debug.h"
#include <stdio.h>

void disassembleChunk(Chunk* chunk, const char* name) {
    int lineCount = 0;
    int lineIndex = 0;

    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;) {
        printf("%04d ", offset);
        if (lineCount > 0 && chunk->lines.runs[lineIndex].count != lineCount) {
            printf("   | ");
        } else {
            printf("%4d ", chunk->lines.runs[lineIndex].line);
        }
        offset = disassembleInstruction(chunk, offset);
        if (chunk->lines.runs[lineIndex].count == lineCount) {
            lineIndex++;
            lineCount = 0;
        }
        lineCount++;
    }
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

int disassembleInstruction(Chunk* chunk, int offset) {
    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}