// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Dumb Test Document.
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
#include "DumbDocument.h"

using namespace ePDFView;

////////////////////////////////////////////////////////////////
// Interface Methods
////////////////////////////////////////////////////////////////

DumbDocument::DumbDocument ():
    IDocument ()
{
    m_Loaded = FALSE;
    m_OpenError = DocumentErrorNone;
    m_SavedFileName = g_strdup ("");
    m_TestPassword = NULL;
    setNumPages (2);
}

DumbDocument::~DumbDocument ()
{
    clearCache ();
    g_free (m_SavedFileName);
    g_free (m_TestPassword);
}

IDocument *
DumbDocument::copy () const
{
    return new DumbDocument ();
}

GList *
DumbDocument::findTextInPage (gint pageNum, const gchar *textToFind)
{
    return NULL;
}

gboolean
DumbDocument::isLoaded ()
{
    return m_Loaded;
}

gboolean
DumbDocument::loadFile (const gchar *filename, const gchar *password, 
                        GError **error)
{
    if ( DocumentErrorNone == m_OpenError )
    {
        m_Loaded = TRUE;
        setFileName (filename);
        setPassword (password);
    }
    else if ( DocumentErrorEncrypted == m_OpenError &&
              NULL != password && NULL != m_TestPassword &&
              0 == g_ascii_strcasecmp (password, m_TestPassword))
    {
        m_Loaded = TRUE;
        setFileName (filename);
        setPassword (password);
    }
    else
    {
        g_set_error (error, EPDFVIEW_DOCUMENT_ERROR, m_OpenError, 
                     "%s", IDocument::getErrorMessage (m_OpenError));
        m_Loaded = FALSE;
    }
    return m_Loaded;
}

void
DumbDocument::getPageSizeForPage (gint pageNum, gdouble *width, gdouble *height)
{
    if ( 90 == getRotation () || 270 == getRotation () )
    {
        *width = 250;
        *height = 100;
    }
    else
    {
        *width = 100;
        *height = 250;
    }
}

void
DumbDocument::outputPostscriptBegin (const gchar *fileName, guint numberOfPages,
                                     gfloat pageWidth, gfloat pageHeight)
{
}

void
DumbDocument::outputPostscriptEnd ()
{
}

void
DumbDocument::outputPostscriptPage (guint pageNumber)
{
}

DocumentPage *
DumbDocument::renderPage (gint pageNum)
{
    return new DocumentPage ();
}

gboolean
DumbDocument::saveFile (const gchar *fileName, GError **error)
{
    g_free (m_SavedFileName);
    m_SavedFileName = g_strdup (fileName);
    return TRUE;
}

////////////////////////////////////////////////////////////////
// Tests Methods
////////////////////////////////////////////////////////////////

const gchar *
DumbDocument::getSavedFileName ()
{
    return m_SavedFileName;
}

void
DumbDocument::setOpenError (DocumentError error)
{
    m_OpenError = error;
}

void
DumbDocument::setOutline (DocumentOutline *outline)
{
    m_Outline = outline;
}

void
DumbDocument::setTestPassword (const gchar *password)
{
    gchar *oldPassword = m_TestPassword;
    m_TestPassword = g_strdup (password);
    g_free (oldPassword);
}
