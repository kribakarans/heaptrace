
#include "test.h"

int main()
{
	init_heap_trace();

	void *p1 = malloc(10);
	void *p2 = calloc(1, 10);
	void *p3 = realloc(p2, 10);
	char *p4 = strdup("I am from strdup...");
	char *p5 = NULL;

	asprintf(&p5, "I am from asprintf...\n");

	printf("\n%s %s p1 %p\n", __FILE__, __func__, p1);
	printf("%s %s p2 %p\n", __FILE__, __func__, p2);
	printf("%s %s p3 %p\n", __FILE__, __func__, p3);
	printf("%s %s p4 %p %s\n", __FILE__, __func__, p4, p4);
	printf("%s %s p5 %p %s\n", __FILE__, __func__, p5, p5);

	free(p5);
	free(p4);
	free(p3);
	//free(p2); /* modified by realloc */
	free(p1);

	return 0;
}

