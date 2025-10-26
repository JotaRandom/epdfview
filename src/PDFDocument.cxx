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
#include <gdk/gdk.h>
#include <time.h>
#include <poppler.h>
#include <unistd.h>
#include <algorithm>
#include "epdfview.h"

using namespace ePDFView;

// Constants.
static const gint PIXBUF_BITS_PER_SAMPLE = 8;
static const gint DATE_LENGTH = 100;

// Forward declarations.
static PageLayout convertPageLayout (gint pageLayout);
static PageMode convertPageMode (gint pageMode);
static gchar *getAbsoluteFileName (const gchar *fileName);

namespace
{
    void
    convert_bgra_to_rgba (guint8 *data, int width, int height)
    {
        using std::swap;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                swap(data[0], data[2]);
                data += 4;
            }
        }
    }
}

///
/// @brief Constructs a new PDFDocument object.
///
PDFDocument::PDFDocument ():
    IDocument ()
{
    m_Document = NULL;
    m_PostScript = NULL;
}

///
/// @brief Deletes all dynamically created objects of PDFDocument.
///
PDFDocument::~PDFDocument ()
{
    clearCache ();
    outputPostscriptEnd ();
    if ( NULL != m_Document )
    {
        g_object_unref (G_OBJECT (m_Document));
        m_Document = NULL;
    }
}

IDocument *
PDFDocument::copy () const
{
    // Making a deep copy is just making a new document and loading the same
    // file.
    PDFDocument *newDocument = new PDFDocument ();
    newDocument->loadFile (getFileName (), getPassword (), NULL);

    return newDocument;
}

///
/// @brief Creates a new document link.
///
/// Based on the passed @a link, this function creates a new document
/// links of the best type for the link's action.
///
/// @param link The link to create the document link from.
/// @param pageHeight The unscaled size of the page's height.
/// @param scale The scale to calculate the link's coordinates.
///
/// @return The link best suited for @a link or NULL if no link can be
///         created.
IDocumentLink *
PDFDocument::createDocumentLink (const PopplerLinkMapping *link,
                                 const gdouble pageHeight,
                                 const gdouble scale)
{
    PopplerAction *action = link->action;
    IDocumentLink *documentLink = NULL;

    // Calculate the four link's corners.
    gdouble topLeft = link->area.x1 * scale;
    gdouble topRight = link->area.x2 * scale;
    gdouble bottomLeft = (pageHeight - link->area.y2) * scale;
    gdouble bottomRight = (pageHeight - link->area.y1) * scale;

    switch (action->type)
    {
        // Internal cross-reference link.
        case POPPLER_ACTION_GOTO_DEST:
        {
            PopplerActionGotoDest *actionGoTo = (PopplerActionGotoDest *)action;
            PopplerDest *destination = actionGoTo->dest;
            int pageNum = destination->page_num;
#if defined (HAVE_POPPLER_0_5_2)
            if ( POPPLER_DEST_NAMED == destination->type )
            {
                destination =
                    poppler_document_find_dest (m_Document,
                                                destination->named_dest);
                if ( NULL != destination )
                {
                    pageNum = destination->page_num;
                    poppler_dest_free (destination);
                }
            }
#endif // HAVE_POPPLER_0_5_2

            documentLink = new DocumentLinkGoto (
                    topLeft, bottomLeft, topRight, bottomRight,
                    pageNum);
            break;
        }
        // Internet address.
        case POPPLER_ACTION_URI:
        {
            PopplerActionUri *actionUri = (PopplerActionUri *)action;
            documentLink = new DocumentLinkUri (
                    topLeft, bottomLeft, topRight, bottomRight,
                    actionUri->uri);
            break;
        }

        default:
            g_warning ("Poppler's link type %d not handled.", action->type);
            break;
    }

    return documentLink;
}

