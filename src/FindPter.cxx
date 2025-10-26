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

G_LOCK_DEFINE_STATIC (cancelJob);
G_LOCK_DEFINE_STATIC (freeResults);

///
/// @brief Constructs a new FindPter object.
///
/// @param document The document to find text from.
///
FindPter::FindPter (IDocument *document)
{
    g_assert (NULL != document && "Tried to set a NULL document.");

    m_CurrentMatch = NULL;
    m_Document = document;
    m_FindPage = 0;
    m_FindResults = NULL;
    m_Job = NULL;
    m_View = NULL;
}

///
/// @brief Destroys all dynamically allocated memory for FindPter.
///
FindPter::~FindPter ()
{
    m_Document = NULL;
    // This also deletes the find results.
    cancelJob ();
}

///
/// @brief Cancels the find job.
///
void
FindPter::cancelJob ()
{
    G_LOCK (cancelJob);
    if ( NULL != m_Job )
    {
        m_Job->cancel ();
        m_Job->enqueue ();
        // When the job is cancelled, it will just delete itself.
        m_Job = NULL;
    }
    freeFindResults ();
    G_UNLOCK (cancelJob);
}

///
/// @brief The Close button has been clicked.
///
void
FindPter::closeActivated ()
{
    getView ().hide ();
    cancelJob ();
    m_Document->notifyFindChanged (m_FindPage, NULL);
}

///
/// @brief The Find Next button has been clicked.
///
void
FindPter::findNextActivated ()
{
    m_CurrentMatch = g_list_next (m_CurrentMatch);
    if ( NULL != m_CurrentMatch )
    {
        DocumentRectangle *rect = ((DocumentRectangle *)m_CurrentMatch->data);
        m_Document->notifyFindChanged (m_FindPage, rect);
        m_Document->goToPage (m_FindPage);
    }
    else
    {
        G_LOCK (cancelJob);
        if ( NULL != m_Job )
        {
            if ( FIND_DIRECTION_BACKWARDS == m_Job->getDirection () )
            {
                m_Job->setCurrentPage (m_Job->getCurrentPage () + 2);
            }
            m_Job->setDirection (FIND_DIRECTION_FORWARDS);
            m_Job->enqueue ();
        }
        G_UNLOCK (cancelJob);
    }
}

///
/// @brief The Find Previous button has been clicked.
///
void
FindPter::findPreviousActivated ()
{
    m_CurrentMatch = g_list_previous (m_CurrentMatch);
    if ( NULL != m_CurrentMatch )
    {
        DocumentRectangle *rect = ((DocumentRectangle *)m_CurrentMatch->data);
        m_Document->notifyFindChanged (m_FindPage, rect);
        m_Document->goToPage (m_FindPage);
    }
    else
    {
        G_LOCK (cancelJob);
        if ( NULL != m_Job )
        {
            if ( FIND_DIRECTION_FORWARDS == m_Job->getDirection () )
            {
                m_Job->setCurrentPage (m_Job->getCurrentPage () - 2);
            }
            m_Job->setDirection (FIND_DIRECTION_BACKWARDS);
            m_Job->enqueue ();
        }
        G_UNLOCK (cancelJob);
    }
}

///
/// @brief Deletes all saved results.
///
void
FindPter::freeFindResults ()
{
    G_LOCK (freeResults);
    for ( GList *item = g_list_first (m_FindResults) ; NULL != item ;
          item = g_list_next (item) )
    {
        DocumentRectangle *rect = (DocumentRectangle *)item->data;
        delete rect;
    }
    g_list_free (m_FindResults);
    m_FindResults = NULL;
    G_UNLOCK (freeResults);
}

///
/// @brief Gets the view that the presenter is controlling.
///
/// @return The view that the presenter is controlling.
///
IFindView &
FindPter::getView ()
{
    g_assert (NULL != m_View && "Tried to get a NULL view.");

    return *m_View;
}

///
/// @brief The find has searched on all pages.
///
void
FindPter::notifyFindFinished (gboolean endOfSearch )
{
    G_LOCK (cancelJob);
    if ( NULL != m_Job )
    {
        IFindView &view = getView ();
        if ( endOfSearch && NULL == m_FindResults )
        {
            view.sensitiveFindNext (FALSE);
            view.sensitiveFindPrevious (FALSE);        
            G_UNLOCK (cancelJob);
            cancelJob ();
            G_LOCK (cancelJob);
            view.setInformationText (_("No Results Found!"));
            m_Document->notifyFindFinished ();
            // Clear any previously set rectangle.
            m_Document->notifyFindChanged (m_FindPage, NULL);
        }
        else
        {
            gchar *infoText = 
                g_strdup_printf (_("Searching in page %d of %d..."),
                                 m_Job->getCurrentPage (),
                                 m_Document->getNumPages ());
            view.setInformationText (infoText);
            g_free (infoText);
            m_Job->enqueue ();
        }
    }
    G_UNLOCK (cancelJob);
}

///
/// @brief The find found results.
///
/// @param pageNum The number of the page that found results.
/// @param results The results that found.
/// @param direction The direction it was searching pages when found results.
///
void
FindPter::notifyFindResults (gint pageNum, GList *results, 
                             FindDirection direction)
{
    m_FindPage = pageNum;
    freeFindResults ();
    G_LOCK (freeResults);
    m_FindResults = results;
    G_UNLOCK (freeResults);
    if ( FIND_DIRECTION_FORWARDS == direction )
    {
        m_CurrentMatch = g_list_first (m_FindResults);
    }
    else
    {
        m_CurrentMatch = g_list_last (m_FindResults);
    }
    DocumentRectangle *rect = (DocumentRectangle *)m_CurrentMatch->data;
    m_Document->notifyFindChanged (m_FindPage, rect);
    m_Document->goToPage (m_FindPage);

    IFindView &view = getView ();
    view.setInformationText ("");
    view.sensitiveFindNext (TRUE);
    view.sensitiveFindPrevious (TRUE);
}

///
/// @brief Sets the view that the presenter will control.
///
/// @param view The view that the presenter will control.
///
void
FindPter::setView (IFindView *view)
{
    g_assert ( NULL != view && "Tried to set a NULL view.");

    m_View = view;
    m_View->setPresenter (this);
    m_View->sensitiveFindNext (FALSE);
    m_View->sensitiveFindPrevious (FALSE);
    m_View->setInformationText ("");
}

///
/// @brief The text to find has been changed.
///
/// This is called by the view when the text to find has been modified. This
/// function checks if the text is empty and sets the sensitivity of Find Next
/// and Find Previous button according.
///
void
FindPter::textToFindChanged ()
{
    cancelJob ();

    const gchar *textToFind = getView ().getTextToFind ();
    if ( 0 != g_ascii_strcasecmp ("", textToFind ) )
    {
        // Inform all observers.
        m_Document->notifyFindStarted ();

        // Queue a find job.
        m_Job = new JobFind ();
        m_Job->setDocument (m_Document);
        m_Job->setStartingPage (m_Document->getCurrentPageNum ());
        m_Job->setTextToFind (textToFind);
        m_Job->setFindPter (this);
        m_Job->enqueue ();
    }
    else
    {
        IFindView &view = getView ();
        view.sensitiveFindNext (FALSE);
        view.sensitiveFindPrevious (FALSE);
        // Clear any previously set rectangle.
        m_Document->notifyFindChanged (m_FindPage, NULL);
    }
}
