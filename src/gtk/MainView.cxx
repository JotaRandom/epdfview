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

// Constants.
static gint CURRENT_PAGE_POS = 5;
static gint CURRENT_PAGE_WIDTH = 5;
static gint CURRENT_ZOOM_POS = 8;
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
static void main_window_about_box_url_hook (GtkAboutDialog *, const gchar *,
                                            gpointer);
static void main_window_find_cb (GtkWidget *, gpointer);
static void main_window_fullscreen_cb (GtkToggleAction *, gpointer);
static gboolean main_window_moved_or_resized_cb (GtkWidget *,
                                                 GdkEventConfigure *, gpointer);
static void main_window_go_to_first_page_cb (GtkWidget *, gpointer);
static void main_window_go_to_last_page_cb (GtkWidget *, gpointer);
static void main_window_go_to_next_page_cb (GtkWidget *, gpointer);
static void main_window_go_to_page_cb (GtkWidget *, gpointer);
static void main_window_go_to_previous_page_cb (GtkWidget *, gpointer);
static void main_window_reload_cb (GtkWidget *, gpointer);
static void main_window_rotate_left_cb (GtkWidget *, gpointer);
static void main_window_rotate_right_cb (GtkWidget *, gpointer);
static void main_window_open_file_cb (GtkWidget *, gpointer);
static void main_window_outline_cb (GtkTreeSelection *, gpointer);
static void main_window_preferences_cb (GtkWidget *, gpointer);
static void main_window_quit_cb (GtkWidget *, gpointer);
static void main_window_save_file_cb (GtkWidget *, gpointer);
static void main_window_show_index_cb (GtkToggleAction *, gpointer);
static void main_window_show_statusbar_cb (GtkToggleAction *, gpointer);
static void main_window_show_toolbar_cb (GtkToggleAction *, gpointer);
static void main_window_zoom_cb (GtkWidget *, gpointer);
static void main_window_zoom_fit_cb (GtkToggleAction *, gpointer);
static void main_window_zoom_in_cb (GtkWidget *, gpointer);
static void main_window_zoom_out_cb (GtkWidget *, gpointer);
static void main_window_zoom_width_cb (GtkToggleAction *, gpointer);
static void main_window_set_page_mode (GtkRadioAction *, GtkRadioAction *, gpointer);
static gboolean main_window_page_scrolled_cb (GtkWidget *widget, GdkEventScroll *event, gpointer data);

#if defined (HAVE_CUPS)
static void main_window_print_cb (GtkWidget *, gpointer);
#endif // HAVE_CUPS

// The actions for menus and toolbars.
static const GtkActionEntry g_NormalEntries[] =
{
    { "FileMenu", NULL, N_("_File"), NULL, NULL, NULL },
    { "EditMenu", NULL, N_("_Edit"), NULL, NULL, NULL },
    { "ViewMenu", NULL, N_("_View"), NULL, NULL, NULL },
    { "GoMenu", NULL, N_("_Go"), NULL, NULL, NULL },
    { "HelpMenu", NULL, N_("_Help"), NULL, NULL, NULL },

    { "OpenFile", GTK_STOCK_OPEN, N_("_Open"), "<control>O",
      N_("Open a PDF document"),
      G_CALLBACK (main_window_open_file_cb) },

    { "ReloadFile", GTK_STOCK_REFRESH, N_("_Reload"), "<control>R",
      N_("Reload the current document"),
      G_CALLBACK (main_window_reload_cb) },

    { "SaveFile", GTK_STOCK_SAVE, N_("_Save a Copy..."), "<control>S",
      N_("Save a copy of the current document"),
      G_CALLBACK (main_window_save_file_cb) },

#if defined (HAVE_CUPS)
    { "Print", GTK_STOCK_PRINT, N_("_Print..."), "<control>P",
      N_("Print the current document"),
      G_CALLBACK (main_window_print_cb) },
#endif // HAVE_CUPS

    { "Quit", GTK_STOCK_CLOSE, N_("_Close"), "<control>W",
      N_("Close this window"),
      G_CALLBACK (main_window_quit_cb) },

    { "Find", GTK_STOCK_FIND, N_("_Find"), "<control>F",
      N_("Find a word in the document"),
      G_CALLBACK (main_window_find_cb) },

    { "Preferences", GTK_STOCK_PREFERENCES, N_("_Preferences..."), NULL,
      N_("Change the application's preferences"),
      G_CALLBACK (main_window_preferences_cb) },

    { "ZoomIn", GTK_STOCK_ZOOM_IN, N_("Zoom _In"), "<control>plus",
      N_("Enlarge the document"),
      G_CALLBACK (main_window_zoom_in_cb) },

    { "ZoomOut", GTK_STOCK_ZOOM_OUT, N_("Zoom _Out"), "<control>minus",
      N_("Shrink the document"),
      G_CALLBACK (main_window_zoom_out_cb) },

    { "RotateRight", EPDFVIEW_STOCK_ROTATE_RIGHT, N_("Rotate _Right"), "<control>bracketright",
      N_("Rotate the document 90 degrees clockwise"),
      G_CALLBACK (main_window_rotate_right_cb) },

    { "RotateLeft", EPDFVIEW_STOCK_ROTATE_LEFT, N_("Rotate _Left"), "<control>bracketleft",
      N_("Rotate the document 90 degrees counter-clockwise"),
      G_CALLBACK (main_window_rotate_left_cb) },

    { "GoToFirstPage", GTK_STOCK_GOTO_FIRST, N_("_First Page"), "<control>Home",
      N_("Go to the first page"),
      G_CALLBACK (main_window_go_to_first_page_cb) },

    { "GoToNextPage", GTK_STOCK_GO_FORWARD, N_("_Next Page"), "<Shift>Page_Down",
      N_("Go to the next page"),
      G_CALLBACK (main_window_go_to_next_page_cb) },

    { "GoToPreviousPage", GTK_STOCK_GO_BACK, N_("_Previous Page"), "<Shift>Page_Up",
      N_("Go to the previous page"),
      G_CALLBACK (main_window_go_to_previous_page_cb) },

    { "GoToLastPage", GTK_STOCK_GOTO_LAST, N_("_Last Page"), "<control>End",
      N_("Go to the last page"),
      G_CALLBACK (main_window_go_to_last_page_cb) },

    { "About", GTK_STOCK_ABOUT, N_("_About"), NULL,
      N_("Display application's credits"),
      G_CALLBACK (main_window_about_box_cb) },

    // Accelerator keys.
    { "Slash", GTK_STOCK_FIND, NULL, "slash", NULL,
      G_CALLBACK (main_window_find_cb) },

    { "KPAdd", GTK_STOCK_ZOOM_IN, NULL, "<control>KP_Add", NULL,
      G_CALLBACK (main_window_zoom_in_cb) },

    { "KPSubtract", GTK_STOCK_ZOOM_OUT, NULL, "<control>KP_Subtract",
      NULL,
      G_CALLBACK (main_window_zoom_out_cb) }
};

