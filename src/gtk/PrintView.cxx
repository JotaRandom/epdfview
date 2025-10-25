// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006, 2007, 2009 Emma's Software.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <config.h>
#include <gettext.h>
#include <gtk/gtk.h>
#include <epdfview.h>
#include "StockIcons.h"
#include "PrintView.h"

using namespace ePDFView;

// Enumerations.
enum printerListColumns
{
    printerListNameColumn,
    printerListStateColumn,
    printerListJobsColumn,
    printerListLocationColumn,
    printerListNumColumn
};

enum printOptionsColumn
{
    printOptionLabelColumn,
    printOptionValueColumn,
    printOptionNumColumn
};

// Callbacks.
#if defined (HAVE_CUPS)
static void print_view_number_of_copies_changed (GtkSpinButton *, gpointer);
static void print_view_page_range_option_changed (GtkToggleButton *, gpointer);
static void print_view_printer_selection_changed (GtkTreeSelection *, gpointer);
static void print_view_dialog_response (GtkDialog *, int, gpointer);
#endif // HAVE_CUPS

PrintView::PrintView (GtkWindow *parent):
    IPrintView ()
{
    m_PrintDialog = gtk_dialog_new_with_buttons (_("Print"),
            parent, GTK_DIALOG_MODAL,
            _("_Cancel"), GTK_RESPONSE_CANCEL,
            _("_Print"), GTK_RESPONSE_ACCEPT,
            NULL);
    // GTK4: gtk_dialog_set_alternative_button_order removed
    // GTK4: gtk_window_set_skip_taskbar_hint removed (not needed on Wayland)

    GtkWidget *notebook = gtk_notebook_new ();
    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (m_PrintDialog));
    gtk_box_append (GTK_BOX (content_area), notebook);
    gtk_widget_set_margin_start (notebook, 6);
    gtk_widget_set_margin_end (notebook, 6);
    gtk_widget_set_margin_top (notebook, 6);
    gtk_widget_set_margin_bottom (notebook, 6);

    // Add the notebook's tabs.
    {
        GtkWidget *label = gtk_label_new (_("Printer"));
        gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
                                  createPrinterTab (), label);
    }
    {
        GtkWidget *label = gtk_label_new (_("Job"));
        gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
                                  createJobTab (), label);
    }
    {
        GtkWidget *label = gtk_label_new (_("Paper"));
        gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
                                  createPaperTab (), label);
    }
    // GTK4: Widgets are visible by default, no need for gtk_widget_show_all
}

PrintView::~PrintView ()
{
    gtk_window_destroy (GTK_WINDOW (m_PrintDialog));
}

void
PrintView::setPresenter (PrintPter *pter)
{
    IPrintView::setPresenter (pter);

    // Connect signals.
    g_signal_connect (G_OBJECT (m_NumberOfCopies), "value_changed",
                      G_CALLBACK (print_view_number_of_copies_changed),
                      pter);
    g_signal_connect (G_OBJECT (m_AllPagesRangeOption), "toggled",
                      G_CALLBACK (print_view_page_range_option_changed),
                      pter);
    g_signal_connect (G_OBJECT (m_CustomPagesRangeOption), "toggled",
                      G_CALLBACK (print_view_page_range_option_changed),
                      pter);
    GtkTreeSelection *selection =
        gtk_tree_view_get_selection (GTK_TREE_VIEW (m_PrinterListView));
    g_signal_connect (G_OBJECT (selection), "changed",
                      G_CALLBACK (print_view_printer_selection_changed),
                      pter);

    // Run the dialog.
    // GTK4: Use modal presentation with GMainLoop
    gtk_window_set_modal (GTK_WINDOW (m_PrintDialog), TRUE);
    gtk_window_present (GTK_WINDOW (m_PrintDialog));
    
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    int response = GTK_RESPONSE_CANCEL;
    int *response_ptr = &response;
    
    // Store loop pointer for response callback
    g_object_set_data (G_OBJECT (m_PrintDialog), "loop", loop);
    g_object_set_data (G_OBJECT (m_PrintDialog), "response", response_ptr);
    
    g_signal_connect (m_PrintDialog, "response",
                     G_CALLBACK (print_view_dialog_response), NULL);
    
    g_main_loop_run (loop);
    g_main_loop_unref (loop);
    
    if ( GTK_RESPONSE_ACCEPT == response )
    {
        pter->printActivated ();
    }
    else
    {
        pter->cancelActivated ();
    }
}

