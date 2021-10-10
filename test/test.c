
#include "test.h"

int main()
{
	init_heap_trace();

	#if 1
	void *p1 = malloc(10);
	char *p5 = NULL;
	asprintf(&p5, "I am from asprintf...\n");
	void *p2 = calloc(1, 10);
	void *p3 = realloc(p2, 10);
	char *p4 = strdup("I am from strdup...");

	fprintf(stderr, "\n%s %s ptr-1 %p\n",  __FILE__, __func__, p1);
	fprintf(stderr, "%s %s ptr-2 %p\n",    __FILE__, __func__, p2);
	fprintf(stderr, "%s %s ptr-3 %p\n",    __FILE__, __func__, p3);
	fprintf(stderr, "%s %s ptr-4 %p %s\n", __FILE__, __func__, p4, p4);
	fprintf(stderr, "%s %s ptr-5 %p %s\n", __FILE__, __func__, p5, p5);

	//free(p5);
	//free(p4);
	//free(p3);
	//free(p2); /* modified by realloc */
	//free(p1);
	#endif

	return 0;
}

