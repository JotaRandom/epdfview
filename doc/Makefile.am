# Process this file with automake to produce a Makefile.in file.

if build_reference
reference:
	doxygen Doxyfile
else
reference:

endif

clean-local:
	rm -fr html

all: reference
