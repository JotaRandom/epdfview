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

namespace
{
    const gchar *g_SlashAccelPath = "<Actions>/ePDFView/Slash";
}

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
    gtk_accel_map_lookup_entry(g_SlashAccelPath, &m_SlashAccelKey);

    m_FindBar = gtk_toolbar_new ();
    // Set the toolbar style to horizontal, so most toolbuttons' labels won't be
    // shown.
    gtk_toolbar_set_style (GTK_TOOLBAR (m_FindBar), GTK_TOOLBAR_BOTH_HORIZ);

    // The "Close" button.
    m_Close = gtk_tool_button_new (NULL, NULL);
    gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (m_Close), GTK_STOCK_CLOSE);
    gtk_toolbar_insert (GTK_TOOLBAR (m_FindBar), m_Close, -1);

    // The text to find entry.
    GtkWidget *textToFindBox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_pack_start (GTK_BOX (textToFindBox), gtk_label_new (_("Find:")),
                        FALSE, FALSE, 0);
    m_TextToFind = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (textToFindBox), m_TextToFind, TRUE, TRUE, 0);

    GtkToolItem *textToFindItem = gtk_tool_item_new ();
    gtk_tool_item_set_child (GTK_TOOL_ITEM (textToFindItem), textToFindBox);
    gtk_toolbar_insert (GTK_TOOLBAR (m_FindBar), textToFindItem, -1);

    // The "Find Next" button.
    m_FindNext = gtk_tool_button_new (NULL, NULL);
    gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (m_FindNext), EPDFVIEW_STOCK_FIND_NEXT);
    gtk_tool_item_set_is_important (m_FindNext, TRUE);
    gtk_toolbar_insert (GTK_TOOLBAR (m_FindBar), m_FindNext, -1);

    // The "Find Previous" button.
    m_FindPrevious = gtk_tool_button_new (NULL, NULL);
    gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (m_FindPrevious), EPDFVIEW_STOCK_FIND_PREVIOUS);
    gtk_tool_item_set_is_important (m_FindPrevious, TRUE);
    gtk_toolbar_insert (GTK_TOOLBAR (m_FindBar), m_FindPrevious, -1);

    // A separator.
    gtk_toolbar_insert (GTK_TOOLBAR (m_FindBar), gtk_separator_tool_item_new (),
                        -1);

    // The information text.
    m_InformationText = gtk_label_new ("");
    GtkToolItem *informationTextItem = gtk_tool_item_new ();
    gtk_tool_item_set_child (GTK_TOOL_ITEM (informationTextItem), m_InformationText);
    gtk_toolbar_insert (GTK_TOOLBAR (m_FindBar), informationTextItem, -1);
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
    // Restore the slash accelerator's value when no longer searching.
    gtk_accel_map_change_entry(g_SlashAccelPath, m_SlashAccelKey.accel_key,
            m_SlashAccelKey.accel_mods, false);
    gtk_entry_set_text (GTK_ENTRY (m_TextToFind), "");
    gtk_widget_hide (m_FindBar);
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

    // Map the slash ("/") accelerator to nothing, in order to be able
    // to add an actual slash to the search text.
    gtk_accel_map_change_entry(g_SlashAccelPath, 0,
            m_SlashAccelKey.accel_mods, false);

    // In GTK4, widgets are visible by default - no need for gtk_widget_show_all
    gtk_widget_grab_focus (m_TextToFind);

    g_signal_connect (G_OBJECT (m_TextToFind), "changed",
                      G_CALLBACK (find_view_text_to_find_changed_cb), pter);
    g_signal_connect (G_OBJECT (m_TextToFind), "activate",
                      G_CALLBACK (find_view_text_to_find_activate_cb), pter);
    
#ifdef GTK_4_0
    // GTK4 uses event controllers for key events
    GtkEventController *key_controller = gtk_event_controller_key_new();
    gtk_widget_add_controller(m_TextToFind, key_controller);
    g_signal_connect(key_controller, "key-press",
                    G_CALLBACK(find_view_text_to_find_key_press_cb), pter);
#else
    // For GTK3 and earlier, use traditional event connections
    g_signal_connect (G_OBJECT (m_TextToFind), "key-press-event",
                      G_CALLBACK (find_view_text_to_find_key_press_cb), pter);
#endif
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