void
PrintView::addColorModel (const gchar *name, const gchar *value)
{
    addOptionToList (m_ColorModel, name, value);
}

void
PrintView::addPageSize (const gchar *name, const gchar *value)
{
    addOptionToList (m_PageSize, name, value);
}

void
PrintView::addPrinter (const gchar *name, int jobs, const gchar *state,
                       const gchar *location)
{
    GtkTreeIter printerIter;
    gtk_list_store_append (m_PrinterList, &printerIter);
    gtk_list_store_set (m_PrinterList, &printerIter,
                        printerListNameColumn, name,
                        printerListJobsColumn, jobs,
                        printerListStateColumn, state,
                        printerListLocationColumn, location,
                        -1);
}

void
PrintView::addResolution (const gchar *name, const gchar *value)
{
    addOptionToList (m_Resolution, name, value);
}

void
PrintView::clearColorModelList ()
{
    gtk_list_store_clear (m_ColorModel);
}

void
PrintView::clearPageSizeList ()
{
    gtk_list_store_clear (m_PageSize);
}

void
PrintView::clearResolutionList ()
{
    gtk_list_store_clear (m_Resolution);
}

unsigned int
PrintView::getNumberOfCopies ()
{
    return gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (m_NumberOfCopies));
}

gchar *
PrintView::getColorModel ()
{
    gchar *colorModel = NULL;
    getOptionFromComboBox (m_ColorModelView, &colorModel);
    return colorModel;
}

PrintPageLayout
PrintView::getPageLayout ()
{
    PrintPageLayout layout = PRINT_PAGE_LAYOUT_PLAIN;
    getOptionFromComboBox (m_LayoutView, &layout);
    return layout;
}

PrintPageOrientation
PrintView::getPageOrientation ()
{
    PrintPageOrientation orientation = PRINT_PAGE_ORIENTATION_PORTRAIT;
    getOptionFromComboBox (m_OrientationView, &orientation);
    return orientation;
}

const gchar *
PrintView::getPageRange ()
{
    // GTK4: gtk_entry_get_text → gtk_editable_get_text
    return gtk_editable_get_text (GTK_EDITABLE (m_PageRange));
}

gchar *
PrintView::getPageSize ()
{
    gchar *pageSize = NULL;
    getOptionFromComboBox (m_PageSizeView, &pageSize);
    return pageSize;
}

gchar *
PrintView::getResolution ()
{
    gchar *resolution = NULL;
    getOptionFromComboBox (m_ResolutionView, &resolution);
    return resolution;
}

gchar *
PrintView::getSelectedPrinterName ()
{
    gchar *printerName = NULL;
    GtkTreeSelection *selectedPrinter =
        gtk_tree_view_get_selection (GTK_TREE_VIEW (m_PrinterListView));
    GtkTreeIter selectedPrinterIter;
    if ( gtk_tree_selection_get_selected (selectedPrinter, NULL,
                                          &selectedPrinterIter) )
    {
        gtk_tree_model_get (GTK_TREE_MODEL (m_PrinterList),
                            &selectedPrinterIter,
                            printerListNameColumn, &printerName,
                            -1);
    }

    return printerName;
}

gboolean
PrintView::isCheckedCollate ()
{
    return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (m_Collate));
}

gboolean
PrintView::isSelectedAllPagesRangeOption ()
{
    return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (m_AllPagesRangeOption));
}

gboolean
PrintView::isSelectedEvenPageSet ()
{
    return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (m_EvenPageSet));
}

