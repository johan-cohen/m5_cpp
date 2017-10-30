CPPFLAGS = -std=c++11 -Wall -Wextra -Werror -O0 -g -Isrc

CXX = g++

SRC_DIR = src

OBJS_DIR = obj

BINS_DIR = bin

TESTS = $(BINS_DIR)/test_AppBuf $(BINS_DIR)/test_Properties $(BINS_DIR)/test_PktConnect

VALGRIND = valgrind -q --leak-check=full --error-exitcode=1

all: dirs $(TESTS)

dirs:
	@mkdir -p obj
	@mkdir -p bin

$(OBJS_DIR)/Properties.o: $(SRC_DIR)/Properties.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

$(OBJS_DIR)/test_AppBuf.o: $(SRC_DIR)/test_AppBuf.cpp $(SRC_DIR)/AppBuf.hpp $(SRC_DIR)/test_Common.hpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

$(BINS_DIR)/test_PktConnect: $(OBJS_DIR)/test_PktConnect.o $(OBJS_DIR)/PktConnect.o $(OBJS_DIR)/Properties.o $(OBJS_DIR)/AppBuf.o
	$(CXX) $(CPPFLAGS) -o $@ $^

$(BINS_DIR)/test_Properties: $(OBJS_DIR)/test_Properties.o $(OBJS_DIR)/Properties.o $(OBJS_DIR)/AppBuf.o
	$(CXX) $(CPPFLAGS) -o $@ $^

$(BINS_DIR)/test_%: $(OBJS_DIR)/test_%.o $(OBJS_DIR)/AppBuf.o $(OBJS_DIR)/%.o
	$(CXX) $(CPPFLAGS) -o $@ $^

tests: $(TESTS)
	@$(foreach test_case, $(TESTS), ./$(test_case) || exit 1;)

memtest: $(TESTS)
	@$(foreach test_case, $(TESTS), $(VALGRIND) ./$(test_case) || exit 1;)

clean:
	rm -rf obj bin

.PHONY: all dirs tests memtest clean
