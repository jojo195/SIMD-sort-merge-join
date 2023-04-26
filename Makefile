HOST_DIR := host
BUILDDIR ?= bin
OP ?= ADD
TYPE ?= INT32
ALGORITHM ?= SORT_NETWORK_8

HOST_TARGET := ${BUILDDIR}/host_code

COMMON_INCLUDES := support
HOST_SOURCES := $(wildcard ${HOST_DIR}/*.c)

.PHONY: all clean test

__dirs := $(shell mkdir -p ${BUILDDIR})

HOST_LIB_LINKED := -lm -lpthread -march=native

COMMON_FLAGS := -Wall -Wextra -g -I${COMMON_INCLUDES}
HOST_FLAGS := ${COMMON_FLAGS} -std=c11 -O0 -D${ALGORITHM}

all: ${HOST_TARGET}

${HOST_TARGET}: ${HOST_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOST_SOURCES} ${HOST_FLAGS} ${HOST_LIB_LINKED}

clean:
	$(RM) -r $(BUILDDIR)

test: all
	./${HOST_TARGET}
