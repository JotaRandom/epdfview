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

#if !defined (__IDOCUMENT_LINK_H__)
#define __IDOCUMENT_LINK_H__

namespace ePDFView
{
    // Forward declarations.
    class IDocument;

    ///
    /// @class IDocumentLink
    /// @brief A single link on a page.
    ///
    /// This class is used by ePDFView::DocumentPage to maintain a list
    /// of all links that a single page have.
    ///
    class IDocumentLink
    {
        public:
            IDocumentLink (gdouble x1, gdouble y1, gdouble x2, gdouble y2);
            virtual ~IDocumentLink (void);

            virtual void activate (IDocument *document) = 0;
            gboolean positionIsOver (gint x, gint y);

        protected:
            /// The link rectangle.
            DocumentRectangle *m_Rect;
    };
}

#endif // !__IDOCUMENT_LINK_H__
