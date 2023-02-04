.DEFAULT_GOAL := build_all

TARGET_EXEC ?= MiniPL

VALGRIND_BUILD_DIR ?= build_valgrind
ASAN_BUILD_DIR ?= build_asan
BUILD_DIR ?= build
SRC_DIRS ?= src
TEST_PROGRAM_DIR ?= test_programs

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
VALGRIND_OBJS := $(SRCS:%=$(VALGRIND_BUILD_DIR)/%.o)
ASAN_OBJS := $(SRCS:%=$(ASAN_BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
TEST_PROGRAMS := $(shell find $(TEST_PROGRAM_DIR) -name "*.mpl")

COMMON_CFLAGS ?= -Wall -Wextra -Werror
CFLAGS ?= $(COMMON_CFLAGS) -O3
VALGRIND_CFLAGS ?= $(COMMON_CFLAGS) -Og -g
ASAN_CFLAGS ?= $(COMMON_CFLAGS) -Og -g -fsanitize=address

COMMON_LDFLAGS ?=
LDFLAGS ?= $(COMMON_LDFLAGS)
VALGRIND_LDFLAGS ?= $(COMMON_LDFLAGS)
ASAN_LDFLAGS ?= $(COMMON_LDFLAGS) -fsanitize=address -static-libasan

VALGRIND_OPTIONS ?= -s --track-origins=yes --leak-check=full --show-leak-kinds=all

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(VALGRIND_BUILD_DIR)/$(TARGET_EXEC): $(VALGRIND_OBJS)
	$(CC) $(VALGRIND_LDFLAGS) $(VALGRIND_OBJS) -o $@

$(ASAN_BUILD_DIR)/$(TARGET_EXEC): $(ASAN_OBJS)
	$(CC) $(ASAN_LDFLAGS) $(ASAN_OBJS) -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(VALGRIND_BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(VALGRIND_CFLAGS) -c $< -o $@

$(ASAN_BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(ASAN_CFLAGS) -c $< -o $@

.PHONY: clean

build: $(BUILD_DIR)/$(TARGET_EXEC)

asan: $(ASAN_BUILD_DIR)/$(TARGET_EXEC)

valgrind: $(VALGRIND_BUILD_DIR)/$(TARGET_EXEC)

build_all: valgrind asan build

run_with_test_programs: $(BUILD_DIR)/$(TARGET_EXEC)
	@$(foreach test_program, $(TEST_PROGRAMS), $(BUILD_DIR)/$(TARGET_EXEC) $(test_program);)

run_with_test_programs_using_valgrind: $(VALGRIND_BUILD_DIR)/$(TARGET_EXEC)
	@$(foreach test_program, $(TEST_PROGRAMS), valgrind $(VALGRIND_OPTIONS) \
	$(VALGRIND_BUILD_DIR)/$(TARGET_EXEC) $(test_program);)

run_with_test_programs_using_asan: $(ASAN_BUILD_DIR)/$(TARGET_EXEC)
	@$(foreach test_program, $(TEST_PROGRAMS), $(ASAN_BUILD_DIR)/$(TARGET_EXEC) $(test_program);)

run: $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC)

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(VALGRIND_BUILD_DIR)
	$(RM) -r $(ASAN_BUILD_DIR)