gboolean
PrintView::isSelectedOddPageSet ()
{
    return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (m_OddPageSet));
}

void
PrintView::selectColorModel (guint colorModelIndex)
{
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_ColorModelView),
                              colorModelIndex);
}

void
PrintView::selectPageSize (guint pageSizeIndex)
{
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_PageSizeView), pageSizeIndex);
}

void
PrintView::selectPrinter (guint printerIndex)
{
    gchar *pathString = g_strdup_printf ("%d", printerIndex);
    GtkTreeIter printerIter;
    if ( gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (m_PrinterList),
                                              &printerIter, pathString) )
    {
        GtkTreeSelection *selection =
            gtk_tree_view_get_selection (GTK_TREE_VIEW (m_PrinterListView));
        gtk_tree_selection_select_iter (selection, &printerIter);
    }
    g_free (pathString);
}

void
PrintView::selectResolution (guint resolutionIndex)
{
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_ResolutionView),
                              resolutionIndex);
}

void
PrintView::sensitiveCollate (gboolean sensitive)
{
    gtk_widget_set_sensitive (m_Collate, sensitive);
}

void
PrintView::sensitivePageRange (gboolean sensitive)
{
    gtk_widget_set_sensitive (m_PageRange, sensitive);
}

void
PrintView::sensitivePrintButton (gboolean sensitive)
{
    gtk_dialog_set_response_sensitive (GTK_DIALOG (m_PrintDialog),
                                       GTK_RESPONSE_ACCEPT, sensitive);
}

void
PrintView::addOptionToList (GtkListStore *optionList, const gchar *name,
                            const gchar *value)
{
    GtkTreeIter newOptionIter;
    gtk_list_store_append (optionList, &newOptionIter);
    gtk_list_store_set (optionList, &newOptionIter,
                        printOptionLabelColumn, name,
                        printOptionValueColumn, value,
                        -1);
}

void
PrintView::getOptionFromComboBox (GtkWidget *comboBox, gpointer value)
{
    GtkTreeIter optionIter;
    if ( gtk_combo_box_get_active_iter (GTK_COMBO_BOX (comboBox), &optionIter) )
    {
        GtkTreeModel *model =
            gtk_combo_box_get_model (GTK_COMBO_BOX (comboBox));
        gtk_tree_model_get (model, &optionIter,
                            printOptionValueColumn, value,
                            -1);
    }
}
// Tab Creators
////////////////////////////////////////////////////////////////

