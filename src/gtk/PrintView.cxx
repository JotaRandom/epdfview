// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - A lightweight PDF Viewer.
// 
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

// Type registration for OptionData
GType option_data_get_type(void)
{
    static GType type = 0;
    if (type == 0) {
        type = g_boxed_type_register_static("OptionData",
            (GBoxedCopyFunc)option_data_copy,
            (GBoxedFreeFunc)option_data_free);
    }
    return type;
}

// Type registration for PrinterData
GType printer_data_get_type(void)
{
    static GType type = 0;
    if (type == 0) {
        type = g_boxed_type_register_static("PrinterData",
            (GBoxedCopyFunc)printer_data_copy,
            (GBoxedFreeFunc)printer_data_free);
    }
    return type;
}

// Copy function for OptionData
OptionData *
option_data_copy(const OptionData *data) {
    OptionData *new_data = g_new0(OptionData, 1);
    new_data->name = g_strdup(data->name);
    new_data->value = g_strdup(data->value);
    return new_data;
}

// Free function for OptionData
void
option_data_free(OptionData *data) {
    g_free(data->name);
    g_free(data->value);
    g_free(data);
}

// Copy function for PrinterData
PrinterData *
printer_data_copy(const PrinterData *data) {
    PrinterData *new_data = g_new0(PrinterData, 1);
    new_data->name = g_strdup(data->name);
    new_data->state = g_strdup(data->state);
    new_data->jobs = data->jobs;
    new_data->location = g_strdup(data->location);
    return new_data;
}

// Free function for PrinterData
void
printer_data_free(PrinterData *data) {
    g_free(data->name);
    g_free(data->state);
    g_free(data->location);
    g_free(data);
}

using namespace ePDFView;

// Enumerations.
// These are no longer needed as we're using GListModel with custom data types
// but keeping them for compatibility with existing code

// Callbacks.
#if defined (HAVE_CUPS)
static void print_view_number_of_copies_changed (GtkSpinButton *, gpointer);
static void print_view_page_range_option_changed (GtkCheckButton *, gpointer);
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
    
    // GTK4: Selection already connected in createPrinterTab via notify::selected
    // No need for TreeView-style selection signals

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
    
    // GTK4: Instead of g_main_loop_run which blocks events, manually iterate
    while (g_main_loop_is_running(loop)) {
        g_main_context_iteration(NULL, TRUE);
    }
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
PrintView::addPrinter (const gchar *name, gint jobs, const gchar *state,
                      const gchar *location)
{
    // Create a new PrinterData structure
    PrinterData *data = g_new0(PrinterData, 1);
    data->name = g_strdup(name);
    data->jobs = jobs;
    data->state = g_strdup(state);
    data->location = g_strdup(location);
    
    // Add the data to the GListStore
    g_list_store_append(m_PrinterList, data);
    
    // The list store takes ownership of the data, so we don't need to free it
    g_object_unref(data);
}

void
PrintView::addResolution (const gchar *name, const gchar *value)
{
    addOptionToList (m_Resolution, name, value);
}

void
PrintView::clearColorModelList ()
{
    // Remove all items from the GListStore
    guint n_items = g_list_model_get_n_items(G_LIST_MODEL(m_ColorModel));
    while (n_items > 0) {
        g_list_store_remove(m_ColorModel, 0);
        n_items--;
    }
}

void
PrintView::clearPageSizeList (void)
{
    // Remove all items from the GListStore
    guint n_items = g_list_model_get_n_items(G_LIST_MODEL(m_PageSize));
    while (n_items > 0) {
        g_list_store_remove(m_PageSize, 0);
        n_items--;
    }
}

