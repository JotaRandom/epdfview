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
#include <stdlib.h>
#include <string.h>
#include <gettext.h>
#include <gtk/gtk.h>
#include <epdfview.h>
#include "StockIcons.h"
#include "FindView.h"
#include "PageView.h"
#include "PreferencesView.h"
#if defined (HAVE_CUPS)
#include "PrintView.h"
#endif // HAVE_CUPS
#include "MainView.h"

using namespace ePDFView;

// GTK4 direct API usage - no compatibility layer needed

// Constants.
// GTK4: Position constants removed (not using GtkToolbar positioning)
static gint CURRENT_PAGE_WIDTH = 5;
static gint CURRENT_ZOOM_WIDTH = 6;

// Enumerations.
enum indexColumns
{
    outlineTitleColumn,
    outlinePageColumn,
    outlineDataColumn,
    outlineNumColumns
};

// Forward declarations.
static void main_window_about_box_cb (GtkWidget *, gpointer);
// GTK4: main_window_about_box_url_hook removed (not needed)
static void main_window_find_cb (GtkWidget *, gpointer);
static void main_window_fullscreen_cb (GSimpleAction *, GVariant *, gpointer);
// GTK4: Window configure events removed
// static gboolean main_window_moved_or_resized_cb (GtkWidget *, gpointer);
static void main_window_go_to_first_page_cb (GtkWidget *, gpointer);
static void main_window_go_to_last_page_cb (GtkWidget *, gpointer);
static void main_window_go_to_next_page_cb (GtkWidget *, gpointer);
static void main_window_go_to_page_cb (GtkWidget *, gpointer);
static void main_window_go_to_previous_page_cb (GtkWidget *, gpointer);
// GTK4: Macro to create GAction callback wrapper for widget callbacks
// GAction signature: void callback(GSimpleAction*, GVariant*, gpointer)
// Widget signature: void callback(GtkWidget*, gpointer)
#define ACTION_CALLBACK(name, widget_callback) \
static void name(GSimpleAction *action, GVariant *parameter, gpointer data) { \
    widget_callback(NULL, data); \
}

// Widget callbacks (for toolbar buttons)
static void main_window_reload_cb (GtkWidget *, gpointer);
static void main_window_rotate_left_cb (GtkWidget *, gpointer);
static void main_window_rotate_right_cb (GtkWidget *, gpointer);
static void main_window_open_file_cb (GtkWidget *, gpointer);
static void main_window_outline_cb (GtkTreeSelection *, gpointer);
static void main_window_preferences_cb (GtkWidget *, gpointer);
static void main_window_quit_cb (GtkWidget *, gpointer);
static void main_window_save_file_cb (GtkWidget *, gpointer);
static void main_window_show_index_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_show_menubar_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_invert_color_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_show_statusbar_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_show_toolbar_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_zoom_fit_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_zoom_in_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_zoom_out_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_zoom_width_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_fit_width_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_fit_height_cb (GSimpleAction *, GVariant *, gpointer);
static void main_window_set_page_mode (GSimpleAction *, GVariant *, gpointer);
static void on_zoom_entry_activate (GtkEntry *, gpointer);
// GTK4: Scroll events handled by PageView event controllers, not needed here

#if defined (HAVE_CUPS)
static void main_window_print_cb (GtkWidget *, gpointer);
#endif // HAVE_CUPS

// GTK4 action entries for GSimpleAction
// GTK4: Create GAction wrappers for all widget callbacks
ACTION_CALLBACK(main_window_open_file_action_cb, main_window_open_file_cb)
ACTION_CALLBACK(main_window_reload_action_cb, main_window_reload_cb)
ACTION_CALLBACK(main_window_save_file_action_cb, main_window_save_file_cb)
#if defined (HAVE_CUPS)
ACTION_CALLBACK(main_window_print_action_cb, main_window_print_cb)
#endif
ACTION_CALLBACK(main_window_quit_action_cb, main_window_quit_cb)
ACTION_CALLBACK(main_window_find_action_cb, main_window_find_cb)
ACTION_CALLBACK(main_window_preferences_action_cb, main_window_preferences_cb)
ACTION_CALLBACK(main_window_rotate_right_action_cb, main_window_rotate_right_cb)
ACTION_CALLBACK(main_window_rotate_left_action_cb, main_window_rotate_left_cb)
ACTION_CALLBACK(main_window_go_to_first_page_action_cb, main_window_go_to_first_page_cb)
ACTION_CALLBACK(main_window_go_to_last_page_action_cb, main_window_go_to_last_page_cb)
ACTION_CALLBACK(main_window_go_to_next_page_action_cb, main_window_go_to_next_page_cb)
ACTION_CALLBACK(main_window_go_to_previous_page_action_cb, main_window_go_to_previous_page_cb)
ACTION_CALLBACK(main_window_about_box_action_cb, main_window_about_box_cb)

static const struct {
    const gchar *name;
    const gchar *icon_name;
    const gchar *label;
    const gchar *accelerator;
    const gchar *tooltip;
    GCallback callback;
} g_NormalEntries[] = {
    { "open-file", "document-open", N_("_Open"), "<Control>o",
      N_("Open a PDF document"),
      G_CALLBACK (main_window_open_file_action_cb) },

    { "reload-file", "view-refresh", N_("_Reload"), "<control>R",
      N_("Reload the current document"),
      G_CALLBACK (main_window_reload_action_cb) },

    // Removed "Save a Copy" - this is a PDF reader, not an editor
    // { "save-file", "document-save", N_("_Save a Copy..."), "<control>S",
    //   N_("Save a copy of the current document"),
    //   G_CALLBACK (main_window_save_file_action_cb) },

#if defined (HAVE_CUPS)
    { "print", "document-print", N_("_Print..."), "<control>P",
      N_("Print the current document"),
      G_CALLBACK (main_window_print_action_cb) },
#endif // HAVE_CUPS

    { "quit", "window-close", N_("_Close"), "<control>W",
      N_("Close this window"),
      G_CALLBACK (main_window_quit_action_cb) },

    { "find", "edit-find", N_("_Find"), "<control>F",
      N_("Find a word in the document"),
      G_CALLBACK (main_window_find_action_cb) },

    { "preferences", "preferences-system", N_("_Preferences..."), NULL,
      N_("Change the application's preferences"),
      G_CALLBACK (main_window_preferences_action_cb) },

    { "zoom-in", "zoom-in", N_("Zoom _In"), "<control>plus",
      N_("Enlarge the document"),
      G_CALLBACK (main_window_zoom_in_cb) },

    { "zoom-out", "zoom-out", N_("Zoom _Out"), "<control>minus",
      N_("Shrink the document"),
      G_CALLBACK (main_window_zoom_out_cb) },

    { "fit-width", NULL, N_("Fit Width"), NULL,
      N_("Fit document to window width"),
      G_CALLBACK (main_window_fit_width_cb) },

    { "fit-height", NULL, N_("Fit Height"), NULL,
      N_("Fit document to window height"),
      G_CALLBACK (main_window_fit_height_cb) },

    { "zoom-in-kp", NULL, NULL, "<control>KP_Add", NULL,
      G_CALLBACK (main_window_zoom_in_cb) },
      
    { "zoom-out-kp", NULL, NULL, "<control>KP_Subtract", NULL,
      G_CALLBACK (main_window_zoom_out_cb) },

    { "rotate-right", "object-rotate-right", N_("Rotate _Right"), "<control>bracketright",
      N_("Rotate the document 90 degrees clockwise"),
      G_CALLBACK (main_window_rotate_right_action_cb) },

    { "rotate-left", "object-rotate-left", N_("Rotate _Left"), "<control>bracketleft",
      N_("Rotate the document 90 degrees counter-clockwise"),
      G_CALLBACK (main_window_rotate_left_action_cb) },

    { "go-first", "go-first", N_("_First Page"), "<control>Home",
      N_("Go to the first page"),
      G_CALLBACK (main_window_go_to_first_page_action_cb) },

    { "go-last", "go-last", N_("_Last Page"), "<control>End",
      N_("Go to the last page"),
      G_CALLBACK (main_window_go_to_last_page_action_cb) },

    { "go-next", "go-next", N_("_Next Page"), "Page_Down",
      N_("Go to the next page"),
      G_CALLBACK (main_window_go_to_next_page_action_cb) },

    { "go-previous", "go-previous", N_("_Previous Page"), "Page_Up",
      N_("Go to the previous page"),
      G_CALLBACK (main_window_go_to_previous_page_action_cb) },

    { "about", "help-about", N_("_About"), NULL,
      N_("Display application's credits"),
      G_CALLBACK (main_window_about_box_action_cb) },

    // Accelerator keys.
    { "slash", "edit-find", NULL, "slash", NULL,
      G_CALLBACK (main_window_find_cb) },

    { "kp-add", "zoom-in", NULL, "<control>KP_Add", NULL,
      G_CALLBACK (main_window_zoom_in_cb) },

    { "kp-subtract", "zoom-out", NULL, "<control>KP_Subtract",
      NULL,
      G_CALLBACK (main_window_zoom_out_cb) }
};

static const struct {
    const gchar *name;
    const gchar *icon_name;
    const gchar *label;
    const gchar *accelerator;
    const gchar *tooltip;
    GCallback callback;
    gboolean default_state;
} g_ToggleEntries[] = {
    { "fullscreen", "view-fullscreen", N_("F_ull screen"), "F11",
      N_("Toggle full screen window"),
      G_CALLBACK (main_window_fullscreen_cb), FALSE },
    
    { "show-menubar", NULL, N_("Hide _Menubar"), "F7",
      N_("Toggle menu bar"),
      G_CALLBACK (main_window_show_menubar_cb), FALSE },

    { "invert-colors", NULL, N_("_Invert Colors"), "F8",
      N_("Toggle color inversion"),
      G_CALLBACK (main_window_invert_color_cb), FALSE },

    { "show-toolbar", NULL, N_("Show _Toolbar"), "F6",
      N_("Show or hide the toolbar"),
      G_CALLBACK (main_window_show_toolbar_cb), TRUE },

    { "show-statusbar", NULL, N_("Show _Statusbar"), NULL,
      N_("Show or hide the statusbar"),
      G_CALLBACK (main_window_show_statusbar_cb), TRUE },

    { "show-index", NULL, N_("Show I_ndex"), "F9",
      N_("Show or hide the document's outline"),
      G_CALLBACK (main_window_show_index_cb), FALSE },

    { "zoom-fit", "zoom-fit-best", N_("Zoom to _Fit"), NULL,
      N_("Make the current document fill the window"),
      G_CALLBACK (main_window_zoom_fit_cb), FALSE },

    { "zoom-width", "zoom-fit-width", N_("Zoom to _Width"), NULL,
      N_("Make the current document fill the window width"),
      G_CALLBACK (main_window_zoom_width_cb), FALSE },
};

static const struct {
    const gchar *name;
    const gchar *icon_name;
    const gchar *label;
    const gchar *tooltip;
    gint value;
} g_PageScrollEntries[] = {   
    { "page-mode-scroll", "view-fullscreen", N_("Scroll"),
      N_("Mouse scroll page"), (int)PagePterModeScroll },

    { "page-mode-text", "edit-select-all", N_("Select Text"),
      N_(" Mouse select text"), (int)PagePterModeSelectText },
};


////////////////////////////////////////////////////////////////
// Interface Methods.
////////////////////////////////////////////////////////////////

MainView::MainView (MainPter *pter):
    IMainView (pter)
{
    // Initialise the stock items.
    epdfview_stock_icons_init ();
    // Create the main window
    m_MainWindow = gtk_window_new ();
    setMainWindowIcon ();
    
    // Set window properties
    gtk_window_set_title (GTK_WINDOW (m_MainWindow), "ePDFView");
    gtk_window_set_default_size (GTK_WINDOW (m_MainWindow), 800, 600);
    gtk_window_set_resizable (GTK_WINDOW (m_MainWindow), TRUE);
    gtk_window_set_decorated (GTK_WINDOW (m_MainWindow), TRUE);
    
    // Enable mnemonics (Alt+key shortcuts for menu navigation)
    gtk_widget_set_can_focus (m_MainWindow, TRUE);
    
    // The application will be set by gtk_application_add_window in main.cxx
    // No need to set it here as it's already handled by the application
    
    // Connect signals
    g_signal_connect (G_OBJECT (m_MainWindow), "destroy",
                     G_CALLBACK (main_window_quit_cb), NULL);
    // Create the main vertical box.
    m_MainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child (GTK_WINDOW (m_MainWindow), m_MainBox);
    gtk_widget_set_visible (m_MainBox, TRUE);
    
    // Create actions and modern headerbar
    createActions ();
    createHeaderBar ();
    
    // Create page view
    GtkWidget *pageViewPaned = createPageView ();
    gtk_box_append (GTK_BOX (m_MainBox), pageViewPaned);
    gtk_widget_set_vexpand (pageViewPaned, TRUE);
    
    // Store the page view widget for later focus management
    m_PageViewWidget = m_PageView->getTopWidget();
    
    // Set initial focus to page view for keyboard navigation
    gtk_widget_grab_focus (m_PageViewWidget);
    
    // Connect to the map signal to handle focus when the window is shown
    g_signal_connect_swapped(m_MainWindow, "map", 
                           G_CALLBACK(+[](GtkWidget* widget, gpointer user_data) {
                               gtk_widget_grab_focus(GTK_WIDGET(user_data));
                           }), 
                           m_PageViewWidget);
    
    // Add the find bar.
    m_FindView = new FindView ();
    gtk_box_append (GTK_BOX (m_MainBox), m_FindView->getTopWidget ());

    // GTK4: Use a simple label instead of deprecated GtkStatusbar
    m_StatusBar = gtk_label_new ("");
    gtk_widget_set_halign (m_StatusBar, GTK_ALIGN_START);
    gtk_widget_set_margin_start (m_StatusBar, 6);
    gtk_widget_set_margin_end (m_StatusBar, 6);
    gtk_widget_set_margin_top (m_StatusBar, 3);
    gtk_widget_set_margin_bottom (m_StatusBar, 3);
    gtk_box_append (GTK_BOX (m_MainBox), m_StatusBar);

    // GTK4: Scroll events handled by PageView event controllers
}

MainView::~MainView ()
{
    delete m_FindView;
    delete m_PageView;
    g_object_unref (G_OBJECT (m_ActionGroup));
}

void
MainView::activeZoomFit (gboolean active)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "zoom-fit");
    if (action) {
        g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (active));
    }
}

void
MainView::activeZoomWidth (gboolean active)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "zoom-width");
    if (action) {
        g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (active));
    }
}

void
MainView::activePageModeScroll (gboolean active)
{
    if (active) {
        GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "page-mode");
        if (action) {
            g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_string ("scroll"));
        }
    }
}

void
MainView::activePageModeText (gboolean active)
{
    if (active) {
        GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "page-mode");
        if (action) {
            g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_string ("text"));
        }
    }
}

gboolean
MainView::isIndexVisible () const
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "show-index");
    if (action) {
        GVariant *state = g_action_get_state (action);
        if (state) {
            gboolean result = g_variant_get_boolean (state);
            g_variant_unref (state);
            return result;
        }
    }
    return FALSE;
}

