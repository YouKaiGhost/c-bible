CC ?= cc
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -g
LDFLAGS ?=

SINGLE_PROGRAMS := \
	00-hello-world/hello \
	01-types/types \
	02-expressions/expressions \
	03-control-flow/control

MULTI_PROGRAMS :=

PROGRAMS := $(SINGLE_PROGRAMS) $(MULTI_PROGRAMS)

.PHONY: all run check clean

all: $(PROGRAMS)

$(SINGLE_PROGRAMS): %: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# 多文件程序在这里显式链接，例如：
# 08-storage/storage: 08-storage/storage.c 08-storage/counter.c
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

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