static GtkToggleActionEntry g_ToggleEntries[] =
{
    { "FullScreen", NULL, N_("F_ull screen"), "F11",
      N_("Toggle full screen window"),
      G_CALLBACK (main_window_fullscreen_cb), FALSE },

    { "ShowToolBar", NULL, N_("Show _Toolbar"), NULL,
      N_("Show or hide the toolbar"),
      G_CALLBACK (main_window_show_toolbar_cb), TRUE },

    { "ShowStatusBar", NULL, N_("Show _Statusbar"), NULL,
      N_("Show or hide the statusbar"),
      G_CALLBACK (main_window_show_statusbar_cb), TRUE },

    { "ShowIndex", NULL, N_("Show I_ndex"), "F9",
      N_("Show or hide the document's outline"),
      G_CALLBACK (main_window_show_index_cb), FALSE },

    { "ZoomFit", GTK_STOCK_ZOOM_FIT, N_("Zoom to _Fit"), NULL,
      N_("Make the current document fill the window"),
      G_CALLBACK (main_window_zoom_fit_cb), FALSE },

    { "ZoomWidth", EPDFVIEW_STOCK_ZOOM_WIDTH, N_("Zoom to _Width"), NULL,
      N_("Make the current document fill the window width"),
      G_CALLBACK (main_window_zoom_width_cb), FALSE },
};

