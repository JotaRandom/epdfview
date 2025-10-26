// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Dumb Test Document Observer.
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

#include <epdfview.h>
#include "DumbDocumentObserver.h"

G_LOCK_DEFINE_STATIC (Searching);

using namespace ePDFView;

DumbDocumentObserver::DumbDocumentObserver ():
    IDocumentObserver ()
{
    m_CurrentPage = 0;
    m_Error = NULL;
    m_FindMatchRect = NULL;
    m_NotifiedError = FALSE;
    m_NotifiedPassword = FALSE;
    m_NotifiedLoad = FALSE;
    m_NotifiedPageRotated = FALSE;
    m_NotifiedPageZoomed = FALSE;
    m_NotifiedReload = FALSE;
    G_LOCK (Searching);
    m_Searching = FALSE;
    G_UNLOCK (Searching);
    m_Zoom = 0.0;
}

DumbDocumentObserver::~DumbDocumentObserver ()
{
    setLoadError (NULL);
}

void
DumbDocumentObserver::notifyFindChanged (DocumentRectangle *matchRect)
{
    m_FindMatchRect = matchRect;
    G_LOCK (Searching);
    m_Searching = FALSE;
    G_UNLOCK (Searching);
}

void
DumbDocumentObserver::notifyFindFinished ()
{
    G_LOCK (Searching);
    m_Searching = FALSE;
    G_UNLOCK (Searching);
}

void
DumbDocumentObserver::notifyFindStarted ()
{
    G_LOCK (Searching);
    m_Searching = TRUE;
    G_UNLOCK (Searching);
}

void
DumbDocumentObserver::notifyLoad ()
{
    m_NotifiedLoad = TRUE;
    m_CurrentPage = 1;
    m_Zoom = 1.0;
}

void
DumbDocumentObserver::notifyLoadError (const GError *error)
{
    setLoadError (error);
    m_NotifiedError = TRUE;
}

void
DumbDocumentObserver::notifyLoadPassword (const gchar *fileName, 
                                          gboolean reload,
                                          const GError *error)
{
    setLoadError (error);
    m_NotifiedPassword = TRUE;
}

void
DumbDocumentObserver::notifyPageChanged (gint pageNum)
{
    m_CurrentPage = pageNum;
}

void
DumbDocumentObserver::notifyPageRotated (gint rotation)
{
    m_NotifiedPageRotated = TRUE;
}

void
DumbDocumentObserver::notifyPageZoomed (gdouble zoom)
{
    m_NotifiedPageZoomed = TRUE;
    m_Zoom = zoom;
}

void
DumbDocumentObserver::notifyReload (void)
{
    m_NotifiedReload = TRUE;
}

////////////////////////////////////////////////////////////////
// Test purposes methods.
////////////////////////////////////////////////////////////////

DocumentRectangle *
DumbDocumentObserver::getFindMatchRect ()
{
    return m_FindMatchRect;
}

gint
DumbDocumentObserver::getCurrentPage (void)
{
    return m_CurrentPage;
}

const GError *
DumbDocumentObserver::getLoadError (void)
{
    return m_Error;
}

gdouble
DumbDocumentObserver::getZoom (void)
{
    return m_Zoom;
}

gboolean
DumbDocumentObserver::isStillSearching (void)
{
    G_LOCK (Searching);
    gboolean searching = m_Searching;
    G_UNLOCK (Searching);
    return searching;
}

gboolean
DumbDocumentObserver::loadFinished (void)
{
    return m_NotifiedError || m_NotifiedPassword || m_NotifiedLoad ||
           m_NotifiedReload;
}

gboolean
DumbDocumentObserver::notifiedError (void)
{
    gboolean notified = m_NotifiedError;
    m_NotifiedError = FALSE;
    return notified;
}

gboolean
DumbDocumentObserver::notifiedLoaded (void)
{
    gboolean notified = m_NotifiedLoad;
    m_NotifiedLoad = FALSE;
    return notified;
}

gboolean
DumbDocumentObserver::notifiedPassword (void)
{
    gboolean notified = m_NotifiedPassword;
    m_NotifiedPassword = FALSE;
    return notified;
}

gboolean
DumbDocumentObserver::notifiedRotation (void)
{
    gboolean notified = m_NotifiedPageRotated;
    m_NotifiedPageRotated = FALSE;
    return notified;
}

gboolean
DumbDocumentObserver::notifiedZoom (void)
{
    gboolean notified = m_NotifiedPageZoomed;
    m_NotifiedPageZoomed = FALSE;
    return notified;
}

void
DumbDocumentObserver::setLoadError (const GError *error)
{
    if ( NULL != m_Error )
    {
        g_error_free (m_Error);
    }
    if ( NULL != error )
    {        
        m_Error = g_error_copy (error);
    }    
}