///
/// @brief Async version of openFileDialog for GTK4.
///
void
MainView::openFileDialogAsync (const gchar *lastFolder,
                               FileChooserCallback callback,
                               gpointer userData)
{
    // GTK4: Use GtkFileChooserNative for proper native modal dialogs
    GtkFileChooserNative *native = gtk_file_chooser_native_new (
            _("Open PDF File"),
            GTK_WINDOW (m_MainWindow),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            _("_Open"),
            _("_Cancel"));
    
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
    
    // Set up PDF filter
    GtkFileFilter *pdfFilter = gtk_file_filter_new ();
    gtk_file_filter_set_name (pdfFilter, _("Portable Document Format (PDF) Files"));
    gtk_file_filter_add_mime_type (pdfFilter, "application/pdf");
    gtk_file_filter_add_pattern (pdfFilter, "*.pdf");
    gtk_file_filter_add_pattern (pdfFilter, "*.PDF");
    gtk_file_chooser_add_filter (chooser, pdfFilter);
    
    // Add "All Files" filter
    GtkFileFilter *anyFilter = gtk_file_filter_new ();
    gtk_file_filter_set_name (anyFilter, _("All Files"));
    gtk_file_filter_add_pattern (anyFilter, "*");
    gtk_file_chooser_add_filter (chooser, anyFilter);
    
    // Set PDF filter as default
    gtk_file_chooser_set_filter (chooser, pdfFilter);
    
    // Set initial folder if provided
    if (lastFolder) {
        GFile *folder = g_file_new_for_path (lastFolder);
        gtk_file_chooser_set_current_folder (chooser, folder, NULL);
        g_object_unref (folder);
    }
    
    // Store callback data
    struct CallbackData {
        FileChooserCallback callback;
        gpointer userData;
        GtkFileChooser *chooser;
    };
    
    CallbackData *data = g_new0 (CallbackData, 1);
    data->callback = callback;
    data->userData = userData;
    data->chooser = chooser;
    
    // GTK4: Pure async approach - no blocking at all
    auto response_cb = +[](GtkNativeDialog *dialog, gint response, gpointer user_data) {
        CallbackData *data = (CallbackData*)user_data;
        gchar *result = NULL;
        
        if (response == GTK_RESPONSE_ACCEPT) {
            GFile *file = gtk_file_chooser_get_file (data->chooser);
            if (file) {
                result = g_file_get_path (file);
                g_object_unref (file);
            }
        }
        
        // Call the user's callback
        data->callback (result, data->userData);
        
        // Clean up
        g_object_unref (dialog);
        g_free (data);
    };
    
    g_signal_connect (native, "response", G_CALLBACK (response_cb), data);
    gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
}

gchar *
MainView::openFileDialog (const gchar *lastFolder)
{
    gchar *result = NULL;
    
    // GTK4: Use GtkFileChooserNative for proper native modal dialogs
    GtkFileChooserNative *native = gtk_file_chooser_native_new (
            _("Open PDF File"),
            GTK_WINDOW (m_MainWindow),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            _("_Open"),
            _("_Cancel"));
    
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
    
    // Set up PDF filter
    GtkFileFilter *pdfFilter = gtk_file_filter_new ();
    gtk_file_filter_set_name (pdfFilter, _("Portable Document Format (PDF) Files"));
    gtk_file_filter_add_mime_type (pdfFilter, "application/pdf");
    gtk_file_filter_add_pattern (pdfFilter, "*.pdf");
    gtk_file_filter_add_pattern (pdfFilter, "*.PDF");
    gtk_file_chooser_add_filter (chooser, pdfFilter);
    
    // Add "All Files" filter
    GtkFileFilter *anyFilter = gtk_file_filter_new ();
    gtk_file_filter_set_name (anyFilter, _("All Files"));
    gtk_file_filter_add_pattern (anyFilter, "*");
    gtk_file_chooser_add_filter (chooser, anyFilter);
    
    // Set PDF filter as default
    gtk_file_chooser_set_filter (chooser, pdfFilter);
    
    // Set initial folder if provided
    if (lastFolder) {
        GFile *folder = g_file_new_for_path (lastFolder);
        gtk_file_chooser_set_current_folder (chooser, folder, NULL);
        g_object_unref (folder);
    }
    
    // GTK4: Use a local GMainLoop for modal behavior
    // This is better than g_main_context_iteration as it doesn't block rendering
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    gint response = GTK_RESPONSE_CANCEL;
    
    auto response_cb = +[](GtkNativeDialog *dialog, gint resp, gpointer user_data) {
        gpointer *data = (gpointer*)user_data;
        gint *response_ptr = (gint*)data[0];
        GMainLoop *loop = (GMainLoop*)data[1];
        *response_ptr = resp;
        g_main_loop_quit (loop);
    };
    
    gpointer cb_data[] = {&response, loop};
    g_signal_connect (native, "response", G_CALLBACK (response_cb), cb_data);
    
    gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
    
    // GTK4: Instead of g_main_loop_run which blocks events, manually iterate
    // This allows GTK to process UI updates (menus, rendering, etc.)
    while (g_main_loop_is_running(loop)) {
        g_main_context_iteration(NULL, TRUE);
    }
    g_main_loop_unref (loop);
    
    // Get the selected file if accepted
    if (response == GTK_RESPONSE_ACCEPT) {
        GFile *file = gtk_file_chooser_get_file (chooser);
        if (file) {
            result = g_file_get_path (file);
            g_object_unref (file);
        }
    }
    
    // Clean up
    g_object_unref (native);
    
    return result;
}

gchar *
MainView::promptPasswordDialog ()
{
    /* Create the password dialog. */
    GtkWidget *primaryLabel = gtk_label_new ("");
    gtk_label_set_markup (GTK_LABEL (primaryLabel),
            "<span weight=\"bold\" size=\"larger\">Encrypted Document</span>");
    gtk_label_set_selectable (GTK_LABEL (primaryLabel), TRUE);
    gtk_widget_set_halign (primaryLabel, GTK_ALIGN_START);
    gtk_widget_set_valign (primaryLabel, GTK_ALIGN_START);
    GtkWidget *secondaryLabel = gtk_label_new ("This document is encrypted "
                                               "and can't be accessed. "
                                               "Please enter the document's "
                                               "password:");
    gtk_label_set_wrap (GTK_LABEL (secondaryLabel), TRUE);
    gtk_label_set_selectable (GTK_LABEL (secondaryLabel), TRUE);
    gtk_widget_set_halign (secondaryLabel, GTK_ALIGN_START);
    gtk_widget_set_valign (secondaryLabel, GTK_ALIGN_START);
    GtkWidget *passwordEntry = gtk_entry_new ();
    gtk_entry_set_visibility (GTK_ENTRY (passwordEntry), FALSE);
    gtk_entry_set_activates_default (GTK_ENTRY (passwordEntry), TRUE);
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_box_append (GTK_BOX (vbox), primaryLabel);
    gtk_box_append (GTK_BOX (vbox), secondaryLabel);
    gtk_widget_set_vexpand (secondaryLabel, TRUE);
    gtk_box_append (GTK_BOX (vbox), passwordEntry);
    gtk_widget_set_margin_top (passwordEntry, 6);

    GtkWidget *image = gtk_image_new_from_icon_name (GTK_STOCK_DIALOG_AUTHENTICATION);
    gtk_image_set_icon_size (GTK_IMAGE (image), GTK_ICON_SIZE_LARGE);
    gtk_widget_set_halign (image, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (image, GTK_ALIGN_START);
    GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append (GTK_BOX (hbox), image);
    gtk_box_append (GTK_BOX (hbox), vbox);
    gtk_widget_set_hexpand (vbox, TRUE);
    // In GTK4, widgets are visible by default


    // GTK4: Use text labels instead of stock IDs for buttons
    GtkWidget *passwordDialog = gtk_dialog_new_with_buttons (
            _("Password"), GTK_WINDOW (m_MainWindow),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            _("_Cancel"), GTK_RESPONSE_CANCEL,
            _("_OK"), GTK_RESPONSE_ACCEPT,
            NULL);
    gtk_dialog_set_default_response (GTK_DIALOG (passwordDialog), 
                                     GTK_RESPONSE_ACCEPT);
    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (passwordDialog));
    gtk_box_append (GTK_BOX (content_area), hbox);

    // GTK4: Use modal dialog with response callback
    gtk_window_set_modal (GTK_WINDOW (passwordDialog), TRUE);
    gtk_window_present (GTK_WINDOW (passwordDialog));
    
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    gint response_id = GTK_RESPONSE_CANCEL;
    
    // Store response ID in a callback
    auto response_cb = +[](GtkDialog *dialog, gint response, gpointer user_data) {
        gint *response_ptr = (gint *)user_data;
        *response_ptr = response;
    };
    
    g_signal_connect (passwordDialog, "response", G_CALLBACK (response_cb), &response_id);
    g_signal_connect_swapped (passwordDialog, "response", G_CALLBACK (g_main_loop_quit), loop);
    
    // GTK4: Instead of g_main_loop_run which blocks events, manually iterate
    while (g_main_loop_is_running(loop)) {
        g_main_context_iteration(NULL, TRUE);
    }
    
    gchar *password = NULL;
    if (response_id == GTK_RESPONSE_ACCEPT)
    {
        // GTK4: Use gtk_editable_get_text
        password = g_strdup(gtk_editable_get_text (GTK_EDITABLE (passwordEntry)));
    }
    
    g_main_loop_unref (loop);
    gtk_window_destroy (GTK_WINDOW (passwordDialog));
    return password;
}

