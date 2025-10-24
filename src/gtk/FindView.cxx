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
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <epdfview.h>
#include "StockIcons.h"
#include "FindView.h"

// GTK4 direct API usage - no compatibility layer needed

using namespace ePDFView;

// Forwards declarations.
static void find_view_close_cb (GtkWidget *, gpointer);
static void find_view_next_cb (GtkWidget *, gpointer);
static void find_view_previous_cb (GtkWidget *, gpointer);
static void find_view_text_to_find_activate_cb (GtkEntry *, gpointer);
static void find_view_text_to_find_changed_cb (GtkEntry *, gpointer);
static gboolean find_view_text_to_find_key_press_cb (GtkEventControllerKey *, guint, guint, GdkModifierType, gpointer);

FindView::FindView ():
    IFindView ()
{
    // GTK4: Replace GtkToolbar with GtkBox (toolbars deprecated)
    m_FindBar = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_margin_start (m_FindBar, 6);
    gtk_widget_set_margin_end (m_FindBar, 6);
    gtk_widget_set_margin_top (m_FindBar, 6);
    gtk_widget_set_margin_bottom (m_FindBar, 6);

    // The "Close" button.
    m_Close = gtk_button_new_from_icon_name (GTK_STOCK_CLOSE);
    gtk_box_append (GTK_BOX (m_FindBar), m_Close);

    // The text to find entry.
    GtkWidget *findLabel = gtk_label_new (_("Find:"));
    gtk_box_append (GTK_BOX (m_FindBar), findLabel);
    
    m_TextToFind = gtk_entry_new ();
    gtk_widget_set_hexpand (m_TextToFind, TRUE);
    gtk_box_append (GTK_BOX (m_FindBar), m_TextToFind);

    // The "Find Next" button.
    m_FindNext = gtk_button_new_from_icon_name (EPDFVIEW_STOCK_FIND_NEXT);
    gtk_button_set_label (GTK_BUTTON (m_FindNext), _("Next"));
    gtk_box_append (GTK_BOX (m_FindBar), m_FindNext);

    // The "Find Previous" button.
    m_FindPrevious = gtk_button_new_from_icon_name (EPDFVIEW_STOCK_FIND_PREVIOUS);
    gtk_button_set_label (GTK_BUTTON (m_FindPrevious), _("Previous"));
    gtk_box_append (GTK_BOX (m_FindBar), m_FindPrevious);

    // A separator.
    GtkWidget *separator = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
    gtk_box_append (GTK_BOX (m_FindBar), separator);

    // The information text.
    m_InformationText = gtk_label_new ("");
    gtk_widget_set_hexpand (m_InformationText, TRUE);
    gtk_box_append (GTK_BOX (m_FindBar), m_InformationText);
}

FindView::~FindView ()
{
}

const gchar *
FindView::getTextToFind ()
{
    return gtk_entry_get_text (GTK_ENTRY (m_TextToFind));
}

void
FindView::hide ()
{
    // GTK4: Accel map removed, accelerators handled differently
    gtk_editable_set_text (GTK_EDITABLE (m_TextToFind), "");
    gtk_widget_set_visible (m_FindBar, FALSE);
}

void
FindView::sensitiveFindNext (gboolean sensitive)
{
    gtk_widget_set_sensitive (GTK_WIDGET (m_FindNext), sensitive);
}

void
FindView::sensitiveFindPrevious (gboolean sensitive)
{
    gtk_widget_set_sensitive (GTK_WIDGET (m_FindPrevious), sensitive);
}

void
FindView::setInformationText (const gchar *text)
{
    gtk_label_set_text (GTK_LABEL (m_InformationText), text);
}

void
FindView::setPresenter (FindPter *pter)
{
    FindPter *oldPter = getPresenter ();
    if ( NULL != oldPter )
    {
        g_signal_handlers_disconnect_by_func (G_OBJECT (m_TextToFind),
                (gpointer) find_view_text_to_find_changed_cb, oldPter);
        g_signal_handlers_disconnect_by_func (G_OBJECT (m_TextToFind),
                (gpointer) find_view_text_to_find_activate_cb, oldPter);
        g_signal_handlers_disconnect_by_func (G_OBJECT (m_TextToFind),
                (gpointer) find_view_text_to_find_key_press_cb, oldPter);
        g_signal_handlers_disconnect_by_func (G_OBJECT (m_FindNext),
                (gpointer) find_view_next_cb, oldPter);
        g_signal_handlers_disconnect_by_func (G_OBJECT (m_FindPrevious),
                (gpointer) find_view_previous_cb, oldPter);
        g_signal_handlers_disconnect_by_func (G_OBJECT (m_Close),
                (gpointer) find_view_close_cb, oldPter);
    }

    IFindView::setPresenter (pter);

    // GTK4: Accel map removed
    // Widgets are visible by default
    gtk_widget_set_visible (m_FindBar, TRUE);
    gtk_widget_grab_focus (m_TextToFind);

    g_signal_connect (G_OBJECT (m_TextToFind), "changed",
                      G_CALLBACK (find_view_text_to_find_changed_cb), pter);
    g_signal_connect (G_OBJECT (m_TextToFind), "activate",
                      G_CALLBACK (find_view_text_to_find_activate_cb), pter);
    
    // GTK4 uses event controllers for key events
    GtkEventController *key_controller = gtk_event_controller_key_new();
    gtk_widget_add_controller(m_TextToFind, key_controller);
    g_signal_connect(key_controller, "key-pressed",
                    G_CALLBACK(find_view_text_to_find_key_press_cb), pter);
    g_signal_connect (G_OBJECT (m_FindNext), "clicked",
                      G_CALLBACK (find_view_next_cb), pter);
    g_signal_connect (G_OBJECT (m_FindPrevious), "clicked",
                      G_CALLBACK (find_view_previous_cb), pter);
    g_signal_connect (G_OBJECT (m_Close), "clicked",
                      G_CALLBACK (find_view_close_cb), pter);
}

////////////////////////////////////////////////////////////////
// GTK+ Functions.
////////////////////////////////////////////////////////////////

GtkWidget *
FindView::getTopWidget ()
{
    return m_FindBar;
}

////////////////////////////////////////////////////////////////
// Callbacks
////////////////////////////////////////////////////////////////

void
find_view_close_cb (GtkWidget *widget, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    FindPter *pter = (FindPter *)data;
    pter->closeActivated ();
}

void
find_view_next_cb (GtkWidget *widget, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    FindPter *pter = (FindPter *)data;
    pter->findNextActivated ();
}
    
void
find_view_previous_cb (GtkWidget *widget, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    FindPter *pter = (FindPter *)data;
    pter->findPreviousActivated ();
}

void
find_view_text_to_find_activate_cb (GtkEntry *entry, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    FindPter *pter = (FindPter *)data;
    pter->findNextActivated ();
}

void
find_view_text_to_find_changed_cb (GtkEntry *entry, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    FindPter *pter = (FindPter *)data;
    pter->textToFindChanged ();
}

gboolean
find_view_text_to_find_key_press_cb (GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    if (GDK_KEY_Escape == keyval)
    {
        FindPter *pter = (FindPter *)data;
        pter->cancelJob ();
        return TRUE;
    }
    return FALSE;
}
