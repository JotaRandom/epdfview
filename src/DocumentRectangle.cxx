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

#include "epdfview.h"

using namespace ePDFView;

///
/// @brief Constructs a new rectangle.
///
/// @param x1 The X coordinate of the rectangle's top-left corner.
/// @param y1 The Y coordinate of the rectangle's top-left corner.
/// @param x2 The X coordinate of the rectangle's bottom-right corner.
/// @param y2 The Y coordinate of the rectangle's bottom-right corner.
///
DocumentRectangle::DocumentRectangle (gdouble x1, gdouble y1,
                                      gdouble x2, gdouble y2)
{
    m_X1 = x1;
    m_X2 = x2;
    m_Y1 = y1;
    m_Y2 = y2;
}

///
/// @brief Constructs a new rectangle as a copy of another rectangle.
///
/// @param original The original rectangle to copy.
///
DocumentRectangle::DocumentRectangle (DocumentRectangle &original)
{
    m_X1 = original.getX1 ();
    m_X2 = original.getX2 ();
    m_Y1 = original.getY1 ();
    m_Y2 = original.getY2 ();
}

///
/// @brief Destroys all dynamically allocated memory for DocumentRectangle.
///
DocumentRectangle::~DocumentRectangle ()
{
}

///
/// @brief Gets the top-left corner X coordinates.
///
/// @return The X coordinates of the rectangle's top-left corner.
///
gdouble
DocumentRectangle::getX1 ()
{
    return m_X1;
}

///
/// @brief Gets the bottom-right corner X coordinates.
///
/// @return The X coordinates of the rectangle's bottom-right corner.
///
gdouble
DocumentRectangle::getX2 ()
{
    return m_X2;
}

///
/// @brief Gets the top-left corner Y coordinates.
///
/// @return The Y coordinates of the rectangle's top-left corner.
///
gdouble
DocumentRectangle::getY1 ()
{
    return m_Y1;
}

///
/// @brief Gets the right-bottom corner Y coordinates.
///
/// @return The Y coordinates of the rectangle's bottom-right corner.
///
gdouble
DocumentRectangle::getY2 ()
{
    return m_Y2;
}
