# ePDFView Documentation

This directory contains additional technical documentation for ePDFView.

## GTK4 Migration Documentation

ePDFView was successfully migrated from GTK2/GTK3 to GTK4 in October 2025.

### Main Migration Documents

- **GTK4_MIGRATION_SUMMARY.md** - Complete technical summary of the migration
  - All API changes documented
  - Before/after code comparisons
  - Migration challenges and solutions

- **COMPLETE_GTK4_MIGRATION.md** - Milestone documentation
  - Full feature list
  - Testing checklist
  - Build instructions

### Specific Component Documentation

- **MENU_SYSTEM_COMPLETE.md** - Menu system implementation
  - GMenuModel structure
  - Action system
  - Keyboard shortcuts

- **TRANSLATIONS_AND_DOCS.md** - i18n system details
  - Meson i18n integration
  - 22 language support

### Build & Installation

- **INSTALLATION.md** - Comprehensive installation guide
  - Detailed dependency information
  - Multiple installation scenarios
  - Packaging guidelines
  - Troubleshooting

- **BUILD_STATUS.md** - Build system milestones
- **COMPILATION_SUCCESS.md** - Initial successful build notes

### Issue Resolution

- **FINAL_FIXES.md** - Icon path resolution
  - Multiple search path implementation
  - Installation location handling

- **PRINT_MIGRATION_TODO.md** - Print system migration notes
  - Completed: Full CUPS integration
  - GTK4 dialog implementation

## For Developers

If you're working on ePDFView:

1. **Start with** GTK4_MIGRATION_SUMMARY.md to understand the codebase changes
2. **Reference** COMPLETE_GTK4_MIGRATION.md for feature overview
3. **Use** INSTALLATION.md for build setup
4. **See** root CONTRIBUTING file for development guidelines

## For Users

Most users should read the files in the root directory:

- **README** - Basic usage and features
- **INSTALL** - Installation instructions
- **TRANSLATION** - Translation guide
- **CONTRIBUTING** - How to contribute

## History

ePDFView started as a lightweight PDF viewer for GTK2/GNOME. After years of
maintenance, it was migrated to GTK4 to ensure future compatibility with modern
Linux desktops.

The migration was completed in October 2025 and included:
- Complete UI reimplementation with GTK4 APIs
- Menu system using GMenuModel
- Event handling with modern controllers
- Print system with CUPS integration
- Build system migration to Meson
- Full i18n support maintained

## Status

✅ **Migration Complete**  
✅ **All Features Working**  
✅ **Production Ready**

Version 0.3.0 represents the first stable GTK4 release.
