
## Heap-Trace: Light weight Heap Memory tracer

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
