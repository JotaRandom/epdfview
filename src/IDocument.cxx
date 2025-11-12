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

#include <config.h>
#include <string.h>
#include "epdfview.h"

using namespace ePDFView;

G_LOCK_EXTERN (JobRender);
G_LOCK_DEFINE_STATIC (pageImage);
G_LOCK_DEFINE_STATIC (pageSearch);

// Constants.
static const gdouble ZOOM_IN_FACTOR = 1.1;  // Smoother zoom steps
static const gdouble ZOOM_IN_MAX = 8.0;     // Increased max zoom
static const gdouble ZOOM_OUT_FACTOR = (1.0 / ZOOM_IN_FACTOR);
static const gdouble ZOOM_OUT_MIN = 0.1;    // More reasonable min zoom
static const gdouble ZOOM_OUT_MAX = 0.1;    // Same as ZOOM_OUT_MIN for consistency
static const guint CACHE_SIZE = 3;

/// This is the error domain that will be used to report Document's errors.
GQuark IDocument::errorQuark = 0;

///
/// @brief Gets the IDocument's error quark.
///
/// The first time it's called will create the new quark value for the 
/// document class. Successive calls will just return the previously created
/// quark value.
///
/// The error quark is used by Glib's g_set_error() function to set the 
/// domain in that the error happened. EPDFVIEW_DOCUMENT_ERROR has been
/// defined to call this function, use that defined string when using
/// g_set_error() inside a document's class.
///
/// @return The IDocument's quark.
///
GQuark
IDocument::getErrorQuark ()
{
    if ( 0 == IDocument::errorQuark )
        IDocument::errorQuark = 
            g_quark_from_static_string ("epdfview-document");

    return IDocument::errorQuark;
}

///
/// @brief Gives the error's description.
///
/// This function looks the error at @a errorCode and gives an human
/// readable error string.
///
/// The error message must be freed using g_free().
///
/// @param errorCode The error code to get the string from.
///
/// @return An human readable error message.
///
gchar *
IDocument::getErrorMessage (DocumentError errorCode)
{
    gchar *errorMessage = NULL;
    switch (errorCode)
    {
        case DocumentErrorNone:
            errorMessage = g_strdup (_("No error."));
            break;
        case DocumentErrorOpenFile:
            errorMessage = g_strdup (_("Could not open file."));
            break;
        case DocumentErrorBadCatalog:
            errorMessage = g_strdup (_("Couldn't read the page catalog."));
            break;
        case DocumentErrorDamaged:
            errorMessage = g_strdup (_("The PDF file is damaged and can't be repaired."));
            break;
        case DocumentErrorEncrypted:
            errorMessage = g_strdup (_("The file is encrypted and the password was incorrect or not supplied."));
            break;
        case DocumentErrorHighlightFile:
            errorMessage = g_strdup (_("Nonexistent or invalid highlight file."));
            break;
        case DocumentErrorBadPrinter:
            errorMessage = g_strdup (_("Invalid printer."));
            break;
        case DocumentErrorPrinting:
            errorMessage = g_strdup (_("Error during printing."));
            break;
        case DocumentErrorPermission:
            errorMessage = g_strdup (_("The PDF file doesn't allow that operation."));
            break;
        case DocumentErrorBadPageNumber:
            errorMessage = g_strdup (_("Invalid page number."));
            break;
        case DocumentErrorFileIO:
            errorMessage = g_strdup (_("File I/O error."));
            break;
        default:
            errorMessage = g_strdup_printf (_("Unknown error (%d)."), errorCode);
    }

    g_assert (NULL != errorMessage && "The error message is NULL");
    return errorMessage;
}

///
/// @brief Constructs a new IDocument object.
///
IDocument::IDocument ()
{
    m_Author = NULL;
    m_CreationDate = NULL;
    m_Creator = NULL;
    m_CurrentPage = 0;
    m_Observers = NULL;
    m_Outline = NULL;
    m_FileName = NULL;
    m_FindRect = NULL;
    m_FindPage = 0;
    m_Format = NULL;
    m_Keywords = NULL;
#if defined (HAVE_POPPLER_0_15_1)
    m_Linearized = FALSE;
#else
    m_Linearized = NULL;
#endif
    m_ModifiedDate = NULL;
    m_PageCache = NULL;
    m_PageCacheAge = 0;
    m_PageLayout = PageLayoutUnset;
    m_PageMode = PageModeUnset;
    m_PageNumber = 0;
    m_Password = NULL;
    m_Producer = NULL;
    m_Rotation = 0;
    m_Scale = 1.0f;
    m_Subject = NULL;
    m_Title = NULL;
}

