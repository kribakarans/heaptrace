
#include "prime.h"
#include "heaptrace.h"
#include "heaptable.h"
#include "htmalloc.h"

extern void print_htbacktrace(uintptr_t key, htbt_t *bt);

static bool dbght = false;
#define DEBUG(x) ((dbght == true) ? (x) : (0))

ht_hash_table *heap_table;         /* Global heap table used by the client program */
static const int HT_PRIME_1 = 151; /* HT_PRIMEs Used for hashing algorithm         */
static const int HT_PRIME_2 = 163;
static ht_node_t HT_DELETED_NODE = { 0xFUL, { NULL, 0xFUL, 0xFUL }}; /* Used to mark the deleted node */

/*
 * Initialises a new empty hash table using a particular size index
 */
static ht_hash_table* ht_new_sized(const int size_index)
{
	ht_hash_table* ht = htmalloc(sizeof(ht_hash_table));
	ht->size_index = size_index;

	const int base_size = HEAPTABLE_SIZE << ht->size_index;
	ht->size = next_prime(base_size);

	ht->count = 0;
	ht->nodes = htcalloc((size_t)ht->size, sizeof(ht_node_t*));
	return ht;
}


/*
 * Initialises a new empty hash table
 */
ht_hash_table* create_heap_table(void)
{
	return ht_new_sized(0);
}


/*
 * Deletes an ht_node
 */
static void ht_delete_node(ht_node_t* i)
{
	enable_hook = false;

	DEBUG(HTLOG("deleted: 0x%lx", i->key));
	//print_htval("Delete node:", &(i->value));

	free(i->value.bt->frame);
	free(i->value.bt);
	free(i);

	enable_hook = true;

	return;
}


/*
 * Deletes the hash table
 */
void ht_del_hash_table(ht_hash_table* ht)
{
	// Iterate through nodes and delete any that are found
	for (int i = 0; i < ht->size; i++) {
		ht_node_t* node = ht->nodes[i];
		if (node != NULL && node != &HT_DELETED_NODE) {
			ht_delete_node(node);
		}
	}

	htfree(ht->nodes);
	htfree(ht);

	return;
}


/*
 * Resize the hash table
 */
static void ht_resize(ht_hash_table* ht, const int direction)
{
	const int new_size_index = ht->size_index + direction;
	if (new_size_index < 0) {
		// Don't resize down the smallest hash table
		return;
	}

	HTLOG("RESIZING HASH TABLE ...");
	// Create a temporary new hash table to insert nodes into
	ht_hash_table* new_ht = ht_new_sized(new_size_index);
	// Iterate through existing hash table, add all nodes to new
	for (int i = 0; i < ht->size; i++) {
		ht_node_t* node = ht->nodes[i];
		if (node != NULL && node != &HT_DELETED_NODE) {
			ht_insert(new_ht, node->key, node->value);
		}
	}

	// Pass new_ht and ht's properties. Delete new_ht
	ht->size_index = new_ht->size_index;
	ht->count = new_ht->count;

	// To delete new_ht, we give it ht's size and nodes 
	const int tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;

	ht_node_t** tmp_nodes = ht->nodes;
	ht->nodes = new_ht->nodes;
	new_ht->nodes = tmp_nodes;

	ht_del_hash_table(new_ht);

	return;
}


/*
 * Create new key-value node (node)
 */
static ht_node_t* ht_new_node(const uintptr_t key, const htval_t value)
{
	ht_node_t* i = htmalloc(sizeof(ht_node_t));
	i->key     = key;
	i->value   = value;
	//print_htval("New node:", &(i->value));

	return i;
}


/*
 * Returns the hash of 'key', an int between 0 and 'm'.
 */
static int ht_generic_hash(const uintptr_t key, const int a, const int m)
{
	int i = 0;
	long int hash = 0;
	size_t size = sizeof(key);

	for (i = 0; i < size/2; i++) {
		hash += labs((long)pow(a, i) * key);
		//HTLOG("hash of %lx: %ld", key, hash);
		hash %= m;
	}

	//HTLOG("hash of %lx: %ld", key, hash);

	return (int)hash;
}


