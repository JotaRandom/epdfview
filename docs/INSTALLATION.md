# ePDFView Installation Guide

## Quick Start

### Option 1: Install to /usr/local (Default)

```bash
# Build
meson setup builddir
meson compile -C builddir

# Install
sudo meson install -C builddir

# Icons will be at: /usr/local/share/pixmaps/
# Binary will be at: /usr/local/bin/epdfview
```

### Option 2: Install to /usr (System-wide)

```bash
# Build with custom prefix
meson setup builddir --prefix=/usr
meson compile -C builddir

# Install
sudo meson install -C builddir

# Icons will be at: /usr/share/pixmaps/
# Binary will be at: /usr/bin/epdfview
```

### Option 3: Build Only (No Install)

```bash
# Build
meson setup builddir
meson compile -C builddir

# Run from build directory
./builddir/src/epdfview
```

---

## Icon Search Paths

ePDFView automatically searches for icons in multiple locations:

1. **Configured prefix** - `$PREFIX/share/pixmaps/` (from meson setup)
2. **Local installation** - `/usr/local/share/pixmaps/`
3. **System location** - `/usr/share/pixmaps/`

This means icons will be found regardless of installation location!

---

## Dependencies

### Arch Linux
```bash
sudo pacman -S gtk4 poppler-glib meson ninja cups base-devel
```

### Ubuntu/Debian
```bash
sudo apt install libgtk-4-dev libpoppler-glib-dev meson ninja-build \
                 libcups2-dev build-essential gettext
```

### Fedora
```bash
sudo dnf install gtk4-devel poppler-glib-devel meson ninja-build \
                 cups-devel gcc-c++ gettext
```

### openSUSE
```bash
sudo zypper install gtk4-devel poppler-glib-devel meson ninja \
                    cups-devel gcc-c++ gettext-tools
```

---

## Build Options

### Disable CUPS (No Printing)
```bash
meson setup builddir -Dcups=disabled
meson compile -C builddir
```

### Debug Build
```bash
meson setup builddir --buildtype=debug
meson compile -C builddir
```

### Release Build (Optimized)
```bash
meson setup builddir --buildtype=release
meson compile -C builddir
```

### Custom Installation Directory
```bash
# Install to /opt/epdfview
meson setup builddir --prefix=/opt/epdfview
meson compile -C builddir
sudo meson install -C builddir
```

---

## Uninstall

```bash
# From the build directory
sudo ninja -C builddir uninstall
```

Or manually remove:
```bash
sudo rm /usr/local/bin/epdfview
sudo rm -r /usr/local/share/locale/*/LC_MESSAGES/epdfview.mo
sudo rm /usr/local/share/man/man1/epdfview.1
sudo rm /usr/local/share/applications/epdfview.desktop
sudo rm -r /usr/local/share/icons/hicolor/*/apps/epdfview.*
sudo rm /usr/local/share/pixmaps/icon_epdfview*
sudo rm /usr/local/share/pixmaps/stock_*
```

---

## Troubleshooting

### Icons Not Found

**Problem:** Warning about missing icons

**Solution:** Icons are automatically searched in multiple locations. If you still have issues:

1. **Check installation:**
   ```bash
   ls /usr/local/share/pixmaps/icon_epdfview*
   ls /usr/share/pixmaps/icon_epdfview*
   ```

2. **Reinstall:**
   ```bash
   sudo meson install -C builddir --skip-subprojects
   ```

3. **Use system prefix:**
   ```bash
   meson setup builddir --prefix=/usr --wipe
   meson compile -C builddir
   sudo meson install -C builddir
   ```

### CUPS Not Found

**Problem:** CUPS support disabled during configure

**Solution:** Install CUPS development files:
```bash
# Arch
sudo pacman -S cups

# Ubuntu/Debian
sudo apt install libcups2-dev

# Fedora
sudo dnf install cups-devel

# openSUSE
sudo zypper install cups-devel
```

Then reconfigure:
```bash
meson setup builddir --reconfigure
meson compile -C builddir
```

