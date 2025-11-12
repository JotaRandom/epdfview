# ePDFView - Current Status

**Version:** 0.3.0  
**Last Updated:** November 12, 2025  
**Status:** ✅ **Stable**

---

## Quick Status

### In Working Features ✅
- PDF viewing and rendering
- Page navigation (keyboard, buttons, jump to page)
- Zoom (fit page, fit width, custom percentages)
- Find/Search with highlighting
- Document outline/index
- Full screen mode
- Page rotation
- Invert colors (night mode)
- Preferences dialog
- 22 language translations
- File operations (open, save copy)

### Known Issues ⚠️
- **Print dialog:** GListStore type compatibility (medium priority)
- **Compilation warnings:** ~82 unused parameter warnings (low priority)

### Recent Fixes (Nov 2025) ✅
- Fixed critical startup crash (SIGSEGV)
- Navigation buttons work correctly
- Page numbers update visually
- Windows build support added
- CI/CD for Linux, FreeBSD, Windows

---

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Linux (X11) | ✅ Fully working | Tested on Arch, Ubuntu, Fedora |
| Linux (Wayland) | ✅ Fully working | Native Wayland support |
| FreeBSD | ✅ Fully working | CI automated |
| Windows (MSYS2) | ✅ Working | See `docs/BUILD_WINDOWS.md` |
| WSL | ✅ Working | Requires X11 server |

---

## Build Status

![CI Build](https://github.com/JotaRandom/epdfview/actions/workflows/build.yml/badge.svg)

- **Linux:** ✅ Passing
- **FreeBSD:** ✅ Passing (fixed bash dependency)
- **Windows:** ✅ Passing (MSYS2)

---

## For More Information

- **Detailed Status:** See `PROJECT_STATUS.md`
- **Development Tasks:** See `docs/TODO_NEXT_SESSION.md`
- **Installation:** See `INSTALL` or `README`
- **Recent Fixes:** See `docs/CRASH_FIX_2025-11-12.md`
- **Build Guide (Windows):** See `docs/BUILD_WINDOWS.md`

---

## Quick Start

```bash
# Linux/BSD
meson setup builddir
meson compile -C builddir
./builddir/src/epdfview

# Install system-wide
sudo meson install -C builddir
```

For Windows or WSL, see platform-specific documentation in `docs/`.
