# ePDFView - A lightweight PDF Viewer

ePDFView is a free lightweight PDF document viewer using the Poppler library and GTK4.

## Features

- Continuous view mode
- Zoom modes (fit to page width, fit to page, custom zoom)
- Page rotation
- Find text in documents
- Print support (via CUPS)
- Bookmarks and outline navigation
- Keyboard shortcuts

## Building from Source

### Dependencies

- GTK4 (>= 4.0.0)
- Poppler (>= 0.5.0)
- Meson build system

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/epdfview.git
cd epdfview

# Configure with Meson
meson setup builddir
cd builddir

# Build
ninja

# Install
ninja install
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
