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
#endif // HAVE_CUPS

PrintView::PrintView (GtkWindow *parent):
    IPrintView ()
{
    m_PrintDialog = gtk_dialog_new_with_buttons (_("Print"),
            parent, GTK_DIALOG_MODAL,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_PRINT, GTK_RESPONSE_ACCEPT,
            NULL);
    // In some window managers the buttons must be reversed.
    gtk_dialog_set_alternative_button_order (GTK_DIALOG (m_PrintDialog),
            GTK_RESPONSE_ACCEPT, GTK_RESPONSE_CANCEL, -1);
    // Utility dialogs shouldn't show themselves in the window list.
    gtk_window_set_skip_taskbar_hint (GTK_WINDOW (m_PrintDialog), TRUE);

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
    // Note: gtk_dialog_run is deprecated but still functional in GTK4
    // For full GTK4 compliance, this should be replaced with async approach
    if ( GTK_RESPONSE_ACCEPT == gtk_dialog_run (GTK_DIALOG (m_PrintDialog)) )
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
    return gtk_entry_get_text (GTK_ENTRY (m_PageRange));
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

////////////////////////////////////////////////////////////////
// Tab Creators
////////////////////////////////////////////////////////////////

GtkWidget *
PrintView::createJobTab ()
{
    GtkWidget *mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width (GTK_CONTAINER (mainBox), 3);

    // Print range frame.
    GtkWidget *printRangeLabel = gtk_label_new (_("<b>Print Range</b>"));
    GtkWidget *printRangeFrame = gtk_frame_new (NULL);
    gtk_box_pack_start_defaults (GTK_BOX (mainBox), printRangeFrame);
    // Set a bold label and no border.
    gtk_label_set_use_markup (GTK_LABEL (printRangeLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (printRangeFrame), printRangeLabel);
    gtk_frame_set_shadow_type (GTK_FRAME (printRangeFrame), GTK_SHADOW_NONE);
    // The alignment.
    GtkWidget *printRangeAlign = gtk_alignment_new (0, 0, 1, 1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (printRangeAlign), 6, 0, 12, 6);
    gtk_container_add (GTK_CONTAINER (printRangeFrame), printRangeAlign);
    // The table to add all controls.
    GtkWidget *printRangeTable = gtk_table_new (2, 2, FALSE);
    gtk_container_add (GTK_CONTAINER (printRangeAlign), printRangeTable);
    gtk_table_set_row_spacings (GTK_TABLE (printRangeTable), 3);
    gtk_table_set_col_spacings (GTK_TABLE (printRangeTable), 12);
    // Create the two radio buttons.
    m_AllPagesRangeOption =
        gtk_radio_button_new_with_mnemonic (NULL, _("_All pages"));
    m_CustomPagesRangeOption =
        gtk_radio_button_new_with_mnemonic_from_widget (
                GTK_RADIO_BUTTON (m_AllPagesRangeOption), _("_Range:"));
    m_PageRange = gtk_entry_new ();
    gtk_table_attach_defaults (GTK_TABLE (printRangeTable),
                               m_AllPagesRangeOption,
                               0, 2, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (printRangeTable),
                               m_CustomPagesRangeOption,
                               0, 1, 1, 2);
    gtk_table_attach_defaults (GTK_TABLE (printRangeTable), m_PageRange,
                               1, 2, 1, 2);

    // Page set frame
    GtkWidget *pageSetLabel = gtk_label_new (_("<b>Page Set</b>"));
    GtkWidget *pageSetFrame = gtk_frame_new (NULL);
    gtk_box_pack_start_defaults (GTK_BOX (mainBox), pageSetFrame);
    // Set a bold label and no border.
    gtk_label_set_use_markup (GTK_LABEL (pageSetLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (pageSetFrame), pageSetLabel);
    gtk_frame_set_shadow_type (GTK_FRAME (pageSetFrame), GTK_SHADOW_NONE);
    // The alignment.
    GtkWidget *pageSetAlign = gtk_alignment_new (0, 0, 1, 1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (pageSetAlign), 6, 0, 12, 6);
    gtk_container_add (GTK_CONTAINER (pageSetFrame), pageSetAlign);
    // The vbox to add all controls.
    GtkWidget *pageSetBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_set_homogeneous (GTK_BOX (pageSetBox), TRUE);
    gtk_container_add (GTK_CONTAINER (pageSetAlign), pageSetBox);
    // Add the three radio buttons
    GtkWidget *allPageSetRadio =
        gtk_radio_button_new_with_mnemonic (NULL, _("A_ll pages"));
    gtk_box_pack_start_defaults (GTK_BOX (pageSetBox), allPageSetRadio);
    m_OddPageSet =
        gtk_radio_button_new_with_mnemonic_from_widget (
                GTK_RADIO_BUTTON (allPageSetRadio), _("O_dd pages"));
    gtk_box_pack_start_defaults (GTK_BOX (pageSetBox), m_OddPageSet);
    m_EvenPageSet =
        gtk_radio_button_new_with_mnemonic_from_widget (
                GTK_RADIO_BUTTON (allPageSetRadio), _("_Even pages"));
    gtk_box_pack_start_defaults (GTK_BOX (pageSetBox), m_EvenPageSet);

    // Copies frame
    GtkWidget *copiesLabel = gtk_label_new (_("<b>Copies</b>"));
    GtkWidget *copiesFrame = gtk_frame_new (NULL);
    gtk_box_pack_start_defaults (GTK_BOX (mainBox), copiesFrame);
    // Set a bold label and no border.
    gtk_label_set_use_markup (GTK_LABEL (copiesLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (copiesFrame), copiesLabel);
    gtk_frame_set_shadow_type (GTK_FRAME (copiesFrame), GTK_SHADOW_NONE);
    // The alignment.
    GtkWidget *copiesAlign = gtk_alignment_new (0, 0, 1, 1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (copiesAlign), 6, 0, 12, 6);
    gtk_container_add (GTK_CONTAINER (copiesFrame), copiesAlign);
    // The table to add all controls.
    GtkWidget *copiesTable = gtk_table_new (2, 2, FALSE);
    gtk_container_add (GTK_CONTAINER (copiesAlign), copiesTable);
    gtk_table_set_row_spacings (GTK_TABLE (copiesTable), 3);
    gtk_table_set_col_spacings (GTK_TABLE (copiesTable), 12);
    // Create the Num. of Copies label and spin.
    GtkWidget *numCopiesLabel = gtk_label_new (_("N_umber of copies:"));
    gtk_misc_set_alignment (GTK_MISC (numCopiesLabel), 1.0, 0.5);
    gtk_label_set_use_underline (GTK_LABEL (numCopiesLabel), TRUE);
    m_NumberOfCopies = gtk_spin_button_new_with_range (1, 999, 1);
    gtk_label_set_mnemonic_widget (GTK_LABEL (numCopiesLabel),
                                   m_NumberOfCopies);
    // Create the collate check box.
    m_Collate = gtk_check_button_new_with_mnemonic (_("C_ollate"));
    gtk_table_attach (GTK_TABLE (copiesTable), numCopiesLabel,
                      0, 1, 0, 1,
                      (GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
                      (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                      0, 0);
    gtk_table_attach_defaults (GTK_TABLE (copiesTable), m_NumberOfCopies,
                               1, 2, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (copiesTable), m_Collate,
                               1, 2, 1, 2);

    return mainBox;
}

GtkWidget *
PrintView::createPaperTab ()
{
    GtkWidget *mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width (GTK_CONTAINER (mainBox), 3);

    // Paper frame
    GtkWidget *paperLabel = gtk_label_new (_("<b>Paper and Layout</b>"));
    GtkWidget *paperFrame = gtk_frame_new (NULL);
    gtk_box_pack_start_defaults (GTK_BOX (mainBox), paperFrame);
    // Set a bold label and no border.
    gtk_label_set_use_markup (GTK_LABEL (paperLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (paperFrame), paperLabel);
    gtk_frame_set_shadow_type (GTK_FRAME (paperFrame), GTK_SHADOW_NONE);
    // The alignment.
    GtkWidget *paperAlign = gtk_alignment_new (0, 0, 1, 1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (paperAlign), 6, 0, 12, 6);
    gtk_container_add (GTK_CONTAINER (paperFrame), paperAlign);
    // The table to add all controls.
    GtkWidget *paperTable = gtk_table_new (3, 2, FALSE);
    gtk_container_add (GTK_CONTAINER (paperAlign), paperTable);
    gtk_table_set_row_spacings (GTK_TABLE (paperTable), 3);
    gtk_table_set_col_spacings (GTK_TABLE (paperTable), 12);
    // Paper size and combobox
    GtkWidget *paperSizeLabel = gtk_label_new (_("Paper _Size:"));
    gtk_misc_set_alignment (GTK_MISC (paperSizeLabel), 1.0, 0.5);
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
    gtk_table_attach (GTK_TABLE (paperTable), paperSizeLabel,
                      0, 1, 0, 1,
                      (GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);
    gtk_table_attach (GTK_TABLE (paperTable), m_PageSizeView,
                      1, 2, 0, 1,
                      (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);
    // Page orientation label and combobox
    GtkWidget *pageOrientationLabel = gtk_label_new (_("Page _orientation:"));
    gtk_misc_set_alignment (GTK_MISC (pageOrientationLabel), 1.0, 0.5);
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

    gtk_table_attach (GTK_TABLE (paperTable), pageOrientationLabel,
                      0, 1, 1, 2,
                      (GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);
    gtk_table_attach (GTK_TABLE (paperTable), m_OrientationView,
                      1, 2, 1, 2,
                      (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);
    // Layout label and combobox
    GtkWidget *layoutLabel = gtk_label_new (_("_Layout:"));
    gtk_misc_set_alignment (GTK_MISC (layoutLabel), 1.0, 0.5);
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

    gtk_table_attach (GTK_TABLE (paperTable), layoutLabel,
                      0, 1, 2, 3,
                      (GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);
    gtk_table_attach (GTK_TABLE (paperTable), m_LayoutView,
                      1, 2, 2, 3,
                      (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);

    // Output frame
    GtkWidget *outputLabel = gtk_label_new (_("<b>Output</b>"));
    GtkWidget *outputFrame = gtk_frame_new (NULL);
    gtk_box_pack_start_defaults (GTK_BOX (mainBox), outputFrame);
    // Set a bold label and no border.
    gtk_label_set_use_markup (GTK_LABEL (outputLabel), TRUE);
    gtk_frame_set_label_widget (GTK_FRAME (outputFrame), outputLabel);
    gtk_frame_set_shadow_type (GTK_FRAME (outputFrame), GTK_SHADOW_NONE);
    // The alignment.
    GtkWidget *outputAlign = gtk_alignment_new (0, 0, 1, 1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (outputAlign), 6, 0, 12, 6);
    gtk_container_add (GTK_CONTAINER (outputFrame), outputAlign);
    // The table to add all controls.
    GtkWidget *outputTable = gtk_table_new (2, 2, FALSE);
    gtk_container_add (GTK_CONTAINER (outputAlign), outputTable);
    gtk_table_set_row_spacings (GTK_TABLE (outputTable), 3);
    gtk_table_set_col_spacings (GTK_TABLE (outputTable), 12);
    // Color mode
    GtkWidget *colorModeLabel = gtk_label_new (_("_Mode:"));
    gtk_misc_set_alignment (GTK_MISC (colorModeLabel), 1.0, 0.5);
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
    gtk_table_attach (GTK_TABLE (outputTable), colorModeLabel,
                      0, 1, 0, 1,
                      (GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);
    gtk_table_attach (GTK_TABLE (outputTable), m_ColorModelView,
                      1, 2, 0, 1,
                      (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);
    // Resolution
    GtkWidget *resolutionLabel = gtk_label_new (_("_Resolution:"));
    gtk_misc_set_alignment (GTK_MISC (resolutionLabel), 1.0, 0.5);
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
    gtk_table_attach (GTK_TABLE (outputTable), resolutionLabel,
                      0, 1, 1, 2,
                      (GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);
    gtk_table_attach (GTK_TABLE (outputTable), m_ResolutionView,
                      1, 2, 1, 2,
                      (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions)(GTK_SHRINK),
                      0, 0);

    return mainBox;
}

GtkWidget *
PrintView::createPrinterTab ()
{
    GtkWidget *mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width (GTK_CONTAINER (mainBox), 3);

    createPrinterListModel ();
    m_PrinterListView =
        gtk_tree_view_new_with_model (GTK_TREE_MODEL (m_PrinterList));

    // Add scrollbars to treeview.
    scrollBox = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollBox),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (scrollBox), m_PrinterListView);
    gtk_container_add (GTK_CONTAINER (mainBox), scrollBox);

    gtk_box_pack_start_defaults (GTK_BOX (mainBox), m_PrinterListView);

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
#endif // HAVE_CUPS