GtkWidget *
PrintView::createJobTab ()
{
    // GTK4: Use margins instead of gtk_container_set_border_width
    GtkWidget *mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start (mainBox, 3);
    gtk_widget_set_margin_end (mainBox, 3);
    gtk_widget_set_margin_top (mainBox, 3);
    gtk_widget_set_margin_bottom (mainBox, 3);

    // Print range frame.
    GtkWidget *printRangeLabel = gtk_label_new (_("<b>Print Range</b>"));
    GtkWidget *printRangeFrame = gtk_frame_new (NULL);
    gtk_box_append (GTK_BOX (mainBox), printRangeFrame); // GTK4: use append
    // Set a bold label.
    gtk_label_set_use_markup (GTK_LABEL (printRangeLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (printRangeFrame), printRangeLabel);
    // GTK4: gtk_frame_set_shadow_type removed
    
    // GTK4: Replace GtkAlignment with margins on child widget
    GtkWidget *printRangeGrid = gtk_grid_new (); // GTK4: GtkGrid instead of GtkTable
    gtk_widget_set_margin_start (printRangeGrid, 12);
    gtk_widget_set_margin_end (printRangeGrid, 6);
    gtk_widget_set_margin_top (printRangeGrid, 6);
    gtk_widget_set_margin_bottom (printRangeGrid, 0);
    gtk_frame_set_child (GTK_FRAME (printRangeFrame), printRangeGrid); // GTK4: set_child
    gtk_grid_set_row_spacing (GTK_GRID (printRangeGrid), 3);
    gtk_grid_set_column_spacing (GTK_GRID (printRangeGrid), 12);
    
    // Create the two radio buttons.
    m_AllPagesRangeOption =
        gtk_check_button_new_with_mnemonic (_("_All pages"));
    m_CustomPagesRangeOption =
        gtk_check_button_new_with_mnemonic (_("_Range:"));
    // GTK4: Radio buttons now use check buttons as group
    gtk_check_button_set_group (GTK_CHECK_BUTTON (m_CustomPagesRangeOption),
                                GTK_CHECK_BUTTON (m_AllPagesRangeOption));
    m_PageRange = gtk_entry_new ();
    
    // GTK4: gtk_grid_attach instead of gtk_table_attach
    gtk_grid_attach (GTK_GRID (printRangeGrid), m_AllPagesRangeOption,
                     0, 0, 2, 1);
    gtk_grid_attach (GTK_GRID (printRangeGrid), m_CustomPagesRangeOption,
                     0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (printRangeGrid), m_PageRange,
                     1, 1, 1, 1);

    // Page set frame
    GtkWidget *pageSetLabel = gtk_label_new (_("<b>Page Set</b>"));
    GtkWidget *pageSetFrame = gtk_frame_new (NULL);
    gtk_box_append (GTK_BOX (mainBox), pageSetFrame);
    gtk_label_set_use_markup (GTK_LABEL (pageSetLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (pageSetFrame), pageSetLabel);
    // GTK4: gtk_frame_set_shadow_type removed
    
    // GTK4: Replace GtkAlignment with margins on child
    GtkWidget *pageSetBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    gtk_widget_set_margin_start (pageSetBox, 12);
    gtk_widget_set_margin_end (pageSetBox, 6);
    gtk_widget_set_margin_top (pageSetBox, 6);
    gtk_widget_set_margin_bottom (pageSetBox, 0);
    gtk_box_set_homogeneous (GTK_BOX (pageSetBox), TRUE);
    gtk_frame_set_child (GTK_FRAME (pageSetFrame), pageSetBox);
    
    // GTK4: Radio buttons now use check buttons as group
    GtkWidget *allPageSetRadio =
        gtk_check_button_new_with_mnemonic (_("A_ll pages"));
    gtk_box_append (GTK_BOX (pageSetBox), allPageSetRadio);
    m_OddPageSet =
        gtk_check_button_new_with_mnemonic (_("O_dd pages"));
    gtk_check_button_set_group (GTK_CHECK_BUTTON (m_OddPageSet),
                                GTK_CHECK_BUTTON (allPageSetRadio));
    gtk_box_append (GTK_BOX (pageSetBox), m_OddPageSet);
    m_EvenPageSet =
        gtk_check_button_new_with_mnemonic (_("_Even pages"));
    gtk_check_button_set_group (GTK_CHECK_BUTTON (m_EvenPageSet),
                                GTK_CHECK_BUTTON (allPageSetRadio));
    gtk_box_append (GTK_BOX (pageSetBox), m_EvenPageSet);

    // Copies frame
    GtkWidget *copiesLabel = gtk_label_new (_("<b>Copies</b>"));
    GtkWidget *copiesFrame = gtk_frame_new (NULL);
    gtk_box_append (GTK_BOX (mainBox), copiesFrame);
    gtk_label_set_use_markup (GTK_LABEL (copiesLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (copiesFrame), copiesLabel);
    // GTK4: gtk_frame_set_shadow_type removed
    
    // GTK4: Replace GtkTable with GtkGrid
    GtkWidget *copiesGrid = gtk_grid_new ();
    gtk_widget_set_margin_start (copiesGrid, 12);
    gtk_widget_set_margin_end (copiesGrid, 6);
    gtk_widget_set_margin_top (copiesGrid, 6);
    gtk_widget_set_margin_bottom (copiesGrid, 0);
    gtk_frame_set_child (GTK_FRAME (copiesFrame), copiesGrid);
    gtk_grid_set_row_spacing (GTK_GRID (copiesGrid), 3);
    gtk_grid_set_column_spacing (GTK_GRID (copiesGrid), 12);
    
    // Create the Num. of Copies label and spin.
    GtkWidget *numCopiesLabel = gtk_label_new (_("N_umber of copies:"));
    // GTK4: gtk_misc_set_alignment removed
    gtk_label_set_xalign (GTK_LABEL (numCopiesLabel), 1.0);
    gtk_label_set_yalign (GTK_LABEL (numCopiesLabel), 0.5);
    gtk_label_set_use_underline (GTK_LABEL (numCopiesLabel), TRUE);
    m_NumberOfCopies = gtk_spin_button_new_with_range (1, 999, 1);
    gtk_label_set_mnemonic_widget (GTK_LABEL (numCopiesLabel),
                                   m_NumberOfCopies);
    // Create the collate check box.
    m_Collate = gtk_check_button_new_with_mnemonic (_("C_ollate"));
    
    gtk_widget_set_hexpand (m_NumberOfCopies, TRUE);
    gtk_widget_set_hexpand (m_Collate, TRUE);
    gtk_grid_attach (GTK_GRID (copiesGrid), numCopiesLabel, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (copiesGrid), m_NumberOfCopies, 1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (copiesGrid), m_Collate, 1, 1, 1, 1);

    return mainBox;
}

GtkWidget *
PrintView::createPaperTab ()
{
    // GTK4: Use margins instead of gtk_container_set_border_width
    GtkWidget *mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start (mainBox, 3);
    gtk_widget_set_margin_end (mainBox, 3);
    gtk_widget_set_margin_top (mainBox, 3);
    gtk_widget_set_margin_bottom (mainBox, 3);

    // Paper frame
    GtkWidget *paperLabel = gtk_label_new (_("<b>Paper and Layout</b>"));
    GtkWidget *paperFrame = gtk_frame_new (NULL);
    gtk_box_append (GTK_BOX (mainBox), paperFrame);
    gtk_label_set_use_markup (GTK_LABEL (paperLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (paperFrame), paperLabel);
    // GTK4: gtk_frame_set_shadow_type removed
    
    // GTK4: Replace GtkTable with GtkGrid
    GtkWidget *paperGrid = gtk_grid_new ();
    gtk_widget_set_margin_start (paperGrid, 12);
    gtk_widget_set_margin_end (paperGrid, 6);
    gtk_widget_set_margin_top (paperGrid, 6);
    gtk_widget_set_margin_bottom (paperGrid, 0);
    gtk_frame_set_child (GTK_FRAME (paperFrame), paperGrid);
    gtk_grid_set_row_spacing (GTK_GRID (paperGrid), 3);
    gtk_grid_set_column_spacing (GTK_GRID (paperGrid), 12);
    
    // Paper size and combobox
    GtkWidget *paperSizeLabel = gtk_label_new (_("Paper _Size:"));
    // GTK4: gtk_misc_set_alignment removed
    gtk_label_set_xalign (GTK_LABEL (paperSizeLabel), 1.0);
    gtk_label_set_yalign (GTK_LABEL (paperSizeLabel), 0.5);
    gtk_label_set_use_underline (GTK_LABEL (paperSizeLabel), TRUE);
    createPageSizeListModel ();
    m_PageSizeView = gtk_combo_box_new_with_model (GTK_TREE_MODEL (m_PageSize));
    gtk_label_set_mnemonic_widget (GTK_LABEL (paperSizeLabel), m_PageSizeView);
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (m_PageSizeView),
                                    renderer, TRUE);
        gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (m_PageSizeView),
                                        renderer,
                                        "text",
                                        printOptionLabelColumn, NULL);
    }
    gtk_widget_set_hexpand (m_PageSizeView, TRUE);
    gtk_grid_attach (GTK_GRID (paperGrid), paperSizeLabel, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (paperGrid), m_PageSizeView, 1, 0, 1, 1);
    // Page orientation label and combobox
    GtkWidget *pageOrientationLabel = gtk_label_new (_("Page _orientation:"));
    gtk_label_set_xalign (GTK_LABEL (pageOrientationLabel), 1.0);
    gtk_label_set_yalign (GTK_LABEL (pageOrientationLabel), 0.5);
    gtk_label_set_use_underline (GTK_LABEL (pageOrientationLabel), TRUE);

    createOrientationListModel ();
    m_OrientationView =
        gtk_combo_box_new_with_model (GTK_TREE_MODEL (m_Orientation));
    gtk_label_set_mnemonic_widget (GTK_LABEL (pageOrientationLabel),
                                   m_OrientationView);
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (m_OrientationView),
                                    renderer, TRUE);
        gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (m_OrientationView),
                                        renderer,
                                        "text",
                                        printOptionLabelColumn, NULL);
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_OrientationView), 0);

    gtk_widget_set_hexpand (m_OrientationView, TRUE);
    gtk_grid_attach (GTK_GRID (paperGrid), pageOrientationLabel, 0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (paperGrid), m_OrientationView, 1, 1, 1, 1);
    // Layout label and combobox
    GtkWidget *layoutLabel = gtk_label_new (_("_Layout:"));
    gtk_label_set_xalign (GTK_LABEL (layoutLabel), 1.0);
    gtk_label_set_yalign (GTK_LABEL (layoutLabel), 0.5);
    gtk_label_set_use_underline (GTK_LABEL (layoutLabel), TRUE);

    createLayoutListModel ();
    m_LayoutView = gtk_combo_box_new_with_model (GTK_TREE_MODEL (m_Layout));
    gtk_label_set_mnemonic_widget (GTK_LABEL (layoutLabel), m_LayoutView);
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (m_LayoutView),
                                    renderer, TRUE);
        gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (m_LayoutView),
                                        renderer,
                                        "text", printOptionLabelColumn, NULL);
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_LayoutView), 0);

    gtk_widget_set_hexpand (m_LayoutView, TRUE);
    gtk_grid_attach (GTK_GRID (paperGrid), layoutLabel, 0, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (paperGrid), m_LayoutView, 1, 2, 1, 1);

    // Output frame
    GtkWidget *outputLabel = gtk_label_new (_("<b>Output</b>"));
    GtkWidget *outputFrame = gtk_frame_new (NULL);
    gtk_box_append (GTK_BOX (mainBox), outputFrame);
    gtk_label_set_use_markup (GTK_LABEL (outputLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (outputFrame), outputLabel);
    // GTK4: gtk_frame_set_shadow_type removed
    
    // GTK4: Replace GtkTable with GtkGrid
    GtkWidget *outputGrid = gtk_grid_new ();
    gtk_widget_set_margin_start (outputGrid, 12);
    gtk_widget_set_margin_end (outputGrid, 6);
    gtk_widget_set_margin_top (outputGrid, 6);
    gtk_widget_set_margin_bottom (outputGrid, 0);
    gtk_frame_set_child (GTK_FRAME (outputFrame), outputGrid);
    gtk_grid_set_row_spacing (GTK_GRID (outputGrid), 3);
    gtk_grid_set_column_spacing (GTK_GRID (outputGrid), 12);
    
    // Color mode
    GtkWidget *colorModeLabel = gtk_label_new (_("_Mode:"));
    gtk_label_set_xalign (GTK_LABEL (colorModeLabel), 1.0);
    gtk_label_set_yalign (GTK_LABEL (colorModeLabel), 0.5);
    gtk_label_set_use_underline (GTK_LABEL (colorModeLabel), TRUE);
    createColorModelListModel ();
    m_ColorModelView =
        gtk_combo_box_new_with_model (GTK_TREE_MODEL (m_ColorModel));
    gtk_label_set_mnemonic_widget (GTK_LABEL (colorModeLabel),
                                   m_ColorModelView);
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (m_ColorModelView),
                                    renderer, TRUE);
        gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (m_ColorModelView),
                                        renderer,
                                        "text",
                                        printOptionLabelColumn, NULL);
    }
    gtk_widget_set_hexpand (m_ColorModelView, TRUE);
    gtk_grid_attach (GTK_GRID (outputGrid), colorModeLabel, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (outputGrid), m_ColorModelView, 1, 0, 1, 1);
    // Resolution
    GtkWidget *resolutionLabel = gtk_label_new (_("_Resolution:"));
    gtk_label_set_xalign (GTK_LABEL (resolutionLabel), 1.0);
    gtk_label_set_yalign (GTK_LABEL (resolutionLabel), 0.5);
    gtk_label_set_use_underline (GTK_LABEL (resolutionLabel), TRUE);

    createResolutionListModel ();
    m_ResolutionView =
        gtk_combo_box_new_with_model (GTK_TREE_MODEL (m_Resolution));
    gtk_label_set_mnemonic_widget (GTK_LABEL (resolutionLabel),
                                   m_ResolutionView);
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (m_ResolutionView),
                                    renderer, TRUE);
        gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (m_ResolutionView),
                                        renderer,
                                        "text",
                                        printOptionLabelColumn, NULL);
    }
    gtk_widget_set_hexpand (m_ResolutionView, TRUE);
    gtk_grid_attach (GTK_GRID (outputGrid), resolutionLabel, 0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (outputGrid), m_ResolutionView, 1, 1, 1, 1);

    return mainBox;
}