static GtkRadioActionEntry g_PageScrollEntries[] =
{   
    { "PageModeScroll", GTK_STOCK_FULLSCREEN, N_("Scroll"), NULL,
      N_("Mouse scroll page"), (int)PagePterModeScroll },

    { "PageModeText", GTK_STOCK_SELECT_ALL, N_("Select Text"), NULL,
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
    // Create the main window.
    m_MainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    setMainWindowIcon ();
    // Connect already the destroy and delete signal.
    g_signal_connect (G_OBJECT (m_MainWindow), "destroy",
                      G_CALLBACK (main_window_quit_cb), NULL);
    g_signal_connect (G_OBJECT (m_MainWindow), "configure-event",
                      G_CALLBACK (main_window_moved_or_resized_cb), NULL);
    // Create the main vertical box.
    m_MainBox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (m_MainWindow), m_MainBox);
    gtk_widget_show (m_MainBox);
    // Generate menu and tool bars.
    createUIManager ();
    // Set the accelerator to the main window.
    GtkAccelGroup *accelGroup = gtk_ui_manager_get_accel_group (m_UIManager);
    gtk_window_add_accel_group (GTK_WINDOW (m_MainWindow), accelGroup);
    // Add the menu bar and tool bar.
    GtkWidget *menuBar = gtk_ui_manager_get_widget (m_UIManager, "/MenuBar");
    gtk_box_pack_start (GTK_BOX (m_MainBox), menuBar, FALSE, FALSE, 0);
    gtk_widget_show (menuBar);
    GtkWidget *toolBar = gtk_ui_manager_get_widget (m_UIManager, "/ToolBar");
    gtk_box_pack_start (GTK_BOX (m_MainBox), toolBar, FALSE, FALSE, 0);
    // Add the current page tool item.
    createCurrentPage ();
    gtk_toolbar_insert (GTK_TOOLBAR (toolBar), m_CurrentPageToolItem,
                        CURRENT_PAGE_POS);
    gtk_widget_show_all (GTK_WIDGET (m_CurrentPageToolItem));
    GtkToolItem *separator = gtk_separator_tool_item_new ();
    gtk_toolbar_insert (GTK_TOOLBAR (toolBar), separator, CURRENT_PAGE_POS + 1);
    gtk_widget_show_all (GTK_WIDGET (separator));
    // Add the current zoom item.
    createCurrentZoom ();
    gtk_toolbar_insert (GTK_TOOLBAR (toolBar), m_CurrentZoomToolItem,
                        CURRENT_ZOOM_POS);
    gtk_widget_show_all (GTK_WIDGET (m_CurrentZoomToolItem));
    // Create the page view
    GtkWidget *pageViewPaned = createPageView ();
    gtk_box_pack_start (GTK_BOX (m_MainBox), pageViewPaned, TRUE, TRUE, 0);
    gtk_widget_show (pageViewPaned);
    // By default set focus to page view so user can navigate pdf document with
    // keyboard right away without need to click to page view first
    gtk_widget_grab_focus (m_PageView->getTopWidget ());
    // Add the find bar.
    m_FindView = new FindView ();
    gtk_box_pack_start (GTK_BOX (m_MainBox), m_FindView->getTopWidget (),
                        FALSE, FALSE, 0);

    // The status bar don't do anything, yet. But without the application
    // look weird.
    m_StatusBar = gtk_statusbar_new ();
    gtk_box_pack_start (GTK_BOX (m_MainBox), m_StatusBar, FALSE, FALSE, 0);

    // Signal for Zooming into the page per ctrl + scroll-wheel.
    g_signal_connect (G_OBJECT (m_PageView->getTopWidget ()), "scroll-event",
                      G_CALLBACK (main_window_page_scrolled_cb), pter);
}

MainView::~MainView ()
{
    delete m_FindView;
    delete m_PageView;
    g_object_unref (G_OBJECT (m_UIManager));
}

void
MainView::activeZoomFit (gboolean active)
{
    GtkAction *zoomFit =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/ViewMenu/ZoomFit");
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (zoomFit), active);
}

void
MainView::activeZoomWidth (gboolean active)
{
    GtkAction *zoomWidth =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/ViewMenu/ZoomWidth");
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (zoomWidth), active);
}

void
MainView::activePageModeScroll (gboolean active)
{
    GtkAction *action =
        gtk_ui_manager_get_action (m_UIManager, "/ToolBar/PageModeScroll");
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), active);
}


void
MainView::activePageModeText (gboolean active)
{
    GtkAction *action =
        gtk_ui_manager_get_action (m_UIManager, "/ToolBar/PageModeText");
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), active);
}

gboolean
MainView::isIndexVisible () const
{
    GtkAction *showIndex =
        gtk_ui_manager_get_action (m_UIManager,
                                   "/MenuBar/ViewMenu/ShowIndex");
    return gtk_toggle_action_get_active(GTK_TOGGLE_ACTION (showIndex));
}

gchar *
MainView::openFileDialog (const gchar *lastFolder)
{
    GtkWidget *openDialog = gtk_file_chooser_dialog_new (_("Open PDF File"),
            GTK_WINDOW (m_MainWindow),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
            NULL);
    gtk_dialog_set_alternative_button_order (GTK_DIALOG (openDialog),
            GTK_RESPONSE_ACCEPT, GTK_RESPONSE_CANCEL, -1);

    // Select the last used folder as the initial folder, if any.
    if ( NULL != lastFolder )
    {
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (openDialog),
                                             lastFolder);
    }

    // Add the file type filters.
    {
        GtkFileFilter *pdfFilter = gtk_file_filter_new ();
        gtk_file_filter_set_name (pdfFilter, 
                                  _("Portable Document Format (PDF) Files"));
        gtk_file_filter_add_mime_type (pdfFilter, "application/pdf");
        gtk_file_filter_add_pattern (pdfFilter, "*.pdf");
        gtk_file_filter_add_pattern (pdfFilter, "*.PDF");

        gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (openDialog), pdfFilter);
        // Set this filter as the default.
        gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (openDialog), pdfFilter);
    }

    {
        GtkFileFilter *anyFilter = gtk_file_filter_new ();
        gtk_file_filter_set_name (anyFilter, _("All Files"));
        gtk_file_filter_add_pattern (anyFilter, "*");

        gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (openDialog), anyFilter);
    }

    if ( GTK_RESPONSE_ACCEPT == gtk_dialog_run (GTK_DIALOG (openDialog)) )
    {
        gchar *fileName = gtk_file_chooser_get_filename (
                                GTK_FILE_CHOOSER (openDialog));
        gtk_widget_destroy (openDialog);
        return fileName;
    }
    gtk_widget_destroy (openDialog);
    return NULL;
}

gchar *
MainView::promptPasswordDialog ()
{
    /* Create the password dialog. */
    GtkWidget *primaryLabel = gtk_label_new ("");
    gtk_label_set_markup (GTK_LABEL (primaryLabel),
            "<span weight=\"bold\" size=\"larger\">Encrypted Document</span>");
    gtk_label_set_selectable (GTK_LABEL (primaryLabel), TRUE);
    gtk_misc_set_alignment (GTK_MISC (primaryLabel), 0.0, 0.0);
    GtkWidget *secondaryLabel = gtk_label_new ("This document is encrypted "
                                               "and can't be accessed. "
                                               "Please enter the document's "
                                               "password:");
    gtk_label_set_line_wrap (GTK_LABEL (secondaryLabel), TRUE);
    gtk_label_set_selectable (GTK_LABEL (secondaryLabel), TRUE);
    gtk_misc_set_alignment (GTK_MISC (secondaryLabel), 0.0, 0.0);
    GtkWidget *passwordEntry = gtk_entry_new ();
    gtk_entry_set_visibility (GTK_ENTRY (passwordEntry), FALSE);
    gtk_entry_set_activates_default (GTK_ENTRY (passwordEntry), TRUE);
    GtkWidget *vbox = gtk_vbox_new (FALSE, 12);
    gtk_box_pack_start (GTK_BOX (vbox), primaryLabel, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), secondaryLabel, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), passwordEntry, FALSE, FALSE, 6);

    GtkWidget *image =
        gtk_image_new_from_stock (GTK_STOCK_DIALOG_AUTHENTICATION,
                                  GTK_ICON_SIZE_DIALOG);
    gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
    GtkWidget *hbox = gtk_hbox_new (FALSE, 12);
    gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
    gtk_widget_show_all (hbox);


    GtkWidget *passwordDialog = gtk_dialog_new_with_buttons (
            _("Password"), GTK_WINDOW (m_MainWindow),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
            NULL);
    gtk_dialog_set_alternative_button_order (GTK_DIALOG (passwordDialog),
            GTK_RESPONSE_ACCEPT, GTK_RESPONSE_CANCEL, -1);
    gtk_dialog_set_default_response (GTK_DIALOG (passwordDialog), 
                                     GTK_RESPONSE_ACCEPT);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (passwordDialog)->vbox),
                        hbox, FALSE, FALSE, 0);

    gchar *password = NULL;
    if ( GTK_RESPONSE_ACCEPT == gtk_dialog_run (GTK_DIALOG (passwordDialog)) )
    {
        password = g_strdup(gtk_entry_get_text (GTK_ENTRY (passwordEntry)));
    }
    gtk_widget_destroy (passwordDialog);
    return password;
}

gchar *
MainView::saveFileDialog (const gchar *lastFolder, const gchar *fileName)
{
    GtkWidget *saveDialog = gtk_file_chooser_dialog_new (_("Save PDF File"),
            GTK_WINDOW (m_MainWindow),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
            NULL);
    gtk_file_chooser_set_do_overwrite_confirmation (
            GTK_FILE_CHOOSER (saveDialog), TRUE);
    gtk_dialog_set_alternative_button_order (GTK_DIALOG (saveDialog),
            GTK_RESPONSE_ACCEPT, GTK_RESPONSE_CANCEL, -1);

    // Select the last used folder as the initial folder, if any.
    if ( NULL != lastFolder )
    {
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (saveDialog),
                                             lastFolder);
    }
    // Set the original file name to use as initial save name, if any.
    if ( NULL != fileName )
    {
        gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (saveDialog),
                                           fileName);
    }

    // Add the file type filters.
    {
        GtkFileFilter *pdfFilter = gtk_file_filter_new ();
        gtk_file_filter_set_name (pdfFilter, 
                                  _("Portable Document Format (PDF) Files"));
        gtk_file_filter_add_mime_type (pdfFilter, "application/pdf");
        gtk_file_filter_add_pattern (pdfFilter, "*.pdf");
        gtk_file_filter_add_pattern (pdfFilter, "*.PDF");

        gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (saveDialog), pdfFilter);
        // Set this filter as the default.
        gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (saveDialog), pdfFilter);
    }

    {
        GtkFileFilter *anyFilter = gtk_file_filter_new ();
        gtk_file_filter_set_name (anyFilter, _("All Files"));
        gtk_file_filter_add_pattern (anyFilter, "*");

        gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (saveDialog), anyFilter);
    }

    if ( GTK_RESPONSE_ACCEPT == gtk_dialog_run (GTK_DIALOG (saveDialog)) )
    {
        gchar *fileName = gtk_file_chooser_get_filename (
                                GTK_FILE_CHOOSER (saveDialog));
        gtk_widget_destroy (saveDialog);
        return fileName;
    }
    gtk_widget_destroy (saveDialog);
    return NULL;
}

