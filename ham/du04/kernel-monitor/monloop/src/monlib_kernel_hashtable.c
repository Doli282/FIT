#include "monlib_kernel_hashtable.h"
#include <linux/rhashtable.h>
#include <linux/refcount.h>
#include <linux/slab.h>


struct kernel_item_t {
    int key;
    struct rhash_head linkage;
    void* value;
    refcount_t ref;
};

typedef struct kernel_item_t* kernel_item;

kernel_item get_kernelitem(hashtable_item_t item)
{
    return (kernel_item)item;
}

struct kernerl_hashtable_t {
    struct rhashtable_params params;
    struct rhashtable rtable;
};

typedef struct kernerl_hashtable_t* kernel_hashtable;

kernel_hashtable get_kerneltable(hashtable_t table)
{
    return (kernel_hashtable) table;
}

// Hash table functions
hashtable_t kernel_hashtable_alloc(void)
{
    int res;
    const static struct rhashtable_params object_params = {
        .key_len     = sizeof(int),
        .key_offset  = offsetof(struct kernel_item_t, key),
        .head_offset = offsetof(struct kernel_item_t, linkage)
    };

    kernel_hashtable hash_table = kmalloc(sizeof(struct kernerl_hashtable_t), GFP_KERNEL);
    hash_table->params = object_params;
    if((res = rhashtable_init(&hash_table->rtable, &object_params)) != 0) {
        printk("Failed to allocate hash table: %d", res);
        return NULL;
    }
    return hash_table;
}

void kernel_hashtable_free(hashtable_t table)
{
    rhashtable_destroy(&get_kerneltable(table)->rtable);
}

void kernel_hashtable_insert(hashtable_t table, hashtable_item_t item)
{
    kernel_hashtable k_table = get_kerneltable(table);
    kernel_item k_item = get_kernelitem(item);
    rhashtable_insert_fast(&k_table->rtable, &k_item->linkage, k_table->params);
}

hashtable_item_t kernel_hashtable_item_alloc(int key, void *val)
{
    kernel_item k_item = kmalloc(sizeof(struct kernel_item_t), GFP_KERNEL);
    k_item->key = key;
    k_item->value = val;
    refcount_set(&k_item->ref, 0);
    return k_item;
}

void kernel_hashtable_item_free(hashtable_item_t item)
{
    kfree(item);
}

void* kernel_hashtable_item_get_val(hashtable_item_t item)
{
    return get_kernelitem(item)->value;
}

hashtable_item_t kernel_hashtable_get(hashtable_t table, int key)
{
    kernel_item obj;
    kernel_hashtable k_table = get_kerneltable(table);
    rcu_read_lock();
    obj = rhashtable_lookup_fast(&k_table->rtable, &key, k_table->params);
    rcu_read_unlock();
    return obj;
}

struct kernel_iter_t {
    struct rhashtable_iter iter;
};

typedef struct kernel_iter_t* kernel_iter;

kernel_iter get_kerneliter(hashtable_iter_t item)
{
    return (kernel_iter)item;
}

// Hash table iteration functions
hashtable_iter_t kernel_hashtable_iter_start(hashtable_t table)
{
    kernel_hashtable ktable = get_kerneltable(table);
    kernel_iter kiter = kmalloc(sizeof(struct kernel_iter_t), GFP_KERNEL);

    rhashtable_walk_enter(&ktable->rtable, &kiter->iter);
    rhashtable_walk_start(&kiter->iter);
    return kiter;
}

hashtable_item_t kernel_hashtable_iter_next(hashtable_iter_t iter)
{
    kernel_iter kiter = get_kerneliter(iter);
    kernel_item obj = rhashtable_walk_next(&kiter->iter);
    return obj;
}

hashtable_item_t kernel_hashtable_remove(hashtable_t table, hashtable_item_t item, hashtable_iter_t iter)
{
    kernel_item obj;
    kernel_hashtable k_table = get_kerneltable(table);
    kernel_item k_item = get_kernelitem(item);
    rcu_read_lock();
    obj = rhashtable_lookup_fast(&k_table->rtable, &k_item->key, k_table->params);
    if (obj)
        rhashtable_remove_fast(&k_table->rtable, &obj->linkage, k_table->params);
    rcu_read_unlock();
    return obj;
}


void kernel_hashtable_iter_stop(hashtable_iter_t iter)
{
    kernel_iter kiter = get_kerneliter(iter);
    rhashtable_walk_stop(&kiter->iter);
    rhashtable_walk_exit(&kiter->iter);
}

void monlib_kernel_hashtable_init(struct monlib_os *os_ctx)
{
    hashtable_api hs_api = {
        .alloc = kernel_hashtable_alloc,
        .insert = kernel_hashtable_insert,
        .get = kernel_hashtable_get,
        .remove = kernel_hashtable_remove,
        .free = kernel_hashtable_free,

        .item_alloc = kernel_hashtable_item_alloc,
        .item_val = kernel_hashtable_item_get_val,
        .item_free = kernel_hashtable_item_free,

        .iter_start = kernel_hashtable_iter_start,
        .iter_next = kernel_hashtable_iter_next,
        .iter_stop = kernel_hashtable_iter_stop
    };
    os_ctx->hashtable = hs_api;
}

#define CHECK_MSG(x, args...) WARN_ON(!(x)); if(!(x)) { printk(args); return -1; }
#define CHECK(x) WARN_ON(!(x)); if(!(x)) { return -1; }

int monlib_kernel_hashtable_test(struct monlib_os *os_ctx)
{
    int i = 0;
    int *content;
    hashtable_item_t item;
    hashtable_iter_t iter;
    hashtable_item_t li;
    hashtable_item_t hi;
    hashtable_api hs_api = os_ctx->hashtable;
    hashtable_t hashtable = hs_api.alloc();
    CHECK(hashtable != NULL);

    for(i = 0; i < 32; i++) {
        content = kmalloc(sizeof(int), GFP_KERNEL);
        if(!content) {
            return -1;
        }
        *content = i+10;
        hi = hs_api.item_alloc(i, content);
        hs_api.insert(hashtable, hi);
    }

    for(i = 0; i < 64; i++) {
        li = hs_api.get(hashtable, i);
        if(i < 32) {
            CHECK_MSG(li != NULL, "Key: %d not found", i);
            content = (int*)(hs_api.item_val(li));
            CHECK(content != NULL);
            CHECK(*content == i+10);
        } else {
            CHECK(li == NULL);
        }
    }

    i = 0;
    iter = hs_api.iter_start(hashtable);
    CHECK(iter != NULL);
    while((item = hs_api.iter_next(iter)) != NULL) {
        i++;
        hs_api.remove(hashtable, item, iter);
        content = (int*)(hs_api.item_val(item));
        kfree(content);
    }
    hs_api.iter_stop(iter);
    CHECK(i == 32);
    hs_api.free(hashtable);
    return 0;
}
