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

#if !defined (__IDOCUMENT_H__)
#define __IDOCUMENT_H__

/// This is the definition of the GQuark used for Document's errors.
#define EPDFVIEW_DOCUMENT_ERROR     ePDFView::IDocument::getErrorQuark ()

namespace ePDFView 
{
    // Forward declarations.
    class DocumentIndex;
    class IDocumentObserver;
    class DocumentPage; 

    ///
    /// @brief Defines the possible errors loading a document.
    ///
    /// This enumeration matches the definitions at ErrorCodes.h of Poppler.
    ///
    typedef enum
    {
        /// No error.
        DocumentErrorNone = 0,
        /// Couldn't open the PDF file.
        DocumentErrorOpenFile = 1,
        /// Couldn't read the page catalog.
        DocumentErrorBadCatalog = 2,
        /// PDF file is damaged and couldn't be repaired.
        DocumentErrorDamaged = 3,
        /// The file is encrypted and the password was incorrect 
        /// or not supplied.
        DocumentErrorEncrypted = 4,
        /// Nonexistent or invalid highlight file.
        DocumentErrorHighlightFile = 5,
        /// Invalid printer.
        DocumentErrorBadPrinter = 6,
        /// Error during printing.
        DocumentErrorPrinting = 7,
        /// PDF File doesn't allow that operation.
        DocumentErrorPermission = 8,
        /// Invalid page number.
        DocumentErrorBadPageNumber = 9,
        /// File I/O Error.
        DocumentErrorFileIO = 10
    } DocumentError;

    ///
    /// @brief Defines the document's page mode when opened.
    ///
    typedef enum 
    {
        /// Document outline visible.
        PageModeOutlines,
        /// Thumbnail images visible.
        PageModeThumbs,
        /// Full-screen mode, with no menu bar, window controls or any
        /// other window visible.
        PageModeFullScreen,
        /// Optional content group panel visible.
        PageModeOC,
        /// Attachments panel visible.
        PageModeAttach,
        /// Not set and neither outline nor thumbnail images visible.
        PageModeUnset
    } PageMode;

    ///
    /// @brief Defines the document's page layout when opened.
    ///
    typedef enum
    {
        /// Display one page at a time.
        PageLayoutSinglePage,
        /// Display the pages on one column.
        PageLayoutOneColumn,
        /// Display the pages in two columns, with odd-numbered pages
        /// on the left.
        PageLayoutTwoColumnLeft,
        /// Display the pages in two columns, with odd-numbered pages
        /// on the right.
        PageLayoutTwoColumnRight,
        /// Display the pages two at a time, with odd-numbered pages
        /// on the left.
        PageLayoutTwoPageLeft,
        /// Display the pages two at a time, with odd-numbered pages
        /// on the right.
        PageLayoutTwoPageRight,
        /// The page layout was not set yet.
        PageLayoutUnset
    } PageLayout;

    ///
    /// @brief A cached page.
    ///
    typedef struct
    {
        /// The page's age in the cache.
        guint32 age;
        /// The page's number.
        gint pageNumber;
        /// The rendered page image.
        DocumentPage *pageImage;
    } PageCache;

    ///
    /// @class IDocument
    /// @brief Interface for documents.
    ///
    /// Every document type that the application can handle must be 
    /// created from this base class.
    /// Besides the abstracts methods for open a document, this class also
    /// handles the rotation and zoom levels for any IDocument's child class.
    ///
    class IDocument
    {
        public:
            virtual ~IDocument (void);

            ///
            /// @brief Makes a copy of the document.
            ///
            /// This function must make a deep copy of the current document
            /// and return the result.
            ///
            /// @return A new document class with the same content than the
            ///         caller document.
            ///
            virtual IDocument *copy (void) const = 0;

            ///
            /// @brief Finds text on a single page.
            ///
            /// The document must find the given text in the indicated page
            /// number.
            ///
            /// @param pageNum The number of the page to find the text in.
            /// @param textToFind The text to find in the page.
            ///
            /// @return The list of positions on the page that the text
            ///         was found. If the text is not found on the page,
            ///         then it must result NULL.
            ///
            virtual GList *findTextInPage (gint pageNum, 
                                           const gchar *textToFind) = 0;

