
#define _GNU_SOURCE
#include <stdio.h>

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

htsym_t *parse_htsymbols(const char *bt)
{
	int i = 0;
	char    *ptr = NULL;
	htsym_t *sym = NULL;

	DEBUG(HTLOG("%s", bt));

	do {
		if (bt == NULL) {
			fprintf(stderr, "%s BT is NULL\n", __func__);
			break;
		}

		sym = (htsym_t *)calloc(1, sizeof *sym);
		if (sym == NULL) {
			perror("parse_btinfo: malloc failed");
			abort();
		}

		ptr = (char *)bt;

		for (i = 0; ptr[i] != '\0'; i++) {
			if (ptr[i] == '(') {
				strncpy(sym->exe, ptr, i);
				sym->exe[i++] = '\0';
				ptr = &ptr[i];

				for (i = 0; ptr[i] != '\0'; i++) {
					if (ptr[i] == '+') {
						strncpy(sym->api, ptr, i);
						sym->api[i++] = '\0';
						ptr = &ptr[i];

						for (i = 0; ptr[i] != '\0'; i++) {
							if (ptr[i] == ')') {
								strncpy(sym->indx, ptr, i);
								sym->indx[i++] = '\0';
								ptr = &ptr[i + 2];

								for (i = 0; ptr[i] != '\0'; i++) {
									if (ptr[i] == ']') {
										strncpy(sym->ptr, ptr, i);
										sym->ptr[i] = '\0';
										goto finish;
									}
								}
							}
						}
					}
				}
			}
		}
	} while(0);

finish:
	//HTLOG("'%s' '%s' '%s' '%s' '%s'\n", sym->ptr, sym->indx, sym->api, sym->exe, sym->file);
	return sym;
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

/*
 * Update symbols @htsyms with addr2line output
 */
int get_addrline(htsym_t *htsyms)
{
	#define NSYMS 8

	int len = 0,i =  0;
	int    nbytes =  0;
	int    retval = -1;
	int fileindex =  2;
	FILE      *fp = NULL;
	char     *cmd = NULL;
	char *saveptr = NULL;
	char      *arr[NSYMS] = { 0 };
	char buff[HTLINE_MAX] = { 0 };

	do {
		if (htsyms == NULL) {
			fprintf(stderr, "%s HT symbols is NULL\n", __func__);
			break;
		}

		asprintf(&cmd, "addr2line -s -p -f -e %s %s", htsyms->exe, htsyms->indx);

		fp = popen(cmd, "r");
		if (fp == NULL) {
			htprintf("addr2line popen failed: %s [%s]\n", strerror(errno), cmd);
			retval = -1;
			break;
		}

		nbytes = fread(buff, sizeof(char), sizeof(buff)-1, fp);
		if (nbytes < 0) {
			htprintf("addr2line fread failed: %s [%s]\n", strerror(errno), cmd);
			retval = -1;
			break;
		}

		pclose(fp);

		DEBUG(HTLOG("%s\n", buff));
		arr[i] = strtok_r(buff, " ", &saveptr);
		/* skip unresolved address (??) */
		if (strcmp(arr[i], "??") == 0) {
			retval = 0;
			break;
		}

		/* if htsyms->api is empty, try to get fun-name from addr2line */
		if ((strlen(htsyms->api) <= 0) && (strcmp(arr[i], "??") != 0)) {
			strncpy(htsyms->api, arr[i], sizeof(htsyms->api)-1);
		}

		while (arr[i] != NULL) {
			arr[++i] = strtok_r(NULL, " ", &saveptr);
		}

		/* get filename and line number  */
		len = strlen(arr[fileindex]);
		if (len != 0) {
			if (arr[fileindex][len-1] == '\n') {
				arr[fileindex][len-1] = '\0';
			}
			if (strncmp(arr[fileindex], "??", 2) != 0) {
				snprintf(htsyms->file, (sizeof(htsyms->file)-1), "(%s)", arr[fileindex]);
			}
		}

		retval = 0;
	} while(0);

	return retval;
}


void print_htbacktrace(uintptr_t key, htbt_t *bt)
{
	int         i = 0;
	htsym_t *htsyms = NULL;
	char buff[HTLINE_MAX] = {0};

	htprintf("Backtrace of heap-pointer : 0x%lx\n", key);

	for (i = 0; i < bt->depth; i++) {
		if (i > 1) { /* skip print_htbacktrace call traces */
			strncpy(buff, bt->frame[i], sizeof(buff)-1);

			htsyms = parse_htsymbols(buff);
			if (htsyms != NULL) {
				get_addrline(htsyms); /* get file name and number */
			}

			htprintf("  |_ %s %20s() %s %s\n", htsyms->ptr, htsyms->api, htsyms->exe, htsyms->file);
			memset(buff, 0x00, sizeof(buff));
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

	/* skip custom backtrace functions */
	for (int i = 0; i < bt->depth; i++) {
		/* skip glibc printf */
		if (strstr(bt->frame[i], "_IO_printf") != NULL) {
			free(bt->frame);
			bt = (htbt_t *)255;
			break;
		}
	}

	enable_hook = true;

	return bt;
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

	return;
}

void print_heap_summary(void)
{
	print_ht_report(heap_table);
	ht_del_hash_table(heap_table);

	return;
}


//EOF