void
PrintView::clearResolutionList (void)
{
    // Remove all items from the GListStore
    guint n_items = g_list_model_get_n_items(G_LIST_MODEL(m_Resolution));
    while (n_items > 0) {
        g_list_store_remove(m_Resolution, 0);
        n_items--;
    }
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

ePDFView::PrintPageLayout
PrintView::getPageLayout (void)
{
    return (ePDFView::PrintPageLayout)gtk_drop_down_get_selected(GTK_DROP_DOWN(m_LayoutView));
}

ePDFView::PrintPageOrientation
PrintView::getPageOrientation (void)
{
    return (ePDFView::PrintPageOrientation)gtk_drop_down_get_selected(GTK_DROP_DOWN(m_OrientationView));
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
PrintView::getSelectedPrinterName (void)
{
    gchar *printerName = NULL;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(m_PrinterListView));
    
    if (selected != GTK_INVALID_LIST_POSITION)
    {
        GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(m_PrinterListView));
        PrinterData *data = NULL;
        g_object_get(G_OBJECT(model), "item", &data, NULL);
        
        if (data != NULL && data->name != NULL)
        {
            printerName = g_strdup(data->name);
            g_object_unref(data);
        }
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
    if (m_ColorModelView != NULL && GTK_IS_DROP_DOWN(m_ColorModelView))
    {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(m_ColorModelView), colorModelIndex);
    }
}

void
PrintView::selectPageSize (guint pageSizeIndex)
{
    if (m_PageSizeView != NULL && GTK_IS_DROP_DOWN(m_PageSizeView))
    {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(m_PageSizeView), pageSizeIndex);
    }
}

void
PrintView::selectPrinter (guint printerIndex)
{
    if (m_PrinterListView != NULL && GTK_IS_DROP_DOWN(m_PrinterListView))
    {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(m_PrinterListView), printerIndex);
    }
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
    // In GTK4, we need to get the button from the dialog and set its sensitivity
    GtkWidget *button = gtk_dialog_get_widget_for_response(GTK_DIALOG(m_PrintDialog), GTK_RESPONSE_ACCEPT);
    if (button != NULL) {
        gtk_widget_set_sensitive(button, sensitive);
    }
}

void
PrintView::addOptionToList (GListStore *optionList, const gchar *name,
                         const gchar *value)
{
    // Create a new OptionData structure
    OptionData *data = g_new0(OptionData, 1);
    data->name = g_strdup(name);
    data->value = g_strdup(value);
    
    // Append to the list store
    g_list_store_append(optionList, data);
    
    // The list store takes ownership of the data, no need to free it here
    // The GListStore will handle the cleanup through the registered free function
}

void
PrintView::getOptionFromComboBox (GtkWidget *comboBox, gpointer value)
{
    if (GTK_IS_DROP_DOWN (comboBox))
    {
        GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(comboBox));
        guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(comboBox));
        
        if (selected != GTK_INVALID_LIST_POSITION)
        {
            OptionData *data = NULL;
            g_object_get(G_OBJECT(model), "item", &data, NULL);
            if (data != NULL && data->value != NULL)
            {
                gchar **string = (gchar **)value;
                *string = g_strdup(data->value);
            }
            if (data != NULL) {
                g_object_unref(data);
            }
        }
    }
}

// Tab Creators

// Helper function for printer item binding
static void
printer_item_bind(GtkSignalListItemFactory *factory,
                 GtkListItem              *list_item,
                 gpointer                  user_data)
{
    GtkWidget *box = gtk_list_item_get_child(list_item);
    GtkLabel *name_label = GTK_LABEL(g_object_get_data(G_OBJECT(box), "name"));
    GtkLabel *state_label = GTK_LABEL(g_object_get_data(G_OBJECT(box), "state"));
    GtkLabel *jobs_label = GTK_LABEL(g_object_get_data(G_OBJECT(box), "jobs"));
    GtkLabel *location_label = GTK_LABEL(g_object_get_data(G_OBJECT(box), "location"));
    
    PrinterData *data = (PrinterData *)gtk_list_item_get_item(list_item);
    
    if (data != NULL) {
        gtk_label_set_text(name_label, data->name ? data->name : "");
        gtk_label_set_text(state_label, data->state ? data->state : "");
        gtk_label_set_text(jobs_label, g_strdup_printf("%d", data->jobs));
        gtk_label_set_text(location_label, data->location ? data->location : "");
    }
}

