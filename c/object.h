#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
    OBJ_STRING,
} ObjType;

struct Obj {
    ObjType type;
    struct Obj* next;
};

/**
 * 構造体はフィールドは宣言された順序でメモリに置かれる
 * ObjStringの先頭から数バイトはObjと完全に一致する
 * これによって構造体のポインタを、最初のフィールドへのポインタに変換することもその逆の変換も安全に行える
 * Objのあとに続くフィールドがあっても無視することができる
 * oopにおける継承のようなことができる
*/
struct ObjString {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash; // hash of the string to use in the hash table
};

ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void printObject(Value value);

/**
 * 関数にする理由は、引数のvalueを二度使うから
 * マクロの場合、同じパラメータをなんども使うと、その式を何度も評価してしまう
*/
static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif