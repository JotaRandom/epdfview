# Data Files

This directory contains application resources and configuration files.

## Current Files

### Active (Used in GTK4 version)
- **epdfview.svg** - Application icon (SVG, scalable)
- **epdfview.desktop** / **epdfview.desktop.in** - Desktop entry file
- **epdfview.1** - Manual page
- **bsd.latexmkrc** / **dot.latexmkrc** - LaTeX integration configs
- **epdfsync** - LaTeX-PDF sync script

### Deprecated (GTK2/GTK3 legacy, not used in GTK4)
- **epdfview-ui.xml** - Old GTK UI definition (replaced by code-based UI)
- **epdfview-ui-print.xml** - Old print dialog UI (replaced by GTK4 PrintOperation)
- **stock_*.png** - Custom stock icons (GTK4 uses symbolic icons from theme)
- **icon_epdfview-*.png** - PNG icons in various sizes (redundant with SVG)

## Icon System

GTK4 uses the system icon theme with symbolic icons. Custom icons are no longer needed:
- Navigation: `go-next-symbolic`, `go-previous-symbolic`
- Zoom: `zoom-in-symbolic`, `zoom-out-symbolic`
- Rotation: System provides rotation icons
- Find: `edit-find-symbolic`

The SVG icon (`epdfview.svg`) is used for:
- Desktop launcher
- Window icon
- About dialog

## Cleanup Recommendation

Safe to remove (not used in GTK4 build):
```bash
rm -f epdfview-ui.xml
rm -f epdfview-ui-print.xml
rm -f stock_*.png
rm -f icon_epdfview-*.png
```

Keep these files for compatibility and functionality:
- All `.latexmkrc` files (LaTeX integration)
- `epdfsync` (LaTeX sync)
- `epdfview.svg` (main icon)
- `epdfview.desktop*` (desktop integration)
- `epdfview.1` (man page)