GList *
PDFDocument::findTextInPage (gint pageNum, const gchar *textToFind)
{
    GList *results = NULL;

    if ( NULL == m_Document )
    {
        return results;
    }

    PopplerPage *page = poppler_document_get_page (m_Document, pageNum - 1);
    if ( NULL != page )
    {
        gdouble height = 1.0;
        poppler_page_get_size (page, NULL, &height);
        GList *matches = poppler_page_find_text (page, textToFind);
        for ( GList *match = g_list_first (matches) ;
              NULL != match ;
              match = g_list_next (match) )
        {
            PopplerRectangle *matchRect = (PopplerRectangle *)match->data;
            DocumentRectangle *rect =
                new DocumentRectangle (matchRect->x1,
                                       (height - matchRect->y2),
                                       matchRect->x2,
                                       (height - matchRect->y1));
            results = g_list_prepend (results, rect);
        }
        g_object_unref (G_OBJECT (page));
    }

    return g_list_reverse (results);
}

///
/// @brief Checks if the document has been loaded.
///
/// @return TRUE if the document has been loaded, FALSE otherwise.
///
gboolean
PDFDocument::isLoaded ()
{
    return (NULL != m_Document);
}

///
/// @brief Loads a PDF file.
///
/// Tries to open the PDF file @a filename using the password in @a password.
///
/// @param filename The name of the file name to open. It must be an absolute
///                 path.
/// @param password The password to use to open @a filename.
/// @param error Location to store the error occurring or NULL to ignore
///              errors.
///
/// @return TRUE if the file could be opened, FALSE otherwise.
///
gboolean
PDFDocument::loadFile (const gchar *filename, const gchar *password,
                    GError **error)
{
    g_assert (NULL != filename && "Tried to load a NULL file name");

    gchar *absoluteFileName = getAbsoluteFileName (filename);
    gchar *filename_uri = g_filename_to_uri (absoluteFileName, NULL, error);
    g_free (absoluteFileName);
    if ( NULL == filename_uri )
    {
        return FALSE;
    }
    // Try to open the PDF document.
    GError *loadError = NULL;
    // In case the user specified to read the PDF file from the stdin
    // (i.e., -), then we save the contents of the stdin to a temporary
    // file and use the URI to this temporary file to load.
    if ( g_ascii_strcasecmp ("-", filename) == 0 )
    {
        gchar *tmpFileName;
        gint fd = g_file_open_tmp ("epdfviewXXXXXX", &tmpFileName, error);
        if ( -1 == fd )
        {
            return FALSE;
        }
        while ( !feof (stdin) )
        {
            gchar inputLine[512];
            size_t readBytes = fread (inputLine, sizeof (char),
                                      sizeof (inputLine) / sizeof (char),
                                      stdin);
            write (fd, inputLine, readBytes);
        }
        close (fd);

        g_free (filename_uri);
        filename_uri = g_filename_to_uri (tmpFileName, NULL, error);
        g_free (tmpFileName);
        if ( NULL == filename_uri )
        {
            return FALSE;
        }
    }
    PopplerDocument *newDocument =
        poppler_document_new_from_file (filename_uri, password, &loadError);
    g_free (filename_uri);
    // Check if the document couldn't be opened successfully and why.
    if ( NULL == newDocument )
    {
        DocumentError errorCode = DocumentErrorNone;
        switch ( loadError->code )
        {
            case POPPLER_ERROR_OPEN_FILE:
            case POPPLER_ERROR_INVALID:
                errorCode = DocumentErrorOpenFile;
                break;

            case POPPLER_ERROR_BAD_CATALOG:
                errorCode = DocumentErrorBadCatalog;
                break;

            case POPPLER_ERROR_DAMAGED:
                errorCode = DocumentErrorDamaged;
                break;

            case POPPLER_ERROR_ENCRYPTED:
                errorCode = DocumentErrorEncrypted;
                break;
        }
        g_error_free (loadError);
        // Get our error message.
        gchar *errorMessage = IDocument::getErrorMessage (errorCode);
        g_set_error (error,
                     EPDFVIEW_DOCUMENT_ERROR, errorCode,
                     _("Failed to load document '%s'.\n%s\n"),
                     filename, errorMessage);
        g_free (errorMessage);

        return FALSE;
    }

    // Set the used filename and password to let the user reload the
    // document.
    setFileName (filename);
    setPassword (password);
    if ( NULL != m_Document )
    {
        g_object_unref (G_OBJECT (m_Document));
        m_Document = NULL;
    }
    m_Document = newDocument;
    // Load the document's information and outline.
    loadMetadata ();
    PopplerIndexIter *outline = poppler_index_iter_new (m_Document);
    m_Outline = new DocumentOutline ();
    setOutline (m_Outline, outline);
    // XXX: I still don't know why, but it seems that if I don't request
    // a page here, when rendering a page Glib tells me
    // that cannot register existing type `PopplerPage'... Yes, this
    // is a kludge, but I don't know enough to check it why it does that.
    PopplerPage *page = poppler_document_get_page (m_Document, 0);
    if ( NULL != page )
    {
        g_object_unref (G_OBJECT (page));
    }

    return TRUE;
}

