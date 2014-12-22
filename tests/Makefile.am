# Process this file with automake to produce a Makefile.in file.
TESTS=test_epdfview
noinst_PROGRAMS=test_epdfview
EXTRA_DIST = 	\
	test1.pdf	\
	test2.pdf	\
	test_encrypted.pdf

test_epdfview_SOURCES =	\
	ConfigTest.cxx	\
	ConfigTest.h	\
	DocumentOutlineTest.cxx	\
	DocumentOutlineTest.h	\
	DumbDocument.cxx	\
	DumbDocument.h	\
	DumbDocumentObserver.cxx	\
	DumbDocumentObserver.h	\
	DumbFindView.cxx	\
	DumbFindView.h	\
	DumbMainView.cxx	\
	DumbMainView.h	\
	DumbPageView.cxx	\
	DumbPageView.h	\
	DumbPreferencesView.cxx	\
	DumbPreferencesView.h	\
	FindPterTest.cxx	\
	FindPterTest.h	\
	main.cxx	\
	MainPterTest.cxx	\
	MainPterTest.h	\
	PagePterTest.cxx	\
	PagePterTest.h	\
	PDFDocumentTest.cxx	\
	PDFDocumentTest.h	\
	PreferencesPterTest.cxx	\
	PreferencesPterTest.h	\
	Utils.cxx	\
	Utils.h


test_epdfview_CXXFLAGS =	\
	-DTEST_DIR='"$(top_srcdir)/tests/"'	\
	-I$(top_srcdir)/src	\
	$(GLIB_CFLAGS)	\
	$(CPPUNIT_CFLAGS)

test_epdfview_LDADD =	\
	$(GLIB_LIBS)	\
	$(POPPLER_LIBS)	\
	$(CPPUNIT_LIBS)	\
	$(top_builddir)/src/libepdfview.a

if cups_printing
test_epdfview_SOURCES +=	\
	DumbPrintView.cxx	\
	DumbPrintView.h	\
	PrintPterTest.cxx	\
	PrintPterTest.h

test_epdfview_CXXFLAGS +=	\
	$(CUPS_CFLAGS)

test_epdfview_LDADD +=	\
	$(CUPS_LIBS)
endif
