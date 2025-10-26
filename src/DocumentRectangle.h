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

#if !defined (__DOCUMENT_RECTANGLE_H__)
#define __DOCUMENT_RECTANGLE_H__

namespace ePDFView
{
    ///
    /// @class DocumentRectangle.
    /// @brief A rectangle on a page.
    ///
    /// This class is just to hold the coordinates of a rectangle on a document.
    /// It is used as a way to know where are the document's link or the
    /// text that has been searched.
    ///
    class DocumentRectangle
    {
        public:
            DocumentRectangle (gdouble x1, gdouble y1, gdouble x2, gdouble y2);
            DocumentRectangle (DocumentRectangle &original);
            ~DocumentRectangle (void);

            gdouble getX1 (void);
            gdouble getX2 (void);
            gdouble getY1 (void);
            gdouble getY2 (void);

        protected:
            /// The X coordinate of the link's top-left corner.
            gdouble m_X1;
            /// The X coordinate of the link's bottom-right corner.
            gdouble m_X2;
            /// The Y coordinate of the link's top-left corner.
            gdouble m_Y1;
            /// The Y coordinate of the link's bottom-right corner.
            gdouble m_Y2;

    };
}

#endif // !__DOCUMENT_RECTANGLE_H__
