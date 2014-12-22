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

#if !defined (__FIND_PTER_H__)
#define __FIND_PTER_H__

namespace ePDFView
{
    // Forward declarations.
    class JobFind;

    ///
    /// @class FindPter
    /// @brief Find bar presenter.
    ///
    /// This presenter is the controlling class of the find bar and
    /// the find next / previous functionality.
    ///
    class FindPter
    {
        public:
            FindPter (IDocument *document);
            ~FindPter (void);

            void closeActivated (void);
            void findNextActivated (void);
            void findPreviousActivated (void);
            IFindView &getView (void);
            void notifyFindFinished (gboolean endOfSearch);
            void notifyFindResults (gint pageNum, GList *results,
                                    FindDirection direction);
            void setView (IFindView *view);
            void textToFindChanged (void);

        protected:
            /// The current selected match from m_FindResults.
            GList *m_CurrentMatch;
            /// The document to search to.
            IDocument *m_Document;
            /// The current page that we found something.
            gint m_FindPage;
            /// The results on m_FindPage.
            GList *m_FindResults;
            /// The current find job.
            JobFind *m_Job;
            /// The view that the presenter is controlling.
            IFindView *m_View;

            void cancelJob (void);
            void freeFindResults (void);
    };
}

#endif // !__FIND_PTER_H__
