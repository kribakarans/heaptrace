
#ifndef __HEAPTABLE_H__
#define __HEAPTABLE_H__

#define MAX_FRAMES 32

#include  <stdint.h>

/* caller details who allocated heap memory */
typedef struct htval {
	uintptr_t hptr;   /* leaked heap pointer address   */
	int nframes;      /* number of back-tracked frames */
	uintptr_t pc[MAX_FRAMES]; /* program counter values  */
} htval_t;

/* key:value pair (node) of the hash table */
typedef struct ht_node {
	uintptr_t key;
	htval_t value;
} ht_node_t;

typedef struct {
	int size_index;
	int size;
	int count;
	ht_node_t **nodes;
} ht_hash_table;

extern ht_hash_table *heap_table;

extern ht_hash_table *create_heap_table(void);
extern void print_ht_report(ht_hash_table* ht);
extern void print_heap_table(ht_hash_table *ht);
extern void ht_del_hash_table(ht_hash_table *ht);
extern void ht_delete(ht_hash_table *ht, const uintptr_t key);
extern ht_node_t *ht_search(ht_hash_table *ht, const uintptr_t key);
extern void ht_insert(ht_hash_table *ht, const uintptr_t key, const htval_t value);
extern void print_htbacktrace(uintptr_t key, htval_t *value);

#endif