///
/// @brief Deletes all dynamically created objects of IDocument.
///
IDocument::~IDocument ()
{
    g_list_free (m_Observers);
    delete m_Outline;
    delete m_FindRect;
    g_free (m_Author);
    g_free (m_CreationDate);
    g_free (m_Creator);
    g_free (m_FileName);
    g_free (m_Format);
    g_free (m_Keywords);
#if !defined (HAVE_POPPLER_0_15_1)
    g_free (m_Linearized);
#endif
    g_free (m_ModifiedDate);
    g_free (m_Password);
    g_free (m_Producer);
    g_free (m_Subject);
    g_free (m_Title);
}

///
/// @brief Attaches a new observer object.
///
/// When a changes happens on the document all classes that
/// have been attached to the document will receive a notification
/// of such a change.
///
/// @param observer The observer object to attach.
///
void
IDocument::attach (const IDocumentObserver *observer)
{
    g_assert (NULL != observer && "Tried to attach a NULL observer.");

    m_Observers = g_list_prepend (m_Observers, (gpointer)observer);
}

///
/// @brief Detaches an observer object.
///
/// When a previously attached object is detached no longer receives
/// notification messages when the document is changed. It in turn
/// notifies all observers.
///
/// @param observer The observer object to detach.
///
void
IDocument::detach (const IDocumentObserver *observer)
{
    g_assert (NULL != observer && "Tried to detach a NULL observer.");

    m_Observers = g_list_remove_all (m_Observers, observer);
}

///
/// @brief The selection find result has changed.
///
/// This is called when the result to show to the user has changed.
///
/// @param pageNum The page where the result belong to.
/// @param matchRect The unscaled rectangle of the new selected result.
///
void
IDocument::notifyFindChanged (gint pageNum, DocumentRectangle *matchRect)
{
    delete m_FindRect;
    m_FindRect = NULL;
    if ( NULL != matchRect )
    {
        m_FindRect = new DocumentRectangle (*matchRect);
    }
    m_FindPage = pageNum;

    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyFindChanged (matchRect);
    }
}

///
/// @brief The find has finished with no results.
///
/// This is called when the find has finished without any results. It in turn
/// notifies all observers.
///
void
IDocument::notifyFindFinished ()
{
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyFindFinished ();
    }
}

///
/// @brief A new search has started.
///
/// This is called just before the find starts. It in turn notifies
/// all observers.
///
void
IDocument::notifyFindStarted ()
{
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyFindStarted ();
    }
}

///
/// @brief The document has been loaded.
///
/// This is called when the JobLoad class is done. It in turn notifies
/// all attached observers about this situation.
///
void
IDocument::notifyLoad ()
{
    // Empty the cache to avoid displaying pages from previous file.
    clearCache ();
    // Add the two first pages, if they exists, to the cache.
    addPageToCache (1);
    if ( 1 < getNumPages () )
    {
        addPageToCache (2);
    }

    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyLoad ();
    }
}

///
/// @brief The document couldn't be loaded.
///
/// This is called by the JobLoad class when the document couldn't
/// be opened. It in turns notifies all attached observers.
///
/// @param fileName The file that was trying to load.
/// @param error The error message of why couldn't open the document.
///
void
IDocument::notifyLoadError (const gchar *fileName, const GError *error)
{
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyLoadError (fileName, error);
    }
}

///
/// @brief The document couldn't be loaded because it's encrypted.
///
/// This is called by the JobLoad class when the document to load is
/// encrypted and no password or an invalid one was supplied. It in turns
/// notifies all attached observers.
///
/// @param fileName The file that was trying to load.
/// @param reload TRUE if the document was trying to reload, FALSE if it
///               was being loaded as new.
/// @param error The error message that the operation produced.
///
void
IDocument::notifyLoadPassword (const gchar *fileName, gboolean reload,
                               const GError *error)
{
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyLoadPassword (fileName, reload, error);
    }
}

///
/// @brief The current page has been changed.
///
/// This is called when the current page has been changed. It notifies
/// all attached observers about this.
///
void
IDocument::notifyPageChanged ()
{
    gint pageNum = getCurrentPageNum ();
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyPageChanged (pageNum);
    }
}

///
/// @brief A document's page has been rendered.
///
/// This is called by the JobRender class when it finished to render
/// a document's page.
///
/// @param pageNumber The number of the page that has been rendered.
/// @param age The age that the page had when started to be rendered.
/// @param pageImage The rendered page's image.
///
void
IDocument::notifyPageRendered (gint pageNumber, guint32 age,
                               DocumentPage *pageImage)
{
    PageCache *cachedPage = getCachedPage (pageNumber);
    if ( NULL != cachedPage && age == cachedPage->age )
    {
        G_LOCK (pageImage);
        delete cachedPage->pageImage;
        cachedPage->pageImage = pageImage;
        G_UNLOCK (pageImage);
    }
    else
    {
        delete pageImage;
    }
}

