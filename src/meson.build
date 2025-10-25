sources = [
  'Config.cxx',
  'DocumentLinkGoto.cxx',
  'DocumentLinkUri.cxx',
  'DocumentOutline.cxx',
  'DocumentPage.cxx',
  'DocumentRectangle.cxx',
  'FindPter.cxx',
  'IDocument.cxx',
  'IDocumentLink.cxx',
  'IJob.cxx',
  'JobFind.cxx',
  'JobLoad.cxx',
  'JobRender.cxx',
  'JobSave.cxx',
  'main.cxx',
  'MainPter.cxx',
  'PagePter.cxx',
  'PDFDocument.cxx',
  'PreferencesPter.cxx',
]

# GTK4 UI sources
gtk_sources = [
  'gtk/FindView.cxx',
  'gtk/MainView.cxx',
  'gtk/PageView.cxx',
  'gtk/PreferencesView.cxx',
  'gtk/StockIcons.cxx',
]

sources += gtk_sources

epdfview_deps = [
  gtk_dep,
  poppler_dep,
  glib_dep,
  pango_dep,
]

# Only compile print-related files when CUPS is available
if cups_dep.found() and host_machine.system() != 'windows'
  epdfview_deps += cups_dep
  sources += ['JobPrint.cxx', 'PrintPter.cxx', 'gtk/PrintView.cxx']
endif

# Create executable
executable('epdfview',
  sources,
  dependencies: epdfview_deps,
  include_directories: inc,
  install: true,
)