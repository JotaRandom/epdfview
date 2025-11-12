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
#include "PreferencesView.h"

using namespace ePDFView;

// Callbacks
static void preferences_view_browser_command_changed (GtkEntry *, gpointer);
static void preferences_view_backsearch_command_changed (GtkEntry *, gpointer);

PreferencesView::PreferencesView (GtkWindow *parent):
    IPreferencesView ()
{
    m_ResponseHandlerId = 0;
    m_CloseRequestHandlerId = 0;
    
    // GTK4: Stock items removed, use text labels
    m_PreferencesDialog = gtk_dialog_new_with_buttons (_("Preferences"),
													   parent, GTK_DIALOG_MODAL,
													   _("_Close"), GTK_RESPONSE_CLOSE,
            NULL);
    // GTK4: gtk_window_set_skip_taskbar_hint removed
    // Utility dialogs are automatically excluded from taskbar in GTK4

    GtkWidget *notebook = gtk_notebook_new ();
    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (m_PreferencesDialog));
    gtk_box_append (GTK_BOX (content_area), notebook);
    gtk_widget_set_margin_start (notebook, 6);
    gtk_widget_set_margin_end (notebook, 6);
    gtk_widget_set_margin_top (notebook, 6);
    gtk_widget_set_margin_bottom (notebook, 6);

    // Add the notebook's tabs.
    {
        GtkWidget *label = gtk_label_new (_("External Commands"));
        gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
                                  createExternalCommandsTab (), label);
    }
    // In GTK4, widgets are visible by default
}

PreferencesView::~PreferencesView ()
{
    // GTK4: Disconnect signal handlers before destroying window
    if (m_PreferencesDialog && GTK_IS_WINDOW (m_PreferencesDialog))
    {
        if (m_ResponseHandlerId > 0)
        {
            g_signal_handler_disconnect (m_PreferencesDialog, m_ResponseHandlerId);
            m_ResponseHandlerId = 0;
        }
        if (m_CloseRequestHandlerId > 0)
        {
            g_signal_handler_disconnect (m_PreferencesDialog, m_CloseRequestHandlerId);
            m_CloseRequestHandlerId = 0;
        }
        gtk_window_destroy (GTK_WINDOW (m_PreferencesDialog));
        m_PreferencesDialog = NULL;
    }
}

void
PreferencesView::setPresenter (PreferencesPter *pter)
{
    IPreferencesView::setPresenter (pter);

    // Connect the signals.
    g_signal_connect (G_OBJECT (m_BrowserCommandLine), "changed",
                      G_CALLBACK (preferences_view_browser_command_changed),
                      pter);
    g_signal_connect (G_OBJECT (m_BacksearchCommandLine), "changed",
                      G_CALLBACK (preferences_view_backsearch_command_changed),
                      pter);
    // Run the dialog.
    // GTK4: Use modal presentation with async callback - NO nested loops!
    gtk_window_set_modal(GTK_WINDOW(m_PreferencesDialog), TRUE);
    
    // GTK4: Connect signals using presenter as user_data
    // Store handler IDs to disconnect them before destruction
    m_ResponseHandlerId = g_signal_connect(m_PreferencesDialog, "response",
                    G_CALLBACK(+[](GtkDialog *, int, gpointer user_data) {
                        PreferencesPter *pter = static_cast<PreferencesPter*>(user_data);
                        pter->closeActivated();
                    }), pter);
    
    m_CloseRequestHandlerId = g_signal_connect(m_PreferencesDialog, "close-request",
                    G_CALLBACK(+[](GtkWindow *, gpointer user_data) -> gboolean {
                        PreferencesPter *pter = static_cast<PreferencesPter*>(user_data);
                        pter->closeActivated();
                        return TRUE; // Prevent default close - handled by response signal
                    }), pter);
    
    gtk_window_present(GTK_WINDOW(m_PreferencesDialog));

    
    // Do NOT run a nested main loop - let GTK4 handle events naturally
    // The callbacks above will be called when the user interacts with the dialog
}

const gchar *
PreferencesView::getBrowserCommandLine ()
{
    return gtk_editable_get_text (GTK_EDITABLE (m_BrowserCommandLine));
}

const gchar *
PreferencesView::getBacksearchCommandLine ()
{
    return gtk_editable_get_text (GTK_EDITABLE (m_BacksearchCommandLine));
}


////////////////////////////////////////////////////////////////
// Tab Creators
////////////////////////////////////////////////////////////////

