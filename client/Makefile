compile_flags :=  -g -g3 -O0 
source_dirs  := . ../common/strings ../common/mail ../common/logger ../common/address ../common/parser
version := -std=c99
cflags:= -Wall -Werror -pedantic -D_GNU_SOURCE   

build_dir := build
source_dirs      := $(addprefix ../,$(source_dirs))
search_wildcards := $(addsuffix /*.c,$(source_dirs))

all: 
	mkdir -p $(build_dir)
	make client --directory=$(build_dir) --makefile=../Makefile

client: $(notdir $(patsubst %.c,%.o, $(wildcard $(search_wildcards))))
	gcc  $(version) $^ -o $@  -lpcre -lm
	
VPATH := $(source_dirs) 

%.o: %.c
	gcc $(version) -c -MD $(compile_flags) $(cflags) $(addprefix -I ,$(source_dirs)) $<

include $(wildcard *.d) 

.PHONY: test
test:
	echo test

.PHONY: clean

clean:
	rm -rf build
