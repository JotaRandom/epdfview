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

#include "epdfview.h"

using namespace ePDFView;

G_LOCK_DEFINE_STATIC (JobFindCancel);
G_LOCK_DEFINE_STATIC (JobFindEnqueued);

// Forward declarations.
static gboolean job_find_end (gpointer data);
static gboolean job_find_results (gpointer data);

///
/// @brief Constructs a new JobFind object.
///
JobFind::JobFind ()
{
    m_Canceled = FALSE;
    m_CurrentPage = 0;
    m_Direction = FIND_DIRECTION_FORWARDS;
    m_Document = NULL;
    m_Enqueued = FALSE;
    m_FindPter = NULL;
    m_Results = NULL;
    m_ResultsPage = 0;
    m_StartingPage = 0;
    m_TextToFind = g_strdup ("");
}

///
/// @brief Deletes all dynamically allocated memory for JobFind.
///
JobFind::~JobFind ()
{
    g_free (m_TextToFind);
}

///
/// @brief Cancels the job.
///
/// The next time the job enters the queue, it won't do anything and
/// will be deleted.
///
void
JobFind::cancel ()
{
    G_LOCK (JobFindCancel);
    m_Canceled = TRUE;
    G_UNLOCK (JobFindCancel);
}

///
/// @brief Tells that the job is enqueued.
///
/// This function doesn't removes the job from the queue, just tells the
/// job that it's dequeued.
///
void
JobFind::dequeue ()
{
    G_LOCK (JobFindEnqueued);
    m_Enqueued = FALSE;
    G_UNLOCK (JobFindEnqueued);
}

///
/// @brief Enqueues the job to the queue of jobs to run.
///
/// This function enqueues itself to the queue of jobs to run by
/// calling the IJob::enqueue() function iif the job is not already
/// enqueued.
///
void
JobFind::enqueue ()
{
    G_LOCK (JobFindEnqueued);
    if ( !isEnqueued () )
    {
        m_Enqueued = TRUE;
        IJob::enqueue (this);
    }
    G_UNLOCK (JobFindEnqueued);
}

///
/// @brief Gets the page to look the text at.
///
/// @return The page to search for the text.
///
gint
JobFind::getCurrentPage (void)
{
    return m_CurrentPage;
}

///
/// @brief Gets the direction to continue searching.
///
/// This function will tell if check for the next or the previous page the
/// next time the job is added in the queue.
///
/// @return FIND_DIRECTION_FORWARD to look at the next page or
///         FIND_DIRECTION_BACKWARDS to look at the previous page.
///
FindDirection
JobFind::getDirection ()
{
    return m_Direction;
}

///
/// @brief Gets the document to look for the text at.
///
/// @return The document to search the text on.
///
IDocument *
JobFind::getDocument ()
{
    return m_Document;
}

///
/// @brief Gets the presenter to notify when the find changes.
///
/// @return The FindPter to notify about find state's changes.
///
FindPter *
JobFind::getFindPter ()
{
    return m_FindPter;
}

///
/// @brief The search's last results.
///
/// @return The list of results from a page.
///
GList *
JobFind::getResults ()
{
    return m_Results;
}

///
/// @brief Gets the page number the results belongs to.
///
/// @return The number of the page the list of results returned by
///         getResults() belongs to.
///
gint
JobFind::getResultsPage ()
{
    return m_ResultsPage;
}

///
/// @brief Gets the first page that has been checked.
///
/// @return The number of the first page where the find started.
///
gint
JobFind::getStartingPage ()
{
    return m_StartingPage;
}

///
/// @brief Gets the text to find in the document.
///
/// @return The text to find.
///
const gchar *
JobFind::getTextToFind ()
{
    return m_TextToFind;
}

///
/// @brief Checks if the job is canceled.
///
/// @return TRUE if the job is canceled, FALSE otherwise.
///
gboolean
JobFind::isCanceled ()
{
    G_LOCK (JobFindCancel);
    gboolean canceled = m_Canceled;
    G_UNLOCK (JobFindCancel);

    return canceled;
}

