#!/bin/bash
# ePDFView - Quick build script (skips system update)

set -e

echo "==================================="
echo "ePDFView GTK4 - Quick Build"
echo "==================================="
echo

cd "$(dirname "$0")"

echo "🧹 Cleaning old build..."
rm -rf builddir

echo
echo "⚙️  Configuring with Meson..."
meson setup builddir

echo
echo "🔨 Compiling..."
meson compile -C builddir

echo
echo "✅ Build complete!"
echo
ls -lh builddir/src/epdfview
echo
echo "🚀 Run with: ./builddir/src/epdfview"