///
/// @brief The page has been rotated.
///
/// This is called when the page rotation changes. It notifies all
/// attached observer about this.
///
void
IDocument::notifyPageRotated ()
{
    gint rotation = getRotation ();
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyPageRotated (rotation);
    }
}

///
/// @brief The page's zoom has been changed.
///
/// This is called when the page scale has been changed. It notifies
/// all attached observers about this.
///
void
IDocument::notifyPageZoomed ()
{
    gdouble zoom = getZoom ();
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyPageZoomed (zoom);
    }
}

///
/// @brief The document has been reloaded.
///
/// This is called by the JobLoad class when the document has been reloaded.
/// It in turns notifies all attached observers about this.
///
void
IDocument::notifyReload ()
{
    // Refresh the cache.
    G_LOCK (JobRender);
    refreshCache ();
    G_UNLOCK (JobRender);

    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifyReload ();
    }
}

///
/// @brief The document has been saved.
///
/// This is called when the JobSave class is done. It in turn notifies
/// all attached observers about this situation.
///
void
IDocument::notifySave ()
{
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifySave ();
    }
}

///
/// @brief The document couldn't be saved.
///
/// This is called by the JobSave class when the document couldn't
/// be saved. It in turns notifies all attached observers.
///
/// @param error The error message of why couldn't save the document.
///
void
IDocument::notifySaveError (const GError *error)
{
    for ( GList *item = g_list_first (m_Observers) ; NULL != item ;
          item = g_list_next (item) )
    {
        IDocumentObserver *observer = (IDocumentObserver *)item->data;
        observer->notifySaveError (error);
    }
}

///
/// @brief Loads a file.
///
/// This is used when loading a new file name.
///
/// @param fileName The file name to load.
/// @param password The password to use to load the file or NULL if no
///                 password is used.
///
void
IDocument::load (const gchar *fileName, const gchar *password)
{
    m_CurrentPage = 1;
    m_Rotation = 0;
    m_Scale = 1.0f;

    JobLoad *job = new JobLoad ();
    job->setDocument (this);
    job->setFileName (fileName);
    job->setPassword (password);
    IJob::enqueue (job);
}

///
/// @brief Reloads the document.
///
/// Opens again the same file name using the previously used password.
///
void
IDocument::reload (void)
{
    JobLoad *job = new JobLoad ();
    job->setDocument (this);
    job->setFileName (getFileName ());
    job->setPassword (getPassword ());
    job->setReload (TRUE);
    IJob::enqueue (job);
}

///
/// @brief Saves a copy of the document.
///
/// Saves the current document in a new file.
///
/// @param fileName The file name to save the document's copy to.
///
void
IDocument::save (const gchar *fileName)
{
    JobSave *job = new JobSave ();
    job->setDocument (this);
    job->setFileName (fileName);
    IJob::enqueue (job);
}

///
/// @brief Gets the document's title.
///
/// @return The title of the document or an empty string if the document don't
///         have a title.
///
const gchar *
IDocument::getTitle ()
{
    if ( NULL == m_Title )
    {
        return "";
    }
    return m_Title;
}

///
/// @brief Sets the document's title.
///
/// @param title The title of the document. IDocument will free it.
///
void
IDocument::setTitle (gchar *title)
{
    g_free (m_Title);
    m_Title = g_strdup (title);
    g_free (title);
}

///
/// @brief Gets the document's author.
///
/// @return The author of the document or an empty string if the document don't
///         have the author name.
///
const gchar *
IDocument::getAuthor ()
{
    if ( NULL == m_Author )
    {
        return "";
    }
    return m_Author;
}

///
/// @brief Sets the document's author.
///
/// @param author The name of the document's author. IDocument will free it.
///
void
IDocument::setAuthor (gchar *author)
{
    g_free(m_Author);
    m_Author = g_strdup (author);
    g_free (author);
}

///
/// @brief Gets the document's subject.
///
/// @return The subject of the document or an empty string if the document don't
///         have subject.
///
const gchar *
IDocument::getSubject ()
{
    if ( NULL == m_Subject )
    {
        return "";
    }
    return m_Subject;
}

///
/// @brief Sets the document's subject.
///
/// @param subject The subject of the document. IDocument will free it.
///
void
IDocument::setSubject (gchar *subject)
{
    g_free (m_Subject);
    m_Subject = g_strdup (subject);
    g_free (subject);
}

