PROJNAME = terraBasic
VERSION = 0.1

CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -DVERSION="$(VERSION)"
CPPFLAGS =
LDFLAGS =
LDLIBS = -lraylib -lpthread -lm

BUILD_DIR = .build

SRC_DIRS = src
INCLUDE_DIRS = include

EXE = $(BUILD_DIR)/$(PROJNAME)

SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS = $(patsubst $(SRC_DIRS)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

.PHONY: all clean

all: $(EXE)

run: $(EXE)
	./$(EXE)

$(EXE): $(OBJS) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(addprefix -I,$(INCLUDE_DIRS)) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

-include $(DEPS)

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)

