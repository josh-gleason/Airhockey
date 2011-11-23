.PHONY: report.pdf

report.pdf:	report.tex
	pdflatex report.tex
	pdflatex report.tex
	rm report.log report.aux

show:
ifeq ($(shell uname),Linux)
	evince report.pdf
else
	open report.pdf
endif

clean:
	rm report.pdf