GtkWidget *
PrintView::createPrinterTab ()
{
    // GTK4: Use margins instead of gtk_container_set_border_width
    GtkWidget *mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start (mainBox, 3);
    gtk_widget_set_margin_end (mainBox, 3);
    gtk_widget_set_margin_top (mainBox, 3);
    gtk_widget_set_margin_bottom (mainBox, 3);

    createPrinterListModel ();
    m_PrinterListView =
        gtk_tree_view_new_with_model (GTK_TREE_MODEL (m_PrinterList));

    // Add scrollbars to treeview.
    // GTK4: gtk_scrolled_window_new takes no parameters
    scrollBox = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollBox),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    // GTK4: gtk_container_add → gtk_scrolled_window_set_child
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scrollBox), m_PrinterListView);
    // GTK4: gtk_box_pack_start_defaults → gtk_box_append with expand
    gtk_widget_set_vexpand (scrollBox, TRUE);
    gtk_box_append (GTK_BOX (mainBox), scrollBox);

    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        GtkTreeViewColumn *column =
            gtk_tree_view_column_new_with_attributes (_("Printer"), renderer,
                    "text", printerListNameColumn, NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (m_PrinterListView),
                                     column);
    }

    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        GtkTreeViewColumn *column =
            gtk_tree_view_column_new_with_attributes (_("State"), renderer,
                    "text", printerListStateColumn, NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (m_PrinterListView),
                                     column);
    }

    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        GtkTreeViewColumn *column =
            gtk_tree_view_column_new_with_attributes (_("Jobs"), renderer,
                    "text", printerListJobsColumn, NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (m_PrinterListView),
                                     column);
    }

    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        GtkTreeViewColumn *column =
            gtk_tree_view_column_new_with_attributes (_("Location"), renderer,
                    "text", printerListLocationColumn, NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (m_PrinterListView),
                                     column);
    }

    return mainBox;
}

////////////////////////////////////////////////////////////////
// Model Creators
////////////////////////////////////////////////////////////////

void
PrintView::createColorModelListModel ()
{
    m_ColorModel = gtk_list_store_new (printOptionNumColumn,
                                       G_TYPE_STRING,  // Label
                                       G_TYPE_STRING);    // Value
}

void
PrintView::createLayoutListModel ()
{
    m_Layout = gtk_list_store_new (printOptionNumColumn,
                                   G_TYPE_STRING,  // Label
                                   G_TYPE_INT);    // Value

    GtkTreeIter iter;
    gtk_list_store_append (m_Layout, &iter);
    gtk_list_store_set (m_Layout, &iter,
                        printOptionLabelColumn, _("Plain"),
                        printOptionValueColumn, PRINT_PAGE_LAYOUT_PLAIN,
                        -1);

    gtk_list_store_append (m_Layout, &iter);
    gtk_list_store_set (m_Layout, &iter,
                        printOptionLabelColumn, _("2 pages in 1"),
                        printOptionValueColumn, PRINT_PAGE_LAYOUT_2IN1,
                        -1);

    gtk_list_store_append (m_Layout, &iter);
    gtk_list_store_set (m_Layout, &iter,
                        printOptionLabelColumn, _("4 pages in 1"),
                        printOptionValueColumn, PRINT_PAGE_LAYOUT_4IN1,
                        -1);

    gtk_list_store_append (m_Layout, &iter);
    gtk_list_store_set (m_Layout, &iter,
                        printOptionLabelColumn, _("6 pages in 1"),
                        printOptionValueColumn, PRINT_PAGE_LAYOUT_6IN1,
                        -1);
}

void
PrintView::createOrientationListModel ()
{
    m_Orientation = gtk_list_store_new (printOptionNumColumn,
                                        G_TYPE_STRING,  // Label
                                        G_TYPE_INT);    // Value

    GtkTreeIter iter;
    gtk_list_store_append (m_Orientation, &iter);
    gtk_list_store_set (m_Orientation, &iter,
                printOptionLabelColumn, _("Portrait"),
                printOptionValueColumn, PRINT_PAGE_ORIENTATION_PORTRAIT,
                -1);

    gtk_list_store_append (m_Orientation, &iter);
    gtk_list_store_set (m_Orientation, &iter,
                printOptionLabelColumn, _("Landscape"),
                printOptionValueColumn, PRINT_PAGE_ORIENTATION_LANDSCAPE,
                -1);
}

void
PrintView::createPageSizeListModel ()
{
    m_PageSize = gtk_list_store_new (printOptionNumColumn,
                                     G_TYPE_STRING,  // Label
                                     G_TYPE_STRING); // Value
}

void
PrintView::createPrinterListModel ()
{
    m_PrinterList =
        gtk_list_store_new (printerListNumColumn,
                            G_TYPE_STRING,  // Name
                            G_TYPE_STRING,  // State
                            G_TYPE_INT,     // Jobs
                            G_TYPE_STRING); // Location

}

