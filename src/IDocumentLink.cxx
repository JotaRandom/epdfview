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

///
/// @brief Construct a new IDocumentLink object.
///
/// @param x1 The X coordinate of the link's top-left corner.
/// @param y1 The Y coordinate of the link's top-left corner.
/// @param x2 The X coordinate of the link's bottom-right corner.
/// @param y2 The Y coordinate of the link's bottom-right corner.
///
IDocumentLink::IDocumentLink (gdouble x1, gdouble y1, gdouble x2, gdouble y2)
{
    m_Rect = new DocumentRectangle (x1, y1, x2, y2);
}

///
/// @brief Destroys all dynamically allocated memory by IDocumentLink.
///
IDocumentLink::~IDocumentLink ()
{
    delete m_Rect;
}

/// @brief Checks if a position is over the link.
///
/// This function just check that the given position (x, y) is either
/// inside the link's rectangle (i.e., is over the link) or not.
///
/// @param x The X coordinate of the position to check.
/// @param y The Y coordinate of the position to check.
///
/// @return TRUE if the position is over the link. FALSE otherwise.
///
gboolean
IDocumentLink::positionIsOver (gint x, gint y)
{
    return ( (gint)m_Rect->getX1 () <= x && (gint)m_Rect->getY1 () <= y &&
             (gint)m_Rect->getX2 () >= x && (gint)m_Rect->getY2 () >= y );
}