            ///
            /// @brief Checks if the document has been loaded.
            ///
            /// @return TRUE if the document has been loaded or FALSE otherwise.
            ///
            virtual gboolean isLoaded (void) = 0;

            ///
            /// @brief Load a document file.
            ///
            /// Tries to open the document file @a filename using @a password
            /// as a password to decrypt the document.
            ///
            /// The rotation degree is reset to 0 and the scale level to 1.0f
            ///
            /// @param filename The path, absolute or relative, to the file name
            ///                 to open.
            /// @param password The password to use to decrypt @a filename. If
            ///                 the file is not encrypted, you can set it to
            ///                 NULL.
            /// @param error Location to store any error that could happen or
            ///              set to NULL to ignore errors.
            ///
            /// @return TRUE if the file could be opened. FALSE otherwise.
            ///
            virtual gboolean loadFile (const gchar *filename,
                                       const gchar *password,
                                       GError **error) = 0;

            ///
            /// @brief Gets a document's page's unscaled size.
            ///
            /// Retrieves the width and height of a document's page before
            /// scaling, but after rotate it.
            ///
            /// @param pageNum The page to get its size.
            /// @param width The location to save the page's width.
            /// @param height The location to save the page's height.
            ///
            virtual void getPageSizeForPage (gint pageNum, gdouble *width,
                                             gdouble *height) = 0;

            ///
            /// @brief Starts the output to PostScript.
            ///
            /// This starts a new PostScript file with name @a fileName and
            /// with @a numPages pages of @a width x @a height.
            ///
            /// @param fileName The file name to output the postscript code.
            /// @param numOfPages The number of pages to output.
            /// @param pageWidth The width of each page.
            /// @param pageHeight The height of each page.
            ///
            virtual void outputPostscriptBegin (const gchar *fileName,
                                                guint numOfPages,
                                                gfloat pageWidth,
                                                gfloat pageHeight) = 0;

            ///
            /// @brief Ends the output to PostScript.
            ///
            /// Ends any started output to PostScript by calling
            /// outputPostscriptBegin ().
            ///
            virtual void outputPostscriptEnd (void) = 0;

            ///
            /// @brief Renders a single page to PostScript.
            ///
            /// Renders the page @a pageNum To the PostScript started
            /// at outputPostscriptBegin ().
            ///
            /// @param pageNum The number of the page to render.
            ///
            virtual void outputPostscriptPage (guint pageNum) = 0;

            ///
            /// @brief Renders a document's page.
            ///
            /// Rendering a document's page means to get the pixels for the
            /// page image given the current rotation and scale level.
            ///
            /// @param pageNum The page number to render.
            ///
            /// @return A DocumentPage with the image. The returned page must
            ///         be freed by calling delete when done with it.
            ///
            virtual DocumentPage *renderPage (gint pageNum) = 0;

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
            virtual gboolean saveFile (const gchar *filename,
                                       GError **error) = 0;

            virtual cairo_region_t* getTextRegion (DocumentRectangle *rect) = 0;
            virtual void setTextSelection (DocumentRectangle *rect) = 0;

            void attach (const IDocumentObserver *observer);
            void detach (const IDocumentObserver *observer);

            void notifyFindChanged (gint pageNum, DocumentRectangle *matchRect);
            void notifyFindFinished (void);
            void notifyFindStarted (void);
            void notifyLoad (void);
            void notifyLoadError (const gchar *fileName, const GError *error);
            void notifyLoadPassword (const gchar *fileName, gboolean reload,
                                     const GError *error);
            void notifyPageChanged (void);
            void notifyPageRendered (gint pageNumber,
                                     guint32 age, DocumentPage *pageImage);
            void notifyPageRotated (void);
            void notifyPageZoomed (void);
            void notifyReload (void);
            void notifySave (void);
            void notifySaveError (const GError *error);

