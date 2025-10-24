# Design Document

## Overview

This design outlines the completion of ePDFView's migration from GTK2/Autotools to GTK4/Meson. The analysis shows the project is indeed halfway migrated, with extensive GTK_CHECK_VERSION conditionals throughout the codebase and both build systems currently present. The migration will be completed in phases to ensure stability and maintainability.

## Architecture

### Current State Analysis

The codebase currently contains:
- Dual GTK2/GTK4 support via `GTK_CHECK_VERSION(4,0,0)` conditionals
- GTK4Compat.h providing compatibility macros and functions
- Both Meson (meson.build) and Autotools (configure.ac, Makefile.am) build systems
- Mixed event handling approaches (legacy signals vs. modern controllers)

### Target Architecture

The final architecture will:
- Use only GTK4 APIs and widgets
- Employ modern GTK4 event controllers and gestures
- Utilize Meson as the sole build system
- Remove all compatibility layers and conditional compilation

## Components and Interfaces

### 1. GTK4 API Migration

**Affected Components:**
- `src/gtk/MainView.cxx` - Main window and menu system
- `src/gtk/PageView.cxx` - PDF page rendering and interaction
- `src/gtk/FindView.cxx` - Search functionality UI
- `src/DocumentPage.cxx` - Cairo region handling
- `src/PagePter.cxx` - Page presenter logic
- `src/PDFDocument.cxx` - Document text selection

**Key Changes:**
- Replace GtkAction with GAction/GSimpleAction
- Migrate from signal-based to controller-based event handling
- Update widget hierarchy management (container_add → specific append methods)
- Convert GdkRegion to cairo_region_t throughout
- Update cursor and display management APIs

### 2. Event System Modernization

**Legacy Event Handling (to be removed):**
```cpp
g_signal_connect(widget, "key-press-event", callback, data);
g_signal_connect(widget, "button-press-event", callback, data);
```

**Modern GTK4 Event Controllers (target):**
```cpp
GtkEventController *key_controller = gtk_event_controller_key_new();
gtk_widget_add_controller(widget, key_controller);
g_signal_connect(key_controller, "key-pressed", callback, data);

GtkGesture *click_gesture = gtk_gesture_click_new();
gtk_widget_add_controller(widget, GTK_EVENT_CONTROLLER(click_gesture));
```

### 3. Build System Consolidation

**Files to Remove:**
- `configure.ac` - Autotools configuration
- All `Makefile.am` files (7 total)
- `configure` script (generated)
- `m4/` directory macros
- `intl/` directory (if using modern gettext)

**Meson Enhancements:**
- Consolidate all source definitions in `src/meson.build`
- Add proper option handling for debug/release builds
- Implement comprehensive dependency checking
- Add install targets for desktop files and icons

### 4. Compatibility Layer Removal

**GTK4Compat.h Elimination:**
- Remove all GTK2/GTK4 compatibility macros
- Update direct API calls throughout codebase
- Eliminate conditional compilation blocks

**Code Cleanup:**
- Remove all `#if GTK_CHECK_VERSION(4,0,0)` blocks
- Keep only GTK4 code paths
- Update includes to use only GTK4 headers

## Data Models

### Configuration Management
- Maintain existing Config class interface
- Ensure Meson-based builds properly handle configuration paths
- Update any build-time configuration definitions

### Document Handling
- Preserve existing PDFDocument interface
- Ensure cairo_region_t is used consistently for text selections
- Maintain compatibility with current Poppler library versions

## Error Handling

### Migration Safety
- Implement feature parity testing between old and new builds
- Provide fallback mechanisms during transition
- Maintain error reporting consistency

### Build System Robustness
- Add comprehensive dependency checking in Meson
- Provide clear error messages for missing dependencies
- Support both debug and release build configurations

## Testing Strategy

### Functional Testing
1. **UI Functionality Tests**
   - Verify all menu actions work correctly
   - Test keyboard shortcuts and accelerators
   - Validate dialog boxes and user interactions

2. **Document Operations Tests**
   - PDF loading and rendering accuracy
   - Text selection and search functionality
   - Zoom and rotation operations
   - Print functionality (where CUPS is available)

3. **Build System Tests**
   - Clean Meson-only builds on multiple distributions
   - Installation and uninstallation procedures
   - Desktop integration (MIME types, .desktop files)

### Regression Testing
- Compare rendering output between old and new versions
- Verify performance characteristics remain acceptable
- Test on multiple GTK4 theme variants

### Platform Testing
- Test on major Linux distributions (Ubuntu, Fedora, Arch)
- Verify Wayland and X11 compatibility
- Test with different GTK4 versions (4.0+)

## Implementation Phases

### Phase 1: GTK4 API Completion
- Remove all GTK_CHECK_VERSION conditionals
- Update event handling to use controllers
- Modernize widget management APIs
- Test core functionality

### Phase 2: Build System Migration
- Remove Autotools files
- Enhance Meson configuration
- Update packaging and installation
- Test build on multiple platforms

### Phase 3: Code Cleanup and Optimization
- Remove compatibility layers
- Optimize for GTK4-specific features
- Update documentation and comments
- Final testing and validation

## Migration Risks and Mitigation

### Risk: Breaking Existing Functionality
**Mitigation:** Comprehensive testing at each phase, maintaining feature parity validation

### Risk: Build System Incompatibilities
**Mitigation:** Gradual transition with parallel build support during development

### Risk: GTK4 API Changes
**Mitigation:** Target stable GTK4 APIs, avoid experimental features

### Risk: Distribution Compatibility
**Mitigation:** Test on multiple distributions, provide clear dependency documentation