///
/// @brief Gets the document's keywords.
///
/// @return An string with all keywords separated by white space, or an empty
///         string if the document have no keywords.
///
const gchar *
IDocument::getKeywords ()
{
    if ( NULL == m_Keywords )
    {
        return "";
    }
    return m_Keywords;
}

///
/// @brief Sets the document's keywords.
///
/// @param keywords A white space separated list of keywords. IDocument will
///                 free it.
///
void
IDocument::setKeywords (gchar *keywords)
{
    g_free(m_Keywords);
    m_Keywords = g_strdup (keywords);
    g_free (keywords);
}

///
/// @brief Gets the document's creator.
///
/// @return The name of the software that created the document or an empty
///         string if the creator's name is not available.
///
const gchar *
IDocument::getCreator ()
{
    if ( NULL == m_Creator )
    {
        return "";
    }
    return m_Creator;
}

///
/// @brief Sets the document's creator.
///
/// @param creator The name of the software that created the document.
///                IDocument will free it.
///
void
IDocument::setCreator (gchar *creator)
{
    g_free (m_Creator);
    m_Creator = g_strdup (creator);
    g_free (creator);
}
///
/// @brief Gets the document's producer.
///
/// @return The name of the software that converted the document or an empty
///         string if the producer's name is not available.
///
const gchar *
IDocument::getProducer ()
{
    if ( NULL == m_Producer )
    {
        return "";
    }
    return m_Producer;
}

///
/// @brief Sets the document's producer.
///
/// @param producer The name of the software that converted the document.
///                 IDocument will free it.
///
void
IDocument::setProducer (gchar *producer)
{
    g_free (m_Producer);
    m_Producer = g_strdup (producer);
    g_free (producer);
}

///
/// @brief Gets the document's format.
///
/// @returns The version of the document or an empty string if the version
///          is not available (usually only when the document has not been
///          loaded yet).
///
const gchar *
IDocument::getFormat ()
{
    if ( NULL == m_Format )
    {
        return "";
    }
    return m_Format;
}

///
/// @brief Sets the document's format.
///
/// @param format Set the format in which the document is. IDocument will free
///               it.
///
void
IDocument::setFormat (gchar *format)
{
    g_free (m_Format);
    m_Format = g_strdup (format);
    g_free (format);
}

///
/// @brief Gets if the document is linearised.
///
/// @return The string "Yes" if the document is optimised for web viewing, "No"
///         if it's not optimised or an empty string if this information is
///         not available.
///
const gchar *
IDocument::getLinearized ()
{
#if defined (HAVE_POPPLER_0_15_1)
    if ( m_Linearized ) return "Yes";
    else return "No";
#else
    if ( NULL == m_Linearized )
    {
        return "No";
    }
    return m_Linearized;
#endif
}

///
/// @brief Sets if the document is linearised.
///
/// @param linearized Set to "Yes" if the document is linearized. "No"
///                   otherwise. IDocument will free it.
///
#if defined (HAVE_POPPLER_0_15_1)
void
IDocument::setLinearized (gboolean *linearized)
{
    m_Linearized = linearized;
}
#else
void
    IDocument::setLinearized (gchar *linearized)
{
    g_free (m_Linearized);
    m_Linearized = g_strdup (linearized);
    g_free (linearized);
}
#endif

///
/// @brief Gets the document's creation date.
///
/// @return The date and time the document was created or an empty string if
///         this information is not available.
///
const gchar *
IDocument::getCreationDate ()
{
    if ( NULL == m_CreationDate )
    {
        return "";
    }
    return m_CreationDate;
}

///
/// @brief Sets the document's creation date.
///
/// @param date The date and time the document was created in format
///             'Y-M-D H:M:s'. IDocument will free it.
///
void
IDocument::setCreationDate (gchar *date)
{
    g_free (m_CreationDate);
    m_CreationDate = g_strdup (date);
    g_free (date);
}

///
/// @brief Gets the document's modification date.
///
/// @return The date and time the document was modified or an empty string if
///         this information is not available.
///
const gchar *
IDocument::getModifiedDate ()
{
    if ( NULL == m_ModifiedDate )
    {
        return "";
    }
    return m_ModifiedDate;
}

///
/// @brief Sets the document's modification date.
///
/// @param date The date and time the document was modified in format 
///             'Y-M-D H:M:s'. IDocument will free it.
///
void
IDocument::setModifiedDate (gchar *date)
{
    g_free (m_ModifiedDate);
    m_ModifiedDate = g_strdup (date);
    g_free (date);
}

///
/// @brief Gets the document's page mode.
///
/// @return The page mode of the document or PageModeUnset if this
///         information is not available.
///
PageMode
IDocument::getPageMode ()
{
    return m_PageMode;
}

