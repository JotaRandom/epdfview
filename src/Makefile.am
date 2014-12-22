# Process this file with automake to produce a Makefile.in file.
SUBDIRS = gtk

noinst_LIBRARIES = libepdfview.a

libepdfview_a_SOURCES =	\
	Config.cxx	\
	Config.h	\
	DocumentLinkGoto.cxx	\
	DocumentLinkGoto.h	\
	DocumentLinkUri.cxx	\
	DocumentLinkUri.h	\
	DocumentOutline.cxx	\
	DocumentOutline.h	\
	DocumentPage.h	\
	DocumentPage.cxx	\
	DocumentRectangle.cxx	\
	DocumentRectangle.h	\
	epdfview.h	\
	FindPter.cxx	\
	FindPter.h	\
	gettext.h	\
	IDocument.cxx	\
	IDocument.h	\
	IDocumentLink.cxx	\
	IDocumentLink.h	\
	IDocumentObserver.h	\
	IFindView.h	\
	IJob.cxx	\
	IJob.h	\
	IMainView.h	\
	IPageView.h	\
	IPreferencesView.h	\
	JobFind.cxx	\
	JobFind.h	\
	JobLoad.cxx	\
	JobLoad.h	\
	JobRender.cxx	\
	JobRender.h	\
	JobSave.cxx	\
	JobSave.h	\
	MainPter.cxx	\
	MainPter.h	\
	PagePter.cxx	\
	PagePter.h	\
	PDFDocument.cxx	\
	PDFDocument.h	\
	PreferencesPter.cxx	\
	PreferencesPter.h

libepdfview_a_CXXFLAGS =	\
	$(GLIB_CFLAGS)	\
	$(POPPLER_CFLAGS)

bin_PROGRAMS = epdfview

epdfview_SOURCES =	\
	main.cxx

libshell_cxxflags = -I$(top_srcdir)/src/gtk $(GTK2_CFLAGS)
libshell_ldadd = $(top_builddir)/src/gtk/libshell-gtk.a $(GTK2_LIBS)

epdfview_CXXFLAGS =	\
	-DLOCALEDIR='"$(datadir)/locale"'	\
	$(GLIB_CFLAGS)	\
	$(POPPLER_CFLAGS)	\
	$(libshell_cxxflags)

epdfview_LDADD =	\
	libepdfview.a	\
	$(GLIB_LIBS)	\
	$(POPPLER_LIBS)	\
	$(libshell_ldadd)

epdfview_LDFLAGS =

if cups_printing
libepdfview_a_SOURCES +=	\
	IPrintView.h	\
	JobPrint.cxx	\
	JobPrint.h	\
	PrintPter.cxx	\
	PrintPter.h

libepdfview_a_CXXFLAGS +=	\
	$(CUPS_CFLAGS)

epdfview_LDADD +=	\
	$(CUPS_LIBS)
endif

if OS_WIN32
epdfview_LDFLAGS += -mwindows
endif

