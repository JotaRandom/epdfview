# Critical Bug Fixes - ePDFView GTK4

## Bug #1: Window Not Showing (FIXED ✅)

**Symptom:** Application compiled and ran but no window appeared. Exited immediately with code 0.

**Root Cause:** Two issues:
1. `MainView::show()` was never called in `on_activate()`
2. Window was not registered with GtkApplication

**Fix:**
1. Added `appData->mainView->show()` in `on_activate()` callback (main.cxx:92)
2. Added `gtk_application_add_window()` to register window (main.cxx:89)
3. Changed `gtk_widget_show()` to `gtk_window_present()` in MainView::show() (MainView.cxx:750)
4. Added sanity checks for window size in WSLg environments (MainView.cxx:749-751)

**Files Modified:**
- `src/main.cxx` - Added window registration and show() call
- `src/gtk/MainView.cxx` - Fixed show() method, added size validation
- `src/gtk/MainView.h` - Added getMainWindow() public getter

**Result:** Window now appears correctly on both native Linux and WSL with WSLg.

---

## Bug #2: File Open Dialog Crashes (IN PROGRESS 🔧)

**Symptom:** Clicking File → Open causes crash (coredump).

**Investigation Needed:**
- GApplication warning: "does not implement g_application_open()"
- Application was created with G_APPLICATION_HANDLES_OPEN but no handler

**Possible Issues:**
1. File dialog using deprecated GTK3 APIs
2. GApplication 'open' signal not connected
3. File loading code accessing invalid memory

**Next Steps:**
- Remove G_APPLICATION_HANDLES_OPEN flag (not needed)
- Debug file dialog with gdb
- Check MainView::openFileDialog() implementation

---

## Bug #3: WSLg Display Issues (FIXED ✅)

**Symptom:** "your 131072x1 screen size is bogus" error in WSLg

**Root Cause:** WSLg sometimes reports invalid screen dimensions

**Fix:** Added sanity checks in MainView::show():
```cpp
if (width <= 0 || width > 10000) width = 800;
if (height <= 0 || height > 10000) height = 600;
```

**Result:** Application uses sensible defaults when bogus dimensions detected.

---

## Testing Status

### Working ✅
- Application starts
- Window appears
- Window can be moved/resized
- Menus display
- Keyboard shortcuts work
- Exit works

### Broken ❌
- File → Open crashes
- Opening PDF from command line (needs open handler)

### Not Tested ⚠️
- All other features (pending file open fix)

---

## Running in WSL

**Required:**
```bash
export DISPLAY=:0
epdfview
```

**Normal Warnings (Safe to Ignore):**
- libEGL warning: DRI3 error
- Unable to acquire session bus
- bogus screen size (now handled)

---

## Next Session Priority

1. Fix file open dialog crash
2. Implement 'open' signal handler or remove G_APPLICATION_HANDLES_OPEN
3. Test with actual PDF files
4. Verify all menu items work

---

**Status:** Window showing works! File opening needs urgent fix.
**Date:** October 24, 2025
**Version:** 0.3.0
