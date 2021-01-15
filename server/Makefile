SRC_DIR = src
REPORT_DIR = report

.PHONY: all clean tests test-unit test-style test-system test-memory make-server make-report clean-server clean-report

all: make-server tests make-report

clean: clean-server clean-report

tests: test-unit test-style test-system test-memory

test-unit:
	make test-unit --directory=$(SRC_DIR)

test-style:
	make test-style --directory=$(SRC_DIR)

test-system:
	echo "system test!"
	#make test-style --directory=$(SRC_DIR)

test-memory:
	make test-memory --directory=$(SRC_DIR)

make-server:
	make --directory=$(SRC_DIR)

make-report:
	make --directory=$(REPORT_DIR)

clean-server:
	make clean --directory=$(SRC_DIR)

clean-report:
	make clean --directory=$(REPORT_DIR)