project('epdfview', 'cpp',
  version : '0.3.0',
  default_options : ['warning_level=1', 'cpp_std=c++11', 'werror=false'],
  meson_version : '>= 0.50.0',
  license : 'GPL-2.0-or-later',
)

i18n = import('i18n')

# Build options
debug_build = get_option('buildtype') == 'debug'
if debug_build
  add_project_arguments('-DDEBUG', language : 'cpp')
else
  add_project_arguments('-DNDEBUG', '-DG_DISABLE_ASSERT', language : 'cpp')
endif

# Additional compiler flags
add_project_arguments('-Wno-long-long', language : 'cpp')

# Dependencies with version requirements
gtk_dep = dependency('gtk4', version : '>= 4.0.0')
poppler_dep = dependency('poppler-glib', version : '>= 0.5.0')
glib_dep = dependency('gthread-2.0', version : '>= 2.8.0')
pango_dep = dependency('pango')

# Check for CUPS
cups_dep = dependency('cups', required : get_option('cups'))
if cups_dep.found() and host_machine.system() != 'windows'
  message('CUPS support: ENABLED')
  add_project_arguments('-DHAVE_CUPS=1', language : 'cpp')
else
  message('CUPS support: DISABLED')
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
conf_data.set_quoted('DATADIR', join_paths(get_option('prefix'), get_option('datadir')))
configure_file(output : 'config.h', configuration : conf_data)

# Include directories
inc = include_directories('src', '.')

# Subdirectories
subdir('src')
subdir('data')
subdir('po')

# Tests (optional)
if get_option('tests')
  subdir('tests')
endif