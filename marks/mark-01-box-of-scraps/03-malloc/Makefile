include ../common/flags.mk

SRC := src/stark_malloc.c src/heap_os.c

all: $(BUILD)/test_malloc $(BUILD)/libstarkmalloc.so

$(BUILD)/test_malloc: tests/test_malloc.c $(SRC) include/stark_malloc.h | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) tests/test_malloc.c $(SRC) -o $@ $(LDLIBS)

# The preload library never uses sanitizers: ASan brings its own malloc.
$(BUILD)/libstarkmalloc.so: $(SRC) src/preload.c include/stark_malloc.h | $(BUILD)
	$(CC) $(filter-out -fsanitize=%,$(CFLAGS)) -fPIC -shared $(SRC) src/preload.c -o $@ -pthread

test: all
	@./$(BUILD)/test_malloc; unit=$$?; \
	bash tests/test_preload.sh; preload=$$?; \
	exit $$((unit || preload))

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
