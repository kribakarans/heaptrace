
#include "heaptrace.h"
#include "htmalloc.h"
#include "heaptable.h"

bool    enable_hook = false;
static bool dbghook = false;
#define DEBUG(x) ((dbghook == true) ? (x) : (0))

/*
  All Hook functions does the same operation mentioned below:
  1. enable_hook = false; -- deactivate hooks before calling libc apis
  2. retval = malloc(10); -- call libc api. this will not call our hooks
  3. enable_hook = true;  -- enable hooks for future hooking
*/

void
hook_free (void *ptr, void *caller)
{
	enable_hook = false;
	free(ptr);
	DEBUG(HTLOG("ptr: %p", ptr));
	enable_hook = true;

	ht_delete(heap_table, (uintptr_t)ptr);

	return;
}

void*
hook_calloc (size_t nmemb, size_t size, void *caller)
{
	void *result = NULL;
	htval_t value = {0};

	enable_hook = false;
	result = calloc(nmemb, size);
	DEBUG(HTLOG("retval: %p", result));
	enable_hook = true;

	value.nframes = 0;
	value.hptr = (uintptr_t)result;
	if (backtrace_simple(htstate, 1, backtrace_simple_cb, NULL, &value) < 0) {
		HTLOG("backtrace_simple failed !!!");
	}

	ht_insert(heap_table, (uintptr_t)result, value);

	return result;
}

void*
hook_realloc (void *ptr, size_t size, void *caller)
{
	void *result = NULL;
	htval_t value = {0};

	enable_hook = false;
	result = realloc(ptr, size);
	DEBUG(HTLOG("retval: %p", result));
	enable_hook = true;

	value.nframes = 0;
	value.hptr = (uintptr_t)result;
	if (backtrace_simple(htstate, 1, backtrace_simple_cb, NULL, &value) < 0) {
		HTLOG("backtrace_simple failed !!!");
	}

	ht_insert(heap_table, (uintptr_t)result, value);

	return result;
}

void*
hook_malloc (size_t size, void *caller)
{
	void *result = NULL;
	htval_t value = {0};

	enable_hook = false;
	result = malloc(size);
	DEBUG(HTLOG("retval: %p", result));
	enable_hook = true;

	value.nframes = 0;
	value.hptr = (uintptr_t)result;
	if (backtrace_simple(htstate, 1, backtrace_simple_cb, NULL, &value) < 0) {
		HTLOG("backtrace_simple failed !!!");
	}

	ht_insert(heap_table, (uintptr_t)result, value);

	return result;
}

void
free (void *ptr)
{
	void *caller = __builtin_return_address(0);

	if (enable_hook)
		hook_free(ptr, caller);
	else
		__libc_free(ptr);

	return;
}

void*
realloc (void *ptr, size_t size)
{
	void *caller = __builtin_return_address(0);

	if (enable_hook)
		return hook_realloc(ptr, size, caller);

	return __libc_realloc(ptr, size);
}

void*
calloc (size_t nmemb, size_t size)
{
	void *caller = __builtin_return_address(0);

	if (enable_hook)
		return hook_calloc(nmemb, size, caller);

	return __libc_calloc(nmemb, size);
}


void*
malloc (size_t size)
{
	void *caller = __builtin_return_address(0);
	if (enable_hook)
		return hook_malloc(size, caller);

	return __libc_malloc(size);
}

//EOF