///
/// @brief Checks if the job is enqueued.
///
/// @return TRUE if the job is enqueued, FALSE otherwise.
///
gboolean
JobFind::isEnqueued ()
{
    return m_Enqueued;
}

gboolean
JobFind::run ()
{
    gboolean canceled = isCanceled ();
    if ( !canceled )
    {
        gint currentPage = getCurrentPage ();
        GList *result = getDocument ()->findTextInPage (currentPage,
                                                        getTextToFind ());

        if ( FIND_DIRECTION_FORWARDS == getDirection () )
        {
            setCurrentPage (currentPage + 1);
        }
        else
        {
            setCurrentPage (currentPage - 1);
        }

        if ( NULL != result )
        {
            setResults (currentPage, result);
            JOB_NOTIFIER (job_find_results, this);
        }
        else
        {
            JOB_NOTIFIER (job_find_end, this);
        }
    }

    // Delete the job iif it's cancelled.
    return canceled;
}

///
/// @brief Sets the current page to find the text to.
///
/// @param pageNum The number of the page to set as the current page.
///
void
JobFind::setCurrentPage (gint pageNum)
{
    m_CurrentPage = pageNum;

    gint numPages = m_Document->getNumPages ();
    if ( m_CurrentPage > numPages )
    {
        m_CurrentPage = 1;
    }
    else if ( 1 > m_CurrentPage )
    {
        m_CurrentPage = numPages;
    }
}

///
/// @brief Sets the direction to continue the find.
///
/// @param direction The direction to continue the find.
///
void
JobFind::setDirection (FindDirection direction)
{
    m_Direction = direction;
}

///
/// @brief Sets the document to find the text to.
///
/// @param document The document to search for the text to.
///
void
JobFind::setDocument (IDocument *document)
{
    g_assert (NULL != document && "Trying to set a NULL document.");

    m_Document = document;
}

///
/// @brief Sets the presenter that will receive the notifications.
///
/// @param pter The presenter that will receive the notifications of changes.
///
void
JobFind::setFindPter (FindPter *pter)
{
    g_assert (NULL != pter && "Tried to set a NULL presenter.");

    m_FindPter = pter;
}

///
/// @brief Sets the find's results.
///
/// @param pageNum The number of the page the results belongs to.
/// @param results The results of the search of the text in @a pageNum.
///
void
JobFind::setResults (gint pageNum, GList *results)
{
    g_assert (NULL != results && "Tried to set NULL results.");

    m_Results = results;
    m_ResultsPage = pageNum;
}

///
/// @brief Sets the find's starting page.
///
/// @param pageNum The number of the page where the find starts.
///
void
JobFind::setStartingPage (gint pageNum)
{
    m_StartingPage = pageNum;
    setCurrentPage (pageNum);
}

///
/// @brief Sets the text to find.
///
/// @param textToFind The text to find in the document.
///
void
JobFind::setTextToFind (const gchar *textToFind)
{
    g_free (m_TextToFind);
    m_TextToFind = g_strdup (textToFind);
}

////////////////////////////////////////////////////////////////
// Static threaded functions.
////////////////////////////////////////////////////////////////

gboolean
job_find_end (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobFind *job = (JobFind *)data;
    gboolean endOfSearch = job->getStartingPage () == job->getCurrentPage ();
    job->dequeue ();
    job->getFindPter ()->notifyFindFinished (endOfSearch);

    return FALSE;
}

///
/// @brief The last checked page had results.
///
/// This is called when the recently checked page had results.
///
/// @param data A pointer to the job that called this callback.
///
gboolean
job_find_results (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobFind *job = (JobFind *)data;    
    job->dequeue ();
    job->getFindPter ()->notifyFindResults (job->getResultsPage (),
                                            job->getResults (),
                                            job->getDirection ());

    return FALSE;
}
