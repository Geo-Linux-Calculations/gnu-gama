FILE=gama-manual

$(FILE).pdf : $(FILE).tex $(FILE).bib Makefile
	pdflatex $(FILE)
	biber    $(FILE)
	pdflatex $(FILE)

clean:
	rm -rf *~ *.aux *.bbl *.bcf *.blg *.log *.xml *.bak

dist-clean:
	make clean
	rm -f $(FILE).pdf