GtkWidget *
PreferencesView::createExternalCommandsTab ()
{
    // GTK4: Use GtkGrid instead of GtkTable and manual margins instead of GtkAlignment
    GtkWidget *grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (grid), 3);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 12);
    gtk_widget_set_margin_start (grid, 12);
    gtk_widget_set_margin_end (grid, 6);
    gtk_widget_set_margin_top (grid, 6);
    gtk_widget_set_margin_bottom (grid, 0);

    m_BrowserCommandLine = gtk_entry_new ();
    GtkWidget *webBrowserLabel = gtk_label_new (_("Web_Browser:"));
    gtk_widget_set_halign (webBrowserLabel, GTK_ALIGN_END);
    gtk_widget_set_valign (webBrowserLabel, GTK_ALIGN_CENTER);
    gtk_label_set_use_markup (GTK_LABEL (webBrowserLabel), TRUE);
    gtk_label_set_use_underline (GTK_LABEL (webBrowserLabel), TRUE);
    gtk_label_set_mnemonic_widget (GTK_LABEL (webBrowserLabel),
                                   m_BrowserCommandLine);
    gtk_grid_attach (GTK_GRID (grid), webBrowserLabel, 0, 0, 1, 1);
    
    // The web browser.
    gchar *browserCommandLine =
        Config::getConfig ().getExternalBrowserCommandLine ();
    gtk_editable_set_text (GTK_EDITABLE (m_BrowserCommandLine), browserCommandLine);
    g_free (browserCommandLine);
    gtk_widget_set_hexpand (m_BrowserCommandLine, TRUE);
    gtk_grid_attach (GTK_GRID (grid), m_BrowserCommandLine, 1, 0, 1, 1);


    // The %s note.
    GtkWidget *note = gtk_label_new (_("Note: <i>%s</i> will be replaced by the URI."));
    gtk_widget_set_halign (note, GTK_ALIGN_START);
    gtk_widget_set_valign (note, GTK_ALIGN_CENTER);
    gtk_label_set_use_markup (GTK_LABEL (note), TRUE);
    gtk_widget_set_margin_bottom (note, 12);
    gtk_grid_attach (GTK_GRID (grid), note, 0, 1, 2, 1);

	//
	// Back-searching (through SyncTeX)
	//
    m_BacksearchCommandLine = gtk_entry_new ();
    GtkWidget *backsearchLabel = gtk_label_new (_("SyncTeX script:"));
    gtk_widget_set_halign (backsearchLabel, GTK_ALIGN_END);
    gtk_widget_set_valign (backsearchLabel, GTK_ALIGN_CENTER);
    gtk_label_set_use_markup (GTK_LABEL (backsearchLabel), TRUE);
    gtk_label_set_use_underline (GTK_LABEL (backsearchLabel), TRUE);
    gtk_label_set_mnemonic_widget (GTK_LABEL (backsearchLabel),
                                   m_BacksearchCommandLine);
    gtk_grid_attach (GTK_GRID (grid), backsearchLabel, 0, 2, 1, 1);
    
    gchar *backsearchCommandLine =
        Config::getConfig ().getExternalBacksearchCommandLine ();
    gtk_editable_set_text (GTK_EDITABLE (m_BacksearchCommandLine), backsearchCommandLine);
    g_free (backsearchCommandLine);
    gtk_widget_set_hexpand (m_BacksearchCommandLine, TRUE);
    gtk_grid_attach (GTK_GRID (grid), m_BacksearchCommandLine, 1, 2, 1, 1);


    // The meaning of %p %x %y %f
    GtkWidget *note2 = gtk_label_new (_("Note: <i>%p</i>=page <i>%x,%y</i>=coordinates <i>%f</i>=PDF file"));
    gtk_widget_set_halign (note2, GTK_ALIGN_START);
    gtk_widget_set_valign (note2, GTK_ALIGN_CENTER);
    gtk_label_set_use_markup (GTK_LABEL (note2), TRUE);
    gtk_widget_set_margin_bottom (note2, 12);
    gtk_grid_attach (GTK_GRID (grid), note2, 0, 3, 2, 1);


    return grid;
}

////////////////////////////////////////////////////////////////
// Callbacks
////////////////////////////////////////////////////////////////
void
preferences_view_browser_command_changed (GtkEntry *entry, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    PreferencesPter *pter = (PreferencesPter *)data;
    pter->browserCommandLineChanged ();
}

void
preferences_view_backsearch_command_changed (GtkEntry *entry, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    PreferencesPter *pter = (PreferencesPter *)data;
    pter->backsearchCommandLineChanged ();
}
