doc : doc-html doc-pdf


doc-html :
	@asciidoctor -r asciidoctor-diagram AFFB_IoT-Project.adoc 

doc-pdf :
	@asciidoctor-pdf -r asciidoctor-diagram AFFB_IoT-Project.adoc

.PHONY : clean
clean :
	@rm -f AFFB_IoT-Project.pdf AFFB_IoT-Project.html

