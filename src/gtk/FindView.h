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

#if !defined (__FIND_VIEW_H__)
#define __FIND_VIEW_H__

namespace ePDFView
{
    // Forward declarations.
    class FindPter;

    class FindView: public IFindView
    {
        public:
            FindView ();
            ~FindView (void);

            const gchar *getTextToFind (void);
            void hide (void);
            void sensitiveFindNext (gboolean sensitive);
            void sensitiveFindPrevious (gboolean sensitive);
            void setInformationText (const gchar *text);
            void setPresenter (FindPter *pter);

            // GTK+ Functions.
            GtkWidget *getTopWidget (void);

        protected:
            // Member variables.
            GtkWidget *m_FindBar;
            GtkWidget *m_Close;
            GtkWidget *m_TextToFind;
            GtkWidget *m_FindNext;
            GtkWidget *m_FindPrevious;
            GtkWidget *m_InformationText;

            // Callbacks
            friend void find_view_close_cb (GtkWidget *widget, gpointer data);
            friend void find_view_next_cb (GtkWidget *widget, gpointer data);
            friend void find_view_previous_cb (GtkWidget *widget, gpointer data);
            friend void find_view_text_to_find_activate_cb (GtkEntry *entry, gpointer data);
            friend void find_view_text_to_find_changed_cb (GtkEntry *entry, gpointer data);
            friend gboolean find_view_text_to_find_key_press_cb (GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
    };
}

#endif // !__FIND_VIEW_H__