void
MainView::sensitiveFind (gboolean sensitive)
{
    GtkAction *find =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/EditMenu/Find");
    gtk_action_set_sensitive (find, sensitive);
}

void
MainView::sensitiveGoToFirstPage (gboolean sensitive)
{
    GtkAction *goToFirstPage = gtk_ui_manager_get_action (m_UIManager,
            "/MenuBar/GoMenu/GoToFirstPage");
    gtk_action_set_sensitive (goToFirstPage, sensitive);
}

void
MainView::sensitiveGoToLastPage (gboolean sensitive)
{
    GtkAction *goToLastPage =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/GoMenu/GoToLastPage");
    gtk_action_set_sensitive (goToLastPage, sensitive);
}

void
MainView::sensitiveGoToNextPage (gboolean sensitive)
{
    GtkAction *goToNextPage =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/GoMenu/GoToNextPage");
    gtk_action_set_sensitive (goToNextPage, sensitive);
}

void
MainView::sensitiveGoToPage (gboolean sensitive)
{
    gtk_widget_set_sensitive (GTK_WIDGET (m_CurrentPageToolItem), sensitive);
}

void 
MainView::sensitiveGoToPreviousPage (gboolean sensitive)
{
    GtkAction *goToPreviousPage = gtk_ui_manager_get_action (m_UIManager, 
            "/MenuBar/GoMenu/GoToPreviousPage");
    gtk_action_set_sensitive (goToPreviousPage, sensitive);
}

void
MainView::sensitiveOpen (gboolean sensitive)
{
    GtkAction *open =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/FileMenu/OpenFile");
    gtk_action_set_sensitive (open, sensitive);
}

#if defined (HAVE_CUPS)
void
MainView::sensitivePrint (gboolean sensitive)
{
    GtkAction *print = gtk_ui_manager_get_action (m_UIManager,
            "/MenuBar/FileMenu/PrintPlaceHolder/Print");
    gtk_action_set_sensitive (print, sensitive);
}
#endif // HAVE_CUPS

void
MainView::sensitiveReload (gboolean sensitive)
{
    GtkAction *reload =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/FileMenu/ReloadFile");
    gtk_action_set_sensitive (reload, sensitive);
}

void
MainView::sensitiveRotateLeft (gboolean sensitive)
{
    GtkAction *rotateLeft =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/ViewMenu/RotateLeft");
    gtk_action_set_sensitive (rotateLeft, sensitive);
}

void
MainView::sensitiveRotateRight (gboolean sensitive)
{
    GtkAction *rotateRight = gtk_ui_manager_get_action (m_UIManager,
            "/MenuBar/ViewMenu/RotateRight");
    gtk_action_set_sensitive (rotateRight, sensitive);
}

void
MainView::sensitiveSave (gboolean sensitive)
{
    GtkAction *save =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/FileMenu/SaveFile");
    gtk_action_set_sensitive (save, sensitive);
}

void
MainView::sensitiveZoom (gboolean sensitive)
{
    gtk_widget_set_sensitive (GTK_WIDGET (m_CurrentZoomToolItem), sensitive);
}

void
MainView::sensitiveZoomIn (gboolean sensitive)
{
    GtkAction *zoomIn =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/ViewMenu/ZoomIn");
    gtk_action_set_sensitive (zoomIn, sensitive);
}

void
MainView::sensitiveZoomOut (gboolean sensitive)
{
    GtkAction *zoomOut =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/ViewMenu/ZoomOut");
    gtk_action_set_sensitive (zoomOut, sensitive);
}

void
MainView::sensitiveZoomFit (gboolean sensitive)
{
    GtkAction *zoomFit =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/ViewMenu/ZoomFit");
    gtk_action_set_sensitive (zoomFit, sensitive);
}

void
MainView::sensitiveZoomWidth (gboolean sensitive)
{
    GtkAction *zoomWidth =
        gtk_ui_manager_get_action (m_UIManager, "/MenuBar/ViewMenu/ZoomWidth");
    gtk_action_set_sensitive (zoomWidth, sensitive);
}

void
MainView::show (void)
{
    Config &config = Config::getConfig ();
    gtk_window_set_default_size (GTK_WINDOW (m_MainWindow),
                                 config.getWindowWidth (),
                                 config.getWindowHeight ());
    gtk_widget_show (m_MainWindow);
    gtk_window_move (GTK_WINDOW (m_MainWindow),
                     config.getWindowX (), config.getWindowY ());
}

void
MainView::showErrorMessage (const gchar *title, const gchar *body)
{
    GtkWidget *errorDialog = gtk_message_dialog_new (
            GTK_WINDOW (m_MainWindow),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            title);
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG(errorDialog),
                                              body);
    gtk_dialog_run (GTK_DIALOG (errorDialog));
    gtk_widget_destroy (errorDialog);
}

void
MainView::showIndex (gboolean show)
{
    if ( show )
    {
        gtk_widget_show_all (m_Sidebar);
    }
    else
    {
        gtk_widget_hide (m_Sidebar);
    }
    GtkAction *showIndex =
        gtk_ui_manager_get_action (m_UIManager,
                                   "/MenuBar/ViewMenu/ShowIndex");
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (showIndex), show);
}

