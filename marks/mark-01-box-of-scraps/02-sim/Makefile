include ../common/flags.mk

SRC  := src/integrators.c src/models.c
HDRS := include/integrators.h include/models.h

all: $(BUILD)/test_sim $(BUILD)/pendulum

$(BUILD)/test_sim: tests/test_sim.c $(SRC) $(HDRS) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) tests/test_sim.c $(SRC) -o $@ $(LDLIBS)

$(BUILD)/pendulum: src/pendulum.c $(SRC) $(HDRS) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) src/pendulum.c $(SRC) -o $@ $(LDLIBS)

test: $(BUILD)/test_sim
	@./$(BUILD)/test_sim

# Compare all three integrators at a coarse step over a long horizon.
demo: $(BUILD)/pendulum
	@for m in euler rk4 verlet; do \
		./$(BUILD)/pendulum --method $$m --dt 0.1 --T 1000 --every 100 > $(BUILD)/$$m.csv; \
	done

clean:
	rm -rf $(BUILD)

.PHONY: all test demo clean
