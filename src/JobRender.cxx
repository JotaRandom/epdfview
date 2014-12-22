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

#include "epdfview.h"

using namespace ePDFView;

G_LOCK_DEFINE (JobRender);

/// Tells if we can render more pages or not. Used in test suites only.
gboolean JobRender::m_CanProcessJobs = TRUE;
/// This tells the minimum age the jobs must have in order to process them.
guint32 JobRender::m_MinAge = 0;

// Forwards declarations.
static gboolean job_render_done (gpointer data);

///
/// @brief Creates a new JobRender object.
///
JobRender::JobRender ():
    IJob ()
{
    m_Age = 0;
    m_Document = NULL;
    m_PageImage = NULL;
    m_PageNumber = 0;
}

///
/// @brief Deletes all dynamically allocated memory for JobRender.
///
JobRender::~JobRender()
{
}

///
/// @brief Renders a page.
///
gboolean
JobRender::run (void)
{
    G_LOCK (JobRender);
    IDocument *doc = getDocument ();
    if ( NULL != doc && m_MinAge <= getAge ())
    {
        m_PageImage = doc->renderPage (getPageNumber ()); 
        G_UNLOCK (JobRender);
        JOB_NOTIFIER (job_render_done, this);
        return JOB_DELETE;
    }
    G_UNLOCK (JobRender);
    return TRUE;
}

///
/// @brief Gets the job's age.
///
/// @return The age of the job.
///
guint32
JobRender::getAge ()
{
    return m_Age;
}

///
/// @brief Gets the document to get the page to render.
///
/// @return The document class to use to render the page or NULL if can't
///         process more jobs (test only.)
///
IDocument *
JobRender::getDocument ()
{
    if ( m_CanProcessJobs )
    {
        return m_Document;
    }
    else
    {
        return NULL;
    }
}

///
/// @brief Gets the rendered page image.
///
/// @return The rendered page image as returned by the document class.
///
DocumentPage *
JobRender::getPageImage ()
{
    return m_PageImage;
}

///
/// @brief Gets the page number to render.
///
/// @return The number of the page to render.
///
gint
JobRender::getPageNumber ()
{
    return m_PageNumber;
}

///
/// @brief Sets the job's age.
///
/// @param age The job's age.
///
void
JobRender::setAge (guint32 age)
{
    m_Age = age;
}

///
/// @brief Sets the document class to use to render.
///
/// @param document The document class to use to render the page.
///
void
JobRender::setDocument (IDocument *document)
{
    g_assert (NULL != document && "Setting a NULL document.");

    m_Document = document;
}

///
/// @brief Sets the minimum job's age.
///
/// In order to prevent rendering previously queued jobs that are no
/// longer needed, a presenter can set the minimum age that a job
/// needs to have in order to be rendered.
///
/// @param age The minimum job's age.
///
void
JobRender::setMinAge (guint32 age)
{
    m_MinAge = age;
}

///
/// @brief Sets the page to render.
///
/// @param pageNumber The number of the page to render.
///
void
JobRender::setPageNumber (gint pageNumber)
{
    m_PageNumber = pageNumber;
}

////////////////////////////////////////////////////////////////
// Static threaded functions.
////////////////////////////////////////////////////////////////

///
/// @brief The page has been rendered.
///
/// This is called when the page is rendered. It does notify the
/// document class about this and destroys the job.
///
/// @param data The job that is done.
///
gboolean
job_render_done (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    G_LOCK (JobRender);

    JobRender *job = (JobRender *)data;
    IDocument *doc = job->getDocument ();
    if ( NULL != doc )
    {
        doc->notifyPageRendered (job->getPageNumber(), 
                                 job->getAge (), job->getPageImage ());
    }
    JOB_NOTIFIER_END();

    G_UNLOCK (JobRender);
   
    return FALSE;
}