gchar *
MainView::saveFileDialog (const gchar *lastFolder, const gchar *fileName)
{
    gchar *result = NULL;
    
    // GTK4: Use GtkFileChooserNative for proper native modal dialogs
    GtkFileChooserNative *native = gtk_file_chooser_native_new (
            _("Save PDF File"),
            GTK_WINDOW (m_MainWindow),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            _("_Save"),
            _("_Cancel"));
    
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
    
    // Select the last used folder as the initial folder, if any.
    if ( NULL != lastFolder )
    {
        GFile *folder = g_file_new_for_path (lastFolder);
        gtk_file_chooser_set_current_folder (chooser, folder, NULL);
        g_object_unref (folder);
    }
    
    // Set the original file name to use as initial save name, if any.
    if ( NULL != fileName )
    {
        gtk_file_chooser_set_current_name (chooser, fileName);
    }

    // Add the file type filters.
    GtkFileFilter *pdfFilter = gtk_file_filter_new ();
    gtk_file_filter_set_name (pdfFilter, 
                              _("Portable Document Format (PDF) Files"));
    gtk_file_filter_add_mime_type (pdfFilter, "application/pdf");
    gtk_file_filter_add_pattern (pdfFilter, "*.pdf");
    gtk_file_filter_add_pattern (pdfFilter, "*.PDF");
    gtk_file_chooser_add_filter (chooser, pdfFilter);
    
    GtkFileFilter *anyFilter = gtk_file_filter_new ();
    gtk_file_filter_set_name (anyFilter, _("All Files"));
    gtk_file_filter_add_pattern (anyFilter, "*");
    gtk_file_chooser_add_filter (chooser, anyFilter);
    
    // Set PDF filter as the default.
    gtk_file_chooser_set_filter (chooser, pdfFilter);

    // GTK4: Use a local GMainLoop for modal behavior
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    gint response = GTK_RESPONSE_CANCEL;
    
    auto response_cb = +[](GtkNativeDialog *dialog, gint resp, gpointer user_data) {
        gpointer *data = (gpointer*)user_data;
        gint *response_ptr = (gint*)data[0];
        GMainLoop *loop = (GMainLoop*)data[1];
        *response_ptr = resp;
        g_main_loop_quit (loop);
    };
    
    gpointer cb_data[] = {&response, loop};
    g_signal_connect (native, "response", G_CALLBACK (response_cb), cb_data);
    
    gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
    
    // GTK4: Instead of g_main_loop_run which blocks events, manually iterate
    while (g_main_loop_is_running(loop)) {
        g_main_context_iteration(NULL, TRUE);
    }
    g_main_loop_unref (loop);
    
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GFile *file = gtk_file_chooser_get_file (chooser);
        if (file) {
            result = g_file_get_path (file);
            g_object_unref (file);
        }
    }
    
    g_object_unref (native);
    return result;
}

void
MainView::sensitiveFind (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "find");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveGoToFirstPage (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "go-first");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveGoToLastPage (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "go-last");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveGoToNextPage (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "go-next");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveGoToPage (gboolean sensitive)
{
    g_message("MainView::sensitiveGoToPage: sensitive=%d", sensitive);
    // Keep the page entry always editable - only disable the label
    // The entry itself handles validation when Enter is pressed
    gtk_widget_set_sensitive (GTK_WIDGET (m_NumberOfPages), sensitive);
    // Optionally gray out the entry text when disabled, but keep it editable
    if (!sensitive) {
        gtk_widget_add_css_class (m_CurrentPage, "dim-label");
    } else {
        gtk_widget_remove_css_class (m_CurrentPage, "dim-label");
    }
}

void 
MainView::sensitiveGoToPreviousPage (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "go-previous");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveOpen (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "open-file");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

#if defined (HAVE_CUPS)
void
MainView::sensitivePrint (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "print");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}
#endif // HAVE_CUPS

void
MainView::sensitiveReload (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "reload-file");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveRotateLeft (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "rotate-left");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveRotateRight (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "rotate-right");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveSave (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "save-file");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveZoom (gboolean sensitive)
{
    // Keep the zoom entry always editable
    // Optionally gray out the entry text when disabled
    if (!sensitive) {
        gtk_widget_add_css_class (m_CurrentZoom, "dim-label");
    } else {
        gtk_widget_remove_css_class (m_CurrentZoom, "dim-label");
    }
}

void
MainView::sensitiveZoomIn (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "zoom-in");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveZoomOut (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "zoom-out");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveZoomFit (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "zoom-fit");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveZoomWidth (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "zoom-width");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::sensitiveFullScreen (gboolean sensitive)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "fullscreen");
    if (action) {
        g_simple_action_set_enabled (G_SIMPLE_ACTION (action), sensitive);
    }
}

void
MainView::show (void)
{
    Config &config = Config::getConfig ();
    gint width = config.getWindowWidth ();
    gint height = config.getWindowHeight ();
    
    // GTK4/WSLg: Sanity check window size (WSLg can report bogus dimensions)
    if (width <= 0 || width > 10000) width = 800;
    if (height <= 0 || height > 10000) height = 600;
    
    gtk_window_set_default_size (GTK_WINDOW (m_MainWindow), width, height);
    
    // CRITICAL: Present the window FIRST, then sync visibility
    // This ensures the window has a valid allocation before we modify child widgets
    gtk_window_present (GTK_WINDOW (m_MainWindow));
    
    // Now that the window is presented and has allocation, sync widget visibility
    // with action states. This must happen AFTER present to avoid the
    // "snapshot without allocation" warning
    GAction *toolbar_action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "show-toolbar");
    if (toolbar_action) {
        GVariant *state = g_action_get_state (toolbar_action);
        showToolbar (g_variant_get_boolean (state));
        g_variant_unref (state);
    }
    
    GAction *statusbar_action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "show-statusbar");
    if (statusbar_action) {
        GVariant *state = g_action_get_state (statusbar_action);
        showStatusbar (g_variant_get_boolean (state));
        g_variant_unref (state);
    }
}

void
MainView::showErrorMessage (const gchar *title, const gchar *body)
{
    GtkWidget *errorDialog = gtk_message_dialog_new (
            GTK_WINDOW (m_MainWindow),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "%s",
            title);
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG(errorDialog), "%s",
                                              body);
    // GTK4: Use async modal dialog - NO nested loops!
    gtk_window_set_modal (GTK_WINDOW (errorDialog), TRUE);
    
    // Connect response to auto-destroy
    g_signal_connect (errorDialog, "response",
                     G_CALLBACK(+[](GtkDialog *dialog, int response, gpointer) {
                         gtk_window_destroy(GTK_WINDOW(dialog));
                     }), NULL);
    
    gtk_window_present (GTK_WINDOW (errorDialog));
    // Do NOT wait for response - let it be async
}

void
MainView::showIndex (gboolean show)
{
    if (m_Sidebar && GTK_IS_WIDGET(m_Sidebar))
    {
        if ( show )
        {
            gtk_widget_set_visible (m_Sidebar, TRUE);
        }
        else
        {
            gtk_widget_set_visible (m_Sidebar, FALSE);
        }
    }
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "show-index");
    if (action) {
        g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (show));
    }
}

void
MainView::setCursor (ViewCursor cursorType)
{
    const char *cursor_name = NULL;
    switch (cursorType)
    {
        case MAIN_VIEW_CURSOR_WAIT:
            cursor_name = "wait";
            break;
        case MAIN_VIEW_CURSOR_DRAG:
            cursor_name = "move";
            break;
        default:
            cursor_name = "default";
    }

    // In GTK4, use gtk_widget_set_cursor_from_name
    if (cursor_name)
    {
        gtk_widget_set_cursor_from_name (m_MainWindow, cursor_name);
    }
}