///
/// @brief Reads the document's meta data.
///
/// After each successful load of a PDF file, its meta data is read and
/// keep in member variables of this class, so a call to get*() function
/// will return it.
///
/// Also resets the rotation degree to 0 and the scale level to 1.0f.
///
void
PDFDocument::loadMetadata (void)
{
    g_assert (NULL != m_Document && "The document has not been loaded.");

    gchar *author = NULL;
    GTime creationDate;
    gchar *creator = NULL;
    gchar *format = NULL;
    gchar *keywords = NULL;
    PopplerPageLayout layout = POPPLER_PAGE_LAYOUT_UNSET;
#if defined (HAVE_POPPLER_0_15_1)
    gboolean *linearized = NULL;
#else
    gchar *linearized = NULL;
#endif
    GTime modDate;
    PopplerPageMode mode = POPPLER_PAGE_MODE_UNSET;
    gchar *producer = NULL;
    gchar *subject = NULL;
    gchar *title = NULL;

    g_object_get (m_Document,
            "author", &author,
            "creation-date", &creationDate,
            "creator", &creator,
            "format", &format,
            "keywords", &keywords,
            "page-layout", &layout,
            "linearized", &linearized,
            "mod-date", &modDate,
            "page-mode", &mode,
            "producer", &producer,
            "subject", &subject,
            "title", &title,
            NULL);
    setAuthor (author);
    if ( 0 < creationDate )
    {
        struct tm *tmpTime = localtime ((const time_t *)&creationDate);
        if ( NULL != tmpTime )
        {
            gchar *date = g_strnfill (DATE_LENGTH + 1, 0);
            strftime (date, DATE_LENGTH, "%Y-%m-%d %H:%M:%S", tmpTime);
            setCreationDate (date);
        }
    }
    else
    {
        setCreationDate (NULL);
    }
    setCreator (creator);
    setFormat (format);
    setKeywords (keywords);
    setLinearized (linearized);
    if ( 0 < modDate )
    {
        struct tm *tmpTime = localtime ((const time_t *)&modDate);
        if ( NULL != tmpTime )
        {
            gchar *date = g_strnfill (DATE_LENGTH + 1, 0);
            strftime (date, DATE_LENGTH, "%Y-%m-%d %H:%M:%S", tmpTime);
            setModifiedDate (date);
        }
    }
    else
    {
        setModifiedDate (NULL);
    }
    setProducer (producer);
    setSubject (subject);
    setTitle (title);

    // For the page mode and layout we need the enumerator value
    GEnumValue *pageLayout = g_enum_get_value (
            (GEnumClass *)g_type_class_peek (POPPLER_TYPE_PAGE_LAYOUT), layout);
    setPageLayout (convertPageLayout (pageLayout->value));
    GEnumValue *pageMode = g_enum_get_value (
            (GEnumClass *)g_type_class_peek (POPPLER_TYPE_PAGE_MODE), mode);
    setPageMode (convertPageMode (pageMode->value));

    // Get the number of pages and set the current to the first.
    setNumPages (poppler_document_get_n_pages (m_Document));
}

