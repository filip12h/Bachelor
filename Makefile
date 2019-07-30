CFLAGS = -std=c++17 -fconcepts -I../ba-graph/include/
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG
EXTRADBGFLAGS = -D_GLIBCXX_DEBUG
COMPILE_DBG = $(CXX) $(CFLAGS) $(DBGFLAGS)
COMPILE_RELEASE = $(CXX) $(CFLAGS)

compile_debug_run: compile_debug
	./main.out

compile_debug: 
	$(COMPILE_DBG) main.cpp -o main.out

compile_release: 
	$(COMPILE_RELEASE) main.cpp -o main.exe

test: test.cpp
	$(COMPILE_DBG) test.cpp -o test.exe

simple_test: simple_test.cpp
	$(COMPILE_DBG) simple_test.cpp -o simple_test.exe

.PHONY: compile_debug_run compile_debug compile_release test