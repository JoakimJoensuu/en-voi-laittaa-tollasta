TARGET_EXEC ?= MiniPL

DEBUG_BUILD_DIR ?= build_debug
BUILD_DIR ?= build
SRC_DIRS ?= src
TEST_PROGRAM_DIR ?= test_programs

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEBUG_OBJS := $(SRCS:%=$(DEBUG_BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
TEST_PROGRAMS := $(shell find $(TEST_PROGRAM_DIR) -name "*.mpl")

COMMON_CFLAGS ?= -Wall -Wextra -Werror
CFLAGS ?= $(COMMON_CFLAGS) -O3
DEBUG_CFLAGS ?= $(COMMON_CFLAGS) -Og -g

VALGRIND_OPTIONS ?= -s --track-origins=yes --leak-check=full --show-leak-kinds=all

$(DEBUG_BUILD_DIR)/$(TARGET_EXEC): $(DEBUG_OBJS)
	$(CC) $(DEBUG_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(DEBUG_BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

run_with_test_programs_using_valgrind: $(DEBUG_BUILD_DIR)/$(TARGET_EXEC)
	@$(foreach test_program, $(TEST_PROGRAMS), valgrind $(VALGRIND_OPTIONS) \
	$(DEBUG_BUILD_DIR)/$(TARGET_EXEC) $(test_program);)

run_with_test_programs: $(BUILD_DIR)/$(TARGET_EXEC)
	@$(foreach test_program, $(TEST_PROGRAMS), $(BUILD_DIR)/$(TARGET_EXEC) $(test_program);)

run: $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC)

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(DEBUG_BUILD_DIR)
