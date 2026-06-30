CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
TARGET  = ranking_copa
SRCS    = src/main.c src/avl.c src/rb.c src/player.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)