void
MainView::setFullScreen (gboolean fullScreen)
{
    if ( fullScreen )
    {
        gtk_window_fullscreen (GTK_WINDOW (m_MainWindow));
        // Hide the header bar, status bar and the index bar. Then zoom to fit.
        if (m_HeaderBar && GTK_IS_WIDGET(m_HeaderBar))
            gtk_widget_set_visible (m_HeaderBar, FALSE);
        if (m_StatusBar && GTK_IS_WIDGET(m_StatusBar))
            gtk_widget_set_visible (m_StatusBar, FALSE);
        if (m_Sidebar && GTK_IS_WIDGET(m_Sidebar))
            gtk_widget_set_visible (m_Sidebar, FALSE);
        activeZoomFit (TRUE);
    }
    else
    {
        gtk_window_unfullscreen (GTK_WINDOW (m_MainWindow));
        if (m_HeaderBar && GTK_IS_WIDGET(m_HeaderBar))
            gtk_widget_set_visible (m_HeaderBar, TRUE);
        // Show again the status bar and index, only if it was enabled.
        main_window_show_index_cb (NULL, NULL, (gpointer)m_Pter);
        main_window_show_statusbar_cb (NULL, NULL, (gpointer)m_Pter);
    }
}

void
MainView::setNumberOfPagesText (const gchar *text)
{
    if (m_NumberOfPages && GTK_IS_LABEL(m_NumberOfPages)) {
        gtk_label_set_text (GTK_LABEL (m_NumberOfPages), text);
    }
}

void
MainView::setGoToPageText (const gchar *text)
{
    g_message("MainView::setGoToPageText: text='%s'", text);
    
    // If the entry has focus, the user is editing - don't interfere!
    if (gtk_widget_has_focus(m_CurrentPage)) {
        g_message("MainView::setGoToPageText: Entry has focus, user is editing - skipping update");
        return;
    }
    
    // Block signals to prevent feedback loops
    g_signal_handlers_block_matched(m_CurrentPage, G_SIGNAL_MATCH_DATA, 
                                     0, 0, NULL, NULL, m_Pter);
    
    // GTK4 workaround: Clear first, then set to force visual update
    gtk_editable_delete_text(GTK_EDITABLE(m_CurrentPage), 0, -1);
    gtk_editable_set_text (GTK_EDITABLE (m_CurrentPage), text);
    
    // Unblock signals
    g_signal_handlers_unblock_matched(m_CurrentPage, G_SIGNAL_MATCH_DATA,
                                       0, 0, NULL, NULL, m_Pter);
    
    g_message("MainView::setGoToPageText: Text updated");
}

const gchar *
MainView::getGoToPageText (void)
{
    return gtk_editable_get_text (GTK_EDITABLE (m_CurrentPage));
}

const gchar *
MainView::getZoomText (void)
{
    return gtk_editable_get_text (GTK_EDITABLE (m_CurrentZoom));
}

void
MainView::setStatusBarText (const gchar *text)
{
    // GTK4: m_StatusBar is now a GtkLabel, not GtkStatusbar
    if (m_StatusBar && GTK_IS_LABEL(m_StatusBar)) {
        if ( NULL != text )
        {
            gtk_label_set_text (GTK_LABEL (m_StatusBar), text);
        }
        else
        {
            gtk_label_set_text (GTK_LABEL (m_StatusBar), "");
        }
    }
}

void
MainView::setZoomText (const gchar *text)
{
    // If the entry has focus, the user is editing - don't interfere!
    if (gtk_widget_has_focus(m_CurrentZoom)) {
        return;
    }
    
    // Clear and set text to force visual update
    gtk_editable_delete_text(GTK_EDITABLE(m_CurrentZoom), 0, -1);
    gtk_editable_set_text (GTK_EDITABLE (m_CurrentZoom), text);
}

IFindView *
MainView::getFindView ()
{
    return m_FindView;
}

IPageView *
MainView::getPageView ()
{
    return m_PageView;
}

IPreferencesView *
MainView::getPreferencesView ()
{
    return new PreferencesView (GTK_WINDOW (m_MainWindow));
}

#if defined (HAVE_CUPS)
IPrintView *
MainView::getPrintView ()
{
    return new PrintView (GTK_WINDOW (m_MainWindow));
}
#endif // HAVE_CUPS

void
MainView::setTitle (const gchar *title)
{
    gtk_window_set_title (GTK_WINDOW (m_MainWindow), title);
}

void
MainView::setOutline (DocumentOutline *outline)
{
    gtk_tree_store_clear (m_Outline);
    setOutlineChildren (outline, NULL);

    // Already select the first outline.
    GtkTreePath *path = gtk_tree_path_new_from_string ("0");
    gtk_tree_view_set_cursor (GTK_TREE_VIEW (m_TreeIndex), path, NULL, FALSE);
    gtk_tree_path_free (path);
}

void
MainView::showMenubar (gboolean show)
{
    // Modern UI: Headerbar is always visible (can't hide it)
    // Just update the action state for consistency
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "show-menubar");
    if (action) {
        g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (TRUE));
    }
}

void
MainView::invertToggle (gboolean show)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "invert-colors");
    if (action) {
        g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (show));
    }
}

void
MainView::showStatusbar (gboolean show)
{
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "show-statusbar");
    if (action) {
        g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (show));
    }
    if (m_StatusBar && GTK_IS_WIDGET(m_StatusBar))
    {
        if ( show )
        {
            gtk_widget_set_visible (m_StatusBar, TRUE);
        }
        else
        {
            gtk_widget_set_visible (m_StatusBar, FALSE);
        }
    }
}

void
MainView::showToolbar (gboolean show)
{
    // Modern UI: No separate toolbar (controls integrated in headerbar)
    // Just update the action state for consistency
    GAction *action = g_action_map_lookup_action (G_ACTION_MAP (m_ActionGroup), "show-toolbar");
    if (action) {
        g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (TRUE));
    }
}

////////////////////////////////////////////////////////////////
// GTK+ Functions.
////////////////////////////////////////////////////////////////

///
/// @brief Creates the "Current Page" widget that will be displayed on toolbar.
///
void
MainView::createCurrentPage ()
{
    GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);

    GtkWidget *pageLabel = gtk_label_new (_("Page"));
    gtk_box_append (GTK_BOX (hbox), pageLabel);

    m_CurrentPage = gtk_entry_new ();
    gtk_editable_set_text (GTK_EDITABLE (m_CurrentPage), "0");
    gtk_editable_set_alignment (GTK_EDITABLE (m_CurrentPage), 1.0f);
    gtk_editable_set_width_chars (GTK_EDITABLE (m_CurrentPage), CURRENT_PAGE_WIDTH);
    gtk_editable_set_editable (GTK_EDITABLE (m_CurrentPage), TRUE);
    gtk_widget_set_can_focus (m_CurrentPage, TRUE);
    gtk_widget_set_focusable (m_CurrentPage, TRUE);
    g_signal_connect (G_OBJECT (m_CurrentPage), "activate",
                      G_CALLBACK (main_window_go_to_page_cb), m_Pter);
    gtk_box_append (GTK_BOX (hbox), m_CurrentPage);
    gtk_widget_set_hexpand (m_CurrentPage, TRUE);
    m_NumberOfPages = gtk_label_new (_("of 0"));
    gtk_widget_set_halign (m_NumberOfPages, GTK_ALIGN_START);
    gtk_widget_set_valign (m_NumberOfPages, GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX (hbox), m_NumberOfPages);

    // GTK4: GtkToolItem removed, just use the box directly
    m_CurrentPageToolItem = hbox;
}


///
/// @brief Creates the "Current Zoom" widget that will be displayed on toolbar.
///
void
MainView::createCurrentZoom ()
{
    m_CurrentZoom = gtk_entry_new ();
    gtk_editable_set_alignment (GTK_EDITABLE (m_CurrentZoom), 1.0f);
    gtk_editable_set_width_chars (GTK_EDITABLE (m_CurrentZoom), CURRENT_ZOOM_WIDTH);
    gtk_editable_set_editable (GTK_EDITABLE (m_CurrentZoom), TRUE);
    gtk_widget_set_can_focus (m_CurrentZoom, TRUE);
    gtk_widget_set_focusable (m_CurrentZoom, TRUE);
    // Connect the activate signal to handle zoom changes
    g_signal_connect (G_OBJECT (m_CurrentZoom), "activate",
                     G_CALLBACK (on_zoom_entry_activate),
                     m_Pter);

    GtkWidget *zoomBox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
    gtk_box_append (GTK_BOX (zoomBox), m_CurrentZoom);
    gtk_widget_set_hexpand (m_CurrentZoom, TRUE);
    gtk_box_append (GTK_BOX (zoomBox), gtk_label_new ("%"));
    // GTK4: GtkToolItem removed, just use the box directly
    m_CurrentZoomToolItem = zoomBox;
}

