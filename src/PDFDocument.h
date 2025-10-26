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

#if !defined (__PDF_DOCUMENT_H__)
#define __PDF_DOCUMENT_H__


/// Forward declarations.
typedef struct _PopplerDocument PopplerDocument;
typedef struct _PopplerIndexIter PopplerIndexIter;
typedef struct _PopplerLinkMapping PopplerLinkMapping;
typedef struct _PopplerPage PopplerPage;
typedef struct _PopplerPSFile PopplerPSFile;

namespace ePDFView 
{
    ///
    /// @class PDFDocument
    /// @brief A PDF document.
    ///
    /// Derived class from IDocument that loads and renders PDF documents.
    ///
    class PDFDocument: public IDocument
    {
        public:
            PDFDocument (void);
            ~PDFDocument (void);

            IDocument *copy (void) const;
            GList *findTextInPage (gint pageNum, const gchar *textToFind);
            gboolean isLoaded (void);
            gboolean loadFile (const gchar *filename, const gchar *password, 
                           GError **error);
            void getPageSizeForPage (gint pageNum, gdouble *width,
                                     gdouble *height);
            void outputPostscriptBegin (const gchar *fileName, guint numOfPages,
                                        gfloat pageWidth, gfloat pageHeight);
            void outputPostscriptEnd (void);
            void outputPostscriptPage (guint pageNum);

            DocumentPage *renderPage (gint pageNum);
            gboolean saveFile (const gchar *fileName, GError **error);
            cairo_region_t* getTextRegion (DocumentRectangle* rect);
            void setTextSelection (DocumentRectangle *rect);

        protected:
            /// The PDF document.
            PopplerDocument *m_Document;
            /// The output to PostScript.
            PopplerPSFile *m_PostScript;

            IDocumentLink *createDocumentLink (const PopplerLinkMapping *link,
                                               const gdouble pageHeight,
                                               const gdouble scale);
            void loadMetadata (void);
            void setOutline (DocumentOutline *outline,
                             PopplerIndexIter *childrenList);
            void setLinks (DocumentPage *renderedPage,
                           PopplerPage *popplerPage);
    };
}

#endif // !__DOCUMENT_H__
