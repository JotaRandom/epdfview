// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Dumb Test Find View.
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

#if !defined (__DUMB_FIND_VIEW_H__)
#define __DUMB_FIND_VIEW_H__

namespace ePDFView
{
    class DumbFindView: public IFindView
    {
        public:
            DumbFindView (void);
            ~DumbFindView (void);

            const gchar *getTextToFind (void);
            void hide (void);
            void sensitiveFindNext (gboolean sensitive);
            void sensitiveFindPrevious (gboolean sensitive);
            void setInformationText (const gchar *text);

            // Methods for test only purposes.
            const gchar *getInformationText (void);
            gboolean isFindNextSensitive (void);
            gboolean isFindPreviousSensitive (void);
            void setTextToFind (const gchar *text);

        protected:
            gboolean m_FindNextSensitive;
            gboolean m_FindPreviousSensitive;
            gchar *m_InformationText;
            gchar *m_TextToFind;
    };
}

#endif // !__DUMB_FIND_VIEW_H__