void
MainView::setCursor (ViewCursor cursorType)
{
    GdkCursor *cursor = NULL;
    switch (cursorType)
    {
        case MAIN_VIEW_CURSOR_WAIT:
            cursor = gdk_cursor_new (GDK_WATCH);
            break;
        case MAIN_VIEW_CURSOR_DRAG:
            cursor = gdk_cursor_new (GDK_FLEUR);
            break;
        default:
            cursor = NULL;
    }

    if ( NULL != m_MainWindow && GDK_IS_WINDOW (m_MainWindow->window) )
    {
        gdk_window_set_cursor (m_MainWindow->window, cursor);
    }
    if ( NULL != cursor )
    {
        gdk_cursor_unref (cursor);
    }
    gdk_flush ();
}

void
MainView::setFullScreen (gboolean fullScreen)
{
    GtkWidget *toolBar = gtk_ui_manager_get_widget (m_UIManager, "/ToolBar");
    GtkWidget *menuBar = gtk_ui_manager_get_widget (m_UIManager, "/MenuBar");
    if ( fullScreen )
    {
        gtk_window_fullscreen (GTK_WINDOW (m_MainWindow));
        // Hide the menu bar, tool bar, status bar and the index bar. Then
        // zoom to fit.
        gtk_widget_hide (menuBar);
        gtk_widget_hide (toolBar);
        gtk_widget_hide (m_StatusBar);
        gtk_widget_hide (m_Sidebar);
        gtk_toggle_action_set_active (
                GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (m_UIManager,
                                   "/MenuBar/ViewMenu/ZoomFit")), TRUE);
    }
    else
    {
        gtk_window_unfullscreen (GTK_WINDOW (m_MainWindow));
        gtk_widget_show (menuBar);
        // Show again the toolbar, status bar and index, only if it was
        // enabled.
        main_window_show_index_cb (
                GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (m_UIManager,
                        "/MenuBar/ViewMenu/ShowIndex")),
                (gpointer)m_Pter);
        main_window_show_statusbar_cb (
                GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (m_UIManager,
                        "/MenuBar/ViewMenu/ShowStatusBar")),
                (gpointer)m_Pter);
        main_window_show_toolbar_cb (
                GTK_TOGGLE_ACTION (gtk_ui_manager_get_action (m_UIManager,
                        "/MenuBar/ViewMenu/ShowToolBar")),
                (gpointer)m_Pter);
    }
}

void
MainView::setNumberOfPagesText (const gchar *text)
{
    gtk_label_set_text (GTK_LABEL (m_NumberOfPages), text);
}

void
MainView::setGoToPageText (const gchar *text)
{
    gtk_entry_set_text (GTK_ENTRY (m_CurrentPage), text);
}

const gchar *
MainView::getGoToPageText (void)
{
    return gtk_entry_get_text (GTK_ENTRY (m_CurrentPage));
}

const gchar *
MainView::getZoomText (void)
{
    return gtk_entry_get_text (GTK_ENTRY (m_CurrentZoom));
}

void
MainView::setStatusBarText (const gchar *text)
{
    gint contextId = gtk_statusbar_get_context_id (GTK_STATUSBAR (m_StatusBar),
            "general_message");
    gtk_statusbar_pop (GTK_STATUSBAR (m_StatusBar), contextId);
    if ( NULL != text )
    {
        gtk_statusbar_push (GTK_STATUSBAR (m_StatusBar), contextId, text);
    }
}

void
MainView::setZoomText (const gchar *text)
{
    gtk_entry_set_text (GTK_ENTRY (m_CurrentZoom), text);
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
MainView::showStatusbar (gboolean show)
{
    GtkAction *toggleAction = gtk_ui_manager_get_action (m_UIManager,
                                            "/MenuBar/ViewMenu/ShowStatusBar");
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (toggleAction), show);
    if ( show )
    {
        gtk_widget_show (m_StatusBar);
    }
    else
    {
        gtk_widget_hide (m_StatusBar);
    }
}

void
MainView::showToolbar (gboolean show)
{
    GtkAction *toggleAction = gtk_ui_manager_get_action (m_UIManager,
                                            "/MenuBar/ViewMenu/ShowToolBar");
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (toggleAction), show);
    GtkWidget *toolBar = gtk_ui_manager_get_widget (m_UIManager, "/ToolBar");
    if ( show )
    {
        gtk_widget_show (toolBar);
    }
    else
    {
        gtk_widget_hide (toolBar);
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
    GtkWidget *hbox = gtk_hbox_new (FALSE, 3);

    GtkWidget *pageLabel = gtk_label_new (_("Page"));
    gtk_box_pack_start (GTK_BOX (hbox), pageLabel, FALSE, FALSE, 0);

    m_CurrentPage = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY (m_CurrentPage), "0");
    gtk_entry_set_alignment (GTK_ENTRY (m_CurrentPage), 1.0f);
    gtk_entry_set_width_chars (GTK_ENTRY (m_CurrentPage), CURRENT_PAGE_WIDTH);
    g_signal_connect (G_OBJECT (m_CurrentPage), "activate",
                      G_CALLBACK (main_window_go_to_page_cb), m_Pter);
    gtk_box_pack_start (GTK_BOX (hbox), m_CurrentPage, TRUE, TRUE, 0);
    m_NumberOfPages = gtk_label_new (_("of 0"));
    gtk_misc_set_alignment (GTK_MISC (m_NumberOfPages), 0.0f, 0.5f);
    gtk_box_pack_start (GTK_BOX (hbox), m_NumberOfPages, FALSE, FALSE, 0);

    m_CurrentPageToolItem = gtk_tool_item_new ();
    gtk_container_add (GTK_CONTAINER (m_CurrentPageToolItem), hbox);
}


