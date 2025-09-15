CC := gcc
CFLAGS := -Wall -Wextra -std=c90 -Iinclude -MMD -MP
LFLAGS := -lcurl
SRC := $(shell find src -name '*.c')
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
DEP := $(OBJ:.o=.d)
BIN := build/app

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(DEP)

-include $(DEP)

.PHONY: all run clean