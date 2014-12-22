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
/// @brief Construct a new DocumentLinkGoto object.
///
/// @param x1 The X coordinate of the link's top-left corner.
/// @param y1 The Y coordinate of the link's top-left corner.
/// @param x2 The X coordinate of the link's bottom-right corner.
/// @param y2 The Y coordinate of the link's bottom-right corner.
/// @param destinationPage The page number the links points to.
///
DocumentLinkGoto::DocumentLinkGoto (gdouble x1, gdouble y1, gdouble x2, 
                                    gdouble y2, gint destinationPage):
    IDocumentLink (x1, y1, x2, y2)
{
    m_DestinationPage = destinationPage;
}

///
/// @brief Destroys all dynamically allocated memory by DocumentLinkGoto.
///
DocumentLinkGoto::~DocumentLinkGoto ()
{
}

///
/// @brief Activates the link.
///
/// @param document The document where the link was activated from.
///
void
DocumentLinkGoto::activate (IDocument *document)
{
    document->goToPage (m_DestinationPage);
}