///
/// @brief Creates the "Current Zoom" widget that will be displayed on toolbar.
///
void
MainView::createCurrentZoom ()
{
    m_CurrentZoom = gtk_entry_new ();
    gtk_entry_set_alignment (GTK_ENTRY (m_CurrentZoom), 1.0f);
    gtk_entry_set_width_chars (GTK_ENTRY (m_CurrentZoom), CURRENT_ZOOM_WIDTH);
    g_signal_connect (G_OBJECT (m_CurrentZoom), "activate",
                      G_CALLBACK (main_window_zoom_cb), m_Pter);

    GtkWidget *zoomBox = gtk_hbox_new (FALSE, 3);
    gtk_box_pack_start_defaults (GTK_BOX (zoomBox), m_CurrentZoom);
    gtk_box_pack_start (GTK_BOX (zoomBox), gtk_label_new ("%"),
                        FALSE, FALSE, 0);
    m_CurrentZoomToolItem = gtk_tool_item_new ();
    gtk_container_add (GTK_CONTAINER (m_CurrentZoomToolItem), zoomBox);
}

///
/// @brief Creates the widget that will display the page.
///
/// Creates the image widget inside the scrolled window, but also creates
/// the side bar.
///
/// Put both the side bar and the image to a HPaned container.
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

    m_Sidebar = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_Sidebar),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_Sidebar),
                                         GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER (m_Sidebar), m_TreeIndex);


    GtkWidget *hPaned = gtk_hpaned_new ();
    gtk_paned_add1 (GTK_PANED (hPaned), m_Sidebar);
    gtk_paned_add2 (GTK_PANED (hPaned), m_PageView->getTopWidget ());
    gtk_widget_show (hPaned);

    return hPaned;
}