///
/// @brief Sets the pages links.
///
/// This function adds all links from a page to the rendered page
/// image of it.
///
/// @param renderedPage The rendered page to add the links to.
/// @param popplerPage The page to get the links from.
///
void
PDFDocument::setLinks (DocumentPage *renderedPage, PopplerPage *popplerPage)
{
    gdouble pageHeight = 1.0;
    // Get the height, to calculate the Y position as the document's origin
    // is at the bottom-left corner, not the top-left as the screen does.
    poppler_page_get_size (popplerPage, NULL, &pageHeight);
    // We'll already calculate the positions scaled.
    gdouble scale = getZoom ();
    GList *pageLinks = poppler_page_get_link_mapping (popplerPage);
    for (GList *pageLink = g_list_first (pageLinks) ;
         NULL != pageLink ;
         pageLink = g_list_next (pageLink) )
    {
        PopplerLinkMapping *link = (PopplerLinkMapping *)pageLink->data;
        IDocumentLink *documentLink = createDocumentLink (link, pageHeight, scale);
        if ( NULL != documentLink )
        {
            renderedPage->addLink (documentLink);
        }
    }
    poppler_page_free_link_mapping (pageLinks);
}

///
/// @brief Sets the document's outline.
///
/// This is a recursive function that adds child outlines
/// from the PDF's outline to the passed @a outline.
///
/// @param outline The outline to set the nodes to. The first
///                call must be set to the root DocumentOutline.
/// @param childrenList The list of children for to set to @a outline.
///                     The first line must be the returned valued of
///                     poppler_index_iter_new().
///
void
PDFDocument::setOutline (DocumentOutline *outline,
                         PopplerIndexIter *childrenList)
{
    if ( NULL != childrenList )
    {
        do
        {
            PopplerAction *action =
                poppler_index_iter_get_action (childrenList);
            if ( POPPLER_ACTION_GOTO_DEST == action->type )
            {
                PopplerActionGotoDest *actionGoTo =
                    (PopplerActionGotoDest *)action;
                DocumentOutline *child = new DocumentOutline ();
                child->setParent (outline);
                child->setTitle (actionGoTo->title);
                PopplerDest *destination = actionGoTo->dest;
                child->setDestination (destination->page_num);
#if defined (HAVE_POPPLER_0_5_2)
                if ( POPPLER_DEST_NAMED == destination->type )
                {
                    destination =
                        poppler_document_find_dest (m_Document,
                                                    destination->named_dest);
                    if ( NULL != destination )
                    {
                        child->setDestination (destination->page_num);
                        poppler_dest_free (destination);
                        destination = NULL;
                    }
                }
#endif // HAVE_POPPLER_0_5_2

                    outline->addChild (child);
                PopplerIndexIter *childIter =
                    poppler_index_iter_get_child (childrenList);
                setOutline (child, childIter);
            }
        }
        while ( poppler_index_iter_next (childrenList) );

        poppler_index_iter_free (childrenList);
    }
}

///
/// @brief Gets a document's page's unscaled size.
///
/// Retrieves the width and height of a document's page before to scale, but
/// after rotation.
///
/// @param pageNum The page to get its size.
/// @param width The output pointer to save the page's width.
/// @param height The output pointer to save the page's height.
///
void
PDFDocument::getPageSizeForPage (gint pageNum, gdouble *width, gdouble *height)
{
    g_assert (NULL != m_Document && "Tried to get size of a NULL document.");
    g_assert (NULL != width && "Tried to save the page's width to NULL.");
    g_assert (NULL != height && "Tried to save the page's height to NULL.");

    PopplerPage *page = poppler_document_get_page (m_Document, pageNum - 1);
    if ( NULL != page )
    {
        gdouble pageWidth;
        gdouble pageHeight;
        // Check which rotation has the document's page to know what is width
        // and what is height.
        gint rotate = getRotation ();
        if ( 90 == rotate || 270 == rotate )
        {
            poppler_page_get_size (page, &pageHeight, &pageWidth);
        }
        else
        {
            poppler_page_get_size (page, &pageWidth, &pageHeight);
        }

        *width = pageWidth;
        *height = pageHeight;

        g_object_unref (G_OBJECT (page));
    }
    else
    {
        // Just give any valor, except 0.
        *width = 1.0;
        *height = 1.0;
    }
}

void
PDFDocument::outputPostscriptBegin (const gchar *fileName, guint numOfPages,
                                    gfloat pageWidth, gfloat pageHeight)
{
    if ( NULL != m_Document )
    {
        if ( NULL != m_PostScript )
        {
            outputPostscriptEnd ();
        }
        m_PostScript =
            poppler_ps_file_new (m_Document, fileName, 0, numOfPages);
        if ( NULL != m_PostScript )
        {
            poppler_ps_file_set_paper_size (m_PostScript,
                                            pageWidth, pageHeight);
        }
    }
}

