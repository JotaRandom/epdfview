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
#include <string.h>
#include <gdk/gdk.h>
#include "epdfview.h"

using namespace ePDFView;

static const gint BYTES_PER_PIXEL = 3;

///
/// @brief Constructs a new DocumentPage.
///
DocumentPage::DocumentPage ()
{
    m_Selection = NULL;
    m_Data = NULL;
    m_HasSelection = FALSE;
    m_Height = 0;
    m_LinkList = NULL;
    m_Width = 0;
}

///
/// @brief Deletes all dynamically allocated memory for DocumentPage.
///
DocumentPage::~DocumentPage ()
{
    delete[] m_Data;
    for ( GList *linkItem = g_list_first (m_LinkList) ;
          NULL != linkItem ;
          linkItem = g_list_next (linkItem) )
    {
        IDocumentLink *link = (IDocumentLink *)linkItem->data;
        delete link;
    }
    g_list_free (m_LinkList);

    if(m_Selection)
        cairo_region_destroy(m_Selection);
}

///
/// @brief Adds a new link to the page.
///
/// When rendering the page, the responsible document will extract the
/// links from the page and call this function with each links it find.
/// 
/// @param link The link to add. The DocumentPage class will delete it.
///
void
DocumentPage::addLink (IDocumentLink *link)
{
    m_LinkList = g_list_prepend (m_LinkList, link);
}

///
/// @brief Clears the current selection.
///
/// This function is called to remove the current selection of a document
/// page. If the page has no selection this function does nothing.
///
void
DocumentPage::clearSelection ()
{
    if ( m_HasSelection )
    {
        invertArea (m_SelectionX1, m_SelectionY1, m_SelectionX2, m_SelectionY2);
        m_HasSelection = FALSE;
    }

    if(NULL != m_Selection){
        invertRegion(m_Selection);
        cairo_region_destroy(m_Selection);
        m_Selection = NULL;
    }
}

///
/// @brief Gets the page's data.
///
/// Gets the actual pixels the page is formed of. The format of the image
/// is RGB, using 3 bytes per pixel and a total of getWidth() * getHeight()
/// pixels.
///
/// @return The page's pixels array.
///
guchar *
DocumentPage::getData ()
{
    g_assert ( NULL != m_Data && 
              "Tried to retrieve data without creating a new page.");

    return m_Data;
}

///
/// @brief Gets the link for a given position.
///
/// This function will get the link whose rectangle is under the position
/// passed as parameter.
///
/// @param x The X coordinate of the link's position.
/// @param y The Y coordinate of the link's position.
///
/// @return The link under the position (x, y) or NULL if no link exists.
///
IDocumentLink *
DocumentPage::getLinkAtPosition (gint x, gint y)
{
    IDocumentLink *link = NULL;

    for ( GList *linkItem = g_list_first (m_LinkList) ;
          NULL != linkItem && NULL == link;
          linkItem = g_list_next (linkItem) )
    {
        IDocumentLink *tmpLink = (IDocumentLink *)linkItem->data;
        if ( tmpLink->positionIsOver (x, y) )
        {
            link = tmpLink;
        }
    }

    return link;
}

///
/// @brief Gets the page's height.
///
/// @return The height of the page's image.
///
gint
DocumentPage::getHeight ()
{
    return m_Height;
}

///
/// @brief Gets the row stride.
///
/// The row stride is the bytes between two consecutive rows of the page's 
/// image retrieved by getData(). Currently this value is 3 * getWidth(), 
/// because I use 3 bytes per pixel, but this can change, so better use this
/// function.
///
/// @return The number of bytes between two consecutive rows.
///
gint
DocumentPage::getRowStride ()
{
#if defined (HAVE_POPPLER_0_17_0)
    return cairo_format_stride_for_width (CAIRO_FORMAT_RGB24, getWidth ());
#else // !HAVE_POPPLER_0_17_0
    return BYTES_PER_PIXEL * getWidth ();
#endif // HAVE_POPPLER_0_17_0
}