///
/// @brief Creates and initialises the main window's UI manager.
///
void
MainView::createUIManager ()
{
    GtkActionGroup *actionGroup = gtk_action_group_new ("ePDFView");
    // Set the translation domain for labels and tool tips.
    gtk_action_group_set_translation_domain (actionGroup, PACKAGE);
    // The data passed to all actions is the presenter
    gtk_action_group_add_actions (actionGroup, g_NormalEntries,
                                  G_N_ELEMENTS (g_NormalEntries), m_Pter);
    gtk_action_group_add_toggle_actions (actionGroup, g_ToggleEntries,
                                         G_N_ELEMENTS (g_ToggleEntries),
                                         m_Pter);
    gtk_action_group_add_radio_actions (actionGroup, g_PageScrollEntries,
                                        G_N_ELEMENTS (g_PageScrollEntries), 0,
                                        G_CALLBACK(main_window_set_page_mode), m_Pter);
    m_UIManager = gtk_ui_manager_new ();
    gtk_ui_manager_insert_action_group (m_UIManager, actionGroup, 0);

    GError *error = NULL;
    if ( !gtk_ui_manager_add_ui_from_file (m_UIManager,
                                           DATADIR"/ui/epdfview-ui.xml",
                                           &error) )
    {
        g_critical (_("Error building UI manager: %s\n"), error->message);
        g_error_free (error);
        exit (EXIT_FAILURE);
    }

#if defined (HAVE_CUPS)
    if ( !gtk_ui_manager_add_ui_from_file (m_UIManager,
                                           DATADIR"/ui/epdfview-ui-print.xml",
                                           &error) )
    {
        g_critical (_("Error building UI manager: %s\n"), error->message);
        g_error_free (error);
        exit (EXIT_FAILURE);
    }
#endif // HAVE_CUPS

    // Set the previous and next pages tool items important, so they will
    // be shown when the toolbar's style is GTK_TOOLBAR_BOTH_HORIZ.
    GtkWidget *prevPage = gtk_ui_manager_get_widget (m_UIManager,
                                  "/ToolBar/GoToPreviousPage");
    gtk_tool_item_set_is_important (GTK_TOOL_ITEM (prevPage), TRUE);
    GtkWidget *nextPage = gtk_ui_manager_get_widget (m_UIManager, 
                                  "/ToolBar/GoToNextPage");
    gtk_tool_item_set_is_important (GTK_TOOL_ITEM (nextPage), TRUE);
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
    for ( int iconIndex = 0 ; iconIndex < iconFilesNum ; iconIndex++ )
    {
        gchar *filename = g_strconcat (DATADIR, "/pixmaps/", 
                                       iconFiles[iconIndex], NULL);
        GError *error = NULL;
        GdkPixbuf *iconPixbuf = gdk_pixbuf_new_from_file (filename, &error);
        if ( NULL != iconPixbuf )
        {
            iconList = g_list_prepend (iconList, iconPixbuf);
        }
        else
        {
            g_warning ("Error loading icon: %s\n", error->message);
            g_error_free (error);
        }
        g_free (filename);
    }
    gtk_window_set_default_icon_list (iconList);
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
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
    gtk_clipboard_set_text(clipboard, text, -1);
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
    const gchar *authors[] = {
        "Jordi Fita <jordi@emma-soft.com>",
        NULL
    };

    const gchar *comments = _("A lightweight PDF viewer");

    const gchar *license[] = {
        N_("ePDFView is free software; you can redistribute it and/or modify\n"
           "it under the terms of the GNU General Public License as published "
           "by\nthe Free Software Foundation; either version 2 of the "
           "License, or\n(at your option) any later version.\n"),
        N_("ePDFView is distributes in the hope that it will be useful,\n"
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
    gtk_about_dialog_set_url_hook (main_window_about_box_url_hook, NULL, NULL);
    gtk_show_about_dialog (NULL,
            "name", _("ePDFView"),
            "version", VERSION,
            "copyright", "\xc2\xa9 2006, 2007, 2009, 2010, 2011 Emma's Software",
            "license", licenseTranslated,
            "website", "http://www.emma-soft.com/projects/epdfview/",
            "authors", authors,
            "comments", comments,
            "translator-credits", _("translator-credits"),
            NULL);

    g_free (licenseTranslated);
}


void
main_window_about_box_url_hook (GtkAboutDialog *about, const gchar *link,
                                gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->aboutBoxLinkActivated (link);
}

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
main_window_fullscreen_cb (GtkToggleAction *action, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->fullScreenActivated (gtk_toggle_action_get_active (action));
}

///
/// @brief Called when the window is moved or resized.
///
/// This is used to save the current window's position and size.
///
gboolean
main_window_moved_or_resized_cb (GtkWidget *widget, GdkEventConfigure *event, 
                                 gpointer data)
{
    // First set the window's size and position to the configuration.
    GtkWindow *m_MainWindow = GTK_WINDOW (widget);
    Config &config = Config::getConfig ();
    config.setWindowSize (event->width, event->height);
    gint x;
    gint y;
    gtk_window_get_position (m_MainWindow, &x, &y);
    config.setWindowPos (x, y);

    return FALSE;
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
    gtk_main_quit ();
}

///
/// @brief Called when the user clicks on the "Show Index" action.
///
void
main_window_show_index_cb (GtkToggleAction *action, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->showIndexActivated (gtk_toggle_action_get_active (action));
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

///
/// @brief Called when the user clicks on the "Show Statusbar" action.
///
void
main_window_show_statusbar_cb (GtkToggleAction *action, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->showStatusbarActivated (gtk_toggle_action_get_active (action));
}

///
/// @brief Called when the user clicks on the "Show Toolbar" action.
///
void
main_window_show_toolbar_cb (GtkToggleAction *action, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->showToolbarActivated (gtk_toggle_action_get_active (action));
}

///
/// @brief The user tries to set a zoom.
///
void
main_window_zoom_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomActivated ();
}

///
/// @brief The user tries to fit the document into the window.
///
void
main_window_zoom_fit_cb (GtkToggleAction *action, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomFitActivated (gtk_toggle_action_get_active (action));
}

///
/// @brief The user tries to expand the document.
///
void
main_window_zoom_in_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomInActivated ();
}

///
/// @brief The user tries to shrink the document.
///
void
main_window_zoom_out_cb (GtkWidget *widget, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomOutActivated ();
}

///
/// @brief The user tries to fit the document in the window's width.
///
void
main_window_zoom_width_cb (GtkToggleAction *action, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    pter->zoomWidthActivated (gtk_toggle_action_get_active (action));
}

void
main_window_set_page_mode (GtkRadioAction *action, GtkRadioAction *current, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    PagePterMode mode = (PagePterMode)gtk_radio_action_get_current_value(action);
    pter->setPageMode (mode);
}

gboolean 
main_window_page_scrolled_cb (GtkWidget *widget, GdkEventScroll *event, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    MainPter *pter = (MainPter *)data;
    // Only zoom when the CTRL-Button is down...
    if ( GDK_CONTROL_MASK == (event->state & GDK_CONTROL_MASK) )
    {
        if ( event->direction == GDK_SCROLL_UP )
        {
            pter->zoomInActivated ();
        }
        else if ( event->direction == GDK_SCROLL_DOWN )
        {
            pter->zoomOutActivated ();
        }
        return TRUE;
    }
    return FALSE;
}

