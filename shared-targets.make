# -*-makefile-*-

tags:
	ctags --languages=c,c++ -Re .


# default doxygen dir, users of this file can override.
DOCSDIR=docs/api

doc: doxygen.project
	mkdir -p $(DOCSDIR)
	OUTPUT_DIRECTORY=$(DOCSDIR) doxygen doxygen.project

# make sure we do not execute the 'tags' goal by default.
# the default goal will be re-established as soon as another one is found.
# FIXME: what if another file before this one had established a default goal??
.DEFAULT_GOAL :=