            const gchar *getTitle (void);
            void setTitle (gchar *title);
            const gchar *getAuthor (void);
            void setAuthor (gchar *author);
            const gchar *getSubject (void);
            void setSubject (gchar *subject);
            const gchar *getKeywords (void);
            void setKeywords (gchar *keywords);
            const gchar *getCreator (void);
            void setCreator (gchar *creator);
            const gchar *getPassword (void) const;
            void setPassword (const gchar *password);
            const gchar *getProducer (void);
            void setProducer (gchar *producer);
            const gchar *getFileName (void) const;
            void setFileName (const gchar *fileName);
            const gchar *getFormat (void);
            void setFormat (gchar *format);
            const gchar *getLinearized (void);
#if defined (HAVE_POPPLER_0_15_1)
            void setLinearized (gboolean *linearized);
#else
            void setLinearized (gchar *linearized);
#endif
            const gchar *getCreationDate (void);
            void setCreationDate (gchar *date);
            const gchar *getModifiedDate (void);
            void setModifiedDate (gchar *date);
            PageMode getPageMode (void);
            void setPageMode (PageMode mode);
            PageLayout getPageLayout (void);
            void setPageLayout (PageLayout layout);
            void getPageSize (gdouble *width, gdouble *height);
            gint getNumPages (void);
            void setNumPages (gint numPages);
            DocumentPage *getCurrentPage (void);
            DocumentPage *getEmptyPage (void);
            gint getCurrentPageNum (void);
            DocumentOutline *getOutline (void);

            void clearCache (void);

            void load (const gchar *fileName, const gchar *password);
            void reload (void);
            void save (const gchar *fileName);

            void goToFirstPage (void);
            void goToLastPage (void);
            void goToNextPage (void);
            void goToPage (gint pageNum);
            void goToPreviousPage (void);

            gint getRotation (void);
            void setRotation (gint rotation);
            void rotateLeft (void);
            void rotateRight (void);

            gboolean canZoomIn (void);
            gboolean canZoomOut (void);
            gdouble getZoom (void);
            void setZoom (gdouble zoom);
            void zoomIn (void);
            void zoomOut (void);
            void zoomToFit (gint width, gint height);
            void zoomToWidth (gint width);

            gboolean hasLinkAtPosition (gint x, gint y);
            void activateLinkAtPosition (gint x, gint y);

            static GQuark getErrorQuark (void);
            static gchar *getErrorMessage (DocumentError errorCode);

        protected:
            static GQuark errorQuark;
            
            IDocument (void);
            void addPageToCache (gint pageNum);
            PageCache *getCachedPage (gint pageNum);
            void refreshCache (void);

            /// The document's author.
            gchar *m_Author;
            /// The document's creation date and time.
            gchar *m_CreationDate;
            /// The document's software creator.
            gchar *m_Creator;
            /// The document's currently shown page.
            gint m_CurrentPage;
            /// The currently selected result from a search.
            DocumentRectangle *m_FindRect;
            /// The page number where IDocument::m_FindRect belongs to.
            gint m_FindPage;
            /// The document's format.
            gchar *m_Format;
            /// The document's file name.
            gchar *m_FileName;
            /// The document's keyword.
            gchar *m_Keywords;
            /// Tells if the document is linearized or not.
#if defined (HAVE_POPPLER_0_15_1)
            gboolean *m_Linearized;
#else
            gchar *m_Linearized;
#endif
            /// The document's modification date and time.
            gchar *m_ModifiedDate;
            /// @brief The list of classes that will receive notifications
            /// when the document changes.
            GList *m_Observers;
            /// The document's outline or index.
            DocumentOutline *m_Outline;
            /// The cache of already rendered document's pages.
            GList *m_PageCache;
            /// The age that will get the next page of the cache.
            gint m_PageCacheAge;
            /// The document's page layout.
            PageLayout m_PageLayout;
            /// The document's page mode.
            PageMode m_PageMode;
            /// The number of pages the document has.
            gint m_PageNumber;
            /// The last password used to open the document.
            gchar *m_Password;
            /// The document's software producer.
            gchar *m_Producer;
            /// The document's current rotation in degrees.
            gint m_Rotation;
            /// The document's current zoom or scale level.
            gdouble m_Scale;
            /// The document's subject.
            gchar *m_Subject;
            /// The document's title.
            gchar *m_Title;
    };
}

#endif // !__IDOCUMENT_H__
