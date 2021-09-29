
#ifndef __HTMALLOC_H__
#define __HTMALLOC_H__

extern void  htfree(void *ptr);
extern void *htmalloc (size_t size);
extern char *htstrdup (const char *s);
extern void *htrealloc (void *ptr, size_t size);
extern void *htcalloc (size_t nmemb, size_t size);

#endif