### Compilation Errors

**Problem:** Build fails with GTK4 errors

**Solution:** Ensure you have GTK4 >= 4.0.0:
```bash
pkg-config --modversion gtk4
```

If too old, upgrade your system or build GTK4 from source.

### Runtime Warnings

**Normal warnings (safe to ignore):**
- `libEGL warning` - WSL/Graphics acceleration
- `Unable to acquire session bus` - WSL/D-Bus
- `GtkTreeView deprecated` - Still functional in GTK4
- `GtkListStore deprecated` - Still functional in GTK4

**Should fix:**
- `Failed to open file` - Icon installation issue
- `assertion failed` - Widget initialization bug

---

## Creating a Package

### Arch Linux (PKGBUILD)

```bash
# Create PKGBUILD
pkgname=epdfview-gtk4
pkgver=0.3.0
pkgrel=1
pkgdesc="Lightweight PDF viewer using GTK4"
arch=('x86_64')
url="https://github.com/JotaRandom/epdfview"
license=('GPL2')
depends=('gtk4' 'poppler-glib' 'cups')
makedepends=('meson' 'ninja')

build() {
    meson setup build --prefix=/usr
    meson compile -C build
}

package() {
    DESTDIR="$pkgdir" meson install -C build
}
```

### Debian/Ubuntu (.deb)

```bash
# Install packaging tools
sudo apt install devscripts debhelper

# Create debian/ directory with control files
# Build package
dpkg-buildpackage -us -uc
```

### Flatpak

```yaml
# org.epdfview.ePDFView.yaml
app-id: org.epdfview.ePDFView
runtime: org.gnome.Platform
runtime-version: '47'
sdk: org.gnome.Sdk
command: epdfview
finish-args:
  - --share=ipc
  - --socket=fallback-x11
  - --socket=wayland
  - --filesystem=host
  - --socket=cups
modules:
  - name: poppler
    buildsystem: cmake
    # ... poppler build config
  
  - name: epdfview
    buildsystem: meson
    sources:
      - type: git
        url: https://github.com/JotaRandom/epdfview
        branch: main
```

---

## Post-Installation

### Update Icon Cache

```bash
# GTK icon cache
sudo gtk-update-icon-cache /usr/share/icons/hicolor/
sudo gtk-update-icon-cache /usr/local/share/icons/hicolor/

# Desktop database
sudo update-desktop-database
```

### Set as Default PDF Viewer

```bash
# XDG default
xdg-mime default epdfview.desktop application/pdf

# Or via GUI
# Settings → Applications → Default Applications → Documents → PDF
```

---

## Verify Installation

```bash
# Check binary
which epdfview
epdfview --version

# Check icons
ls -l /usr/local/share/pixmaps/icon_epdfview*

# Check desktop entry
desktop-file-validate /usr/local/share/applications/epdfview.desktop

# Check translations
ls /usr/local/share/locale/*/LC_MESSAGES/epdfview.mo

# Test run
epdfview /path/to/test.pdf
```

---

## Development Installation

For development, install to a local prefix:

```bash
# Install to ~/local
meson setup builddir --prefix=$HOME/local
meson compile -C builddir
meson install -C builddir

# Add to PATH
export PATH="$HOME/local/bin:$PATH"
export LD_LIBRARY_PATH="$HOME/local/lib:$LD_LIBRARY_PATH"

# Run
epdfview
```

---

## Clean Build

```bash
# Remove build directory
rm -rf builddir

# Clean git working directory (careful!)
git clean -xdf

# Start fresh
meson setup builddir
meson compile -C builddir
```

---

## Version Information

**Current Version:** 0.3.0  
**GTK Version Required:** >= 4.0.0  
**Poppler Version Required:** >= 0.5.0  
**Meson Version Required:** >= 0.50.0  

---

## Support

**Documentation:** See README.md  
**Issues:** https://github.com/JotaRandom/epdfview/issues  
**License:** GPL-2.0-or-later
