project('epdfview', 'cpp',
  version : '0.1.8',
  default_options : ['warning_level=3', 'cpp_std=c++11'],
  meson_version : '>= 0.50.0',
)

# Dependencies
gtk_dep = dependency('gtk4')
poppler_dep = dependency('poppler-glib')
glib_dep = dependency('gthread-2.0')
pango_dep = dependency('pango')

# Check for CUPS
cups_dep = dependency('cups', required : false)
if cups_dep.found() and host_machine.system() != 'windows'
  add_project_arguments('-DHAVE_CUPS=1', language : 'cpp')
endif

# Check for Poppler version
if poppler_dep.version().version_compare('>=0.5.2')
  add_project_arguments('-DHAVE_POPPLER_0_5_2=1', language : 'cpp')
endif
if poppler_dep.version().version_compare('>=0.6.0')
  add_project_arguments('-DHAVE_POPPLER_0_6_0=1', language : 'cpp')
endif
if poppler_dep.version().version_compare('>=0.8.0')
  add_project_arguments('-DHAVE_POPPLER_0_8_0=1', language : 'cpp')
endif
if poppler_dep.version().version_compare('>=0.15.0')
  add_project_arguments('-DHAVE_POPPLER_0_15_0=1', language : 'cpp')
endif
if poppler_dep.version().version_compare('>=0.15.1')
  add_project_arguments('-DHAVE_POPPLER_0_15_1=1', language : 'cpp')
endif
if poppler_dep.version().version_compare('>=0.17.0')
  add_project_arguments('-DHAVE_POPPLER_0_17_0=1', language : 'cpp')
endif

# Configuration
conf_data = configuration_data()
conf_data.set_quoted('PACKAGE', 'epdfview')
conf_data.set_quoted('VERSION', meson.project_version())
conf_data.set_quoted('LOCALEDIR', join_paths(get_option('prefix'), get_option('localedir')))
configure_file(output : 'config.h', configuration : conf_data)

# Include directories
inc = include_directories('src')

# Subdirectories
subdir('src')
subdir('data')
subdir('po')
