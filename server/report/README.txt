to generate tex/make_pdf makefile_visualizer is used:

cmd for report make:
make -p | python3 ./utils/makefile_visualizer/make_p_to_json.py | python3 ./utils/makefile_visualizer/json_to_dot.py | dot -Tpdf >| ./tex/make_pdfs/report_make.pdf

cmd for server make:
make -p | python3 ../report/utils/makefile_visualizer/make_p_to_json.py | python3 ../report/utils/makefile_visualizer/json_to_dot.py | dot -Tpdf >| ../report/tex/make_pdfs/server_make.pdf