// Helper functions for printer item rendering
static void
printer_item_setup(GtkListItemFactory *factory,
                   GtkListItem        *list_item,
                   gpointer            user_data)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *name_label = gtk_label_new(NULL);
    GtkWidget *state_label = gtk_label_new(NULL);
    GtkWidget *jobs_label = gtk_label_new(NULL);
    GtkWidget *location_label = gtk_label_new(NULL);

    gtk_label_set_xalign(GTK_LABEL(name_label), 0.0);
    gtk_label_set_xalign(GTK_LABEL(state_label), 0.0);
    gtk_label_set_xalign(GTK_LABEL(jobs_label), 1.0);
    gtk_label_set_xalign(GTK_LABEL(location_label), 0.0);

    gtk_widget_set_hexpand(name_label, TRUE);
    gtk_widget_set_hexpand(state_label, TRUE);
    gtk_widget_set_hexpand(jobs_label, FALSE);
    gtk_widget_set_hexpand(location_label, TRUE);

    gtk_box_append(GTK_BOX(box), name_label);
    gtk_box_append(GTK_BOX(box), state_label);
    gtk_box_append(GTK_BOX(box), jobs_label);
    gtk_box_append(GTK_BOX(box), location_label);

    gtk_list_item_set_child(list_item, box);

    g_object_set_data_full(G_OBJECT(box), "name", name_label, NULL);
    g_object_set_data_full(G_OBJECT(box), "state", state_label, NULL);
    g_object_set_data_full(G_OBJECT(box), "jobs", jobs_label, NULL);
    g_object_set_data_full(G_OBJECT(box), "location", location_label, NULL);
}

static void
print_view_printer_selected(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data)
{
    PrintView *view = (PrintView *)user_data;
    guint selected = gtk_drop_down_get_selected(dropdown);
    
    if (selected != GTK_INVALID_LIST_POSITION)
    {
        // Use the public interface to notify the presenter
        view->selectPrinter(selected);
    }
}

GtkWidget *
PrintView::createPrinterTab ()
{
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    
    // Create a scrolled window for the printer list
    GtkWidget *scrolledWindow = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
    
    // Create the list store for printers
    m_PrinterList = g_list_store_new(PRINTER_DATA_TYPE);
    
    // Create a factory to render each item
    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    
    // Set up the factory
    g_signal_connect(factory, "setup",
                    G_CALLBACK(printer_item_setup), NULL);
    g_signal_connect(factory, "bind",
                    G_CALLBACK(printer_item_bind), NULL);
    
    // Create the list view
    m_PrinterListView = gtk_list_view_new(GTK_SELECTION_MODEL(
        gtk_single_selection_new(G_LIST_MODEL(m_PrinterList))), factory);
    
    // Connect the selection changed signal
    GtkSingleSelection *selection = GTK_SINGLE_SELECTION(gtk_list_view_get_model(GTK_LIST_VIEW(m_PrinterListView)));
    g_signal_connect(selection, "notify::selected",
                    G_CALLBACK(print_view_printer_selected), this);
    
    // Add the list view to the scrolled window
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), m_PrinterListView);
    
    // Add the scrolled window to the vbox
    gtk_box_append(GTK_BOX(vbox), scrolledWindow);
    
    return vbox;
}

