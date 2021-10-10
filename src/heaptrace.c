
#define _GNU_SOURCE
#include <dlfcn.h>
#include <execinfo.h>
#include <assert.h>

#include "heaptable.h"
#include "heaptrace.h"
#include "htmalloc.h"

struct backtrace_state *htstate;
static bool   dbgtrace = false;
#define DEBUG(x) ((dbgtrace == true) ? (x) : (0))

void print_ht_keyvalue(char *msg, htval_t *value);
void print_htbacktrace(uint_least64_t key, htval_t *value);

void print_ht_keyvalue(char *msg, htval_t *value)
{
	if (value == NULL) {
		HTLOG("key-value is NULL !!!");
		return;
	}

	htprintf("\n%s\n"
	         "  Heap.ptr : %lx\n"
	         "  Nframes : %d\n",
	         msg, value->hptr, value->nframes
	);

	print_htbacktrace(0xFUL, value);

	return;
}

void print_htitem(char *msg, ht_node_t *i)
{
	if (i == NULL) {
		HTLOG("key-value is NULL !!!");
		return;
	}

	htprintf("\n%s key: %lx\n"
	         "  Heap.ptr : %lx\n"
	         "  Nframes  : %d\n",
	         msg, i->key, i->value.hptr, i->value.nframes
	);

	print_htbacktrace(i->key, &(i->value));

	return;
}

static void backrace_state_error(void *data, const char *msg, int errnum)
{
	char *caller = (char *)data;
	htprintf("heap_trace: backtrace_create_state() failed at %s() !!! [%s/%s]\n", caller, msg, strerror(errno));

	return;
}

void init_heap_trace(void)
{
	static int htinit = 0;

	/* prevent multiple inits */
	if (htinit == 1) {
		printf("heap-trace is initiated already !!!\n");
		return;
	}

	htinit++;
	enable_hook = true;

	heap_table = create_heap_table();
	if (heap_table == NULL) {
		HTLOG("failed to create Heap Table !!!");
		exit(1);
	}

	atexit(&print_heap_summary);
	DEBUG(HTLOG("Heap-Table: %p", heap_table));

	htstate = backtrace_create_state(NULL, 0, backrace_state_error, NULL);

	return;
}

void print_heap_summary(void)
{
	static int called = 0;

	/* prevent multiple calls */
	if (called == 1) {
		printf("heap-trace: summary already printed !!!\n");
		return;
	}

	called++;

	print_ht_report(heap_table);
	ht_del_hash_table(heap_table);

	return;
}

static void backtrace_error(void *data, const char *msg, int errnum)
{
	fprintf(stderr, "Backtrace failed !!! [%s/%s]\n", msg, strerror(errno));

	return;
}

static int backtrace_full_cb_1 (void *data, uintptr_t pc, const char *file, int lineno, const char *function)
{
	if (file != NULL || function != NULL) {
		htprintf("  |__  0x%-13lx: %25s (in %s:%d)\n", pc, function, file, lineno);
	}

	return 0;
}

/*
  Print verbose backtrace with help of dladdr library
*/
static int backtrace_full_cb_2(void *data, uintptr_t pc, const char *file, int lineno, const char *function)
{
	int       rc = -1;
	int   retval = -1;
	Dl_info info = {0};

	do {
		rc = dladdr((void*)pc ,&info);

		if (rc != 0) {
			htprintf("  |__  0x%-13lx: %25s (in %s:%d)\n", pc, 
			                 ((info.dli_sname)?info.dli_sname:(function?function:"??")),
			                 ((file)?file:info.dli_fname), lineno);
		}

		retval = 0;
	} while(0);

	return retval;
}

void print_htbacktrace(uintptr_t key, htval_t *value)
{
	htprintf("Backtrace of heap-pointer : 0x%lx\n", key);
	DEBUG(htprintf("%s: key: %lx hptr: %lx nframes: %d \n", __func__, key, value->hptr, value->nframes));

	if (key != value->hptr) {
		htprintf("%s: invalid key-value pair !!! [%lx != %lx]\n", __func__, key, value->hptr);
	}

	for (int i = 0; i < value->nframes; i++) {
		#ifdef VERBOSE_BACKTRACE
		backtrace_pcinfo(htstate, value->pc[i], backtrace_full_cb_2, backtrace_error, NULL);
		#else
		backtrace_pcinfo(htstate, value->pc[i], backtrace_full_cb_1, backtrace_error, NULL);
		#endif
	}

	htprintf("\n");

	return;
}

int backtrace_simple_cb(void *data, uintptr_t pc)
{
	htval_t *value = (htval_t *)data;

	assert(data != NULL);

	value->pc[value->nframes] = pc;
	DEBUG(HTLOG("saving frame[%d]: value: %p hptr %lx pc: %lx",
	             value->nframes, value, value->hptr, value->pc[value->nframes]));
	value->nframes = (value->nframes + 1);

	return 0;
}

//EOF
