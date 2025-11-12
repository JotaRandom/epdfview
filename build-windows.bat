@echo off
REM ePDFView Build Script for Windows (Batch)
REM Requires: MSYS2 with mingw-w64 toolchain

echo ePDFView Windows Build Script
echo =============================

REM Check if MSYS2 is installed
if not exist "C:\msys64\msys2.exe" (
    echo ERROR: MSYS2 not found at C:\msys64\
    echo Please install MSYS2 from https://www.msys2.org/
    exit /b 1
)

echo.
echo Step 1: Installing dependencies via MSYS2...
C:\msys64\usr\bin\bash.exe -lc "pacman -S --needed --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-ninja mingw-w64-x86_64-gtk4 mingw-w64-x86_64-poppler mingw-w64-x86_64-gettext"

if errorlevel 1 (
    echo ERROR: Failed to install dependencies
    exit /b 1
)

echo.
echo Step 2: Configuring build with Meson...
C:\msys64\usr\bin\bash.exe -lc "cd '%CD%' && meson setup build-windows --buildtype=release --prefix=/mingw64"

if errorlevel 1 (
    echo ERROR: Meson configuration failed
    exit /b 1
)

echo.
echo Step 3: Building...
C:\msys64\usr\bin\bash.exe -lc "cd '%CD%' && meson compile -C build-windows"

if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

echo.
echo ============================================
echo Build successful!
echo Executable: build-windows\src\epdfview.exe
echo ============================================
pause
