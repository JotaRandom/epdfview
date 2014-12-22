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
    class FindView: public IFindView
    {
        public:
            FindView (void);
            ~FindView (void);

            const gchar *getTextToFind (void);
            GtkWidget *getTopWidget (void);
            void hide (void);
            void sensitiveFindNext (gboolean sensitive);
            void sensitiveFindPrevious (gboolean sensitive);
            void setInformationText (const gchar *text);
            void setPresenter (FindPter *pter);

        protected:
            GtkToolItem *m_Close;
            GtkWidget *m_FindBar;
            GtkToolItem *m_FindNext;
            GtkToolItem *m_FindPrevious;
            GtkWidget *m_InformationText;
            GtkWidget *m_TextToFind;
            GtkAccelKey m_SlashAccelKey;
    };
}

#endif // !__FIND_VIEW_H__
