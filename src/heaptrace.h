
#ifndef __HEAPTRACE_H__
#define __HEAPTRACE_H__

#include <backtrace.h>
#include    <math.h>
#include   <errno.h>
#include   <stdio.h>
#include  <stdlib.h>
#include  <stdint.h>
#include  <string.h>
#include  <malloc.h>
#include <stdbool.h>

#define VERBOSE_BACKTRACE

enum ht_limits {
	HEAPTABLE_SIZE = 128,
	BT_DEPTH = 32
};

extern bool enable_hook;
extern struct backtrace_state *htstate;

#define htprintf(fmt, args...) (fprintf(stderr, fmt, ##args))
#define HTLOG(fmt, args...)    (fprintf(stderr, "%17s:%-4d %17s: " fmt "\n", __FILE__, __LINE__, __func__, ##args))

#define HT_CALL(fx)                    \
do {                                   \
	enable_hook = false;               \
	/* printf("%s.%s\n", __func__, #fx); */ \
	(fx);                              \
	enable_hook = true;                \
} while(0);

extern void  __libc_free(void *ptr);
extern void *__libc_malloc(size_t size);
extern void *__libc_realloc(void *ptr, size_t size);
extern void *__libc_calloc(size_t nmemb, size_t size);

extern void  free(void *ptr);
extern void *malloc(size_t size);
extern void *realloc(void *ptr, size_t size);
extern void *calloc(size_t nmemb, size_t size);

extern void init_heap_trace(void);
extern void print_heap_summary(void);
extern void ht_callback(const char *name, void *(*fptr)());

extern int backtrace_simple_cb(void *data, uintptr_t pc);

#endif
