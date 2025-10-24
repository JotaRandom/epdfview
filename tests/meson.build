if get_option('tests')
  test_sources = [
    'ConfigTest.cxx',
    'DocumentOutlineTest.cxx',
    'DumbDocument.cxx',
    'DumbDocumentObserver.cxx',
    'DumbFindView.cxx',
    'DumbMainView.cxx',
    'DumbPageView.cxx',
    'DumbPreferencesView.cxx',
    'DumbPrintView.cxx',
    'FindPterTest.cxx',
    'main.cxx',
    'MainPterTest.cxx',
    'PagePterTest.cxx',
    'PDFDocumentTest.cxx',
    'PreferencesPterTest.cxx',
    'PrintPterTest.cxx',
    'Utils.cxx',
  ]

  test_deps = [
    gtk_dep,
    poppler_dep,
    glib_dep,
    pango_dep,
  ]

  # Add CUPS dependency if available
  if cups_dep.found() and host_machine.system() != 'windows'
    test_deps += cups_dep
  endif

  # Add CppUnit dependency if available
  cppunit_dep = dependency('cppunit', required: false)
  if cppunit_dep.found()
    test_deps += cppunit_dep
    
    epdfview_test = executable('epdfview-test',
      test_sources,
      include_directories: inc,
      dependencies: test_deps,
    )
    
    test('epdfview unit tests', epdfview_test)
  else
    warning('CppUnit not found, tests will not be built')
  endif
endif