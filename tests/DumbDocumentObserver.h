// ePDFView - Dumb Test Document Observer.
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

#if !defined (__DUMB_DOCUMENT_OBSERVER_H__)
#define __DUMB_DOCUMENT_OBSERVER_H__

namespace ePDFView
{
    class DumbDocumentObserver: public IDocumentObserver
    {
        public:
            DumbDocumentObserver (void);
            ~DumbDocumentObserver (void);

            void notifyFindChanged (DocumentRectangle *matchRect);
            void notifyFindFinished (void);
            void notifyFindStarted (void);
            void notifyLoad (void);
            void notifyLoadError (const GError *error);
            void notifyLoadPassword (const gchar *fileName, gboolean reload,
                                     const GError *error);
            void notifyPageChanged (gint pageNum);
            void notifyPageRotated (gint rotation);
            void notifyPageZoomed (gdouble zoom);
            void notifyReload (void);

            // Functions for test only purposes.
            gint getCurrentPage (void);
            DocumentRectangle *getFindMatchRect (void);
            const GError *getLoadError (void);
            gdouble getZoom (void);
            gboolean isStillSearching (void);
            gboolean loadFinished (void);
            gboolean notifiedError (void);
            gboolean notifiedLoaded (void);
            gboolean notifiedPassword (void);
            gboolean notifiedRotation (void);
            gboolean notifiedZoom (void);
            void setLoadError (const GError *error);

        protected:
            gint m_CurrentPage;
            GError *m_Error;
            DocumentRectangle *m_FindMatchRect;
            gboolean m_NotifiedError;
            gboolean m_NotifiedLoad;
            gboolean m_NotifiedPassword;
            gboolean m_NotifiedPageRotated;
            gboolean m_NotifiedPageZoomed;
            gboolean m_NotifiedReload;
            volatile gboolean m_Searching;
            gdouble m_Zoom;
    };
}

#endif // !__DUMB_DOCUMENT_OBSERVER_H__