///
/// @brief Sets the document's page mode.
///
/// @param mode The page mode of the document.
///
void
IDocument::setPageMode (PageMode mode)
{
    m_PageMode = mode;
}

///
/// @brief Gets the document's page layout.
///
/// @return The initial page layout of the document or PageLayoutUnset if this
///         information is not available.
///
PageLayout
IDocument::getPageLayout ()
{
    return m_PageLayout;
}

///
/// @brief Sets the document's page layout.
///
/// @param layout The initial layout of the document.
///
void
IDocument::setPageLayout (PageLayout layout)
{
    m_PageLayout = layout;
}

///
/// @brief Get the document's number of pages.
///
/// @return The total number of pages the document has or 0 if the document has
///         not been loaded yet.
///
gint
IDocument::getNumPages ()
{
    return m_PageNumber;
}

///
/// @brief Gets the password used to open the document.
///
/// @return The password used to open the document, or NULL
///         if no password was used.
///
const gchar *
IDocument::getPassword () const
{
    return m_Password;
}

///
/// @brief Sets the password user to open the document.
///
/// @param password The password used.
///
void
IDocument::setPassword (const gchar *password)
{
    g_free (m_Password);
    m_Password = g_strdup (password);
}

///
/// @brief Sets the document's number of pages.
///
/// @param numPages The number of pages the document has.
///
void
IDocument::setNumPages (gint numPages)
{
    m_PageNumber = numPages;
}

///
/// @brief Gets the document's file name.
///
/// @return The file name that contains the document, or an empty string
///         if the document isn't loaded yet.
///
const gchar *
IDocument::getFileName () const
{
    if ( NULL == m_FileName )
    {
        return "";
    }
    return m_FileName;
}

///
/// @brief Sets the document's file name.
///
/// @param fileName The name to set as the document's file name.
///
void
IDocument::setFileName (const gchar *fileName)
{
    g_assert (NULL != fileName && "Tried to set a NULL file name.");

    g_free(m_FileName);
    m_FileName = g_strdup (fileName);
}

///
/// @brief Gets the document's current page image.
///
/// @return The rendered image of the current page or NULL if the image
///         is not yet available.
///

DocumentPage *
IDocument::getCurrentPage ()
{
    PageCache *cachedPage = getCachedPage (m_CurrentPage);
    if ( NULL == cachedPage )
    {
        return NULL;
    }

    G_LOCK (pageImage);
    DocumentPage *pageImage = cachedPage->pageImage;
    G_UNLOCK (pageImage);

    if ( NULL != pageImage )
    {
        /// XXX: Only non rotated documents for now.
        if ( m_FindPage == m_CurrentPage && NULL != m_FindRect &&
             0 == getRotation () )
        {
            pageImage->setSelection (*m_FindRect, getZoom ());
        }
        else
        {
            pageImage->clearSelection ();
        }
    }

    return pageImage;
}

///
/// @brief Gets an empty page of the same size than the current page.
///
/// This is used when the page is now ready on the cache and the presenter
/// wants to show something to the user. This function just will create 
/// an empty document (i.e., no text or image) using the size that the
/// page would have it it was rendered.
///
/// @return An DocumentPage whose image is just a blank page. The caller
///         must delete this DocumentPage object.
///
DocumentPage *
IDocument::getEmptyPage ()
{
    gdouble pageWidth;
    gdouble pageHeight;
    getPageSizeForPage (getCurrentPageNum (), &pageWidth, &pageHeight);
    gint width = MAX((gint) ((pageWidth * getZoom ()) + 0.5), 1);
    gint height = MAX((gint) ((pageHeight * getZoom ()) + 0.5) , 1);
    DocumentPage *emptyPage = new DocumentPage ();
    emptyPage->newPage (width, height);

    return emptyPage;
}

///
/// @brief Get the document's current page number.
///
/// @return The number of the current page that we are viewing or 0 if the
///         document has not been loaded yet.
///
gint
IDocument::getCurrentPageNum ()
{
    return m_CurrentPage;
}

///
/// @brief Get the document's outline.
///
/// @return The top level DocumentOutline for this document.
///         The returned object must not be freed, the 
///         IDocument class will do it.
///
DocumentOutline *
IDocument::getOutline ()
{
    return m_Outline;
}

///
/// @brief Gets the current page's unscaled size.
///
/// Retrieves the width and height of the current page before to scale, but
/// after rotation.
/// 
/// @param width The output pointer to save the page's width.
/// @param height The output pointer to save the page's height.
///
void
IDocument::getPageSize (gdouble *width, gdouble *height)
{
    getPageSizeForPage (getCurrentPageNum (), width, height);
}

