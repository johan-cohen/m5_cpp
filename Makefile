CPPFLAGS = -Wall -Wextra -O0 -g -Isrc

CXX = g++

SRC_DIR = src
OBJS_DIR = obj
OBJS = $(OBJS_DIR)/AppBuf.o

all: dirs $(OBJS) test

dirs:
	mkdir -p obj
	mkdir -p bin

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

test:
	$(CXX) $(CPPFLAGS) -Isrc src/test_AppBuf.cpp $(OBJS) -o bin/test_AppBuf

clean:
	rm -rf obj bin

.PHONY: clean