///
/// @brief Creates the widget that will display the page.
///
/// Creates the image widget inside the scrolled window, but also creates
/// the side bar.
///
/// Put both the side bar and the scrolled window to a HPaned container.
///
/// @return The HPaned where the side bar and the scrolled window are.
///
GtkWidget *
MainView::createPageView ()
{
    // Create the page view.
    m_PageView = new PageView ();

    // Create the side bar, with the index tree.
    m_Outline = gtk_tree_store_new (outlineNumColumns, 
                                    G_TYPE_STRING, G_TYPE_INT, G_TYPE_POINTER);
    m_TreeIndex = gtk_tree_view_new_with_model (GTK_TREE_MODEL (m_Outline));
    gtk_widget_set_size_request (m_TreeIndex, 200, -1);

    GtkTreeViewColumn *column = gtk_tree_view_column_new ();
    gtk_tree_view_column_set_expand (column, TRUE);
    gtk_tree_view_column_set_title (column, _("Index"));
    gtk_tree_view_append_column (GTK_TREE_VIEW (m_TreeIndex), column);

    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        GValue ellipsizeMode = { 0, };
        g_value_init (&ellipsizeMode, PANGO_TYPE_ELLIPSIZE_MODE);
        g_value_set_enum (&ellipsizeMode, PANGO_ELLIPSIZE_END);
        g_object_set_property (G_OBJECT (renderer),
                               "ellipsize", &ellipsizeMode);
        gtk_tree_view_column_pack_start (column, renderer, TRUE);
        gtk_tree_view_column_set_attributes (column, renderer, "text", 0, NULL);
    }

    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
        gtk_tree_view_column_pack_end (column, renderer, FALSE);
        gtk_tree_view_column_set_attributes (column, renderer, "text", 1, NULL);
    }

    // Set the change signal of the selection.
    GtkTreeSelection *selection = 
        gtk_tree_view_get_selection (GTK_TREE_VIEW (m_TreeIndex));
    gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
    g_signal_connect (G_OBJECT (selection), "changed",
                      G_CALLBACK (main_window_outline_cb), m_Pter);

    m_Sidebar = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_Sidebar),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    // GTK4: gtk_scrolled_window_set_shadow_type removed, use CSS instead if needed
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (m_Sidebar), m_TreeIndex);


    GtkWidget *hPaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_start_child (GTK_PANED (hPaned), m_Sidebar);
    gtk_paned_set_end_child (GTK_PANED (hPaned), m_PageView->getTopWidget ());

    return hPaned;
}

///
/// @brief Creates and initialises the action group with GSimpleAction.
///
void
MainView::createActions ()
{
    m_ActionGroup = g_simple_action_group_new ();
    
    // Add normal actions
    for (guint i = 0; i < G_N_ELEMENTS (g_NormalEntries); i++) {
        GSimpleAction *action = g_simple_action_new (g_NormalEntries[i].name, NULL);
        g_signal_connect (action, "activate", g_NormalEntries[i].callback, m_Pter);
        g_action_map_add_action (G_ACTION_MAP (m_ActionGroup), G_ACTION (action));
        g_object_unref (action);
    }
    
    // Add toggle actions
    // GTK4: Stateful actions must connect to "change-state" signal, not "activate"
    for (guint i = 0; i < G_N_ELEMENTS (g_ToggleEntries); i++) {
        GSimpleAction *action = g_simple_action_new_stateful (
            g_ToggleEntries[i].name, NULL, 
            g_variant_new_boolean (g_ToggleEntries[i].default_state));
        g_signal_connect (action, "change-state", g_ToggleEntries[i].callback, m_Pter);
        g_action_map_add_action (G_ACTION_MAP (m_ActionGroup), G_ACTION (action));
        g_object_unref (action);
    }
    
    // Add radio actions for page mode
    GSimpleAction *page_mode_action = g_simple_action_new_stateful (
        "page-mode", g_variant_type_new("s"), 
        g_variant_new_string("scroll"));
    g_signal_connect (page_mode_action, "activate", 
                     G_CALLBACK(main_window_set_page_mode), m_Pter);
    g_action_map_add_action (G_ACTION_MAP (m_ActionGroup), G_ACTION (page_mode_action));
    g_object_unref (page_mode_action);
    
    // Insert action group into the main window
    gtk_widget_insert_action_group (m_MainWindow, "win", G_ACTION_GROUP (m_ActionGroup));
}

///
/// @brief Creates the menu bar using GTK4's GMenuModel and GtkPopoverMenuBar.
///
/// @brief Creates modern headerbar with integrated menu and navigation
///
void
MainView::createHeaderBar ()
{
    m_HeaderBar = gtk_header_bar_new ();
    
    // LEFT SIDE: Hamburger menu
    createMainMenu ();
    gtk_header_bar_pack_start (GTK_HEADER_BAR (m_HeaderBar), m_MenuButton);
    
    // CENTER: Navigation controls (prev, page, next, zoom)
    m_NavigationBox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
    
    // Previous page button
    GtkWidget *prev_button = gtk_button_new_from_icon_name ("go-previous-symbolic");
    gtk_widget_set_tooltip_text (prev_button, _("Previous Page"));
    gtk_actionable_set_action_name (GTK_ACTIONABLE (prev_button), "win.go-previous");
    gtk_box_append (GTK_BOX (m_NavigationBox), prev_button);
    
    // Current page widgets
    createCurrentPage ();
    gtk_box_append (GTK_BOX (m_NavigationBox), m_CurrentPageToolItem);
    
    // Next page button
    GtkWidget *next_button = gtk_button_new_from_icon_name ("go-next-symbolic");
    gtk_widget_set_tooltip_text (next_button, _("Next Page"));
    gtk_actionable_set_action_name (GTK_ACTIONABLE (next_button), "win.go-next");
    gtk_box_append (GTK_BOX (m_NavigationBox), next_button);
    
    // Separator
    GtkWidget *sep = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
    gtk_box_append (GTK_BOX (m_NavigationBox), sep);
    
    // Zoom controls
    GtkWidget *zoom_out_button = gtk_button_new_from_icon_name ("zoom-out-symbolic");
    gtk_widget_set_tooltip_text (zoom_out_button, _("Zoom Out"));
    gtk_actionable_set_action_name (GTK_ACTIONABLE (zoom_out_button), "win.zoom-out");
    gtk_box_append (GTK_BOX (m_NavigationBox), zoom_out_button);
    
    createCurrentZoom ();
    gtk_box_append (GTK_BOX (m_NavigationBox), m_CurrentZoomToolItem);
    
    GtkWidget *zoom_in_button = gtk_button_new_from_icon_name ("zoom-in-symbolic");
    gtk_widget_set_tooltip_text (zoom_in_button, _("Zoom In"));
    gtk_actionable_set_action_name (GTK_ACTIONABLE (zoom_in_button), "win.zoom-in");
    gtk_box_append (GTK_BOX (m_NavigationBox), zoom_in_button);
    
    // Separator
    GtkWidget *sep2 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
    gtk_box_append (GTK_BOX (m_NavigationBox), sep2);
    
    // Fit width button (horizontal arrows: ↔)
    GtkWidget *fit_width_button = gtk_button_new_from_icon_name ("view-fullscreen-symbolic");
    gtk_widget_set_tooltip_text (fit_width_button, _("Fit Width"));
    gtk_actionable_set_action_name (GTK_ACTIONABLE (fit_width_button), "win.fit-width");
    gtk_box_append (GTK_BOX (m_NavigationBox), fit_width_button);
    
    // Fit height button (vertical arrows: ↕)
    GtkWidget *fit_height_button = gtk_button_new_from_icon_name ("view-restore-symbolic");
    gtk_widget_set_tooltip_text (fit_height_button, _("Fit Height"));
    gtk_actionable_set_action_name (GTK_ACTIONABLE (fit_height_button), "win.fit-height");
    gtk_box_append (GTK_BOX (m_NavigationBox), fit_height_button);
    
    gtk_header_bar_set_title_widget (GTK_HEADER_BAR (m_HeaderBar), m_NavigationBox);
    
    // Set as window titlebar
    gtk_window_set_titlebar (GTK_WINDOW (m_MainWindow), m_HeaderBar);
}

