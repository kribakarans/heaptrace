
#include "heap_trace.h"

void ht_free_hook(void *ptr, const void *caller)
{
	/* Restore system hooks */
	RESTORE_SYSTEM_HOOKS();

	printf("%s: sys_free_hook: %p\n", __func__, ptr);

	/* Call libc free recursively */
	free(ptr);

	/* Save underlying hooks */
	SAVE_SYSTEM_HOOKS();

	/* Restore our own hooks */
	RESTORE_LOCAL_HOOKS();

	return;
}

void *ht_realloc_hook(void *ptr, size_t size, const void *caller)
{
	void *retval = NULL;

	/* Restore system hooks */
	RESTORE_SYSTEM_HOOKS();

	/* Call libc malloc recursively */
	retval = realloc(ptr, size);
	printf("%s: ht_realloc_retval: %p\n", __func__, retval);

	/* Save underlying hooks */
	SAVE_SYSTEM_HOOKS();

	/* Restore our own hooks */
	RESTORE_LOCAL_HOOKS();

	return retval;
}

void *ht_malloc_hook(size_t size, const void *caller)
{
	void *retval = NULL;

	/* Restore system hooks */
	RESTORE_SYSTEM_HOOKS();

	/* Call libc malloc recursively */
	retval = malloc(size);
	printf("%s: ht_malloc_retval: %p\n", __func__, retval);

	/* Save underlying hooks */
	SAVE_SYSTEM_HOOKS();

	/* Restore our own hooks */
	RESTORE_LOCAL_HOOKS();

	return retval;
}

void init_heap_trace(void)
{
	SAVE_SYSTEM_HOOKS();
	RESTORE_LOCAL_HOOKS();

	/* This will call malloc internally */
	//printf("%s: sys_malloc_hook: %p ht_malloc_hook: %p\n", __func__, sys_malloc_hook, ht_malloc_hook);
	//printf("%s: sys_free_hook: %p ht_free_hook: %p\n", __func__,  sys_free_hook, ht_free_hook);

	return;
}

#if 0
int main()
{
    init_heap_trace();

    void *p1 = malloc(10);
    void *p2 = calloc(1, 10);
    void *p3 = realloc(p2, 10);

    printf("p1 %p\n", p1);
    printf("p2 %p\n", p2);
    printf("p3 %p\n", p3);

    free(p3);
    //free(p2); /* modified by realloc */
    free(p1);

    return 0;
}
#endif

//EOF
