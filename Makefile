TARGET_EXEC ?= MiniPL

BUILD_DIR ?= build
SRC_DIRS ?= src
TEST_PROGRAM_DIR ?= test_programs

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
TEST_PROGRAMS := $(shell find $(TEST_PROGRAM_DIR) -name '*.mpl')

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS ?= -O2

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

run_with_test_programs: $(BUILD_DIR)/$(TARGET_EXEC)
	@$(foreach test_program, $(TEST_PROGRAMS), $(BUILD_DIR)/$(TARGET_EXEC) $(test_program);)


run: $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC)

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