///
/// @brief Creates the hamburger menu with all options
///
void
MainView::createMainMenu ()
{
    // Create hamburger menu button
    m_MenuButton = gtk_menu_button_new ();
    gtk_menu_button_set_icon_name (GTK_MENU_BUTTON (m_MenuButton), "open-menu-symbolic");
    gtk_widget_set_tooltip_text (m_MenuButton, _("Menu"));
    
    // Create menu model (standard GTK4 way)
    GMenu *menu = g_menu_new ();
    
    // File section
    GMenu *file_section = g_menu_new ();
    g_menu_append (file_section, _("Open…"), "win.open-file");
    g_menu_append (file_section, _("Reload"), "win.reload-file");
    // Removed "Save a Copy" - this is a PDF reader, not an editor
    // g_menu_append (file_section, _("Save a Copy…"), "win.save-file");
#if defined (HAVE_CUPS)
    g_menu_append (file_section, _("Print…"), "win.print");
#endif
    g_menu_append_section (menu, NULL, G_MENU_MODEL (file_section));
    g_object_unref (file_section);
    
    // View section with checkboxes
    GMenu *view_section = g_menu_new ();
    g_menu_append (view_section, _("Invert Colors"), "win.invert-colors");
    g_menu_append (view_section, _("Show Index"), "win.show-index");
    g_menu_append (view_section, _("Fullscreen"), "win.fullscreen");
    g_menu_append_section (menu, NULL, G_MENU_MODEL (view_section));
    g_object_unref (view_section);
    
    // Other options section
    GMenu *other_section = g_menu_new ();
    g_menu_append (other_section, _("Preferences"), "win.preferences");
    g_menu_append (other_section, _("About ePDFView"), "win.about");
    g_menu_append_section (menu, NULL, G_MENU_MODEL (other_section));
    g_object_unref (other_section);
    
    // Set the menu model
    gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (m_MenuButton), G_MENU_MODEL (menu));
    g_object_unref (menu);
}

///
/// @brief Loads and sets the application's icon.
///
void
MainView::setMainWindowIcon ()
{
    const gchar *iconFiles[] =
    {
        "icon_epdfview-48.png",
        "icon_epdfview-32.png",
        "icon_epdfview-24.png",
    };

    GList *iconList = NULL;
    int iconFilesNum = G_N_ELEMENTS (iconFiles);
    
    // Search paths for icons (system and local)
    const gchar *icon_dirs[] = {
        DATADIR "/pixmaps",           // Configured installation
        "/usr/local/share/pixmaps",   // Local installation
        "/usr/share/pixmaps",         // System location
        NULL
    };
    
    for ( int iconIndex = 0 ; iconIndex < iconFilesNum ; iconIndex++ )
    {
        GdkPixbuf *iconPixbuf = NULL;
        
        // Try each search path
        for (int dirIndex = 0; icon_dirs[dirIndex] != NULL && iconPixbuf == NULL; dirIndex++) {
            gchar *filename = g_build_filename (icon_dirs[dirIndex], 
                                               iconFiles[iconIndex], NULL);
            if (g_file_test(filename, G_FILE_TEST_EXISTS)) {
                GError *error = NULL;
                iconPixbuf = gdk_pixbuf_new_from_file (filename, &error);
                if ( NULL == iconPixbuf )
                {
                    g_warning ("Error loading icon %s: %s\n", filename, error->message);
                    g_error_free (error);
                }
            }
            g_free (filename);
        }
        
        if ( NULL != iconPixbuf )
        {
            iconList = g_list_prepend (iconList, iconPixbuf);
        }
    }
    // GTK4: gtk_window_set_default_icon_list removed
    // Use gtk_window_set_default_icon_name or individual window icons
    if (iconList) {
        // Set first icon as default
        GdkPixbuf *first_icon = (GdkPixbuf *)iconList->data;
        if (first_icon) {
            GdkTexture *texture = gdk_texture_new_for_pixbuf (first_icon);
            gtk_window_set_icon_name (GTK_WINDOW (m_MainWindow), "epdfview");
            g_object_unref (texture);
        }
    }
    g_list_foreach (iconList, (GFunc)g_object_unref, NULL);
    g_list_free (iconList);
}

///
/// @brief Recusively sets the outline data.
///
/// This function adds the outline's data to the tree model.
///
/// @param outline The outline element to add in @a rootIter
///                all its children. The first time it must be the root
///                outline element.
/// @param rootIter The iter to add the @a outline's children to. The first
///                 time it must be the tree's root iter.
///
void
MainView::setOutlineChildren (DocumentOutline *outline, GtkTreeIter *rootIter)
{
    if ( NULL != outline )
    {
        DocumentOutline *child = outline->getFirstChild ();
        while (NULL != child)
        {
            GtkTreeIter childIter;
            gtk_tree_store_append (m_Outline, &childIter, rootIter);
            gtk_tree_store_set (m_Outline, &childIter,
                                outlineTitleColumn, child->getTitle (),
                                outlinePageColumn, child->getDestinationPage (),
                                outlineDataColumn, child,
                                -1);
            setOutlineChildren (child, &childIter);
            child = outline->getNextChild ();
        }
    }
}

void
MainView::copyTextToClibboard(const gchar* text)
{
    // GTK4: Use GdkClipboard instead of deprecated GtkClipboard
    GdkDisplay *display = gdk_display_get_default ();
    GdkClipboard *clipboard = gdk_display_get_clipboard (display);
    gdk_clipboard_set_text (clipboard, text);
}

////////////////////////////////////////////////////////////////
// GTK+ Callbacks.
////////////////////////////////////////////////////////////////

///
/// @brief The user tries to open the dialog box.
///
void
main_window_about_box_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");
    
    MainPter *pter = (MainPter *)data;
    MainView *view = (MainView *)&(pter->getView());
    GtkWidget *parent_window = view->getMainWindow();
    const gchar *authors[] = {
        "Jordi Fita <jordi@emma-soft.com>",
        "Pablo Lezaeta <prflr88@gmail.com>",
        NULL
    };

    const gchar *comments = _("A lightweight PDF viewer");

    const gchar *license[] = {
        N_("ePDFView is free software; you can redistribute it and/or modify\n"
           "it under the terms of the GNU General Public License as published "
           "by\nthe Free Software Foundation; either version 2 of the License, or\n(at your option) any later version.\n"),
        N_("ePDFView is distributed in the hope that it will be useful,\n"
           "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
           "GNU General Public License for more details.\n"),
        N_("You should have received a copy of the GNU General Public License\n"
           "along with ePDFView; if not, write to the Free Software Foundation,"
           "Inc.,\n59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n")
    };
    gchar *licenseTranslated = g_strconcat (_(license[0]), "\n",
                                            _(license[1]), "\n",
                                            _(license[2]), "\n", NULL);
    
    // GTK4: Load logo from SVG file
    GdkPixbuf *logo_pixbuf = NULL;
    GdkTexture *logo_texture = NULL;
    
    // Try multiple paths: source dir (for development), then installed locations
    gchar *source_logo = g_build_filename(g_get_current_dir(), "data", "epdfview.svg", NULL);
    const gchar *logo_paths[] = {
        source_logo,                              // Development/source directory
        "../data/epdfview.svg",                  // Relative to build directory
        DATADIR "/epdfview.svg",                 // Installed location
        "/usr/share/pixmaps/epdfview.svg",      // Standard system location
        "/usr/local/share/pixmaps/epdfview.svg", // Local installation
        "/usr/share/icons/hicolor/scalable/apps/epdfview.svg", // Hicolor icon location
        "/usr/local/share/icons/hicolor/scalable/apps/epdfview.svg", // Hicolor local icon location
        NULL
    };
    
    for (int i = 0; logo_paths[i] != NULL; i++) {
        GError *error = NULL;
        if (g_file_test(logo_paths[i], G_FILE_TEST_EXISTS)) {
            logo_pixbuf = gdk_pixbuf_new_from_file_at_size(logo_paths[i], 128, 128, &error);
            if (logo_pixbuf) {
                // GTK4: Convert GdkPixbuf to GdkTexture (GdkPaintable) for about dialog
                logo_texture = gdk_texture_new_for_pixbuf(logo_pixbuf);
                g_object_unref(logo_pixbuf);
                break;
            }
            if (error) {
                g_error_free(error);
            }
        }
    }
    g_free(source_logo);
    
    // GTK4: gtk_about_dialog_set_url_hook removed - links work automatically
    // GTK4: About dialogs automatically have a close button
    // GTK4: Use parent window instead of NULL to avoid errors
    gtk_show_about_dialog (GTK_WINDOW (parent_window),
            "program-name", _("ePDFView"),
            "version", VERSION,
            "copyright", "\xc2\xa9 2006-2011 Emma's Software\n"
                         "\xc2\xa9 2025 Pablo Lezaeta",
            "license", licenseTranslated,
            "website", "http://www.emma-soft.com/projects/epdfview/\n"
                       "https://github.com/JotaRandom/epdfview",
            "authors", authors,
            "comments", comments,
            "translator-credits", _("translator-credits"),
            "logo", logo_texture,
            "wrap-license", TRUE,
            NULL);

    g_free (licenseTranslated);
    if (logo_texture) {
        g_object_unref(logo_texture);
    }
}


