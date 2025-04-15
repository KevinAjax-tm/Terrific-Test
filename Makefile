CC := gcc
CFLAGS := -Wall -Wextra -pthread -Iinclude
LDFLAGS := -pthread
LDLIBS := -lncurses -lrt  # ncurses for UI, rt for shared memory
SRC_DIR := src
BIN := distask

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean