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

#if !defined (__DOCUMENT_PAGE_H__)
#define __DOCUMENT_PAGE_H__

typedef struct _cairo_region cairo_region_t;

namespace ePDFView
{
    ///
    /// @class DocumentPage
    /// @brief A single document's page.
    ///
    /// Contains the rendered image of a single document's page.
    ///
    class DocumentPage
    {
        public:
            DocumentPage (void);
            ~DocumentPage (void);

            void addLink (IDocumentLink *link);
            void clearSelection (void);
            guchar *getData (void);
            gint getHeight (void);
            IDocumentLink *getLinkAtPosition (gint x, gint y);
            gint getRowStride (void);
            gint getWidth (void);
            gboolean hasAlpha (void);
            gboolean newPage (gint width, gint height);
            void setSelection (DocumentRectangle &selection, gdouble scale);
            void setSelection (cairo_region_t *region);

        protected:
            /// The page's image.
            guchar *m_Data;
            /// Tells if the page has a text selection.
            gboolean m_HasSelection;
            /// The page's height.
            gint m_Height;
            /// The selection top-left X coordinate.
            gint m_SelectionX1;
            /// The selection bottom-right X coordinate.
            gint m_SelectionX2;
            /// The selection top-left Y coordinate.
            gint m_SelectionY1;
            /// The selection bottom-right Y coordinate.
            gint m_SelectionY2;
            /// The page's width.
            gint m_Width;
            /// The list of links from the page.
            GList *m_LinkList;
            /// Selection region
            cairo_region_t *m_Selection;
            
            void invertRegion (cairo_region_t*);
            void invertArea (gint x1, gint y1, gint x2, gint y2);
    };
}

#endif // !__DOCUMENT_PAGE_H__
