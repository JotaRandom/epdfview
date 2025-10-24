# Install application icon
install_data('epdfview.svg',
  install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', 'scalable', 'apps')
)

# Install man page
install_man('epdfview.1')

# Install desktop file
desktop_file = 'epdfview.desktop'
i18n.merge_file(
  input: desktop_file + '.in',
  output: desktop_file,
  type: 'desktop',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'applications')
)