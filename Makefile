CC ?= cc
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -g
LDFLAGS ?=

LESSON := 00-hello-world
TARGET := $(LESSON)/hello
SOURCE := $(LESSON)/hello.c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
