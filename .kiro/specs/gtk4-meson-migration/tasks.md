# Implementation Plan

- [x] 1. Complete GTK4 API migration in core components
  - Remove all GTK_CHECK_VERSION conditionals from core source files
  - Update cairo region handling to use only GTK4 APIs
  - Modernize widget and display management
  - _Requirements: 1.1, 1.2, 1.5_

- [x] 1.1 Update DocumentPage.cxx for GTK4-only cairo regions
  - Remove GTK_CHECK_VERSION blocks in invertRegion and setSelection methods
  - Use only cairo_region_t type throughout the file
  - Update method signatures to remove GdkRegion compatibility
  - _Requirements: 1.1, 1.4_

- [x] 1.2 Modernize PDFDocument.cxx text selection handling
  - Remove GTK_CHECK_VERSION conditionals in getTextRegion method
  - Use only cairo_region_t for text selection regions
  - Update poppler integration to work with GTK4 types
  - _Requirements: 1.1, 1.4_

- [x] 1.3 Update PagePter.cxx selection management
  - Remove GTK_CHECK_VERSION blocks in selection handling code
  - Use only cairo_region_t for m_LastSelection member
  - Update region comparison and copying logic
  - Update .gitignore and .gitattribute
  - _Requirements: 1.1, 1.4_

- [x] 2. Modernize GTK UI components with GTK4 event system
  - Replace legacy signal-based event handling with modern controllers
  - Update widget hierarchy management
  - Implement GTK4 gesture handling for mouse interactions
  - _Requirements: 1.2, 1.3_

- [x] 2.1 Modernize MainView.cxx event handling and actions
  - Replace GtkAction with GAction/GSimpleAction system
  - Remove GTK_CHECK_VERSION blocks and compatibility functions
  - Update menu and toolbar creation for GTK4
  - Implement modern accelerator handling
  - _Requirements: 1.2, 1.3_

- [x] 2.2 Update PageView.cxx to use GTK4 event controllers
  - Replace signal-based key and mouse event handling
  - Implement GtkEventControllerKey for keyboard events
  - Use GtkGestureClick for mouse button handling
  - Update scroll event handling with modern controllers
  - _Requirements: 1.2, 1.3_

- [x] 2.3 Modernize FindView.cxx event handling
  - Remove GTK_CHECK_VERSION conditionals in event callbacks
  - Update key press handling to use GtkEventControllerKey
  - Modernize widget creation and layout management
  - _Requirements: 1.2, 1.3_

- [x] 3. Remove compatibility layers and clean up code
  - Eliminate GTK4Compat.h compatibility layer
  - Remove all conditional compilation blocks
  - Update includes to use only GTK4 headers
  - _Requirements: 1.5, 4.1, 4.4_

- [x] 3.1 Remove GTK4Compat.h and update all references
  - Delete src/gtk/GTK4Compat.h file
  - Update all source files to use direct GTK4 API calls
  - Remove compatibility macro usage throughout codebase
  - _Requirements: 1.5, 4.4_

- [x] 3.2 Clean up main.cxx GTK initialization
  - Remove GTK_CHECK_VERSION conditionals in main function
  - Use only GTK4 initialization and main loop functions
  - Update application setup for GTK4 patterns
  - _Requirements: 1.1, 4.4_

- [x] 3.3 Update StockIcons.cxx for GTK4 icon handling
  - Remove GTK_CHECK_VERSION blocks in icon definitions
  - Use only named icons (no stock icons)
  - Update icon loading and management code
  - Fix ANY warning and build error
  - _Requirements: 1.2, 4.4_

- [x] 4. Complete Meson build system migration
  - Remove all Autotools configuration files
  - Update .gitignore
  - Enhance Meson build configuration
  - Update installation and packaging
  - Fix ANY warning related to building the program
  - _Requirements: 2.1, 2.2, 2.5_

- [x] 4.1 Remove Autotools build system files
  - Delete configure.ac and all Makefile.am files
  - Remove configure script and generated autotools files
  - Clean up m4 macros directory if no longer needed
  - _Requirements: 2.5, 4.2_

- [x] 4.2 Enhance Meson build configuration
  - Update src/meson.build to include all source files properly
  - Add comprehensive dependency checking and version requirements
  - Implement debug/release build options
  - Configure proper installation targets
  - _Requirements: 2.1, 2.4_

- [x] 4.3 Update root meson.build for complete build system
  - Remove any autotools references or fallbacks
  - Add proper project metadata and options
  - Configure internationalization with modern gettext
  - Set up testing framework integration
  - _Requirements: 2.1, 2.3_

- [ ] 5. Validate migration and ensure functionality
  - Test all PDF viewer features work correctly and fix any build issue
  - Verify build system works on multiple platforms
  - Ensure desktop integration functions properly
  - Update translations (po) files
  - _Requirements: 3.1, 3.2, 3.3, 5.1, 5.2_

- [ ] 5.1 Test core PDF functionality after migration
  - Update translations (po) files
  - Test all PDF viewer features work correctly and fix any build issue
  - Update translations (po) files again
  - Verify PDF loading and rendering accuracy
  - Test zoom modes (fit, width, custom) work correctly
  - Validate text search and selection functionality
  - Check page navigation and rotation features
  - _Requirements: 3.1, 3.2, 3.3_

- [ ] 5.2 Validate build system and installation
  - Test clean Meson build from scratch
  - Verify installation creates proper desktop integration
  - Check MIME type registration and file associations
  - Test on multiple Linux distributions
  - _Requirements: 2.2, 2.3, 5.1, 5.4_

- [ ] 5.3 Write unit tests for critical functionality
  - Create tests for document loading and rendering
  - Add tests for UI component initialization
  - Test build system configuration validation
  - _Requirements: 3.1, 3.2, 3.3_

- [ ] 5.4 Performance and compatibility testing
  - Benchmark rendering performance vs. original version
  - Test with various GTK4 themes and configurations
  - Validate Wayland and X11 display server compatibility
  - Clean repository from unneded and deprecated files
  - Update translations strings (PO directory).
  - _Requirements: 5.2, 5.5_