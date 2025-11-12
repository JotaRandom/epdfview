# ePDFView Build Script for Windows (PowerShell)
# Requires: MSYS2 with mingw-w64 toolchain

Write-Host "ePDFView Windows Build Script" -ForegroundColor Cyan
Write-Host "=============================" -ForegroundColor Cyan

# Check if running in MSYS2
if (-not (Test-Path "C:\msys64\msys2.exe")) {
    Write-Host "ERROR: MSYS2 not found at C:\msys64\" -ForegroundColor Red
    Write-Host "Please install MSYS2 from https://www.msys2.org/" -ForegroundColor Yellow
    exit 1
}

Write-Host "`nStep 1: Installing dependencies via MSYS2..." -ForegroundColor Green

# Run pacman to install dependencies
& "C:\msys64\usr\bin\bash.exe" -lc @"
pacman -S --needed --noconfirm \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-meson \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-gtk4 \
    mingw-w64-x86_64-poppler \
    mingw-w64-x86_64-gettext
"@

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Failed to install dependencies" -ForegroundColor Red
    exit 1
}

Write-Host "`nStep 2: Configuring build with Meson..." -ForegroundColor Green

& "C:\msys64\usr\bin\bash.exe" -lc @"
cd '$PWD'
meson setup build-windows --buildtype=release --prefix=/mingw64
"@

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Meson configuration failed" -ForegroundColor Red
    exit 1
}

Write-Host "`nStep 3: Building..." -ForegroundColor Green

& "C:\msys64\usr\bin\bash.exe" -lc @"
cd '$PWD'
meson compile -C build-windows
"@

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Build failed" -ForegroundColor Red
    exit 1
}

Write-Host "`n============================================" -ForegroundColor Cyan
Write-Host "Build successful!" -ForegroundColor Green
Write-Host "Executable: build-windows\src\epdfview.exe" -ForegroundColor Yellow
Write-Host "============================================" -ForegroundColor Cyan