void
PDFDocument::outputPostscriptEnd ()
{
    if ( NULL != m_PostScript )
    {
        poppler_ps_file_free (m_PostScript);
        m_PostScript = NULL;
    }
}

void
PDFDocument::outputPostscriptPage (guint pageNum)
{
    if ( NULL != m_Document && NULL != m_PostScript )
    {
        PopplerPage *page = poppler_document_get_page (m_Document, pageNum - 1);
        if ( NULL != page )
        {
            poppler_page_render_to_ps (page, m_PostScript);
        }
    }
}

///
/// @brief Renders a document's page.
///
/// Rendering a document's page means to get the pixels for the page,
/// given the current rotation level and scale.
///
/// @param pageNum The page to render.
///
/// @return A DocumentPage with the image. The returned page must be freed
///         by calling delete.
///
DocumentPage *
PDFDocument::renderPage (gint pageNum)
{
    if ( NULL == m_Document )
    {
        return NULL;
    }

    // First create the document's page.
    gdouble pageWidth;
    gdouble pageHeight;
    getPageSizeForPage (pageNum, &pageWidth, &pageHeight);
    
    // Calculate dimensions with zoom applied only once
    gint width = MAX((gint) ((pageWidth * getZoom ()) + 0.5), 1);
    gint height = MAX((gint) ((pageHeight * getZoom ()) + 0.5), 1);
    
    DocumentPage *renderedPage = new DocumentPage ();
    renderedPage->newPage (width, height);

    PopplerPage *page = poppler_document_get_page (m_Document, pageNum - 1);
    if ( NULL != page )
    {
#if defined (HAVE_POPPLER_0_17_0)
        cairo_surface_t *surface = cairo_image_surface_create_for_data (
                renderedPage->getData (), 
                CAIRO_FORMAT_ARGB32, width, height,
                renderedPage->getRowStride ());
        cairo_t *context = cairo_create (surface);
        
        // Set high-quality rendering options
        cairo_set_antialias(context, CAIRO_ANTIALIAS_BEST);
        cairo_set_operator(context, CAIRO_OPERATOR_OVER);
        
        // Fill with white background
        cairo_save(context);
        cairo_set_source_rgb (context, 1.0, 1.0, 1.0);
        cairo_rectangle (context, 0, 0, width, height);
        cairo_fill(context);
        cairo_restore(context);
        
        // Save the context state before transformations
        cairo_save(context);

        // Apply rotation transformations
        switch(getRotation())
        {
            case 90:
                cairo_translate(context, width, 0);
                break;

            case 180:
                cairo_translate(context, width, height);
                break;

            case 270:
                cairo_translate(context, 0, height);
                break;

            default:
                cairo_translate(context, 0, 0);
                break;
        }

        // Apply rotation (in radians)
        cairo_rotate(context, getRotation() * G_PI / 180.0);
        
        // Set high-quality scaling
        cairo_pattern_t *pattern = cairo_get_source(context);
        cairo_pattern_set_filter(pattern, CAIRO_FILTER_BEST);
        
        // Render the page (zoom is already applied in the width/height)
        poppler_page_render (page, context);
        cairo_destroy(context);
        cairo_surface_destroy (surface);
        convert_bgra_to_rgba(renderedPage->getData (), width, height);
#else // !HAVE_POPPLER_0_17_0
        // Create the pixbuf from the data and render to it.
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data (renderedPage->getData (),
                                                      GDK_COLORSPACE_RGB,
                                                      FALSE,
                                                      PIXBUF_BITS_PER_SAMPLE,
                                                      width, height,
                                                      renderedPage->getRowStride (),
                                                      NULL, NULL);
        poppler_page_render_to_pixbuf (page, 0, 0, width, height, getZoom (),
                                       getRotation (), pixbuf);
        g_object_unref (pixbuf);
#endif // HAVE_POPPLER_0_17_0
        setLinks (renderedPage, page);
        g_object_unref (G_OBJECT (page));
    }

    return (renderedPage);
}

