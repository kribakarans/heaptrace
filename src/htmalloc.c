
#include "heaptrace.h"

static bool dbghtallox = false;
#define DEBUG(x) ((dbghtallox == true) ? (x) : (0))

/*
   Hook funcions does same task mentioned below:
   1. Disable malloc hooks
   2. Call libc functions (this will not call our hooks)
   3. Enable malloc hooks again
   4. Validate the return values
*/

static void *alloc_error(size_t size)
{
	if (size == 0)
		return NULL;

	htprintf("Out of memory: %s\n", strerror(errno));

	abort();
}

void *htmalloc (size_t size)
{
	void *ptr = NULL;

	enable_hook = false;
	ptr = malloc(size);
	DEBUG(HTLOG("ptr=%p", ptr));
	enable_hook = true;

	if (ptr == NULL)
		return alloc_error(size);

	return ptr;
}

void *htcalloc (size_t nmemb, size_t size)
{
	void *ptr = NULL;

	enable_hook = false;
	ptr = calloc (nmemb, size);
	DEBUG(HTLOG("ptr=%p", ptr));
	enable_hook = true;

	if (ptr == NULL)
		return alloc_error(nmemb*size);

	return ptr;
}

void *htrealloc (void *ptr, size_t size)
{
	void *retval = NULL;

	enable_hook = false;
	retval = realloc(ptr, size);
	DEBUG(HTLOG("ptr=%p", retval));
	enable_hook = true;

	if (retval == NULL)
		return alloc_error(size);

	return retval;
}

char *htstrdup (const char *s)
{
	void *ptr = NULL;

	enable_hook = false;
	ptr = htmalloc(strlen(s)+1);
	strcpy (ptr, s);
	DEBUG(HTLOG("ptr=%p data=%s", ptr, (char *)ptr));
	enable_hook = true;

	return (char*) ptr;
}

void htfree(void *ptr)
{
	enable_hook = false;
	DEBUG(HTLOG("ptr=%p", ptr));
	free(ptr);
	enable_hook = true;

	return;
}


//EOF
