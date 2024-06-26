include variables.mk

HEADERS := constructor iface alloc
HEADERS += io/closer io/reader io/writer io/read_closer io/read_writer io/write_closer io/buf_reader
HEADERS += bytes/slice bytes/buffer bytes/reader bytes/writer
HEADERS += iter arena option result
HEADERS := $(HEADERS:%=%.h)

TEST_SRCS := $(HEADERS:%.h=$(TEST_DIR)/%.c)
TEST_BINS := $(TEST_SRCS:%.c=$(BUILD_DIR)/%)

debug:
	@echo "HEADERS: $(HEADERS)"
	@echo "TEST_SRCS: $(TEST_SRCS)"
	@echo "TEST_BINS: $(TEST_BINS)"

.PHONY: compile_flags.txt
compile_flags.txt:
	echo $(CFLAGS) | tr ' ' '\n' > compile_flags.txt

$(BUILD_DIR)/$(TEST_DIR)/%: $(TEST_DIR)/%.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) $(TEST_LDFLAGS) $^ -o $@
	valgrind --quiet --leak-check=full --errors-for-leak-kinds=definite $@

.PHONY: tests
tests: $(TEST_BINS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f xstd.h

.PHONY: bundle
bundle: clean xstd.h

xstd.h: internal.h $(HEADERS)
	cat <(printf "// XSTD bundle from https://github.com/negrel/xstd.h\n\n") $^ \
		| grep -Ev '^#include ".*"$$' > $@
	$(MKDIR_P) $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $@ -o $(BUILD_DIR)/xstd.o
	test "$$(grep -cn '#define XSTD_IMPLEMENTATION' < $@)" = "0"
