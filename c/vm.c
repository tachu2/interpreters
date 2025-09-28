#include "vm.h"
#include "compiler.h"
#include "debug.h"
#include <stdio.h>

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
}

void freeVM() {
}

void push(Value value) {
    *(vm.stackTop) = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *(vm.stackTop);
}

static InterpretResult run() {
    #define READ_BYTE() (*vm.ip++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    /**
     * do whileを使うことでマクロ内で複数の文をブロック内で書くことができる
     * マクロの裏技的なテクニック
     * do whileを使わないでif文の条件分岐をするとセミコロンを使ったタイミングでマクロの処理が終わりと認識される
    */
    #define BINARY_OP(op) { \
        do { \
            Value b = pop(); \
            Value a = pop(); \
            push(a op b); \
        } while (false); \
    }
    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            // dispatching, decoding instruction
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_ADD: {
                BINARY_OP(+);
                break;
            }
            case OP_SUBTRACT: {
                BINARY_OP(-);
                break;
            }
            case OP_MULTIPLY: {
                BINARY_OP(*);
                break;
            }
            case OP_DIVIDE: {
                BINARY_OP(/);
                break;
            }
            case OP_NEGATE: {
                push(-pop());
                break;
            }
            case OP_RETURN:
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
        }
    }
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    compile(source);
    run();
    return INTERPRET_OK;
}