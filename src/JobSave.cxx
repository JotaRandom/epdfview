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

#include <config.h>
#include "epdfview.h"

using namespace ePDFView;

// Forward declarations.
static gboolean job_save_done (gpointer data);
static gboolean job_save_error (gpointer data);

///
/// @brief Constructs a new JobSave object.
///
JobSave::JobSave ():
    IJob ()
{
    m_Document = NULL;
    m_Error = NULL;
    m_FileName = NULL;
}

///
/// @brief Deletes all dynamically allocated memory by JobSave.
///
JobSave::~JobSave ()
{
    g_free (m_FileName);
    if ( NULL != m_Error )
    {
        g_error_free (m_Error);
    }
}

///
/// @brief The document that's saving a copy from.
///
/// @return The document that is saving a copy from.
///
IDocument &
JobSave::getDocument ()
{
    g_assert (NULL != m_Document && "The document is NULL.");

    return *m_Document;
}

///
/// @brief Gets the last error.
///
/// @return The last save error.
///
GError *
JobSave::getError ()
{
    return m_Error;
}

///
/// @brief Gets the file name to save to.
///
/// @return The file name to save to.
///
const gchar *
JobSave::getFileName ()
{
    return m_FileName;
}

///
/// @brief Saves a copy of the document.
///
gboolean
JobSave::run ()
{
    GError *error = NULL;
    if ( getDocument ().saveFile (getFileName (), &error) )
    {
        JOB_NOTIFIER (job_save_done, this);
    }
    else
    {
        setError (error);
        JOB_NOTIFIER (job_save_error, this);
    }

    return JOB_DELETE;
}

///
/// @brief Sets the document to save a copy from.
///
/// @param document The document that will be saved.
///
void
JobSave::setDocument (IDocument *document)
{
    g_assert ( NULL != document && "Tried to set a NULL document.");

    m_Document = document;
}

///
/// @brief Sets the last error.
///
/// @param error The last error saving a file.
///
void
JobSave::setError (GError *error)
{
    if ( NULL != m_Error )
    {
        g_error_free (m_Error);
    }
    m_Error = error;
}

///
/// @brief Sets the file name to save to.
///
/// @param fileName The file name to save to.
///
void
JobSave::setFileName (const gchar *fileName)
{
    g_free (m_FileName);
    m_FileName = g_strdup (fileName);
}

////////////////////////////////////////////////////////////////
// Static threaded functions.
////////////////////////////////////////////////////////////////

///
/// @brief The save has finished correctly.
///
/// @param data This parameter holds the JobSave that finished.
///
gboolean
job_save_done (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobSave *job = (JobSave *)data;
    job->getDocument ().notifySave ();
    JOB_NOTIFIER_END();

    return FALSE;
}

///
/// @brief The save process had errors.
///
/// This is called when the save of the document's copy had any error.
///
/// @param data This parameter holds the JobSave that failed.
///
gboolean
job_save_error (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobSave *job = (JobSave *)data;
    job->getDocument ().notifySaveError (job->getError ());
    JOB_NOTIFIER_END();

    return FALSE;
}
