INC_DIR := $$PWD
INCS := $(shell find $(INC_DIR) -regex '.+\.h')

BUILD_DIR ?= ./build
TEST_DIR ?= ./tests

CC := clang
CFLAGS ?=
CFLAGS := $(CFLAGS) -std=c11 \
	-Wall -Wextra -Werror -pedantic -Wmissing-prototypes -Wstrict-prototypes \
	-I$$PWD

TEST_LDFLAGS := $(shell pkg-config --cflags --libs check)

MKDIR := mkdir
MKDIR_P := mkdir -p

