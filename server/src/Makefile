CC 					:= gcc
CFLAGS 				:= -std=c99 -g -g3 -O0 -Wall -Werror -pedantic -D_GNU_SOURCE
CFLAGS_END			:= -lpcre -lm -lconfig

STYLE_CHECKER   	:=  ../../common/linter/cpplint.py
STYLE_CHECKER_ARGS  := --linelength=120

BUILD_DIR 			:= build
TEST_BUILD_DIR  	:= test_build
COMMON_DIR			:=  ../../common/
TEST_UNIT_OUT		:= test-unit.out
TEST_STYLE_OUT		:= test-style.out
TEST_SYSTEM_OUT		:= test-system.out
TEST_MEMORY_OUT		:= test-memory.out


COMMON_SRC_DIRS		:=  logger strings mail address tests privileges process_info
COMMON_SRC_DIRS		:=  $(addprefix $(COMMON_DIR),$(COMMON_SRC_DIRS))
TEST_DIRS     		:= $(addprefix ../, ./tests/unit $(addprefix $(COMMON_DIR), tests)) #нужны для исключения из обычной сборки

SRC_DIRS  	 		:= . ./tests/unit ./config  ./autogen $(COMMON_SRC_DIRS)
STYLE_DIRS			:= $(filter-out ./autogen, $(SRC_DIRS))
STYLE_FILES 		:= $(wildcard $(addsuffix /*.c, $(STYLE_DIRS))) $(wildcard $(addsuffix /*.h, $(STYLE_DIRS)))

SRC_DIRS     		:= $(addprefix ../,$(SRC_DIRS)) # потому что будем собирать в /build
SRC_FILES 			:= $(wildcard $(addsuffix /*.c,$(SRC_DIRS)))
OBJ_FILES 			:= $(notdir $(patsubst %.c,%.o, $(SRC_FILES)))

SERVER_OBJ_EXCEPTIONS := $(notdir $(patsubst %.c,%.o, $(wildcard $(addsuffix /*.c,$(TEST_DIRS)))))
TEST_OBJ_EXCEPTIONS   := main.o
SERVER_OBJ_FILES      := $(filter-out $(SERVER_OBJ_EXCEPTIONS), $(OBJ_FILES))
TEST_OBJ_FILES 		  :=  $(filter-out $(TEST_OBJ_EXCEPTIONS), $(OBJ_FILES))

.PHONY: all clean server test test-unit test-style test-memory test-system

VPATH := $(SRC_DIRS)

all:
	mkdir -p $(BUILD_DIR)
	make server --directory=$(BUILD_DIR) --makefile=../Makefile

test-unit:
	mkdir -p $(TEST_BUILD_DIR)
	make test --directory=$(TEST_BUILD_DIR) --makefile=../Makefile
	./test_build/test > $(TEST_UNIT_OUT)
	#cat $(TEST_UNIT_OUT)

test-style:
	$(STYLE_CHECKER) $(STYLE_CHECKER_ARGS) $(STYLE_FILES) > $(TEST_STYLE_OUT)
	cat $(TEST_STYLE_OUT)

test-memory:
	mkdir -p $(TEST_BUILD_DIR)
	make test --directory=$(TEST_BUILD_DIR) --makefile=../Makefile
	valgrind --log-file="$(TEST_MEMORY_OUT)" ./test_build/test
	cat $(TEST_MEMORY_OUT)

test-system:
	python3 ./tests/system/test.py ./tests/system/test_files > $(TEST_SYSTEM_OUT)
	#cat $(TEST_SYSTEM_OUT)

server: $(SERVER_OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(CFLAGS_END)

test: $(TEST_OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(CFLAGS_END) -lcunit

%.o: %.c
	$(CC) -c $(CFLAGS) $(addprefix -I ,$(SRC_DIRS)) $< $(CFLAGS_END)

include $(wildcard *.d)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(TEST_BUILD_DIR)
	rm -rf *.out