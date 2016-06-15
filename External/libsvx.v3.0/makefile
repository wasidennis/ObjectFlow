OUTDIR=/tmp/texouts
FILENAME=$(shell ls -rt1 *.tex | tail -n 1 | xargs basename -s .tex)
# the above FILENAME variable will be populated with the most recently saved .tex file in the local directory
#  if you want to have a specific tex file compile, then just name it below instead
#FILENAME=name_me_here.tex

# these set some environment variables of interest.
#  You can edit the TEXINPUTS one to include additional paths on the tex search path
TEXMFOUTPUT := ${OUTDIR}
TEXINPUTS := ${TEXINPUTS}
BSTINPUTS := ${BSTINPUTS}

.SUFFIXES:	.otl .tex 

	

# note, with Skim and Mac OS X, we don't want to clear the outdir 
# first... as they will just reload the pdf.  Shouldn't cause a tex 
# problem, should it?
pmac:	vars $(FILENAME).tex
		mkdir -p ${OUTDIR}
		pdflatex -halt-on-error -output-directory ${OUTDIR} $(FILENAME).tex
		cp ${OUTDIR}/${FILENAME}.pdf .

plnx: vars clearoutdir	
		mkdir ${OUTDIR}
		pdflatex -halt-on-error -output-directory ${OUTDIR} $(FILENAME).tex
		cp ${OUTDIR}/${FILENAME}.pdf .

bib:	vars b
b:
		bibtex ${OUTDIR}/$(FILENAME).aux
		pdflatex -halt-on-error -draftmode -output-directory ${OUTDIR} $(FILENAME).tex
		pdflatex -halt-on-error -output-directory ${OUTDIR} $(FILENAME).tex
		cp ${OUTDIR}/${FILENAME}.pdf .

index: vars i
i:
		makeindex ${OUTDIR}/$(FILENAME)
		pdflatex -halt-on-error -output-directory ${OUTDIR} $(FILENAME).tex
		cp ${OUTDIR}/${FILENAME}.pdf .


# info for these lores directives is available at 
#  http://www.peteryu.ca/tutorials/publishing/pdf_manipulation_tips
lores300:	vars
		cp ${OUTDIR}/${FILENAME}.pdf ${OUTDIR}/temptemptemptemp.pdf
	  gs -sDEVICE=pdfwrite -dMaxSubsetPct=100 -dPDFSETTINGS=/printer -sOutputFile="${OUTDIR}/${FILENAME}.pdf" -dNOPAUSE -dBATCH "${OUTDIR}/temptemptemptemp.pdf" 
		rm ${OUTDIR}/temptemptemptemp.pdf
		cp ${OUTDIR}/${FILENAME}.pdf .

lores:	vars
		cp ${OUTDIR}/${FILENAME}.pdf ${OUTDIR}/temptemptemptemp.pdf
	  gs -sDEVICE=pdfwrite -dMaxSubsetPct=100 -dPDFSETTINGS=/ebook -sOutputFile="${OUTDIR}/${FILENAME}.pdf" -dNOPAUSE -dBATCH "${OUTDIR}/temptemptemptemp.pdf" 
		rm ${OUTDIR}/temptemptemptemp.pdf
		cp ${OUTDIR}/${FILENAME}.pdf .

tf:	vars
		latex2rtf -f0 $(FILENAME)

wc:	vars
		detex $(FILENAME) > $(OUTDIR)/$(FILENAME).txt
		wc -w $(OUTDIR)/$(FILENAME).txt

vars:
		@echo "TEXMFOUTPUT: " ${TEXMFOUTPUT}
		@echo "TEXINPUTS: "${TEXINPUTS}
		@echo "BSTINPUTS: "${TEXINPUTS}
		@echo "-------------------------------------------------------"

clearoutdir:
	rm -rf ${OUTDIR}

.otl.tex:	vars
	python /jjc/P/python/otl2latex/otl2latex.py -aq $< $@
		
			
