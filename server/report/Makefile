TEX_DIR			:= tex
BUILD_DIR		:= build
REPORT_TEX   	:= report.tex
REPORT_PDF   	:= report.pdf
SRC_DIR			:= ../../src
AUTOGEN_DIR		:= $(SRC_DIR)/autogen
AUTOGEN_NAME	:= server.def

SRCS_FOR_CFLOW  := main server fsm_handlers

FSM2DOT			:= ../utils/def2dot

.PHONY: all report clean clean-report $(BUILD_DIR)/fsm.dot

all:
	mkdir -p $(TEX_DIR)/$(BUILD_DIR)
	make report --directory=$(TEX_DIR) --makefile=../Makefile

report: $(BUILD_DIR)/fsm.pdf  $(addsuffix _cflow, $(addprefix $(BUILD_DIR)/, $(SRCS_FOR_CFLOW)))
	# хак, чтобы нормально сгенерилось оглавление... ¯\_(ツ)_/¯
	pdflatex -interaction=nonstopmode -output-directory $(BUILD_DIR) $(REPORT_TEX) > /dev/null
	pdflatex -interaction=nonstopmode -output-directory $(BUILD_DIR) $(REPORT_TEX) > /dev/null
	cp $(BUILD_DIR)/$(REPORT_PDF) ./$(REPORT_PDF)

# .c -> .pdf
$(BUILD_DIR)/%_cflow: $(SRC_DIR)/%.c
	cflow2dot -x cflowignore.txt -i $< -o $@ -f pdf

$(BUILD_DIR)/fsm.dot:
	$(FSM2DOT) $(AUTOGEN_DIR)/server.def > $@

# .dot -> .tex
$(BUILD_DIR)/%.tex: $(BUILD_DIR)/%.dot
	dot2tex -ftikz --autosize --crop  $< > $@

# .tex -> .pdf
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