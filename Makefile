CPPFLAGS = -std=c++11 -Wall -Wextra -Werror -O0 -g -Isrc

CXX = g++

SRC_DIR = src

OBJS_DIR = obj

BINS_DIR = bin

TESTS = $(BINS_DIR)/test_AppBuf $(BINS_DIR)/test_PktConnect $(BINS_DIR)/test_PropertyData  $(BINS_DIR)/test_Properties

VALGRIND = valgrind -q --leak-check=full --error-exitcode=1

all: dirs $(TESTS)

dirs:
	@mkdir -p obj
	@mkdir -p bin

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

$(BINS_DIR)/test_Properties: $(OBJS_DIR)/test_Properties.o $(OBJS_DIR)/PropertyData.o $(OBJS_DIR)/PropertyNode.o $(OBJS_DIR)/Properties.o
	$(CXX) $(CPPFLAGS) -o $@ $^

$(BINS_DIR)/test_%: $(SRC_DIR)/test_Common.hpp $(OBJS_DIR)/AppBuf.o $(OBJS_DIR)/%.o $(OBJS_DIR)/test_%.o
	$(CXX) $(CPPFLAGS) -o $@ $^

tests: $(TESTS)
	@$(foreach test_case, $(TESTS), ./$(test_case) || exit 1;)

memtest: $(TESTS)
	@$(foreach test_case, $(TESTS), $(VALGRIND) ./$(test_case) || exit 1;)

clean:
	rm -rf obj bin

.PHONY: all dirs tests memtest clean
