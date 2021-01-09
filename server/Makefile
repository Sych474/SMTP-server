SRC_DIR = src
REPORT_DIR = report

.PHONY: all clean tests test-unit test-style report

all:
	make --directory=$(SRC_DIR)

clean:
	make clean --directory=$(SRC_DIR)

tests: test-unit test-style test-system test-memory

test-unit:
	make test-unit --directory=$(SRC_DIR)

test-style:
	make test-style --directory=$(SRC_DIR)

test-system:
	echo "system test!"
	#make test-style --directory=$(SRC_DIR)

test-memory:
	echo "memory test!"
	#make test-unit --directory=$(SRC_DIR)

report:
	make --directory=$(REPORT_DIR)