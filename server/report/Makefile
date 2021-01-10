TEX_DIR		:= tex
REPORT_TEX   := report.tex

.PHONY: all report clean

all:
	make report --directory=$(TEX_DIR) --makefile=../Makefile

report:
	# хак, чтобы нормально сгенерилось оглавление... ¯\_(ツ)_/¯
	pdflatex -interaction=nonstopmode $(REPORT_TEX)
	pdflatex -interaction=nonstopmode $(REPORT_TEX)


clean:
	rm -rf $(TEX_DIR)/*.pdf
	rm -rf $(TEX_DIR)/*.log
	rm -rf $(TEX_DIR)/*.out
	rm -rf $(TEX_DIR)/*.gz
	rm -rf $(TEX_DIR)/*.toc
	rm -rf $(TEX_DIR)/*.aux