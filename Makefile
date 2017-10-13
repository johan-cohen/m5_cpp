CPPFLAGS = -std=c++11 -Wall -Wextra -O0 -g -Isrc

CXX = g++

SRC_DIR = src

OBJS_DIR = obj

BINS_DIR = bin

TESTS = $(BINS_DIR)/test_AppBuf

all: dirs $(TESTS)

dirs:
	@mkdir -p obj
	@mkdir -p bin

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

$(BINS_DIR)/test_%: $(OBJS_DIR)/%.o $(OBJS_DIR)/test_%.o
	$(CXX) $(CPPFLAGS) -o $@ $^

clean:
	rm -rf obj bin

.PHONY: clean
