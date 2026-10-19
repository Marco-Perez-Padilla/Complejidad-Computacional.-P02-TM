CXX = g++
CXXFLAGS = -std=c++23 -Wall -g -I. -I./include -MMD -MP

BUILD_DIR = build
BIN_DIR = bin

SOURCES = $(shell find src -name "*.cc")
OBJ = $(patsubst src/%.cc,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS = $(OBJ:.o=.d)

MAIN_FILE = $(shell grep -l "int main" src/*.cc src/*/*.cc 2>/dev/null | head -1)
EXECUTABLE = $(BIN_DIR)/$(basename $(notdir $(MAIN_FILE)))

LIB_OBJ = $(filter-out $(patsubst src/%.cc,$(BUILD_DIR)/%.o,$(MAIN_FILE)),$(OBJ))

GTEST_DIR = libs/googletest/googletest
GTEST_FLAGS = -I$(GTEST_DIR)/include -I$(GTEST_DIR)
GTEST_OBJ = $(BUILD_DIR)/gtest/gtest-all.o $(BUILD_DIR)/gtest/gtest_main.o

TEST_SOURCES = $(shell find tests -name "*.cc")
TEST_OBJ = $(patsubst tests/%.cc,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))
TEST_DEPS = $(TEST_OBJ:.o=.d)

TEST_EXECUTABLE = $(BIN_DIR)/run_tests

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_EXECUTABLE): $(LIB_OBJ) $(TEST_OBJ) $(GTEST_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -pthread -o $@ $^

$(BUILD_DIR)/tests/%.o: tests/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) -c $< -o $@

$(BUILD_DIR)/gtest/%.o: $(GTEST_DIR)/src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) -pthread -c $< -o $@

.PHONY: clean run test

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

-include $(DEPS)
-include $(TEST_DEPS)
