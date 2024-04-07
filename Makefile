include variables.mk

.PHONY: compile_flags.txt
compile_flags.txt:
	echo $(CFLAGS) | tr ' ' '\n' > compile_flags.txt

$(BUILD_DIR)/$(TEST_DIR)/%: $(TEST_DIR)/%.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) $(TEST_LDFLAGS) $^ -o $@
	valgrind --quiet --leak-check=full --errors-for-leak-kinds=definite $@

.PHONY: tests
tests: $(BUILD_DIR)/$(TEST_DIR)/io_closer_test $(BUILD_DIR)/$(TEST_DIR)/io_writer_test $(BUILD_DIR)/$(TEST_DIR)/io_reader_test $(BUILD_DIR)/$(TEST_DIR)/io_read_writer_test $(BUILD_DIR)/$(TEST_DIR)/vec_test $(BUILD_DIR)/$(TEST_DIR)/iter_test $(BUILD_DIR)/$(TEST_DIR)/list_test $(BUILD_DIR)/$(TEST_DIR)/alloc_test $(BUILD_DIR)/$(TEST_DIR)/arena_test

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f xstd.h

.PHONY: bundle
bundle: xstd.h

xstd.h: xstd_internal.h xstd_io_reader.h xstd_io_writer.h xstd_iter.h xstd_alloc.h xstd_arena.h xstd_vec.h xstd_list.h
	cat <(printf "// XSTD bundle from https://github.com/negrel/xstd.h\n\n") $^ \
		| grep -Ev '^#include "xstd_.*"$$' > $@
	$(MKDIR_P) $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $@ -o $(BUILD_DIR)/xstd.o
