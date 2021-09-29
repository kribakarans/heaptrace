
## Heap-Trace:
**Simple custom implementation of Heap Memory tracer**

Heaptrace detects memory leak by hooking memory functions (e.g. malloc).<br>
Simply attach the shared library ```-lheaptrace``` to the target program while compiling.<br>
At exist of target program, it prints the full backtrace of detected memory leak pointers.<br>

## Build Heap trace

## Usage

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
