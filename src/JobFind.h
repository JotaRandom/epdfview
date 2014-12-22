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

#if !defined (__JOB_FIND_H__)
#define __JOB_FIND_H__

namespace ePDFView
{
    // Forward declarations.
    class FindPter;

    ///
    /// @brief The search direction.
    ///
    typedef enum _FindDirection
    {
        /// Search forwards.
        FIND_DIRECTION_FORWARDS,
        /// Search backwards.
        FIND_DIRECTION_BACKWARDS
    } FindDirection;
   
    ///
    /// @class JobFind
    /// @brief A background job that finds text on the document.
    ///
    class JobFind: public IJob
    {
        public:
            JobFind (void);
            ~JobFind (void);
            
            void cancel (void);
            void dequeue (void);
            void enqueue (void);
            gint getCurrentPage (void);
            FindDirection getDirection (void);
            IDocument *getDocument (void);
            FindPter *getFindPter (void);
            GList *getResults (void);
            gint getResultsPage (void);
            gint getStartingPage (void);
            const gchar *getTextToFind (void);
            gboolean isCanceled (void);
            gboolean isEnqueued (void);
            gboolean run (void);
            void setCurrentPage (gint pageNum);
            void setDirection (FindDirection direction);
            void setDocument (IDocument *document);
            void setFindPter (FindPter *pter);
            void setResults (gint pageNum, GList *results);
            void setStartingPage (gint pageNum);
            void setTextToFind (const gchar *textToFind);

        protected:
            /// Tells if the job has been canceled.
            gboolean m_Canceled;
            /// The current page to search the text.
            gint m_CurrentPage;
            /// The next page to look for the text: the next or the previous.
            FindDirection m_Direction;
            /// The document to search the text from.
            IDocument *m_Document;
            /// Tells if the job is enqueued.
            gboolean m_Enqueued;
            /// The presenter to tell when a change happens.
            FindPter *m_FindPter;
            /// The search results of a page.
            GList *m_Results;
            /// The page number where m_Results belongs to.
            gint m_ResultsPage;
            /// The starting page of the search.
            gint m_StartingPage;
            /// The search to find on the document.
            gchar *m_TextToFind;
    };
}

#endif // !__JOB_FIND_H__
