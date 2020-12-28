CC 				:= gcc
CFLAGS 			:= -std=c99 -g -g3 -O0 -Wall -Werror -pedantic -D_GNU_SOURCE
CFLAGS_END		:= -lpcre -lm

BUILD_DIR 		:= build
TEST_BUILD_DIR  := test_build

SRC_DIRS  	 		:= . ../common/logger ../common/strings ../common/mail ../common/address ./autogen ../common/privileges
SRC_DIRS     		:= $(addprefix ../,$(SRC_DIRS))
SRC_FILES 			:= $(wildcard $(addsuffix /*.c,$(SRC_DIRS)))
OBJ_FILES 			:= $(notdir $(patsubst %.c,%.o, $(SRC_FILES)))

# TEST_OBJ_FILES нужны для того, чтобы удалить их из целей server
TEST_DIRS			:= ./unit_tests
TEST_DIRS     		:= $(addprefix ../,$(TEST_DIRS))
TEST_SRC_FILES		:= $(wildcard $(addsuffix /*.c,$(TEST_DIRS)))
TEST_OBJ_FILES      := $(notdir $(patsubst %.c,%.o, $(TEST_SRC_FILES)))

# MAIN_OBJ_FILE нужно удалить из целей tests, чтобы не было дважды определенного main
MAIN_OBJ_FILE 		:= main.o
SERVER_OBJ_FILES	:= $(filter-out $(TEST_OBJ_FILES), $(OBJ_FILES))
TEST_OBJ_FILES 		:=  $(filter-out $(MAIN_OBJ_FILE), $(OBJ_FILES))

.PHONY: all clean server test tests

VPATH := $(SRC_DIRS) 

all:
	mkdir -p $(BUILD_DIR)
	make server --directory=$(BUILD_DIR) --makefile=../Makefile

tests:
	mkdir -p $(TEST_BUILD_DIR)
	make test --directory=$(TEST_BUILD_DIR) --makefile=../Makefile

server: $(SERVER_OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(CFLAGS_END)

test: $(TEST_OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(CFLAGS_END)

%.o: %.c
	$(CC) -c $(CFLAGS) $(addprefix -I ,$(SRC_DIRS)) $< $(CFLAGS_END)

include $(wildcard *.d) 

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(TEST_BUILD_DIR)