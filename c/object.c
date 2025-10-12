#include "object.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>

#define ALLOCATE_OBJ(type, objType) \
    (type*)allocateObject(sizeof(type), objType)

static void* allocateObject(size_t size, ObjType type) {
    Obj* ptr = (Obj*)reallocate(NULL, 0, size);
    ptr->type = type;
    return ptr;
}

static ObjString* allocateString(char* chars, int length) {
    ObjString* string = ALLOCATE(ObjString, 1);
    string->length = length;
    string->chars = chars;
    return string;
}

ObjString* takeString(char* chars, int length) {
    return allocateString(chars, length);
}

ObjString* copyString(const char* chars, int length) {
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length);
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING: {
            printf("%s", AS_CSTRING(value));
            break;
        }
    }
}