///
/// @brief Goes to the document's first page.
///
/// Changes the current page to be the first page of the
/// document.
///
void
IDocument::goToFirstPage ()
{
    goToPage (1);
}

///
/// @brief Goes to the document's last page.
///
/// Changes the current page to be the last page of the
/// document.
///
void
IDocument::goToLastPage ()
{
    goToPage (getNumPages ());
}

///
/// @brief Goes the the document's next page.
///
/// Changes the current page to be the next page. If the current page is
/// already the last, then it will not change.
///
void
IDocument::goToNextPage ()
{
    goToPage (getCurrentPageNum() + 1);
}

// Structure to hold data for async page loading
///
/// @brief Goes to the document's last page.

///
/// @brief Goes to a document page.
///
/// Changes the current page to be the page @a pageNum. If @a pageNum is
/// greater than the document's last page, then the current page becomes
/// the last page. If @a pageNum is instead less than the first page (i.e., 
/// <= 0) then the current page becomes the first (i.e., 1).
///
/// @param pageNum The page index to go to.
///
void
IDocument::goToPage(gint pageNum)
{
    // Clamp to valid range and check if different from current
    pageNum = CLAMP(pageNum, 1, getNumPages());
    
    // Only change page if different - this prevents infinite loops
    if (pageNum != m_CurrentPage) {
        m_CurrentPage = pageNum;

        addPageToCache(m_CurrentPage);
        if (m_CurrentPage > 1) {
            addPageToCache(m_CurrentPage - 1);
        }
        if (m_CurrentPage < getNumPages()) {
            addPageToCache(m_CurrentPage + 1);
        }

        notifyPageChanged();
    }
}

///
/// @brief Goes to the document's previous page.
///
/// Changes the current page to be the previous page. If the current page
/// is already the first page, then it will not change.
///
void
IDocument::goToPreviousPage ()
{
    goToPage (getCurrentPageNum () - 1);
}

///
/// @brief Gets the current rotation degrees.
///
/// @return The current document's rotation degrees.
///
gint
IDocument::getRotation ()
{
    return m_Rotation;
}

///
/// @brief Sets the current rotation degrees.
///
/// @param rotation The rotation in positive degrees. 
///                 If the parameter is greater than 360 degrees, its set
///                 between 0 and 360 as a result of @a rotation % 360.
///
void
IDocument::setRotation (gint rotation)
{
    if ( rotation != m_Rotation )
    {
        G_LOCK (JobRender);
        m_Rotation = (rotation % 360);
        refreshCache ();
        G_UNLOCK (JobRender);
        notifyPageRotated ();
    }
}

///
/// @brief Rotates 90 to the left.
///
/// Substract 90 degrees to the document's rotation.
///
void
IDocument::rotateLeft ()
{
    gint rotation = (getRotation () - 90);
    if ( rotation < 0 )
    {
        rotation += 360;
    }
    setRotation (rotation);
}

///
/// @brief Rotates 90 to the right.
///
/// Adds 90 degrees to the document's rotation.
///
void
IDocument::rotateRight ()
{
    setRotation (getRotation () + 90);
}

///
/// @brief Checks if is possible to zoom in.
///
/// Checks if after zooming in the zoom level will be below the max level.
///
gboolean
IDocument::canZoomIn (void)
{
    return ( getZoom () * ZOOM_IN_FACTOR <= ZOOM_IN_MAX );
}

///
/// @brief Checks if is possible to zoom out.
///
/// Checks if after zooming out the zoom level will be above the min level.
///
gboolean
IDocument::canZoomOut (void)
{
    return ( getZoom () * ZOOM_OUT_FACTOR >= ZOOM_OUT_MAX );
}

///
/// @brief Gets the current zoom.
///
/// @return The current zoom or scale level.
///
gdouble
IDocument::getZoom (void)
{
    return m_Scale;
}

///
/// @brief Sets the current zoom.
///
/// @param zoom The new zoom level.
///
void
IDocument::setZoom (gdouble zoom)
{
    if ( ABS (zoom - m_Scale) > 0.00001 )
    {
        G_LOCK (JobRender);
        m_Scale = CLAMP (zoom, ZOOM_OUT_MAX, ZOOM_IN_MAX);
        refreshCache ();
        G_UNLOCK (JobRender);
        notifyPageZoomed ();
    }
}

///
/// @brief Zooms In.
///
/// Adds a fixed amount to the scale / zoom level if the zoom level has not
/// reached the max.
///
/// @see canZoomIn()
///
void
IDocument::zoomIn (void)
{
    if (canZoomIn ())
    {
        setZoom (getZoom () * ZOOM_IN_FACTOR);
    }
}

