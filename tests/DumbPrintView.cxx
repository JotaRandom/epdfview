// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Dumb Test Preferences View.
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
#include "DumbPrintView.h"

using namespace ePDFView;

DumbPrintView::DumbPrintView ():
    IPrintView ()
{
    m_AllPagesRangeOptionSelected = TRUE;
    m_NumberOfCopies = 1;
    m_SensitiveCollate = TRUE;
    m_SensitivePageRange = TRUE;
}

DumbPrintView::~DumbPrintView ()
{
}

void
DumbPrintView::addColorModel (const gchar *name, const gchar *value)
{
}

void
DumbPrintView::addPageSize (const gchar *name, const gchar *value)
{
}

void
DumbPrintView::addPrinter (const gchar *name, int jobs, const gchar *state,
                           const gchar *location)
{
}

void
DumbPrintView::addResolution (const gchar *name, const gchar *value)
{
}


void
DumbPrintView::clearColorModelList ()
{
}

void
DumbPrintView::clearPageSizeList ()
{
}

void
DumbPrintView::clearResolutionList ()
{
}

guint
DumbPrintView::getNumberOfCopies ()
{
    return m_NumberOfCopies;
}

gchar*
DumbPrintView::getColorModel ()
{
    return g_strdup ("");
}

PrintPageLayout
DumbPrintView::getPageLayout ()
{
    return PRINT_PAGE_LAYOUT_PLAIN;
}

PrintPageOrientation
DumbPrintView::getPageOrientation ()
{
    return PRINT_PAGE_ORIENTATION_PORTRAIT;
}

gchar *
DumbPrintView::getResolution ()
{
    return g_strdup ("");
}

const gchar *
DumbPrintView::getPageRange ()
{
    return "";
}

gchar *
DumbPrintView::getPageSize ()
{
    return NULL;
}

gchar *
DumbPrintView::getSelectedPrinterName (void)
{
    return NULL;
}

gboolean
DumbPrintView::isCheckedCollate ()
{
    return FALSE;
}

gboolean
DumbPrintView::isSelectedAllPagesRangeOption ()
{
    return m_AllPagesRangeOptionSelected;
}

gboolean
DumbPrintView::isSelectedEvenPageSet ()
{
    return FALSE;
}

gboolean
DumbPrintView::isSelectedOddPageSet ()
{
    return FALSE;
}

void
DumbPrintView::selectColorModel (guint colorModelIndex)
{
}

void
DumbPrintView::selectPageSize (guint pageSizeIndex)
{
}

void
DumbPrintView::selectPrinter (guint printerIndex)
{
}

void
DumbPrintView::selectResolution (guint resolutionIndex)
{
}

void
DumbPrintView::sensitiveCollate (gboolean sensitive)
{
    m_SensitiveCollate = sensitive;
}

void
DumbPrintView::sensitivePageRange (gboolean sensitive)
{
    m_SensitivePageRange = sensitive;
}

void
DumbPrintView::sensitivePrintButton (gboolean sensitive)
{
}

////////////////////////////////////////////////////////////////
// Test Only Functions
////////////////////////////////////////////////////////////////

gboolean
DumbPrintView::isSensitiveCollate ()
{
    return m_SensitiveCollate;
}

gboolean
DumbPrintView::isSensitivePageRange ()
{
    return m_SensitivePageRange;
}

void
DumbPrintView::selectAllPagesRangeOption ()
{
    m_AllPagesRangeOptionSelected = TRUE;
}

void
DumbPrintView::selectCustomPagesRangeOption ()
{
    m_AllPagesRangeOptionSelected = FALSE;
}

void
DumbPrintView::setNumberOfCopies (unsigned int copies)
{
    m_NumberOfCopies = copies;
}
