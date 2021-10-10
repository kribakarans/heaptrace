
## Heap-Trace: Inline Heap Memory tracker
Heaptrace detects memory leak by hooking dynamic memory functions (e.g. malloc).<br>
Simply attach the shared library ```-lheaptrace``` to the target program while compiling.<br>
At exit, the target program itself prints the backtrace of leaked heap-memory pointers.<br>

Heaptrace intercepts the following library calls:
```
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void  free(void *ptr);
```
## Build Heap Trace from here
```
$ git clone https://github.com/kribakarans/heaptrace.git
$ cd heaptrace/
$ make
```
## Install Heap Trace from source
- Build package from source mentioned above
- Do ```make install``` to install heap-trace program locally to the current user
- Installation will create ```.lib``` and ```.include``` in ```$HOME``` path
- Shared library location ```$HOME/.lib/libheaptrace.so```
- Header files location ```$HOME/.include/heaptrace.h```
- Installation to ```root``` user is not implemented for now
- Run ```make uninstall``` to un-install ```heap-trace``` library
## Usage
**1. Add INCLUDE and LDFLAGS while building target program**
- Add ```-I /home/<user>/.include -Wl,-rpath=/home/<user>/.lib -L /home/<user>/.lib``` option in GCC command line (or)<br>
- Add below snippet in Makefile of target program
```
INCLUDE += -I$(HOME)/.include
LDFLAGS += -Wl,-rpath=$(HOME)/.lib -L$(HOME)/.lib -lheaptrace -lm -ldl -lbacktrace
``` 

**2. Attach heap-trace to target program**
- Add below code snippet in ```main()``` (recommended) to register ```heap-trace``` to target program
```
    init_heap_trace();
```
**3. Below shows the code snippet and compilation steps to use heap-trace**<br>
**main.c:**
```
#include <heaptrace.h>

int main()
{
	init_heap_trace();

	int *ptr = malloc(10);
	printf("%s ptr=%p\n", __func__, ptr);

	/*
	   returning program without freeing
	   the heap memory pointer - ptr
	 */
	return 0;
}
```
**GCC:**
```
$ cc -g3 main.c -I $HOME/.include -Wl,-rpath=$HOME/.lib -L $HOME/.lib -lheaptrace -lm -ldl -lbacktrace
```
**Output:**
```
$ ./a.out 
main ptr=0x55ac31fb86e0

HEAP TRACE SUMMARY:
Backtrace of heap-pointer : 0x55ac31fb86e0
  |__  0x7f4fb1c451cf :                    malloc (in ./hthooks.c:137)
  |__  0x55ac31505483 :                      main (in ./main.c:7)
  |__  0x7f4fb18e40b2 :         __libc_start_main (in /lib/x86_64-linux-gnu/libc.so.6:0)
  |__  0x55ac315053ad :                        ?? (in ./a.out:0)

Heap trace: Memory leak at 1 blocks !!!
```

## Changelog
**Oct 11 2021 03:10**
- Implemented ```libbacktrace``` to print filename and line number
- Removed ```addr2line``` method and replaced with ```libbacktrace``` library
- Faster than ```gnubacktrace + addr2line``` that uses ```popen()``` calls
- Fixed crash by calling multiple ```atexit()``` handlers

**Oct 03 2021 13:50**
- Print filename and line number in backtrace
- Added ```addr2line``` utility to find file and line number

**Sep 29 2021 09:20**
- Added build and usage guide in readme file
- Reverted shared-library name to libktrace for internal use
- Enable heap-trace test code

**Sep 29 2021 07:10**
- Renamed as shared-library to libheaptrace to Github users
- Renamed .so file to libheaptrace
- Removed test module
- Updated variable and file names

**Sep 28 2021 22:50**
- Implemented Heap Trace to track memory leaks
- Hash table to track heap memory usages
- Insert node into hash table on allocs
- Delete node from hash table on frees
- Removed gnu malloc hooks and externing glibc alloc apis for hooking 
- Print ```heap_trace``` report with backtrace at exit

**Sep 25 2021 09:10**
- Added Makefile rules to info, install, uninstall and ktags

**Sep 23 2021 08:25**
- Implemented ```heap_trace``` as shared library ```libktrace.so```
- Added test application to validate the library functionalities

**Sep 21 2021 01:15**
- Implemented memory hooking mechanism

**Sep 20 2021 07:50**
- Initiated prototype ```init_heap_trace();```
