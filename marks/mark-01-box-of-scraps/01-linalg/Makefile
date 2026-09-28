include ../common/flags.mk

SRC := src/linalg.c

all: $(BUILD)/test_linalg $(BUILD)/bench

$(BUILD)/test_linalg: tests/test_linalg.c $(SRC) include/linalg.h | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) tests/test_linalg.c $(SRC) -o $@ $(LDLIBS)

$(BUILD)/bench: bench/bench_matmul.c $(SRC) include/linalg.h | $(BUILD)
	$(CC) $(CFLAGS) -O3 -march=native $(LDFLAGS) bench/bench_matmul.c $(SRC) -o $@ $(LDLIBS)

test: $(BUILD)/test_linalg
	@./$(BUILD)/test_linalg

bench: $(BUILD)/bench
	@./$(BUILD)/bench

clean:
	rm -rf $(BUILD)

.PHONY: all test bench clean
