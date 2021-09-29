
## Heap-Trace:
**Simple custom implementation of Heap Memory tracer**

Heaptrace detects memory leak by hooking memory functions (e.g. malloc).<br>
Simply attach the shared library ```-lheaptrace``` to the target program while compiling.<br>
At exist of target program, it prints the full backtrace of detected memory leak pointers.<br>

## Build Heap trace from here
```
$ git clone https://github.com/kribakarans/heaptrace.git
$ cd heaptrace/
$ make
```

## Install heaptrace from source
- Build package from source mentioned above
- Do ```make install``` to install heap-trace program locally to the current user
- Installation will create ```.lib``` and ```.include``` in ```$HOME``` path
- Library path will be saved to ```$HOME/.lib/libheaptrace.so```
- Header files will be saved to ```$HOME/.include/heaptrace.h```
- Installation to ```root``` user is not implemented for now
- Run ```make uninstall``` to un-install ```heap-trace```

## Usage
**1. Add INCLUDE and LDFLAGS while building target program**
- Add ```-I /home/<user>/.include -Wl,-rpath=/home/<user>/.lib -L /home/<user>/.lib``` option in GCC command line
    (or)
- Add below snippet in Makefile of target program
```
INCLUDE += -I$(HOME)/.include
LDFLAGS += -Wl,-rpath=$(HOME)/.lib -L$(HOME)/.lib -lheaptrace -lm
``` 

**2. Attach heap-trace to target program**
- Add below code snippet in ```main()``` to register ```heap-trace``` to target program
```
#include <heaptrace.h>
...
int main() {
	...
    init_heap_trace();
    atexit(&print_heap_summary);
	...
}
```

**3. Below shows the code snippet and compilation steps to use heap-trace**
```
main.c:

#include <heaptrace.h>

int main()
{
	init_heap_trace();
	atexit(&print_heap_summary);

	int *ptr = malloc(10);
	
	return 0; /* returning without freeing the heap memory */
}
```
**GCC:**
```
$ cc -g3 main.c -I $HOME/.include/ -Wl,-rpath=$HOME/.lib -L $HOME/.lib/  -lheaptrace -lm
```

**Output:**
```
$ ./a.out 

HEAP TRACE SUMMARY:
Backtrace of heap-pointer : 0x55efb2e6b6e0
 |_ /home/shanmugk/.lib/libheaptrace.so(malloc+0x39) [0x7f2cb6c53cef]
 |_ ./a.out(+0x11b3) [0x55efb12aa1b3]
 |_ /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7f2cb69170b3]
 |_ ./a.out(+0x10ce) [0x55efb12aa0ce]


Heap trace: Memory leak at 1 blocks !!!
```

## Changelog

**Sep 29 2021 07:10**
- Renamed as shared-library to libheaptrace
- Renamed .so file to libheaptrace
- Removed test module
- Updated variable and file names

**Sep 28 2021 22:50**
- Implemented Heap Trace to track memory leaks
- Hash table to track heap memory usages
- Insert node into hash table on allocs
- Delete node from hash table on frees
- Removed gnu malloc hooks and externing glibc alloc apis for hooking 
- Print heap_trace report with backtrace at exit

**Sep 25 2021 09:10**
- Added Makefile rules to info, install, uninstall and ktags.

**Sep 23 2021 08:25**
- Implemented ```heap_trace``` as shared library ```libktrace.so```
- Added test application to validate the library functionalities`

**Sep 21 2021 01:15**
- Implemented memory hooking mechanism

**Sep 20 2021 07:50**
- Initiated prototype init_heap_trace();
