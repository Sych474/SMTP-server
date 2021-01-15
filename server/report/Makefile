TEX_DIR			:= tex
DOT_DIR			:= $(TEX_DIR)/dot
BUILD_DIR		:= build
REPORT_TEX   	:= report.tex
REPORT_PDF   	:= report.pdf
SRC_DIR			:= ../../src
AUTOGEN_DIR		:= $(SRC_DIR)/autogen
AUTOGEN_NAME	:= server.def

SRCS_FOR_PDFS	:= fsm processes
SRCS_FOR_PDFS	:= $(addprefix $(BUILD_DIR)/, $(SRCS_FOR_PDFS))
SRCS_FOR_CFLOW  := main server fsm_handlers
CFLOW_TARGETS   := $(addsuffix _cflow, $(addprefix $(BUILD_DIR)/, $(SRCS_FOR_CFLOW)))
FSM2DOT			:= ../utils/def2dot

.PHONY: all report clean clean-report $(addsuffix .dot, $(SRCS_FOR_PDFS))

all:
	echo $(addprefix $(BUILD_DIR)/, $(PDFS))
	mkdir -p $(TEX_DIR)/$(BUILD_DIR)
	cp -a $(DOT_DIR)/. $(TEX_DIR)/$(BUILD_DIR)
	make report --directory=$(TEX_DIR) --makefile=../Makefile

report: $(addsuffix .pdf, $(SRCS_FOR_PDFS)) $(CFLOW_TARGETS)
	# хак, чтобы нормально сгенерилось оглавление... ¯\_(ツ)_/¯
	pdflatex -interaction=nonstopmode -output-directory $(BUILD_DIR) $(REPORT_TEX) > /dev/null
	pdflatex -interaction=nonstopmode -output-directory $(BUILD_DIR) $(REPORT_TEX) > /dev/null
	cp $(BUILD_DIR)/$(REPORT_PDF) ./$(REPORT_PDF)


$(BUILD_DIR)/%_cflow: $(SRC_DIR)/%.c
	cflow2dot -x cflowignore.txt -i $< -o $@ -f pdf

$(BUILD_DIR)/fsm.dot:
	$(FSM2DOT) $(AUTOGEN_DIR)/server.def > $@

$(BUILD_DIR)/%.tex: $(BUILD_DIR)/%.dot
	dot2tex -ftikz --autosize --crop  $< > $@

$(BUILD_DIR)/%.pdf: $(BUILD_DIR)/%.tex
	pdflatex -output-directory $(BUILD_DIR) $< > /dev/null


clean:
	rm -rf $(TEX_DIR)/$(BUILD_DIR)
	rm -rf $(TEX_DIR)/*.aux
	rm -rf $(TEX_DIR)/*.log
	rm -rf $(TEX_DIR)/*.lol
	rm -rf $(TEX_DIR)/*.gz
	rm -rf $(TEX_DIR)/*.toc
	rm -rf $(TEX_DIR)/*.out

clean-report:
	rm -rf $(TEX_DIR)/$(REPORT_PDF)