GtkWidget*
PrintView::createJobTab ()
{
    // Create a vertical box for the job tab
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start (vbox, 12);
    gtk_widget_set_margin_end (vbox, 12);
    gtk_widget_set_margin_top (vbox, 12);
    gtk_widget_set_margin_bottom (vbox, 12);

    // Copies frame
    GtkWidget *copiesFrame = gtk_frame_new (_("Copies"));
    GtkWidget *copiesBox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_start (copiesBox, 6);
    gtk_widget_set_margin_end (copiesBox, 6);
    gtk_widget_set_margin_top (copiesBox, 6);
    gtk_widget_set_margin_bottom (copiesBox, 6);
    gtk_frame_set_child (GTK_FRAME (copiesFrame), copiesBox);

    // Number of copies
    GtkWidget *copiesLabel = gtk_label_new (_("Number of copies:"));
    gtk_box_append (GTK_BOX (copiesBox), copiesLabel);

    m_NumberOfCopies = gtk_spin_button_new_with_range (1, 999, 1);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (m_NumberOfCopies), 1);
    g_signal_connect (G_OBJECT (m_NumberOfCopies), "value-changed",
                     G_CALLBACK (print_view_number_of_copies_changed), this);
    gtk_box_append (GTK_BOX (copiesBox), m_NumberOfCopies);

    // Collate checkbox
    m_Collate = gtk_check_button_new_with_label (_("Collate"));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_Collate), TRUE);
    gtk_box_append (GTK_BOX (copiesBox), m_Collate);

    // Page range frame
    GtkWidget *rangeFrame = gtk_frame_new (_("Page Range"));
    GtkWidget *rangeBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start (rangeBox, 6);
    gtk_widget_set_margin_end (rangeBox, 6);
    gtk_widget_set_margin_top (rangeBox, 6);
    gtk_widget_set_margin_bottom (rangeBox, 6);
    gtk_frame_set_child (GTK_FRAME (rangeFrame), rangeBox);

    // Page range radio buttons - GTK4: use GtkCheckButton groups
    m_AllPagesRangeOption = gtk_check_button_new_with_label (_("All"));
    gtk_check_button_set_active (GTK_CHECK_BUTTON (m_AllPagesRangeOption), TRUE);
    g_signal_connect (G_OBJECT (m_AllPagesRangeOption), "toggled",
                     G_CALLBACK (print_view_page_range_option_changed), this);
    gtk_box_append (GTK_BOX (rangeBox), m_AllPagesRangeOption);

    m_CustomPagesRangeOption = gtk_check_button_new_with_label (_("Pages:"));
    // GTK4: Set as group member (makes it a radio button)
    gtk_check_button_set_group (GTK_CHECK_BUTTON (m_CustomPagesRangeOption),
                                GTK_CHECK_BUTTON (m_AllPagesRangeOption));
    gtk_box_append (GTK_BOX (rangeBox), m_CustomPagesRangeOption);

    // Page range entry
    m_PageRange = gtk_entry_new ();
    gtk_editable_set_width_chars (GTK_EDITABLE (m_PageRange), 20);
    gtk_widget_set_halign (m_PageRange, GTK_ALIGN_END);
    gtk_widget_set_margin_start (m_PageRange, 24);
    gtk_box_append (GTK_BOX (rangeBox), m_PageRange);

    // Page set frame
    GtkWidget *pageSetFrame = gtk_frame_new (_("Pages"));
    GtkWidget *pageSetBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start (pageSetBox, 6);
    gtk_widget_set_margin_end (pageSetBox, 6);
    gtk_widget_set_margin_top (pageSetBox, 6);
    gtk_widget_set_margin_bottom (pageSetBox, 6);
    gtk_frame_set_child (GTK_FRAME (pageSetFrame), pageSetBox);

    // Page set radio buttons
    m_OddPageSet = gtk_check_button_new_with_label (_("Odd pages"));
    gtk_box_append (GTK_BOX (pageSetBox), m_OddPageSet);

    m_EvenPageSet = gtk_check_button_new_with_label (_("Even pages"));
    gtk_box_append (GTK_BOX (pageSetBox), m_EvenPageSet);

    // Pack all frames into the main vbox
    gtk_box_append (GTK_BOX (vbox), copiesFrame);
    gtk_box_append (GTK_BOX (vbox), rangeFrame);
    gtk_box_append (GTK_BOX (vbox), pageSetFrame);

    return vbox;
}

