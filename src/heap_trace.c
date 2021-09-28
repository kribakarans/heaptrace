
#include <execinfo.h>

#include "heap_table.h"
#include "heap_trace.h"
#include "htmalloc.h"

static bool dbgtrace = false;
#define DEBUG(x) ((dbgtrace == true) ? (x) : (0))

htbt_t *ht_backtrace(void);
void print_htval(char *msg, htval_t *value);
void print_htbacktrace(uint_least64_t key, htbt_t *bt);


/*
 * HT_CALLBACK:
 * 1. disable heap_trace hooks
 * 2. call function @fptr
 * 3. enable hooks after the funtion returns
 */
void ht_callback(const char *name, void *(*fptr)())
{
	htprintf("%s calling ... %s()\n", __func__, name);

	fptr(); /* callback */

	return;
}

void print_htval(char *msg, htval_t *value)
{
	if (value == NULL) {
		HTLOG("key-value is NULL !!!");
		return;
	}

	htprintf("\n%s\n"
	         "  Fun.ptr  : %lx\n"
	         "  Heap.ptr : %lx\n",
	         msg, value->fptr, value->hptr
	);

	print_htbacktrace(0xFUL, value->bt);

	return;
}

void print_htitem(char *msg, ht_item *i)
{
	if (i == NULL) {
		HTLOG("key-value is NULL !!!");
		return;
	}

	htprintf("\n%s key: %lx\n"
	         "  Fun.ptr  : %lx\n"
	         "  Heap.ptr : %lx\n",
	         msg, i->key, i->value.fptr, i->value.hptr
	);

	print_htbacktrace(i->key, i->value.bt);

	return;
}
void test_heap_table(void)
{
	htval_t value = {0};

	ht_hash_table* ht = create_heap_table();

	value.fptr = 0xF1; value.hptr = 0xD1;
	value.bt = ht_backtrace();
	ht_insert(ht, 0xA1, value);

	#if 1
	value.fptr = 0xF2; value.hptr = 0xD2; 
	value.bt = ht_backtrace();
	ht_insert(ht, 0xA2, value);

	value.fptr = 0xF3; value.hptr = 0xD3;
	value.bt = ht_backtrace();
	ht_insert(ht, 0xA3, value);

	value.fptr = 0xF4; value.hptr = 0xD4;
	value.bt = ht_backtrace();
	ht_insert(ht, 0xA4, value);

	value.fptr = 0xF5;value.hptr = 0xD5;
	value.bt = ht_backtrace();
	ht_insert(ht, 0xA5, value);

	value.fptr = 0xF6; value.hptr = 0xD6;
	value.bt = ht_backtrace();
	ht_insert(ht, 0xA6, value);

	print_htitem("ht_search:", ht_search(ht, 0xA1));
	print_htitem("ht_search:", ht_search(ht, 0xA2));
	print_htitem("ht_search:", ht_search(ht, 0xA9));

	print_heap_table(ht);

	ht_delete(ht, 0xA3);
	ht_delete(ht, 0xA5);
	ht_delete(ht, 0xA1);
	ht_delete(ht, 0xA4);
	#endif

	print_heap_table(ht);

	ht_del_hash_table(ht);

	return;
}

void print_htbacktrace(uintptr_t key, htbt_t *bt)
{
	int         i = 0;
	#ifndef NATIVE_BT
	char     *ptr = NULL;
	char *saveptr = NULL;
	char buff[HTLINE_MAX] = {0};
	char objx[HTLINE_MAX] = {0};
	#endif

	htprintf("Backtrace of heap-pointer : 0x%lx\n", key);

	for (i = 0; i < bt->depth; i++) {
		if (i > 1) { /* skip print_htbacktrace call traces */
			#ifdef NATIVE_BT /* uncomment to print native backtrace   */
			htprintf(" |_ %s\n", bt->frame[i]);
			#else
			strncpy(buff, bt->frame[i], sizeof(buff) - 1);
			ptr = strtok_r(buff, "(", &saveptr);
			strncpy(objx, ptr, sizeof(objx)-1);
			while(ptr != NULL) {
				if (saveptr[0] == '+') {
					ptr = strtok_r(NULL, "+", &saveptr);
				} else {
					ptr = strtok_r(NULL, "+", &saveptr);
					htprintf("%25s() -- %s\n", ptr, objx);
				}
				break;
			}
			#endif
		}
	}

	puts("");

	return;
}

htbt_t *ht_backtrace(void)
{
	htbt_t *bt = NULL;
	void *buffer[BT_SIZE] = {0};

	enable_hook = false;

	bt = malloc(sizeof(*bt));

	/* fetch backtrace of the caller */
	bt->depth = backtrace(buffer, BT_SIZE);
	bt->frame = backtrace_symbols(buffer, bt->depth);
	if (bt->frame == NULL) {
		perror("backtrace_symbols failed");
		exit(EXIT_FAILURE);
	}

	#if 1
	for (int i = 0; i < bt->depth; i++) {
		if (strstr(bt->frame[i], "_IO_printf") != NULL) { /* skip libc printf */
			free(bt->frame);
			bt = (htbt_t *)255;
			break;
		}
	}
	#endif

	enable_hook = true;

	return bt;
}

void init_heap_trace(void)
{
	enable_hook = true;

	heap_table = create_heap_table();
	if (heap_table == NULL) {
		HTLOG("failed to create Heap Table !!!");
		exit(1);
	}

	DEBUG(HTLOG("Heap-Table: %p", heap_table));

	//test_heap_table();

	return;
}

void finish_heap_trace(void)
{
	print_ht_report(heap_table);
	ht_del_hash_table(heap_table);

	return;
}


//EOF
