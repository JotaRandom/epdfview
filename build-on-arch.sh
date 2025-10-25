#!/bin/bash
# ePDFView - Build script for Arch Linux (WSL)

set -e  # Exit on error

echo "==================================="
echo "ePDFView GTK4 - Arch Linux Build"
echo "==================================="
echo

# Navigate to project directory
cd "$(dirname "$0")"

echo "📦 Step 0: Updating system..."
sudo pacman -Syu --noconfirm

echo "📦 Step 1: Installing dependencies..."
sudo pacman -S --needed --noconfirm \
    base-devel \
    gtk4 \
    poppler-glib \
    meson \
    ninja \
    cups \
    gettext

echo
echo "✅ Dependencies installed!"
echo

echo "🧹 Step 2: Cleaning old build..."
rm -rf builddir

echo
echo "⚙️  Step 3: Configuring with Meson..."
meson setup builddir

echo
echo "🔨 Step 4: Compiling..."
meson compile -C builddir

echo
echo "✅ Build complete!"
echo
echo "📊 Build Summary:"
ls -lh builddir/src/epdfview
echo
echo "🚀 To run:"
echo "   ./builddir/src/epdfview [file.pdf]"
echo
echo "💾 To install:"
echo "   sudo meson install -C builddir"
echo