///
/// @brief Zooms Out.
///
/// Substracts a fixed amount to the scale / zoom level is the zoom level
/// has not reached the min.
///
/// @see canZoomOut()
///
void
IDocument::zoomOut (void)
{
    if (canZoomOut ())
    {
        setZoom (getZoom () * ZOOM_OUT_FACTOR);
    }
}

///
/// @brief Zooms the document to fit the width and the height.
///
/// It tries to get the best zoom / scale level that will let the document
/// fit into @a width and @a height, maintaining the aspect ratio.
///
/// @param width The width to fit the document to.
/// @param height The height to fit the document to.
///
void
IDocument::zoomToFit (gint width, gint height)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    gdouble pageWidth;
    gdouble pageHeight;
    getPageSize(&pageWidth, &pageHeight);
    
    if (pageWidth <= 0 || pageHeight <= 0)
    {
        return;
    }
    
    // Calculate scale factors for width and height
    gdouble widthScale = (gdouble)width / pageWidth;
    gdouble heightScale = (gdouble)height / pageHeight;
    
    // Use the smaller scale to ensure the entire page fits
    gdouble scale = MIN(widthScale, heightScale);
    
    // Apply zoom constraints
    scale = CLAMP(scale, ZOOM_OUT_MAX, ZOOM_IN_MAX);
    
    setZoom(scale);
}

///
/// @brief Zooms the document to fit the width.
///
/// It tries to get the best zoom / scale level that will let the document
/// fit into @a width, maintaining the aspect ratio.
///
/// @param width The width to fit the document to.
///
void
IDocument::zoomToWidth (gint width)
{
    if (width <= 0)
    {
        return;
    }

    gdouble pageWidth;
    gdouble pageHeight;
    getPageSize(&pageWidth, &pageHeight);
    
    if (pageWidth <= 0)
    {
        return;
    }
    
    // Calculate scale based on width while maintaining aspect ratio
    gdouble scale = (gdouble)width / pageWidth;
    
    // Apply zoom constraints
    scale = CLAMP(scale, ZOOM_OUT_MAX, ZOOM_IN_MAX);
    
    // Set the zoom level (this will trigger a redraw via notifyPageZoomed)
    setZoom(scale);
}

///
/// @brief Zooms the document to fit the height.
///
/// It tries to get the best zoom / scale level that will let the document
/// fit into @a height, maintaining the aspect ratio.
///
/// @param height The height to fit the document to.
///
void
IDocument::zoomToHeight (gint height)
{
    if (height <= 0)
    {
        return;
    }

    gdouble pageWidth;
    gdouble pageHeight;
    getPageSize(&pageWidth, &pageHeight);
    
    if (pageHeight <= 0)
    {
        return;
    }
    
    // Calculate scale based on height while maintaining aspect ratio
    gdouble scale = (gdouble)height / pageHeight;
    
    // Apply zoom constraints
    scale = CLAMP(scale, ZOOM_OUT_MAX, ZOOM_IN_MAX);
    
    // Set the zoom level (this will trigger a redraw via notifyPageZoomed)
    setZoom(scale);
}

///
/// @brief Adds a new page to the cache and starts to render it.
///
/// This function checks if a page is already on the cache. If it is, then
/// only updates the age and returns. Otherwise adds the page to the cache and
/// creates a new job for rendering it.
///
/// After adding the new page, if the cache size is larger that it should be,
/// then this deletes the oldest request from the cache.
///
/// @param pageNum The page number to add to the cache.
///
void
IDocument::addPageToCache (gint pageNum)
{
    // Check if the page is already on cache.
    PageCache *cached = getCachedPage (pageNum);
    // If it's not.
    G_LOCK (pageImage);
    if ( NULL == cached || NULL == cached->pageImage )
    {
        G_UNLOCK (pageImage);
        //Create it.
        cached = new PageCache;
        cached->age = m_PageCacheAge++;
        cached->pageNumber = pageNum;
        cached->pageImage = NULL;

        JobRender *job = new JobRender ();
        job->setAge (cached->age);
        job->setDocument (this);
        job->setPageNumber (pageNum);
        IJob::enqueue (job);

        G_LOCK (pageSearch);
        // Check which cached page to drop.
        if ( g_list_length (m_PageCache) >= CACHE_SIZE )
        {
            PageCache *oldestCachedPage = NULL;
            guint32 oldestAge = G_MAXUINT32;
            for ( GList *page = g_list_first (m_PageCache) ; NULL != page ;
                  page = g_list_next (page) )
            {
                PageCache *tmpCachedPage = (PageCache *)page->data;
                if ( tmpCachedPage->age < oldestAge )
                {
                    oldestAge = tmpCachedPage->age;
                    oldestCachedPage = tmpCachedPage;
                }
            }
            m_PageCache = g_list_remove_all (m_PageCache, oldestCachedPage);
            G_LOCK (JobRender);
            JobRender::setMinAge (oldestCachedPage->age);
            G_UNLOCK (JobRender);
            G_LOCK (pageImage);
            delete oldestCachedPage->pageImage;
            G_UNLOCK (pageImage);
            delete oldestCachedPage;            
        }
        // Add the page to the cache.
        m_PageCache = g_list_append (m_PageCache, cached);
        G_UNLOCK (pageSearch);
    }
    else
    {
        G_UNLOCK (pageImage);
        // If the page is already on cache, just update the age.
        cached->age = m_PageCacheAge++;
    }
}

