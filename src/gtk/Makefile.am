# Process this file with automake to produce a Makefile.in file.
noinst_LIBRARIES = libshell-gtk.a
libshell_gtk_a_SOURCES =	\
	FindView.cxx	\
	FindView.h	\
	MainView.cxx	\
	MainView.h	\
	PageView.cxx	\
	PageView.h	\
	PreferencesView.cxx	\
	PreferencesView.h	\
	StockIcons.cxx	\
	StockIcons.h

libshell_gtk_a_CXXFLAGS =	\
	-DDATADIR='"$(pkgdatadir)"'	\
	-I$(top_srcdir)/src	\
	$(GTK2_CFLAGS)

if cups_printing
libshell_gtk_a_SOURCES +=	\
	PrintView.cxx	\
	PrintView.h
endif