static int ht_hash(const uintptr_t key, const int num_buckets, const int attempt)
{
	const int hash_a = ht_generic_hash(key, HT_PRIME_1, num_buckets);
	const int hash_b = ht_generic_hash(key, HT_PRIME_2, num_buckets);
	const int hash = (hash_a + (attempt * (hash_b + 1))) % num_buckets;

	//HTLOG("key: %lx hash: %d", key, hash);

	return hash;
}


/*
 * Returns the value associated with 'key', or -1 if the key doesn't exist
 */
ht_node_t *ht_search(ht_hash_table* ht, const uintptr_t key)
{
	int index = ht_hash(key, ht->size, 0);
	ht_node_t *node = ht->nodes[index];
	int i = 1;
	while (node != NULL && node != &HT_DELETED_NODE) {
		if (node->key == key) {
			return node;
		}
		index = ht_hash(key, ht->size, i);
		node = ht->nodes[index];
		i++;
	}

	HTLOG("node not found !!! key=%lx", key);

	return NULL;
}


/*
 * Deletes key's node from the hash table. Does nothing if 'key' doesn't exist
 */
void ht_delete(ht_hash_table* ht, const uintptr_t key)
{
	// Resize if load < 0.1
	const int load = ht->count * 100 / ht->size;
	if (load < 10) {
		ht_resize(ht, -1);
	}

	int index = ht_hash(key, ht->size, 0);
	ht_node_t* node = ht->nodes[index];
	int i = 1;
	while (node != NULL && node != &HT_DELETED_NODE) {
		if (node->key == key) {
			ht_delete_node(node);
			ht->nodes[index] = &HT_DELETED_NODE;
		}
		index = ht_hash(key, ht->size, i);
		node = ht->nodes[index];
		i++;
	}
	ht->count--;

	return;
}


/*
 * Inserts the 'key': 'value' pair into the hash table
 */
void ht_insert(ht_hash_table* ht, const uintptr_t key, const htval_t value)
{
	//HTLOG("ht: %p key: %lx value: %lx", ht, key, value);

	// Resize if load > 0.7
	const int load = ht->count * 100 / ht->size;
	if (load > 70) {
		//HTLOG("resizing table ... key: %s value: %s", key, value);
		ht_resize(ht, 1);
	}

	ht_node_t* node = ht_new_node(key, value);

	// Cycle though filled buckets until we hit an empty or deleted one
	int index = ht_hash(node->key, ht->size, 0);
	//HTLOG("hashed index: %d", index);

	ht_node_t* cur_node = ht->nodes[index];
	int i = 1;
	while (cur_node != NULL && cur_node != &HT_DELETED_NODE) {
		if (cur_node->key == key) {
			ht_delete_node(cur_node);
			ht->nodes[index] = node;
			//HTLOG("loop return: index: %d", index);
			return;
		}
		index = ht_hash(node->key, ht->size, i);
		cur_node = ht->nodes[index];
		i++;
	}

	// index points to a free bucket
	ht->nodes[index] = node;
	ht->count++;
	DEBUG(HTLOG("inserted: index: %3d key: 0x%lx", index, key));

	return;
}


void print_heap_table(ht_hash_table* ht)
{
	htprintf("\n--heap-table--\n");
	for (int i = 0; i < ht->size; i++) { /* iterate each nodes in table */
		ht_node_t* node = ht->nodes[i];
		if (node == NULL) {
			htprintf("%.4d: --\n", i);
		} else if (node == &HT_DELETED_NODE) {
			htprintf("%.4d: -- <deleted>\n", i);
		} else {
			htprintf("%.4d: -- %lx::%lx\n", i, node->key, node->value.hptr);
		}
	}

	htprintf("--end--\n");

	return;
}

void print_ht_report(ht_hash_table* ht)
{
	int i = 0, c =0;

	enable_hook = false;

	//print_heap_table(ht);
	htprintf("\nHEAP TRACE SUMMARY:\n");
	for (i = 0; i < ht->size; i++) {
		ht_node_t* node = ht->nodes[i];
		if ((node != NULL) && (node != &HT_DELETED_NODE)) {
			c++;
			print_htbacktrace(node->key, node->value.bt);
		}
	}

	if (c == 0) {
		htprintf("\nHeap trace: All heap blocks were freed !!!\n\n");
	} else {
		htprintf("\nHeap trace: Memory leak at %d blocks !!!\n\n", c);
	}

	enable_hook = true;

	return;
}


//EOF
