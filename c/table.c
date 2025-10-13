#include "table.h"
#include "memory.h"
#include "object.h"
#include <string.h>

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
    Entry* tombstone = NULL;

    for (;;) {
        Entry* entry = &entries[index];
        if (entry->key == NULL) {
            // entryが空いている場合
            if (IS_NIL(entry->value)) {
                // 新しいnodeをsetするときに使用するために、tombstoneを返す
                return tombstone != NULL ? tombstone : entry;
            } else {
                // entryがtombstoneの場合
                if (tombstone == NULL) {
                    tombstone = entry;
                }
            }
        } else if (entry->key == key) {
            return entry;
        }
        // collision衝突が発生
        // linear probing線形探索で次のインデックスを探す
        index = (index + 1) % capacity;
    }
}

bool tableGet(Table* table, ObjString* key, Value* value) {
    if (table->count == 0) {
        return false;
    }
    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) {
        return false;
    }
    *value = entry->value;
    return true;
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

    table->count = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key == NULL) {
            continue;
        }
        Entry* dest = findEntry(newEntries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
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
    // tombstoneも占有率に含める
    if (isNewKey && IS_NIL(entry->value)) {
        table->count++;
    }
    entry->key = key;
    entry->value = value;
    return isNewKey;
}

/**
 * キーを削除する
 * entryのクリアするだけでなく、tombstoneを使って削除を表現する
 * そうすることで、keyのindexが重複している場合でも、keyを見つけることができる
 * @param table the table to delete the key from
 * @param key the key to delete
 * @return true if the key was deleted, false otherwise
 */
bool tableDelete(Table* table, ObjString* key) {
    if (table->count == 0) {
        return false;
    }
    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) {
        return false;
    }
    entry->key = NULL;
    // tombstoneを使って削除を表現する
    entry->value = BOOL_VAL(true);
    return true;
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

ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash) {
    if (table->count == 0) {
        return NULL;
    }

    uint32_t index = hash % table->capacity;
    for (;;) {
        Entry* entry = &table->entries[index];
        if (entry->key == NULL) {
            if (IS_NIL(entry->value)) {
                return NULL;
            }
        } else if (entry->key->length == length && entry->key->hash == hash && memcmp(entry->key->chars, chars, length) == 0) {
            // ここで唯一文字単位の比較を行う
            // ここ以外ではアドレスの比較でok
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}