///
/// @brief Gets the page's width.
///
/// @return The width of the page's image.
///
gint
DocumentPage::getWidth ()
{
    return m_Width;
}

///
/// @brief Tells whether the page has an alpha component or not.
///
/// This is mainly used to detect whether the data is 24 or 32 bits.  This is
/// important for pages rendered using cairo, because even though the format is
/// indeed RGB, the data is 32-bit always.
///
/// @return @c TRUE if the document page has alpha, @c FALSE otherwise.
///
gboolean
DocumentPage::hasAlpha ()
{
#if defined (HAVE_POPPLER_0_17_0)
    return TRUE;
#else // !HAVE_POPPLER_0_17_0
    return FALSE;
#endif // HAVE_POPPLER_0_17_0
}

///
/// @brief Inverts the colours of a page's area.
///
/// This function is used to mark or clear a selection to the document
/// page. It just inverts the colours of the rectangle area given as parameters.
///
/// @param x1 The X coordinate of the area's top-right corner.
/// @param y1 The Y coordinate of the area's top-right corner.
/// @param x2 The X coordinate of the area's bottom-left corner.
/// @param y2 The Y coordinate of the area's bottom-left corner.
///
void
DocumentPage::invertArea (gint x1, gint y1, gint x2, gint y2)
{
    gint rowStride = getRowStride ();
    guchar *data = getData ();
    for ( gint y = y1 ; y < y2 ; y++ )
    {
        for ( gint x = x1 ; x < x2 ; x++ )
        {
            gint position = y * rowStride + ( x * BYTES_PER_PIXEL );
            data[position + 0] = 255 - data[position + 0];
            data[position + 1] = 255 - data[position + 1];
            data[position + 2] = 255 - data[position + 2];
        }
    }
}

void
DocumentPage::invertRegion (cairo_region_t* region)
{
    int count = cairo_region_num_rectangles(region);
    for(int i = 0; i < count; i++){
        cairo_rectangle_int_t rect;
        cairo_region_get_rectangle(region, i, &rect);
        invertArea(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    }
}


///
/// @brief Allocates the memory for a new page.
///
/// It also stores the @a width and @a height, to be retrieved by
/// calling getWidth() and getHeight() respectively.
///
/// @param width The width of the new page.
/// @param height The height of the new page.
///
/// @return true if the image could be created. False otherwise.
///
gboolean
DocumentPage::newPage (gint width, gint height)
{
    g_assert ( 1 <= width && "Tried to create a 0 width page.");
    g_assert ( 1 <= height && "Tried to create a 0 height page.");

    m_Width = width;
    m_Height = height;

    gint dataSize = height * getRowStride();
    delete[] m_Data;
    m_Data = new guchar[dataSize];
    if ( NULL != m_Data )
    {
        memset (m_Data, 0xff, dataSize);
    }

    return NULL != m_Data;
}

///
/// @brief Marks a selection on the page.
///
/// This functions selects an area of the page as a selection and 
/// modifies the image to make it visible. For now it does invert the
/// colours of the selection.
///
/// This function also clears any previously set selection, if any.
///
/// @param selection The selection to set on the page. The coordinates
///                  are unscaled.
/// @param scale The scale to draw the selection.
///
void
DocumentPage::setSelection (DocumentRectangle &selection, gdouble scale)
{
    clearSelection ();

    m_SelectionX1 = MAX ((gint)(selection.getX1 () * scale - 0.5), 0);
    m_SelectionX2 = MAX ((gint)(selection.getX2 () * scale + 0.5), 1);
    m_SelectionY1 = MAX ((gint)(selection.getY1 () * scale - 0.5), 0);
    m_SelectionY2 = MAX ((gint)(selection.getY2 () * scale + 0.5), 1);

    invertArea (m_SelectionX1, m_SelectionY1, m_SelectionX2, m_SelectionY2);
    
    m_HasSelection = TRUE;
}

void
DocumentPage::setSelection (cairo_region_t *region)
{
    clearSelection ();
    
    invertRegion (region);
    
    m_Selection = cairo_region_copy(region);
}
