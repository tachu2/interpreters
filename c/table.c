#include "table.h"
#include "memory.h"
#include "object.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void freeTable(Table* table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

static Entry* findEntry(Entry* entries, int capacity, ObjString* key) {
    uint32_t index = key->hash % capacity;
    for (;;) {
        Entry* entry = &entries[index];
        if (entry->key == key || entry->key == NULL) {
            return entry;
        }
        // collision衝突が発生
        // linear probing線形探索で次のインデックスを探す
        index = (index + 1) % capacity;
    }
}

/**
 * 配列の占有率が75%を超えたら、配列を拡張する
 * すでに存在するキーの値は新しい配列にコピーする
 * @param table the table to adjust the capacity of
 * @param capacity the new capacity of the table
 */
static void adjustCapacity(Table* table, int capacity) {
    Entry* newEntries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        newEntries[i].key = NULL;
        newEntries[i].value = NIL_VAL;
    }

    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key == NULL) {
            continue;
        }
        Entry* dest = findEntry(newEntries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = newEntries;
    table->capacity = capacity;
}

bool tableSet(Table* table, ObjString* key, Value value) {
    // 配列の占有率が75%を超えたら、配列を拡張する
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }
    Entry* entry = findEntry(table->entries, table->capacity, key);
    bool isNewKey = entry->key == NULL;
    if (isNewKey) {
        table->count++;
    }
    entry->key = key;
    entry->value = value;
    return isNewKey;
}

void tableAddAll(Table* from, Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if (entry->key == NULL) {
            continue;
        }
        if (entry->key != NULL) {
            tableSet(to, entry->key, entry->value);
        }
    }
}