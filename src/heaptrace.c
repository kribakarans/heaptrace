
#include <execinfo.h>

#include "heaptable.h"
#include "heaptrace.h"
#include "htmalloc.h"

static bool dbgtrace = false;
#define DEBUG(x) ((dbgtrace == true) ? (x) : (0))

htbt_t *ht_backtrace(void);
void print_ht_keyvalue(char *msg, htval_t *value);
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

void print_ht_keyvalue(char *msg, htval_t *value)
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

void print_htitem(char *msg, ht_node_t *i)
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

	return;
}

void print_heap_summary(void)
{
	print_ht_report(heap_table);
	ht_del_hash_table(heap_table);

	return;
}


//EOF
