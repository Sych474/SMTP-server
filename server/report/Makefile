TEX_DIR			:= tex
REPORT_TEX   	:= report.tex
AUTOGEN_DIR		:= ../../src/autogen
AUTOGEN_NAME	:= server.def

FSM2DOT			:= ../utils/def2dot

.PHONY: all report clean

all:
	make report --directory=$(TEX_DIR) --makefile=../Makefile

report: fsm.dotg
	# хак, чтобы нормально сгенерилось оглавление... ¯\_(ツ)_/¯
	pdflatex -interaction=nonstopmode $(REPORT_TEX)
	pdflatex -interaction=nonstopmode $(REPORT_TEX)

fsm.dotg:
	$(FSM2DOT) $(AUTOGEN_DIR)/server.def > $@

clean:
	rm -rf $(TEX_DIR)/*.pdf
	rm -rf $(TEX_DIR)/*.log
	rm -rf $(TEX_DIR)/*.out
	rm -rf $(TEX_DIR)/*.gz
	rm -rf $(TEX_DIR)/*.toc
	rm -rf $(TEX_DIR)/*.aux
	rm -rf $(TEX_DIR)/*.dotg