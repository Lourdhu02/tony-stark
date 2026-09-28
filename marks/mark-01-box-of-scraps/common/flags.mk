# Shared build settings for every Mark I project.
#   make SAN=1    build with AddressSanitizer + UndefinedBehaviorSanitizer

CC      ?= cc
CFLAGS  := -std=c11 -D_GNU_SOURCE -Wall -Wextra -Wpedantic -Werror \
           -Wshadow -Wstrict-prototypes -g -O2 -Iinclude -I../common
LDLIBS  := -lm

ifeq ($(SAN),1)
CFLAGS  += -O1 -fno-omit-frame-pointer -fsanitize=address,undefined
LDFLAGS += -fsanitize=address,undefined
endif

BUILD   := build

$(BUILD):
	@mkdir -p $@
