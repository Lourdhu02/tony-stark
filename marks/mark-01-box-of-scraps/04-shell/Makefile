include ../common/flags.mk

LIB  := src/parse.c src/exec.c src/builtins.c
HDRS := include/shell.h

all: $(BUILD)/stark-shell $(BUILD)/test_parse

$(BUILD)/stark-shell: src/main.c $(LIB) $(HDRS) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) src/main.c $(LIB) -o $@ $(LDLIBS)

$(BUILD)/test_parse: tests/test_parse.c src/parse.c $(HDRS) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) tests/test_parse.c src/parse.c -o $@ $(LDLIBS)

test: all
	@./$(BUILD)/test_parse; unit=$$?; \
	bash tests/test_shell.sh; e2e=$$?; \
	exit $$((unit || e2e))

run: $(BUILD)/stark-shell
	@./$(BUILD)/stark-shell

clean:
	rm -rf $(BUILD)

.PHONY: all test run clean
