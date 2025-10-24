# Technology Stack & Build System

## Core Technologies

- **Language**: C++ (C++11 standard)
- **UI Framework**: GTK4 (with GTK3 compatibility layer)
- **PDF Engine**: Poppler library (>= 0.5.0)
- **Threading**: GLib threading (gthread-2.0)
- **Printing**: CUPS (Unix/Linux only)
- **Internationalization**: GNU gettext
- **Testing**: CppUnit (optional, for unit tests)

## Build Systems

The project supports two build systems:

### Meson (Recommended)
```bash
# Configure
meson setup builddir
cd builddir

# Build
ninja

# Install
ninja install

# Run tests (if enabled)
ninja test
```

### Autotools (Legacy)
```bash
# Configure
./configure
# or for debug builds:
./configure --enable-debug

# Build
make

# Install
make install

# Run tests (if available)
make check
```

## Dependencies

- GTK4 (>= 4.0.0)
- Poppler-glib (>= 0.5.0) with GDK support
- GLib (>= 2.8.0) with threading
- Pango for text rendering
- CUPS (optional, for printing on Unix/Linux)
- CppUnit (optional, for unit tests)

## Compiler Flags

- Warning level 3 (`-Wall` for autotools, `warning_level=3` for meson)
- Debug builds include `-DDEBUG`, release builds use `-DNDEBUG -DG_DISABLE_ASSERT`
- C++11 standard required
- Additional flags: `-Wno-long-long` for C++

## Platform Support

- Primary: Linux/Unix systems
- Secondary: Windows (with some limitations, no CUPS printing)
- Cross-platform compatibility maintained through GTK4