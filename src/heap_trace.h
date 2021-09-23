
#ifndef __HEAPTRACE_H__
#define __HEAPTRACE_H__

#include <stdio.h>
#include <malloc.h>

#define SAVE_SYSTEM_HOOKS()             \
do {                                    \
	sys_malloc_hook  = __malloc_hook;   \
	sys_realloc_hook = __realloc_hook;  \
	sys_free_hook    = __free_hook;     \
} while(0);

#define RESTORE_LOCAL_HOOKS()         \
do {                                    \
	__malloc_hook  = ht_malloc_hook;     \
	__realloc_hook = ht_realloc_hook;    \
	__free_hook    = ht_free_hook;       \
} while(0);

#define RESTORE_SYSTEM_HOOKS(void)      \
do {                                    \
	__malloc_hook  = sys_malloc_hook;   \
	__realloc_hook = sys_realloc_hook;  \
	__free_hook    = sys_free_hook;     \
} while(0);

/* Variables to save original hooks */
void  (*sys_free_hook)(void *ptr, const void *caller);
void *(*sys_malloc_hook)(size_t size, const void *caller);
void *(*sys_realloc_hook)(void *ptr, size_t size, const void *caller);

/* Prototypes for our hooks */
void  init_heap_trace(void);
void  ht_free_hook(void *ptr, const void *caller);
void *ht_malloc_hook(size_t size, const void *caller);
void *ht_realloc_hook(void *ptr, size_t size, const void *caller);

#endif
