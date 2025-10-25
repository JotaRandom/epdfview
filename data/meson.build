# Install application icons
install_data('epdfview.svg',
  install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', 'scalable', 'apps')
)

# Install PNG icons at various sizes
# Keep original names in pixmaps for backward compatibility
install_data('icon_epdfview-24.png',
  install_dir: join_paths(get_option('datadir'), 'pixmaps')
)

install_data('icon_epdfview-32.png',
  install_dir: join_paths(get_option('datadir'), 'pixmaps')
)

install_data('icon_epdfview-48.png',
  install_dir: join_paths(get_option('datadir'), 'pixmaps')
)

# Also install to icon theme directories with standard names
install_data('icon_epdfview-32.png',
  install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', '32x32', 'apps'),
  rename: 'epdfview.png'
)

install_data('icon_epdfview-48.png',
  install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', '48x48', 'apps'),
  rename: 'epdfview.png'
)

# Install stock icons (used in toolbar)
stock_icons = [
  'stock_find_next_24.png',
  'stock_find_previous_24.png',
  'stock_rotate-270.png',
  'stock_rotate-90.png',
  'stock_zoom-page-width.png'
]

foreach icon : stock_icons
  install_data(icon,
    install_dir: join_paths(get_option('datadir'), 'pixmaps')
  )
endforeach

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