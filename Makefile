CFLAGS = -std=c++17 -fconcepts -I../BAgraph/BAgraph/include/
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG
EXTRADBGFLAGS = -D_GLIBCXX_DEBUG
COMPILE_DBG = $(CXX) $(CFLAGS) $(DBGFLAGS)
COMPILE_RELEASE = $(CXX) $(CFLAGS)

test: test.cpp
	$(COMPILE_DBG) test.cpp -o test.out

compile_debug_run: compile_debug
	./main.out

compile_debug: 
	$(COMPILE_DBG) main.cpp -o main.out

compile_release: 
	$(COMPILE_RELEASE) main.cpp -o main.exe

.PHONY: compile_debug_run compile_debug compile_release test