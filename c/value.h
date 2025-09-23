#ifndef VALUE_H
#define VALUE_H

#include "common.h"

typedef double Value;

/**
 * constant pool
 * 定数プール
 * 定数リテラルを格納する配列
 */
typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif