# Project Structure & Architecture

## Directory Organization

```
├── src/                    # Main source code
│   ├── gtk/               # GTK4-specific UI implementations
│   ├── *.h *.cxx         # Core application logic
│   └── main.cxx          # Application entry point
├── tests/                 # Unit tests (CppUnit)
├── data/                  # Application resources
│   ├── *.desktop         # Desktop integration files
│   ├── *.svg *.png       # Icons and images
│   └── *.xml             # UI definitions
├── po/                    # Internationalization files
├── doc/                   # Documentation
├── m4/                    # Autotools macros
├── intl/                  # GNU gettext internationalization
└── builddir/             # Meson build output (generated)
```

## Architecture Patterns

### MVP (Model-View-Presenter)
- **Model**: `PDFDocument`, `DocumentPage`, `DocumentOutline` classes
- **View**: Interface classes (`IMainView`, `IPageView`, etc.) with GTK implementations
- **Presenter**: `MainPter`, `PagePter`, `FindPter` classes that coordinate between model and view

### Interface Segregation
- Abstract interfaces define contracts (`IDocument`, `IMainView`, `IPageView`)
- Concrete implementations in `src/gtk/` for GTK4-specific code
- Clean separation between business logic and UI toolkit

### Observer Pattern
- `IDocumentObserver` interface for document state changes
- Presenters observe document events and update views accordingly

## File Naming Conventions

- **Headers**: `.h` extension
- **Implementation**: `.cxx` extension (not `.cpp`)
- **Interfaces**: Prefixed with `I` (e.g., `IMainView.h`)
- **Presenters**: Suffixed with `Pter` (e.g., `MainPter.h`)
- **GTK Views**: Located in `src/gtk/` directory

## Code Organization

### Core Components
- `main.cxx` - Application entry point and GTK initialization
- `Config.*` - Application configuration management
- `PDFDocument.*` - PDF document handling via Poppler
- `Job*.*` - Background job system for threading

### UI Layer
- `src/gtk/MainView.*` - Main application window
- `src/gtk/PageView.*` - PDF page rendering widget
- `src/gtk/FindView.*` - Search functionality UI
- `src/gtk/GTK4Compat.h` - GTK3/GTK4 compatibility layer

### Business Logic
- `MainPter.*` - Main application presenter
- `PagePter.*` - Page display logic
- `FindPter.*` - Search functionality logic

## Build Configuration

- `meson.build` - Modern build system configuration
- `configure.ac` + `Makefile.am` - Legacy autotools configuration
- Conditional compilation for optional features (CUPS, different Poppler versions)