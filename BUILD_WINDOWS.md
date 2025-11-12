# Building ePDFView on Windows

## Prerequisites

1. **Install MSYS2**
   - Download from: https://www.msys2.org/
   - Install to default location: `C:\msys64`
   - After installation, run MSYS2 and update packages:
     ```bash
     pacman -Syu
     ```

2. **Install Build Dependencies**
   - Open MSYS2 MinGW 64-bit terminal
   - Run:
     ```bash
     pacman -S --needed mingw-w64-x86_64-gcc \
         mingw-w64-x86_64-meson \
         mingw-w64-x86_64-ninja \
         mingw-w64-x86_64-gtk4 \
         mingw-w64-x86_64-poppler \
         mingw-w64-x86_64-gettext
     ```

## Building

### Option 1: Using PowerShell Script (Recommended)

1. Open PowerShell as Administrator
2. Navigate to the ePDFView directory
3. Run:
   ```powershell
   .\build-windows.ps1
   ```

### Option 2: Using Batch Script

1. Open Command Prompt
2. Navigate to the ePDFView directory
3. Run:
   ```batch
   build-windows.bat
   ```

### Option 3: Manual Build

1. Open MSYS2 MinGW 64-bit terminal
2. Navigate to the ePDFView directory
3. Run:
   ```bash
   meson setup build-windows --buildtype=release --prefix=/mingw64
   meson compile -C build-windows
   ```

## Running

After building, the executable will be at:
```
build-windows\src\epdfview.exe
```

To run it, you need the MSYS2 MinGW 64-bit environment. Either:
- Run from MSYS2 MinGW 64-bit terminal: `./build-windows/src/epdfview.exe`
- Or copy required DLLs from `C:\msys64\mingw64\bin` to the same folder as the executable

## Distribution

To create a distributable package, you'll need to bundle the executable with its dependencies:

1. Create a folder for the package
2. Copy `epdfview.exe`
3. Use `ldd` to find required DLLs:
   ```bash
   ldd build-windows/src/epdfview.exe
   ```
4. Copy all mingw64 DLLs to the package folder
5. Copy GTK4 runtime files (icons, schemas, etc.)

## Troubleshooting

- **MSYS2 not found**: Make sure MSYS2 is installed at `C:\msys64`
- **Build fails**: Update MSYS2 packages with `pacman -Syu`
- **Missing DLLs**: Ensure you're using MSYS2 MinGW 64-bit terminal, not MSYS2 MSYS terminal

## Notes

- ePDFView uses C++14 standard without GNU extensions for maximum portability
- GTK4 on Windows is fully supported through MSYS2/MinGW
- For native MSVC builds, additional work would be needed (not currently supported)
