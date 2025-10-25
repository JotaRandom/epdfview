# Print Functionality - GTK4 Migration TODO

## Status: ⚠️ NOT YET MIGRATED

Print functionality (PrintView.cxx) contains extensive GTK3 code that requires significant refactoring for GTK4.

## Current Build Status

**Recommended:** Keep CUPS disabled during build:
```bash
meson setup builddir -Dcups=disabled
```

This excludes PrintView.cxx and JobPrint.cxx from compilation.

## Required Changes for GTK4

PrintView.cxx (840 lines) uses many deprecated APIs:

### 1. Container APIs (70+ occurrences)
- ❌ `gtk_container_set_border_width()` → ✅ Use widget margins
- ❌ `gtk_container_add()` → ✅ `gtk_frame_set_child()`, `gtk_box_append()`
- ❌ `gtk_box_pack_start_defaults()` → ✅ `gtk_box_append()` with properties

### 2. Layout Widgets (30+ occurrences)
- ❌ `GtkTable` / `gtk_table_new()` → ✅ `GtkGrid` / `gtk_grid_new()`
- ❌ `gtk_table_attach()` → ✅ `gtk_grid_attach()`
- ❌ `GtkAlignment` / `gtk_alignment_new()` → ✅ Use widget margins
- ❌ `gtk_alignment_set_padding()` → ✅ `gtk_widget_set_margin_*()`

### 3. Misc Widgets (20+ occurrences)
- ❌ `GtkMisc` / `gtk_misc_set_alignment()` → ✅ `gtk_label_set_xalign/yalign()`
- ❌ `gtk_frame_set_shadow_type()` → ✅ Removed (use CSS if needed)
- ❌ `GtkRadioButton` groups → ✅ `GtkCheckButton` with `gtk_check_button_set_group()`

### 4. Dialog APIs
- ❌ `gtk_dialog_run()` → ✅ Async with `gtk_window_present()` + GMainLoop
- ❌ `gtk_window_set_skip_taskbar_hint()` → ✅ Removed (not needed)
- ❌ Stock buttons → ✅ Use custom labels

### 5. Tree/List View (if using deprecated patterns)
- ⚠️ `GtkTreeView` → Modern apps use `GtkListView`/`GtkColumnView`
- Current usage may work but generates warnings

## Migration Strategy

### Phase 1: Dialog Structure ✅ (Partially Done)
- [x] Basic dialog creation
- [ ] Remove all container deprecated APIs
- [ ] Replace GtkTable with GtkGrid
- [ ] Replace GtkAlignment with margins

### Phase 2: Tab Content (3 tabs to fix)
- [ ] **Printer Tab** - GtkTreeView for printer list
- [ ] **Job Tab** - Radio buttons, entry, spin button
- [ ] **Paper Tab** - Combo boxes for paper settings

### Phase 3: CUPS Integration
- [ ] Review JobPrint.cxx for GTK4 changes
- [ ] Test actual printing functionality
- [ ] Handle print job status

### Phase 4: Modern Alternative (Optional)
Consider using GTK4's built-in print dialog:
```c
GtkPrintOperation *print_op = gtk_print_operation_new();
// Configure and run
gtk_print_operation_run(print_op, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, 
                        parent, NULL);
```

This would:
- ✅ Be fully GTK4 native
- ✅ Integrate with system print dialog
- ✅ Handle all OS-specific printing
- ❌ Lose custom UI we currently have
- ❌ Less control over CUPS-specific options

## Estimated Effort

**Full Custom Dialog Migration:** 8-12 hours
- Complex due to 3 tabs, many widgets
- Lots of testing needed
- CUPS integration verification

**Switch to GtkPrintOperation:** 2-4 hours  
- Simpler, more maintainable
- Standard GTK4 approach
- May lose some custom features

## For Now

**Recommendation:** Build without CUPS until ready to tackle print migration:

```bash
# In meson_options.txt
option('cups', type: 'feature', value: 'disabled')

# Or at build time
meson setup builddir -Dcups=disabled
```

All other functionality works perfectly without printing!

## When Ready to Migrate

1. Study GTK4 print examples: https://docs.gtk.org/gtk4/class.PrintOperation.html
2. Decide: Custom dialog vs GtkPrintOperation
3. Systematically replace deprecated APIs
4. Test with actual printers

## Resources

- GTK4 Printing: https://docs.gtk.org/gtk4/printing.html
- Migration Guide: https://docs.gtk.org/gtk4/migrating-3to4.html
- GtkGrid: https://docs.gtk.org/gtk4/class.Grid.html
- Widget Margins: https://docs.gtk.org/gtk4/method.Widget.set_margin_start.html