///
/// @brief Saves a document's copy to a file.
///
/// Tries to save the document to file @a filename.
///
/// @param filename The path, absolute or relative, to the file name
///                 to save the copy to.
/// @param error Location to store any error that could happen or
///              set to NULL to ignore errors.
///
/// @return TRUE if the file could be saved. FALSE otherwise.
///
gboolean
PDFDocument::saveFile (const gchar *filename, GError **error)
{
    g_assert (NULL != m_Document && "No document loaded yet.");
    g_assert (NULL != filename && "Tried to save to a NULL file name.");

    gchar *absoluteFileName = getAbsoluteFileName (filename);
    gchar *filename_uri = g_filename_to_uri (absoluteFileName, NULL, error);
    g_free (absoluteFileName);
    if ( NULL == filename_uri )
    {
        return FALSE;
    }
    // Try to save the PDF document.
#if defined (HAVE_POPPLER_0_8_0)
    gboolean result = poppler_document_save_a_copy (m_Document, filename_uri, error);
#else // !HAVE_POPPLER_0_8_0
    gboolean result = poppler_document_save (m_Document, filename_uri, error);
#endif // HAVE_POPPLER_0_8_0
    g_free (filename_uri);

    return result;
}

static void
repairEmpty(PopplerRectangle& rect)
{
    //rect must have be least 1x1
    if(rect.y1 == rect.y2)
        rect.y2++;
    if(rect.x1 == rect.x2)
        rect.x2++;
}

void
PDFDocument::setTextSelection (DocumentRectangle *rect)
{
    g_assert(rect);

    PopplerPage *page = poppler_document_get_page (m_Document, getCurrentPageNum()-1);
    if(!page)
        return;

    gdouble pageWidth, pageHeight;
    poppler_page_get_size(page, &pageWidth, &pageHeight);

#if defined (HAVE_POPPLER_0_15_0)
    PopplerRectangle textRect = { rect->getX1() / getZoom(),
                                  rect->getY1() / getZoom(),
                                  rect->getX2() / getZoom(),
                                  rect->getY2() / getZoom()};
#else // !HAVE_POPPLER_0_15_0
    //for get text we must exchange y coordinate, don't ask me where logic here.
    PopplerRectangle textRect = { rect->getX1() / getZoom(),
                                  (pageHeight - rect->getY2() / getZoom()),
                                  rect->getX2() / getZoom(),
                                  (pageHeight - rect->getY1() / getZoom())};
#endif // HAVE_POPPLER_0_15_0
    repairEmpty(textRect);

#if defined (HAVE_POPPLER_0_15_0)
    gchar *text = poppler_page_get_selected_text(page, POPPLER_SELECTION_GLYPH,
            &textRect);
#elif defined (HAVE_POPPLER_0_6_0)
    gchar *text = poppler_page_get_text(page, POPPLER_SELECTION_GLYPH,
            &textRect);
#else // !HAVE_POPPLER_0_6_0
    gchar *text = poppler_page_get_text(page, &textRect);
#endif // HAVE_POPPLER_0_6_0
    if(!text)
        goto cleanup;

    for ( GList *obs = g_list_first (m_Observers) ;
          NULL != obs ;
          obs = g_list_next (obs) )
        {
            IDocumentObserver *observer = (IDocumentObserver*)obs->data;
            observer->notifyTextSelected(text);
        }

 cleanup:
    if(page)
        g_object_unref(page);
    if(text)
        g_free(text);
}

