# ePDFView Documentation Index

Welcome to ePDFView documentation! This project uses Markdown for all documentation.

## 📚 Documentation Files

### For End Users

- **[README.md](README.md)** - Project overview, features, and quick start
- **[INSTALL](INSTALL)** - Detailed build and installation instructions
- **[BUILD_STATUS.md](BUILD_STATUS.md)** - Current project status and known issues

### For Developers

- **[GTK4_MIGRATION_SUMMARY.md](GTK4_MIGRATION_SUMMARY.md)** - Complete GTK4 migration details
- **[COMPILATION_SUCCESS.md](COMPILATION_SUCCESS.md)** - Build system and compilation info
- **[MENU_SYSTEM_COMPLETE.md](MENU_SYSTEM_COMPLETE.md)** - Menu implementation details
- **[TRANSLATIONS_AND_DOCS.md](TRANSLATIONS_AND_DOCS.md)** - Translation and documentation systems

### Source Code Documentation

The source code is well-documented with inline comments. Key files:

```
src/
  ├── gtk/          # GTK4 UI implementation
  │   ├── MainView.cxx    # Main window and menu system
  │   ├── PageView.cxx    # PDF rendering view
  │   ├── FindView.cxx    # Search functionality
  │   └── PreferencesView.cxx
  ├── MainPter.cxx  # Main presenter/controller
  ├── PagePter.cxx  # Page presenter
  └── PDFDocument.cxx # Poppler integration
```

## 🌍 Translations

ePDFView supports 22 languages. Translation files are in the `po/` directory.

**Supported Languages:**
Arabic, Catalan, Czech, German, Greek, Spanish, Basque, French, Galician,
Hebrew, Hungarian, Italian, Japanese, Dutch, Polish, Portuguese,
Brazilian Portuguese, Russian, Swedish, Vietnamese, Chinese (Simplified),
Chinese (Traditional)

**For translators:** See [TRANSLATIONS_AND_DOCS.md](TRANSLATIONS_AND_DOCS.md) for details.

## 🔧 Build System

ePDFView uses **Meson** build system (no Autotools, no Doxygen).

**Quick Build:**
```bash
meson setup builddir -Dcups=disabled
meson compile -C builddir
./builddir/src/epdfview
```

**Full details:** See [INSTALL](INSTALL)

## 🎯 Project Status

**GTK4 Migration:** ✅ 100% Complete  
**Compilation:** ✅ Successful  
**Menu System:** ✅ Fully functional  
**Translations:** ✅ All working  

See [COMPILATION_SUCCESS.md](COMPILATION_SUCCESS.md) for details.

## 🐛 Reporting Issues

When reporting bugs, please include:
1. GTK4 version: `pkg-config --modversion gtk4`
2. Poppler version: `pkg-config --modversion poppler-glib`
3. Linux distribution and version
4. Steps to reproduce
5. Error messages or logs

## 📄 License

ePDFView is licensed under GPL-2.0-or-later.
See source files for full license text.

## 🤝 Contributing

Contributions welcome! Please:
1. Follow existing code style
2. Test your changes thoroughly
3. Update documentation if needed
4. Ensure translations still work

**Priority areas:**
- Testing on different distributions
- Wayland-specific testing
- Translation updates
- Bug fixes

## 📞 Contact

- **Project:** https://github.com/JotaRandom/epdfview
- **Original Project:** Emma's Software (archived)

---

**Note:** This project has been migrated from GTK2/GTK3 to GTK4.
Old Autotools and Doxygen documentation systems have been replaced
with modern Meson and Markdown for simplicity and maintainability.
