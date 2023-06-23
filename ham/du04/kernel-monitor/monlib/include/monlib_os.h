#ifndef MONLIB_OS_H
#define MONLIB_OS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void* hashtable_t;
typedef void* hashtable_item_t;
typedef void* hashtable_iter_t;

// Hash table functions
typedef hashtable_t (*hashtable_api_alloc)(void);
typedef void (*hashtable_api_free)(hashtable_t table);
typedef void (*hashtable_api_insert)(hashtable_t table, hashtable_item_t item);
typedef hashtable_item_t (*hashtable_api_get)(hashtable_t table, int key);
//Note retrieved items needs to be deallocated
typedef hashtable_item_t (*hashtable_api_remove)(hashtable_t table, hashtable_item_t item, hashtable_iter_t iter);
//Note that when iterating the iterator needs to be passed to the removal function to update its state, otherwise pass null

// Hash table item functions
typedef hashtable_item_t (*hashtable_api_item_alloc)(int key, void *val);
typedef void (*hashtable_api_item_free)(hashtable_item_t item);
typedef void* (*hashtable_api_item_get_val)(hashtable_item_t item);

// Hash table iteration functions
typedef hashtable_iter_t (*hashtable_api_iter_start)(hashtable_t table);
typedef hashtable_item_t (*hashtable_api_iter_next)(hashtable_iter_t iter);
typedef void (*hashtable_api_iter_stop)(hashtable_iter_t iter);
// Note that iteration inside kernel should be done in sequence start while(next != NULL) stop
// Removal of object is allowed inside walking

typedef struct hashtable_api {
    hashtable_api_alloc alloc;
    hashtable_api_insert insert;
    hashtable_api_get get;
    hashtable_api_remove remove;
    hashtable_api_free free;

    hashtable_api_item_alloc item_alloc;
    hashtable_api_item_get_val item_val;
    hashtable_api_item_free item_free;

    hashtable_api_iter_start iter_start;
    hashtable_api_iter_next iter_next;
    hashtable_api_iter_stop iter_stop;
} hashtable_api;


enum {
	HEXDUMP_PREFIX_NONE,
	HEXDUMP_PREFIX_ADDRESS,
	HEXDUMP_PREFIX_OFFSET
};

typedef void* (*monlib_malloc)(unsigned int);
typedef void (*monlib_free)(void *);
typedef int (*monlib_printf)(const char * format, ... );
typedef void (*monlib_print_hexdump)(const char *prefix_str, int prefix_type, const void *buf, long unsigned int len);

struct monlib_os {
    monlib_malloc alloc;
    monlib_free free;
    hashtable_api hashtable;

    /* Debug interface functions. */
    monlib_printf printf;
    monlib_print_hexdump hexdump;
};
#ifdef __cplusplus
}
#endif

#endif
