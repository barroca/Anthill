all: manual.pdf
	
manual.aux: manual.tex
	latex manual.tex

#manual.bbl: manual.aux
#	bibtex manual

manual.dvi: manual.tex
	latex manual.tex
	latex manual.tex
	latex manual.tex

manual.pdf: manual.dvi
	dvipdfm manual.dvi

clean:
	rm -f *.log *.aux *.bbl *.blg *.dvi *.ps *.lot *.toc *.lof *.pdf
