TEX_DIR			:= tex
BUILD_DIR		:= build
REPORT_TEX   	:= report.tex
REPORT_PDF   	:= report.pdf
AUTOGEN_DIR		:= ../../src/autogen
AUTOGEN_NAME	:= server.def

FSM2DOT			:= ../utils/def2dot

.PHONY: all report clean

all:
	mkdir -p $(TEX_DIR)/$(BUILD_DIR)
	make report --directory=$(TEX_DIR) --makefile=../Makefile

report: $(BUILD_DIR)/fsm.pdf
	# хак, чтобы нормально сгенерилось оглавление... ¯\_(ツ)_/¯
	pdflatex -interaction=nonstopmode -output-directory $(BUILD_DIR) $(REPORT_TEX)
	pdflatex -interaction=nonstopmode -output-directory $(BUILD_DIR) $(REPORT_TEX)
	cp $(BUILD_DIR)/$(REPORT_PDF) ./$(REPORT_PDF)

$(BUILD_DIR)/fsm.dot:
	$(FSM2DOT) $(AUTOGEN_DIR)/server.def > $@

# .dot -> .tex
$(BUILD_DIR)/%.tex: $(BUILD_DIR)/%.dot
	dot2tex -ftikz --autosize --crop  $< > $@

# .tex -> .pdf
$(BUILD_DIR)/%.pdf: $(BUILD_DIR)/%.tex
	pdflatex -output-directory $(BUILD_DIR) $<

clean:
	rm -rf $(TEX_DIR)/$(BUILD_DIR)

clean-report:
	rm -rf $(TEX_DIR)/$(REPORT_PDF)