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
static gboolean job_load_done (gpointer data);
static gboolean job_load_error (gpointer data);
static gboolean job_load_password (gpointer data);
static gboolean job_reload_done (gpointer data);

///
/// @brief Constructs a new JobLoad object.
///
JobLoad::JobLoad ():
    IJob ()
{
    m_Document = NULL;
    m_Error = NULL;
    m_FileName = NULL;
    m_Password = NULL;
    m_Reload = FALSE;
}

///
/// @brief Deletes all dynamically allocated memory by JobLoad.
///
JobLoad::~JobLoad ()
{
    g_free (m_FileName);
    if ( NULL != m_Error )
    {
        g_error_free (m_Error);
    }
    g_free (m_Password);
}

///
/// @brief The document that's loading.
///
/// @return The document that is loading.
///
IDocument &
JobLoad::getDocument ()
{
    g_assert (NULL != m_Document && "The document is NULL.");

    return *m_Document;
}

///
/// @brief Gets the last error.
///
/// @return The last load error.
///
GError *
JobLoad::getError ()
{
    return m_Error;
}

///
/// @brief Gets the file name to load or reload.
///
/// @return The file name to load or reload.
///
const gchar *
JobLoad::getFileName ()
{
    return m_FileName;
}

///
/// @brief Gets the password.
///
/// @return The password to use to open the document.
///
const gchar *
JobLoad::getPassword ()
{
    return m_Password;
}

///
/// @brief Check if we are reloading.
///
/// @return TRUE if are reloading, FALSE otherwise.
///
gboolean
JobLoad::isReloading ()
{
    return m_Reload;
}

///
/// @brief Opens the document.
///
gboolean
JobLoad::run ()
{
    GError *error = NULL;
    if ( getDocument ().loadFile (getFileName (), getPassword (), &error) )
    {
        if ( isReloading () )
        {
            JOB_NOTIFIER (job_reload_done, this);
        }
        else
        {
            JOB_NOTIFIER (job_load_done, this);
        }
    }
    else
    {
        setError (error);
        if ( DocumentErrorEncrypted == error->code )
        {
            JOB_NOTIFIER (job_load_password, this);
        }
        else
        {
            JOB_NOTIFIER (job_load_error, this);
        }
    }

    return JOB_DELETE;
}

///
/// @brief Sets the document to load.
///
/// @param document The document that will load.
///
void
JobLoad::setDocument (IDocument *document)
{
    g_assert ( NULL != document && "Tried to set a NULL document.");
    
    m_Document = document;
}

///
/// @brief Sets the last error.
///
/// @param error The last error opening a file.
///
void
JobLoad::setError (GError *error)
{
    if ( NULL != m_Error )
    {
        g_error_free (m_Error);
    }
    m_Error = error;
}

///
/// @brief Sets the file name to open.
///
/// @param fileName The file name to load or reload.
///
void
JobLoad::setFileName (const gchar *fileName)
{
    g_free (m_FileName);
    m_FileName = g_strdup (fileName);
}

///
/// @brief Sets the password to open the document.
///
/// @param password The password to use.
///
void
JobLoad::setPassword (const gchar *password)
{
    g_free (m_Password);
    m_Password = g_strdup (password);
}

///
/// @brief Sets if reload.
///
/// @param reload TRUE if must reload, FALSE otherwise.
///
void
JobLoad::setReload (gboolean reload)
{
    m_Reload = reload;
}

////////////////////////////////////////////////////////////////
// Static threaded functions.
////////////////////////////////////////////////////////////////

///
/// @brief The load is finished correctly.
///
/// @param data This parameter holds the JobLoad that finished.
/// 
gboolean
job_load_done (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobLoad *job = (JobLoad *)data;
    job->getDocument ().notifyLoad ();
    JOB_NOTIFIER_END();

    return FALSE;
}

///
/// @brief The load had errors.
///
/// This is called when the load of the document had any error except
/// when it's encrypted.
///
/// @param data This parameter hold the JobLoad to open.
/// 
gboolean
job_load_error (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobLoad *job = (JobLoad *)data;
    job->getDocument ().notifyLoadError (job->getFileName(),
            job->getError ());
    JOB_NOTIFIER_END();

    return FALSE;
}

///
/// @brief The document is encrypted.
///
/// This function is called when the document is encrypted. It asks to the
/// user the password and tries until the user doesn't supply a password
/// anymore (returned NULL) or canTryPassword() returns FALSE.
///
/// When trying a new password, it creates a new thread.
///
/// @param data This parameter hold the JobLoad to open.
/// 
gboolean
job_load_password (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobLoad *job = (JobLoad *)data;
    job->getDocument ().notifyLoadPassword (job->getFileName (),
                                            job->isReloading (),
                                            job->getError ());
    JOB_NOTIFIER_END();

    return FALSE;
}

///
/// @brief The reload is finished correctly.
///
/// @param data This parameter holds the JobLoad that finished.
///
gboolean
job_reload_done (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobLoad *job = (JobLoad *)data;
    job->getDocument ().notifyReload ();
    JOB_NOTIFIER_END();

    return FALSE;
}
