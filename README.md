# ePDFView - A lightweight PDF Viewer

ePDFView is a free lightweight PDF document viewer using the Poppler library and GTK4.

**Status:** Currently undergoing GTK4 migration (~∞% complete)

## Features

- Continuous view mode
- Zoom modes (fit to page width, fit to page, custom zoom)
- Page rotation
- Find text in documents
- ~~Print support (via CUPS)~~ - Temporarily disabled during GTK4 migration
- Bookmarks and outline navigation
- Keyboard shortcuts
- Color inversion mode (F8)
- Full screen mode (F11)

## Building from Source

### Dependencies

**Required:**
- GTK4 (>= 4.0.0)
- Poppler with GLib bindings (>= 0.5.0)
- GLib2 (>= 2.0)
- Pango
- Meson build system (>= 0.50)
- Ninja build tool

**Optional:**
- CUPS (for printing support - currently disabled)
- CppUnit (for running tests)

### Build Instructions

#### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt install meson ninja-build pkg-config \
                 libgtk-4-dev libpoppler-glib-dev \
                 libglib2.0-dev libpango1.0-dev

# Optional: for printing (not yet migrated to GTK4)
# sudo apt install libcups2-dev

# Clone the repository
git clone https://github.com/JotaRandom/epdfview.git
cd epdfview

# Configure with Meson (CUPS disabled for now)
meson setup builddir -Dcups=disabled

# Build
meson compile -C builddir

# Install (optional)
sudo meson install -C builddir

# Or run directly from build directory
./builddir/epdfview [file.pdf]
```

#### Linux (Fedora/RHEL)

```bash
# Install dependencies
sudo dnf install meson ninja-build pkgconfig \
                 gtk4-devel poppler-glib-devel \
                 glib2-devel pango-devel

# Build as above
meson setup builddir -Dcups=disabled
meson compile -C builddir
```

#### Linux (Arch)

```bash
# Install dependencies
sudo pacman -S meson ninja pkgconf gtk4 poppler-glib pango

# Build as above
meson setup builddir -Dcups=disabled
meson compile -C builddir
```

### Build Options

```bash
# Disable CUPS (printing)
meson setup builddir -Dcups=disabled

# Disable tests
meson setup builddir -Dtests=false

# Set custom prefix
meson setup builddir --prefix=/usr

# Reconfigure existing build
meson --reconfigure builddir
```

## GTK4 Migration Status

ePDFView is currently being migrated from GTK3 to GTK4. Most functionality is working:

### ✅ Completed
- Application lifecycle (GtkApplication)
- Page rendering and navigation
- Zoom and rotation
- Find text functionality
- Preferences dialog
- Keyboard shortcuts
- Event handling (using GTK4 event controllers)
- Modern widget APIs

### ⚠️ In Progress
- Menu system (currently placeholder - actions available via toolbar and shortcuts)
- File chooser dialogs (need final GTK4 API fixes)
- Window positioning/sizing (GTK4/Wayland compatibility)

### ❌ Not Yet Migrated
- Print functionality (requires extensive rewrite for GTK4)
- Traditional menu bar (removed in GTK4 - will use GMenuModel)

See `GTK4_MIGRATION_SUMMARY.md` and `BUILD_STATUS.md` for detailed migration information.

## Known Issues

- Traditional menu bar is not present (GTK4 removed GtkMenu widgets)
- All menu actions are accessible via toolbar buttons and keyboard shortcuts
- Window position is not saved/restored (GTK4/Wayland limitation)
- Printing is temporarily disabled

## Keyboard Shortcuts

- **Ctrl+O** - Open file
- **Ctrl+R** - Reload
- **Ctrl+S** - Save copy
- **Ctrl+W** - Close
- **Ctrl+F** - Find text
- **Ctrl+Plus/Minus** - Zoom in/out
- **Ctrl+\[ / Ctrl+\]** - Rotate left/right
- **F11** - Full screen
- **F9** - Toggle sidebar
- **F8** - Invert colors
- **F6** - Toggle toolbar

## Troubleshooting

### Application won't start
```bash
# Check GTK4 is installed
pkg-config --modversion gtk4

# Run with debug output
G_MESSAGES_DEBUG=all ./builddir/epdfview
```

### Icons missing
```bash
# Install icon theme
sudo apt install adwaita-icon-theme
```

### Build errors
```bash
# Clean and rebuild
rm -rf builddir
meson setup builddir -Dcups=disabled
meson compile -C builddir
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

**Priority areas for contributions:**
- Complete remaining GTK4 API fixes (see BUILD_STATUS.md)
- Implement GMenuModel for proper menu system
- Migrate print functionality to GTK4
- Testing on different Linux distributions
- Wayland-specific testing

## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