// GTK4: gtk_about_dialog_set_url_hook removed - links work automatically
// This callback is no longer needed

///
/// @brief The user tried to find a word in the document.
///
void
main_window_find_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->findActivated ();
}

///
/// @brief The user tried to switch to full screen.
///
void
main_window_fullscreen_cb (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    // GTK4 change-state: parameter contains the new state
    gboolean new_state = g_variant_get_boolean (parameter);
    
    if (pter->isDocumentLoaded()) {
        g_simple_action_set_state (action, parameter);
        pter->fullScreenActivated (new_state);
    }
}

///
/// @brief The user tries to expand the document.
///
void
main_window_zoom_in_cb (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomInActivated();
}

///
/// @brief The user tries to shrink the document.
///
void
main_window_zoom_out_cb (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomOutActivated();
}

///
/// @brief The user wants to fit width to window.
///
static void
main_window_fit_width_cb (GSimpleAction *action,
                         GVariant      *parameter,
                         gpointer       data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomToWidthActivated();
}

///
/// @brief The user wants to fit height to window.
///
static void
main_window_fit_height_cb (GSimpleAction *action,
                          GVariant      *parameter,
                          gpointer       data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomToHeightActivated();
}

///
/// @brief The user tries to go to the first page.
///
void
main_window_go_to_first_page_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->goToFirstPageActivated ();
}

///
/// @brief The user tries to go the last page.
///
void
main_window_go_to_last_page_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->goToLastPageActivated ();
}

///
/// @brief The user tries to go to the next page.
///
void
main_window_go_to_next_page_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->goToNextPageActivated ();
}

///
/// @brief The user tries to go to a given page on the toolbar.
///
void
main_window_go_to_page_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->goToPageActivated ();
}

///
/// @brief The user tries to go to the previous page.
///
void
main_window_go_to_previous_page_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->goToPreviousPageActivated ();
}

///
/// @brief The user tried to reload the current document.
///
void
main_window_reload_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->reloadActivated ();
}

///
/// @brief The user tried to rotate the document counter-clockwise.
///
void
main_window_rotate_left_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->rotateLeftActivated ();
}

///
/// @brief The user tried to rotate the document clockwise.
///
void
main_window_rotate_right_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->rotateRightActivated ();
}

///
/// @brief The user tries to open a file.
///
void
main_window_open_file_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->openFileActivated ();
}

///
/// @brief The user selected an outline index item.
///
void
main_window_outline_cb (GtkTreeSelection *treeSelection, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    // Since the selection is in single mode, get the currently
    // selected item.
    GtkTreeModel *indexModel = NULL;
    GtkTreeIter iter;
    if ( gtk_tree_selection_get_selected (treeSelection, &indexModel, &iter) )
    {
        gpointer outlineData;
        gtk_tree_model_get (indexModel, &iter,
                            outlineDataColumn, &outlineData, -1);
        DocumentOutline *outline = (DocumentOutline *)outlineData;
        MainPter *pter = (MainPter *)data;
        pter->outlineActivated (outline);
    }
}

///
/// @brief The user wants to change the preferences.
///
void
main_window_preferences_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->preferencesActivated ();
}

#if defined (HAVE_CUPS)
///
///
/// @brief The user wants to print the current document.
///
void
main_window_print_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->printActivated ();
}
#endif // HAVE_CUPS

///
/// @brief Called when the window is closed or Quit is activated.
///
void
main_window_quit_cb (GtkWidget *widget, gpointer data)
{
    // GTK4: gtk_main_quit removed - use g_application_quit or exit
    // Since we're using GtkApplication, we should get the app instance
    // For now, just exit the main loop
    exit(0);
}

///
/// @brief Called when the user clicks on the "Show Index" action.
///
void
main_window_show_index_cb (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    if (action && parameter) {
        // GTK4 change-state: parameter contains the new state
        gboolean new_state = g_variant_get_boolean (parameter);
        g_simple_action_set_state (action, parameter);
        pter->showIndexActivated (new_state);
    } else {
        // Called from setFullScreen - use current state
        pter->showIndexActivated (TRUE);
    }
}

///
/// @brief The user tries to save a copy of the document.
///
void
main_window_save_file_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->saveFileActivated ();
}

/// KROGAN EDIT
/// @brief Called when the user clicks on the "Show Menubar" action.
///
void
main_window_show_menubar_cb (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    if (action && parameter) {
        // GTK4 change-state: parameter contains the new state
        gboolean new_state = g_variant_get_boolean (parameter);
        g_simple_action_set_state (action, parameter);
        pter->showMenubarActivated (new_state);
    }
}

/// KROGAN EDIT
/// @brief Called when the user clicks on the "Invert Colors" action.
///
void
main_window_invert_color_cb (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    // GTK4 change-state: parameter contains the new state
    gboolean new_state = g_variant_get_boolean (parameter);
    g_simple_action_set_state (action, parameter);
    pter->invertToggleActivated (new_state);
}

///
/// @brief Called when the user clicks on the "Show Statusbar" action.
///
void
main_window_show_statusbar_cb (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    if (action && parameter) {
        // GTK4 change-state: parameter contains the new state
        gboolean new_state = g_variant_get_boolean (parameter);
        g_simple_action_set_state (action, parameter);
        pter->showStatusbarActivated (new_state);
    } else {
        // Called from setFullScreen - use current state
        pter->showStatusbarActivated (TRUE);
    }
}

///
/// @brief Called when the user clicks on the "Show Toolbar" action.
///
void
main_window_show_toolbar_cb (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    if (action && parameter) {
        // GTK4 change-state: parameter contains the new state
        gboolean new_state = g_variant_get_boolean (parameter);
        g_simple_action_set_state (action, parameter);
        pter->showToolbarActivated (new_state);
    } else {
        // Called from setFullScreen - use current state
        pter->showToolbarActivated (TRUE);
    }
}

///
/// @brief Callback when the zoom entry is activated (user presses Enter)
///
static void
on_zoom_entry_activate (GtkEntry *entry, gpointer data)
{
    g_assert (data != NULL && "The data parameter is NULL.");
    
    MainPter *pter = (MainPter *)data;
    pter->zoomActivated();
}


///
/// @brief The user tries to fit the document into the window.
///
void
main_window_zoom_fit_cb (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    // GTK4 change-state: parameter contains the new state
    gboolean new_state = g_variant_get_boolean (parameter);
    g_simple_action_set_state (action, parameter);
    pter->zoomFitActivated (new_state);
}

///
/// @brief The user tries to fit the document width to the window width.
///
void
main_window_zoom_width_cb (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    // GTK4 change-state: parameter contains the new state
    gboolean new_state = g_variant_get_boolean (parameter);
    g_simple_action_set_state (action, parameter);
    pter->zoomWidthActivated (new_state);
}

///
/// @brief The user tries to set a page mode.
///
void
main_window_set_page_mode (GSimpleAction *action, GVariant *parameter, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    const gchar *mode_str = g_variant_get_string (parameter, NULL);
    PagePterMode mode = PagePterModeScroll;
    
    if (g_strcmp0 (mode_str, "text") == 0) {
        mode = PagePterModeSelectText;
    }
    
    g_simple_action_set_state (action, parameter);
    pter->setPageMode (mode);
}

// GTK4: This callback removed - scroll events for zoom handled via event controllers
// Zoom with Ctrl+Scroll can be implemented in PageView if needed
