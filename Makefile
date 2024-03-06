MAKE_DIR = $(PWD)

SRC_DIR       := $(MAKE_DIR)/src
BUILD_DIR	  := $(MAKE_DIR)/build
OBJ_DIR       := $(BUILD_DIR)/obj
BIN_DIR		  := $(BUILD_DIR)/bin

INC_SRCH_PATH := 
INC_SRCH_PATH += -I$(SRC_DIR)

CC = gcc
LD = ld

BIN = $(BUILD_DIR)/bin/snakeshell

LIBS := -lreadline

CFLAGS := -std=gnu99 -pedantic -Wall -Wextra
CFLAGS += $(INC_SRCH_PATH)

ifeq ($(BUILD),dev)
# "Dev" build - no optimization, and debugging symbols
CFLAGS += -g -ggdb3 -Og -DDEBUG -finstrument-functions -fdump-rtl-expand -fsanitize=address,undefined -fno-omit-frame-pointer -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code
else
# "Release" build - optimization, and no debug symbols
CFLAGS += -O2 -s -DNDEBUG
endif

LDFLAGS :=

export MAKE_DIR CC LD CFLAGS LDFLAGS LIBS INC_SRCH_PATH OBJ_DIR BIN_DIR BIN

all:
	@$(MAKE) -C $(SRC_DIR)

dev:
	make "BUILD=dev"

run:
	$(BIN)

valgrind:
	@mkdir -p $(BUILD_DIR)
	valgrind --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --verbose \
        --log-file=$(BUILD_DIR)/valgrind-out.txt \
        $(BIN) $(ARGS)

clangformat:
	find . -iname *.h -o -iname *.c | xargs clang-format -i

.PHONY: clean
clean:
	@$(MAKE) -C $(SRC_DIR)		clean
