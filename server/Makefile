compile_flags := -g -g3 -O0
source_dirs  := . ../common/logger ../common/strings ../common/mail ../common/address ./autogen
version := -std=c99
cflags:= -Wall -Werror -pedantic -D_GNU_SOURCE
# -lpcre is separated, because only if it is on end of gcc command it works correct on my machine...
endcflags:= -lpcre -lm

build_dir := build
source_dirs      := $(addprefix ../,$(source_dirs))
search_wildcards := $(addsuffix /*.c,$(source_dirs))

all: 
	mkdir -p $(build_dir)
	make server --directory=$(build_dir) --makefile=../Makefile

server: $(notdir $(patsubst %.c,%.o, $(wildcard $(search_wildcards))))
	gcc $(version) $^ -o $@ $(endcflags)
	
VPATH := $(source_dirs) 

%.o: %.c
	gcc $(version) -c -MD $(compile_flags) $(cflags) $(addprefix -I ,$(source_dirs)) $< $(endcflags)

include $(wildcard *.d) 

.PHONY: test
test:
	echo test

.PHONY: clean

clean:
	rm -rf build