void
PrintView::createResolutionListModel ()
{
    m_Resolution = gtk_list_store_new (printOptionNumColumn,
                                       G_TYPE_STRING,  // Label
                                       G_TYPE_STRING); // Value
}

// Callbacks
////////////////////////////////////////////////////////////////

#if defined (HAVE_CUPS)
void
print_view_number_of_copies_changed (GtkSpinButton *spin, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    PrintPter *pter = (PrintPter *)data;
    pter->numberOfCopiesChanged ();
}

void
print_view_page_range_option_changed (GtkToggleButton *button, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    PrintPter *pter = (PrintPter *)data;
    pter->pageRangeOptionChanged ();
}

void
print_view_printer_selection_changed (GtkTreeSelection *sel, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    PrintPter *pter = (PrintPter *)data;
    pter->printerSelectionChanged ();
}

void
print_view_dialog_response (GtkDialog *dialog, int resp, gpointer data)
{
    // Get stored pointers
    GMainLoop *loop = (GMainLoop *)g_object_get_data (G_OBJECT (dialog), "loop");
    int *response_ptr = (int *)g_object_get_data (G_OBJECT (dialog), "response");
    
    if (response_ptr)
        *response_ptr = resp;
    
    if (loop && g_main_loop_is_running (loop))
        g_main_loop_quit (loop);
}
#endif // HAVE_CUPS