GtkWidget *
PrintView::createPaperTab ()
{
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *frame = gtk_frame_new(_("Paper"));
    GtkWidget *paperGrid = gtk_grid_new();
    GtkWidget *pageSizeLabel = gtk_label_new(_("Page _size:"));
    GtkWidget *orientationLabel = gtk_label_new(_("O_rientation:"));
    GtkWidget *layoutLabel = gtk_label_new(_("Page _layout:"));
    GtkWidget *colorModelLabel = gtk_label_new(_("Color _model:"));
    GtkWidget *resolutionLabel = gtk_label_new(_("_Resolution:"));

    // Set up the paper grid
    gtk_grid_set_row_spacing(GTK_GRID(paperGrid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(paperGrid), 12);
    gtk_grid_attach(GTK_GRID(paperGrid), pageSizeLabel, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(paperGrid), orientationLabel, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(paperGrid), layoutLabel, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(paperGrid), colorModelLabel, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(paperGrid), resolutionLabel, 0, 4, 1, 1);

    // Create the page size dropdown
    m_PageSize = g_list_store_new(OPTION_DATA_TYPE);
    m_PageSizeView = gtk_drop_down_new(G_LIST_MODEL(m_PageSize), NULL);
    gtk_widget_set_hexpand(m_PageSizeView, TRUE);
    gtk_grid_attach(GTK_GRID(paperGrid), m_PageSizeView, 1, 0, 1, 1);
    gtk_label_set_mnemonic_widget(GTK_LABEL(pageSizeLabel), m_PageSizeView);

    // Create the orientation dropdown
    m_Orientation = g_list_store_new(OPTION_DATA_TYPE);
    m_OrientationView = gtk_drop_down_new(G_LIST_MODEL(m_Orientation), NULL);
    gtk_widget_set_hexpand(m_OrientationView, TRUE);
    gtk_grid_attach(GTK_GRID(paperGrid), m_OrientationView, 1, 1, 1, 1);
    gtk_label_set_mnemonic_widget(GTK_LABEL(orientationLabel), m_OrientationView);

    // Create the layout dropdown
    m_Layout = g_list_store_new(OPTION_DATA_TYPE);
    m_LayoutView = gtk_drop_down_new(G_LIST_MODEL(m_Layout), NULL);
    gtk_widget_set_hexpand(m_LayoutView, TRUE);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(m_LayoutView), 0);
    gtk_grid_attach(GTK_GRID(paperGrid), m_LayoutView, 1, 2, 1, 1);
    gtk_label_set_mnemonic_widget(GTK_LABEL(layoutLabel), m_LayoutView);

    // Create the color model dropdown
    m_ColorModel = g_list_store_new(OPTION_DATA_TYPE);
    m_ColorModelView = gtk_drop_down_new(G_LIST_MODEL(m_ColorModel), NULL);
    gtk_widget_set_hexpand(m_ColorModelView, TRUE);
    gtk_grid_attach(GTK_GRID(paperGrid), m_ColorModelView, 1, 3, 1, 1);
    gtk_label_set_mnemonic_widget(GTK_LABEL(colorModelLabel), m_ColorModelView);

    // Create the resolution dropdown
    m_Resolution = g_list_store_new(OPTION_DATA_TYPE);
    m_ResolutionView = gtk_drop_down_new(G_LIST_MODEL(m_Resolution), NULL);
    gtk_widget_set_hexpand(m_ResolutionView, TRUE);
    gtk_grid_attach(GTK_GRID(paperGrid), m_ResolutionView, 1, 4, 1, 1);
    gtk_label_set_mnemonic_widget(GTK_LABEL(resolutionLabel), m_ResolutionView);

    // Add the paper grid to the frame
    gtk_frame_set_child(GTK_FRAME(frame), paperGrid);
    gtk_box_append(GTK_BOX(vbox), frame);

    // Create the output options frame
    GtkWidget *outputFrame = gtk_frame_new(_("Output"));
    GtkWidget *outputGrid = gtk_grid_new();
    GtkWidget *colorModeLabel = gtk_label_new(_("Mode:"));
    
    // Create a GListStore for the color mode combo
    GListStore *colorModeList = g_list_store_new(OPTION_DATA_TYPE);
    GtkWidget *colorModeCombo = gtk_drop_down_new(G_LIST_MODEL(colorModeList), NULL);
    g_object_unref(colorModeList); // The dropdown takes ownership
    
    GtkWidget *collateCheck = gtk_check_button_new_with_label(_("_Collate"));

    // Set up the output grid
    gtk_grid_set_row_spacing(GTK_GRID(outputGrid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(outputGrid), 12);
    gtk_grid_attach(GTK_GRID(outputGrid), colorModeLabel, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(outputGrid), colorModeCombo, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(outputGrid), collateCheck, 1, 1, 1, 1);

    // Add the output grid to the frame
    gtk_frame_set_child(GTK_FRAME(outputFrame), outputGrid);
    gtk_box_append(GTK_BOX(vbox), outputFrame);

    return vbox;
}

////////////////////////////////////////////////////////////////
// Model Creators
////////////////////////////////////////////////////////////////

void
PrintView::createColorModelListModel ()
{
    m_ColorModel = g_list_store_new(OPTION_DATA_TYPE);
}

void
PrintView::createLayoutListModel ()
{
    m_Layout = g_list_store_new(OPTION_DATA_TYPE);
    
    // Add layout options
    OptionData data = {0};
    
    // 1 page per sheet
    data.name = g_strdup(_("1 page per sheet"));
    data.value = g_strdup_printf("%d", PRINT_PAGE_LAYOUT_1IN1);
    g_list_store_append(m_Layout, &data);
    g_free(data.name);
    g_free(data.value);
    
    // 2 pages in 1
    data.name = g_strdup(_("2 pages in 1"));
    data.value = g_strdup_printf("%d", PRINT_PAGE_LAYOUT_2IN1);
    g_list_store_append(m_Layout, &data);
    g_free(data.name);
    g_free(data.value);
    
    // 4 pages in 1
    data.name = g_strdup(_("4 pages in 1"));
    data.value = g_strdup_printf("%d", PRINT_PAGE_LAYOUT_4IN1);
    g_list_store_append(m_Layout, &data);
    g_free(data.name);
    g_free(data.value);
    
    // 6 pages in 1
    data.name = g_strdup(_("6 pages in 1"));
    data.value = g_strdup_printf("%d", PRINT_PAGE_LAYOUT_6IN1);
    g_list_store_append(m_Layout, &data);
    g_free(data.name);
    g_free(data.value);
}

void
PrintView::createOrientationListModel ()
{
    m_Orientation = g_list_store_new(OPTION_DATA_TYPE);
    
    // Add orientation options
    OptionData data = {0};
    
    // Portrait
    data.name = g_strdup(_("Portrait"));
    data.value = g_strdup_printf("%d", PRINT_PAGE_ORIENTATION_PORTRAIT);
    g_list_store_append(m_Orientation, &data);
    g_free(data.name);
    g_free(data.value);
    
    // Landscape
    data.name = g_strdup(_("Landscape"));
    data.value = g_strdup_printf("%d", PRINT_PAGE_ORIENTATION_LANDSCAPE);
    g_list_store_append(m_Orientation, &data);
    g_free(data.name);
    g_free(data.value);
}

void
PrintView::createPageSizeListModel ()
{
    m_PageSize = g_list_store_new(OPTION_DATA_TYPE);
}

void
PrintView::createPrinterListModel ()
{
    m_PrinterList = g_list_store_new(PRINTER_DATA_TYPE);
}

void
PrintView::createResolutionListModel ()
{
    m_Resolution = g_list_store_new(OPTION_DATA_TYPE);
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
print_view_page_range_option_changed (GtkCheckButton *, gpointer data)
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
