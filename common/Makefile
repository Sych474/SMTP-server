CC 				:= gcc
CFLAGS 			:= -std=c99 -g -g3 -O0 -Wall -Werror -pedantic -D_GNU_SOURCE
CFLAGS_END		:= -lpcre -lm  -lcunit

BUILD_DIR 		:= build

SRC_DIRS  	 	:= . ./logger ./mail ./strings ./tests ./address ./privileges
SRC_DIRS     	:= $(addprefix ../,$(SRC_DIRS))
SRC_FILES 		:= $(wildcard $(addsuffix /*.c,$(SRC_DIRS)))
OBJ_FILES 		:= $(notdir $(patsubst %.c,%.o, $(SRC_FILES)))

.PHONY: all clean test test-unit

VPATH := $(SRC_DIRS)

all: test-unit

test-unit:
	mkdir -p $(BUILD_DIR)
	make test --directory=$(BUILD_DIR) --makefile=../Makefile
	./$(BUILD_DIR)/test

test: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(CFLAGS_END)

%.o: %.c
	$(CC) -c $(CFLAGS) $(addprefix -I ,$(SRC_DIRS)) $< $(CFLAGS_END)

include $(wildcard *.d)

clean:
	rm -rf $(BUILD_DIR)
