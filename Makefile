CC ?= cc
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -g
LDFLAGS ?=

SINGLE_PROGRAMS := \
	00-hello-world/hello \
	01-types/types \
	02-expressions/expressions \
	03-control-flow/control \
	04-functions/functions \
	04-functions/args \
	05-arrays-pointers/arrays \
	05-arrays-pointers/pointers \
	06-strings/strings \
	07-aggregates/aggregates \
	09-preprocessor/preprocessor

MULTI_PROGRAMS := \
	08-storage/storage

PROGRAMS := $(SINGLE_PROGRAMS) $(MULTI_PROGRAMS)

.PHONY: all run check clean

all: $(PROGRAMS)

$(SINGLE_PROGRAMS): %: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# 头文件依赖。
09-preprocessor/preprocessor: 09-preprocessor/config.h

# 多文件程序在这里显式链接。
08-storage/storage: 08-storage/storage.c 08-storage/counter.c 08-storage/counter.h
	$(CC) $(CFLAGS) 08-storage/storage.c 08-storage/counter.c -o $@ $(LDFLAGS)

run: all
	@set -e; for program in $(PROGRAMS); do \
		printf '\n===== %s =====\n' "$$program"; \
		./"$$program"; \
	done

check:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -Werror" all

clean:
	rm -f $(PROGRAMS)
