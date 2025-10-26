// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Dumb Test Page View.
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

#include <epdfview.h>
#include "DumbPageView.h"

using namespace ePDFView;

DumbPageView::DumbPageView ():
    IPageView ()
{
    m_HorizontalScroll = 0.0;
    m_VerticalScroll = 0.0;
}

DumbPageView::~DumbPageView ()
{
}

gdouble
DumbPageView::getHorizontalScroll ()
{
    return m_HorizontalScroll;
}

void
DumbPageView::getSize (gint *width, gint *height)
{
    *width = 75;
    *height = 50;
}

gdouble
DumbPageView::getVerticalScroll ()
{
    return m_VerticalScroll;
}

void
DumbPageView::makeRectangleVisible (DocumentRectangle &rect, gdouble scale)
{
}

void
DumbPageView::resizePage (gint width, gint height)
{
}

void
DumbPageView::scrollPage (gdouble scrollX, gdouble scrollY, gint dx, gint dy)
{
    m_HorizontalScroll = (gdouble)dx;
    m_VerticalScroll = (gdouble)dy;
}

void
DumbPageView::setCursor (PageCursor cursorType)
{
}

void
DumbPageView::showPage (DocumentPage *page, PageScroll scroll)
{
}

void
DumbPageView::showText (const gchar *text)
{
}
