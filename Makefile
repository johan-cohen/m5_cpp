CPPFLAGS = -Wall -Wextra -O0 -g -Isrc

CXX = g++

OBJS = obj/AppBuf.o

all: dirs $(OBJS) test

dirs:
	mkdir -p obj
	mkdir -p bin

obj/AppBuf.o: src/AppBuf.cpp src/AppBuf.hpp
	$(CXX) $(CPPFLAGS) -c src/AppBuf.cpp -o obj/AppBuf.o

test:
	$(CXX) $(CPPFLAGS) -Isrc src/test_AppBuf.cpp $(OBJS) -o bin/test_AppBuf

clean:
	rm -rf obj bin