///
/// @brief Retrieves a page from the cache.
///
/// @param pageNum The number of the page to get from the cache.
///
/// @return The page cached if the page is in the cache or NULL if it's not
///         in the cache.
///
PageCache *
IDocument::getCachedPage (gint pageNum)
{
    G_LOCK (pageSearch);
    PageCache *cached = NULL;
    for ( GList *page = g_list_first (m_PageCache) ;
          NULL != page && NULL == cached ;
          page = g_list_next (page) )
    {
        PageCache *tmpCached = (PageCache *)page->data;
        if ( NULL != tmpCached && pageNum == tmpCached->pageNumber )
        {
            cached = tmpCached;
        }
    }
    G_UNLOCK (pageSearch);

    return cached;
}

///
/// @brief Refreshes the cache list.
///
/// Renders again all requested pages on the cache that had been rendered
/// already.  This is useful when rotating or zooming.
///
void
IDocument::refreshCache ()
{
    gint pageCount = g_list_length (m_PageCache);
    guint32 minAge = G_MAXUINT32;
    for ( GList *page = g_list_first (m_PageCache) ;
          NULL != page ;
          page = g_list_next (page) )
    {
        PageCache *cachedPage = (PageCache *)page->data;
        G_LOCK (pageImage);
        delete cachedPage->pageImage;
        G_UNLOCK (pageImage);
        cachedPage->pageImage = NULL;
        cachedPage->age += pageCount;
        if ( cachedPage->age < minAge )
        {
            minAge = cachedPage->age;
        }
        JobRender *job = new JobRender;
        job->setDocument (this);
        job->setPageNumber (cachedPage->pageNumber);
        job->setAge (cachedPage->age);
        IJob::enqueue (job);
    }
    JobRender::setMinAge (minAge);
    m_PageCacheAge += pageCount;
}

///
/// @brief Clears the cache.
///
/// Deletes all elements from the cache. This must be done from
/// the derived classes destructor.
///
void
IDocument::clearCache ()
{
    G_LOCK (pageSearch);
    // Delete all cached pages.
    for ( GList *page = g_list_first (m_PageCache) ;
          NULL != page ;
          page = g_list_next (page) )
    {
        PageCache *cachedPage = (PageCache *)page->data;
        G_LOCK (pageImage);
        delete cachedPage->pageImage;
        G_UNLOCK (pageImage);
        delete cachedPage;
    }
    g_list_free (m_PageCache);
    m_PageCache = NULL;
    G_UNLOCK (pageSearch);
}

///
/// @brief Checks if the current page has a link on a given position.
///
/// Checks if under a given position the document's current page have a 
/// link.
///
/// @param x The X coordinate of the position to check.
/// @param y The Y coordinate of the position to check.
///
/// @return TRUE if the document's current page has a link under the given
///         position. FALSE otherwise.
///
gboolean
IDocument::hasLinkAtPosition (gint x, gint y)
{
    // XXX For now only non rotated pages.
    if ( 0 == getRotation () )
    {
        DocumentPage *page = getCurrentPage ();
        if ( NULL != page )
        {
            return (NULL != page->getLinkAtPosition (x, y));
        }
    }
    return FALSE;
}

///
/// @brief Actives a link under a given position.
///
/// Activating a link means following the link: goes to the page
/// that the links point to.
///
/// @param x The X coordinate of the current page's link to activate.
/// @param y The Y coordinate of the current page's link to activate.
///
void
IDocument::activateLinkAtPosition (gint x, gint y)
{
    DocumentPage *page = getCurrentPage ();
    // XXX For now only non rotated pages.
    if ( NULL != page && 0 == getRotation () )
    {
        IDocumentLink *link = page->getLinkAtPosition (x, y);
        if ( NULL != link )
        {
            link->activate (this);
        }
    }
}