cairo_region_t*
PDFDocument::getTextRegion (DocumentRectangle *r)
{
    cairo_region_t *res = NULL;
    PopplerPage *page = poppler_document_get_page (m_Document, getCurrentPageNum()-1);
    if(!page)
        return NULL;

    //calulate rect
    PopplerRectangle rect;
    rect.x1 = r->getX1() / getZoom();
    rect.y1 = r->getY1() / getZoom();
    rect.x2 = r->getX2() / getZoom();
    rect.y2 = r->getY2() / getZoom();
    repairEmpty(rect);

    //calc selection size
#if defined (HAVE_POPPLER_0_8_0)
    GList *selections = poppler_page_get_selection_region(page, getZoom(),
             POPPLER_SELECTION_GLYPH, &rect);
    res = cairo_region_create();
    for (GList *selection = g_list_first (selections) ;
         NULL != selection ; selection = g_list_next (selection)) {
        PopplerRectangle *rectangle = (PopplerRectangle *)selection->data;
        GdkRectangle rect;

        rect.x = (gint)rectangle->x1;
        rect.y = (gint)rectangle->y1;
        rect.width  = (gint) (rectangle->x2 - rectangle->x1);
        rect.height = (gint) (rectangle->y2 - rectangle->y1);

        cairo_region_union_rectangle(res, (const cairo_rectangle_int_t*)&rect);
    }
    poppler_page_selection_region_free (selections);
#elif defined (HAVE_POPPLER_0_6_0)
    res = poppler_page_get_selection_region(page, getZoom(),
            POPPLER_SELECTION_GLYPH, &rect);
#else // !HAVE_POPPLER_0_6_0
    res = poppler_page_get_selection_region(page, getZoom(), &rect);
#endif // HAVE_POPPLER_0_6_0

    //free some local data
    g_object_unref(page);

    return res;
}


///
/// @brief Gets the document's page layout from Poppler's page layout.
///
/// @param pageLayout Is the page layout that Poppler's glib wrapper gives.
///
/// @return The PageLayout based on @a pageLayout.
///
PageLayout
convertPageLayout (gint pageLayout)
{
    PageLayout layout = PageLayoutUnset;
    switch (pageLayout)
    {
        case POPPLER_PAGE_LAYOUT_SINGLE_PAGE:
            layout = PageLayoutSinglePage;
            break;
        case POPPLER_PAGE_LAYOUT_ONE_COLUMN:
            layout = PageLayoutOneColumn;
            break;
        case POPPLER_PAGE_LAYOUT_TWO_COLUMN_LEFT:
            layout = PageLayoutTwoColumnLeft;
            break;
        case POPPLER_PAGE_LAYOUT_TWO_COLUMN_RIGHT:
            layout = PageLayoutTwoColumnRight;
            break;
        case POPPLER_PAGE_LAYOUT_TWO_PAGE_LEFT:
            layout = PageLayoutTwoPageLeft;
            break;
        case POPPLER_PAGE_LAYOUT_TWO_PAGE_RIGHT:
            layout = PageLayoutTwoPageRight;
            break;
        case POPPLER_PAGE_LAYOUT_UNSET:
        default:
            layout = PageLayoutUnset;
    }

    return layout;
}

///
/// @brief Get the document's page mode.
///
/// @param pageLayout Is the page mode that Poppler's catalog gives.
///
/// @return The PageLayout based on @a pageMode.
///
PageMode
convertPageMode (gint pageMode)
{
    PageMode mode = PageModeUnset;
    switch (pageMode)
    {
        case POPPLER_PAGE_MODE_USE_OUTLINES:
            mode = PageModeOutlines;
            break;
        case POPPLER_PAGE_MODE_USE_THUMBS:
            mode = PageModeThumbs;
            break;
        case POPPLER_PAGE_MODE_FULL_SCREEN:
            mode = PageModeFullScreen;
            break;
        case POPPLER_PAGE_MODE_USE_OC:
            mode = PageModeOC;
            break;
        case POPPLER_PAGE_MODE_USE_ATTACHMENTS:
            mode = PageModeAttach;
            break;
        case POPPLER_PAGE_MODE_NONE:
        case POPPLER_PAGE_MODE_UNSET:
        default:
            mode = PageModeUnset;
    }

    return mode;
}

///
/// @brief Gets the absolute path of a filename.
///
/// This function checks if the given @a fileName is an absolute path. If
/// it is then it returns a copy of it, otherwise it prepends the current
/// working directory to it.
///
/// @param fileName The filename to get the absolute path from.
///
/// @return A copy of the absolute path to the file name. This copy must be
///         freed when no longer needed.
///
gchar *
getAbsoluteFileName (const gchar *fileName)
{
    gchar *absoluteFileName = NULL;
    if ( g_path_is_absolute (fileName) )
    {
        absoluteFileName = g_strdup (fileName);
    }
    else
    {
        gchar *currentDir = g_get_current_dir ();
        absoluteFileName = g_build_filename (currentDir, fileName, NULL);
        g_free (currentDir);
    }

    return absoluteFileName;
}
