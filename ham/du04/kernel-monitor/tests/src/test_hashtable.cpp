#include "test_hashtable.h"
#include <unordered_map>

struct cpp_item_t {
    int key;
    void *value;
};

typedef struct cpp_item_t* cpp_item;

cpp_item get_cppitem(hashtable_item_t item)
{
    return static_cast<cpp_item>(item);
}

typedef std::unordered_map<int, cpp_item> cpp_hashtable;
typedef cpp_hashtable* cpp_hashtable_ptr;
cpp_hashtable_ptr get_cpptable(hashtable_t table)
{
    return static_cast<cpp_hashtable_ptr>(table);
}

// Hash table functions
hashtable_t cpp_hashtable_alloc()
{
    return new std::unordered_map<int, void*>();
}

void cpp_hashtable_free(hashtable_t table)
{
    delete get_cpptable(table);
}

void cpp_hashtable_insert(hashtable_t table, hashtable_item_t item)
{
    auto cpptable = get_cpptable(table);
    auto cppitem = get_cppitem(item);
    (*cpptable)[cppitem->key] = cppitem;
}

hashtable_item_t cpp_hashtable_item_alloc(int key, void *val)
{
    auto cppitem = new struct cpp_item_t;
    cppitem->key = key;
    cppitem->value = val;
    return cppitem;
}

void cpp_hashtable_item_free(hashtable_item_t item)
{
    delete get_cppitem(item);
}

void* cpp_hashtable_item_get_val(hashtable_item_t item)
{
    return get_cppitem(item)->value;
}

hashtable_item_t cpp_hashtable_get(hashtable_t table, int key)
{
    auto cpptable = get_cpptable(table);
    auto it = cpptable->find(key);
    if(it == cpptable->end()) return NULL;
    return it->second;
}

struct cppiter_t {
    cpp_hashtable_ptr table;
    cpp_hashtable::iterator iter;
};

typedef struct cppiter_t* cpp_iter;

cpp_iter get_cppiter(hashtable_iter_t item)
{
    return static_cast<cpp_iter>(item);
}

// Hash table iteration functions
hashtable_iter_t cpp_hashtable_iter_start(hashtable_t table)
{
    auto cppiter = new struct cppiter_t;
    cppiter->table = get_cpptable(table);
    cppiter->iter = cppiter->table->begin();
    return cppiter;
}

hashtable_item_t cpp_hashtable_iter_next(hashtable_iter_t iter)
{
    auto cppiter = get_cppiter(iter);
    if(cppiter->iter == cppiter->table->end()) return NULL;
    auto item = cppiter->iter->second;
    cppiter->iter++;
    return item;
}

hashtable_item_t cpp_hashtable_remove(hashtable_t table, hashtable_item_t item, hashtable_iter_t iter)
{
    auto cpptable = get_cpptable(table);
    auto it = cpptable->find(get_cppitem(item)->key);
    if(it == cpptable->end()) return NULL;
    auto newiter = cpptable->erase(it);
    if(iter != NULL) {
        get_cppiter(iter)->iter = newiter;
    }
    return it->second;
}


void cpp_hashtable_iter_stop(hashtable_iter_t iter)
{
    delete get_cppiter(iter);
}

hashtable_api cpp_hashtable_ops()
{
    hashtable_api cpp_api = {
        .alloc = cpp_hashtable_alloc,
        .insert = cpp_hashtable_insert,
        .get = cpp_hashtable_get,
        .remove = cpp_hashtable_remove,
        .free = cpp_hashtable_free,

        .item_alloc = cpp_hashtable_item_alloc,
        .item_val = cpp_hashtable_item_get_val,
        .item_free = cpp_hashtable_item_free,

        .iter_start = cpp_hashtable_iter_start,
        .iter_next = cpp_hashtable_iter_next,
        .iter_stop = cpp_hashtable_iter_stop
    };
    return cpp_api;
}
