# Mark XI: build and test every system.
PROJECTS := $(sort $(wildcard [0-9][0-9]-*))

all:
	@for p in $(PROJECTS); do $(MAKE) -s -C $$p all || exit 1; done

test:
	@../../tools/armory.sh $(CURDIR)

clean:
	@for p in $(PROJECTS); do $(MAKE) -s -C $$p clean; done

.PHONY: all test clean
