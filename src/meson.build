sources = [
  'Config.cxx',
  'FindPter.cxx',
  'IDocument.cxx',
  'IJob.cxx',
  'JobFind.cxx',
  'JobLoad.cxx',
  'JobPrint.cxx',
  'JobRender.cxx',
  'JobSave.cxx',
  'MainPter.cxx',
  'PagePter.cxx',
  'PrintPter.cxx',
  'main.cxx',
]

# GTK4 UI sources
subdir('gtk')

epdfview_deps = [
  gtk_dep,
  poppler_dep,
  glib_dep,
  pango_dep,
]

if cups_dep.found() and host_machine.system() != 'windows'
  epdfview_deps += cups_dep
endif

executable('epdfview',
  sources + gtk_sources,
  include_directories : inc,
  dependencies : epdfview_deps,
  install : true,
)