# Mark I: build and test every project.
#   make          build everything
#   make test     run all suites and show the armory dashboard
#   make clean

PROJECTS := $(sort $(wildcard [0-9][0-9]-*))

all:
	@for p in $(PROJECTS); do $(MAKE) -s -C $$p all || exit 1; done

test:
	@../../tools/armory.sh $(CURDIR)

clean:
	@for p in $(PROJECTS); do $(MAKE) -s -C $$p clean; done

.PHONY: all test clean
