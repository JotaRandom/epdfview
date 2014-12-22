// ePDFView - Dumb Test Document.
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

#if !defined(__DUMB_DOCUMENT_H__)
#define __DUMB_DOCUMENT_H__

namespace ePDFView
{
    class DumbDocument: public IDocument
    {
        public:
            DumbDocument ();
            ~DumbDocument ();

            // Interface methods.
            IDocument *copy (void) const;
            GList *findTextInPage (gint pageNum, const gchar *text);
            gboolean isLoaded (void);
            gboolean loadFile (const gchar *filename, const gchar *password,
                               GError **error);
            void getPageSizeForPage (gint pageNum, gdouble *width,
                                     gdouble *height);
            void outputPostscriptBegin (const gchar *fileName, guint numberOfPages, gfloat pageWidth, gfloat pageHeight);
            void outputPostscriptEnd (void);
            void outputPostscriptPage (guint pageNumber);
            DocumentPage *renderPage (gint pageNum);
            gboolean saveFile (const gchar *fileName, GError **error);

            // Test functions.
            const gchar *getSavedFileName (void);
            void setOpenError (DocumentError error);
            void setOutline (DocumentOutline *outline);
            void setTestPassword (const gchar *password);

        private:
            gboolean m_Loaded;
            DocumentError m_OpenError;
            gchar *m_TestPassword;
            gchar *m_SavedFileName;
    };
}

#endif // !__DUMB_DOCUMENT_H__
