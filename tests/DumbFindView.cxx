// ePDFView - Dumb Test Find View.
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

#include <epdfview.h>
#include "DumbFindView.h"

using namespace ePDFView;

DumbFindView::DumbFindView ():
    IFindView ()
{
    m_FindNextSensitive = TRUE;
    m_FindNextSensitive = TRUE;
    m_InformationText = g_strdup ("");
    m_TextToFind = g_strdup ("");
}

DumbFindView::~DumbFindView ()
{
    g_free (m_InformationText);
    g_free (m_TextToFind);
}

const gchar *
DumbFindView::getTextToFind ()
{
    return m_TextToFind;
}

void
DumbFindView::hide ()
{
}

void
DumbFindView::sensitiveFindNext (gboolean sensitive)
{
    m_FindNextSensitive = sensitive;
}

void
DumbFindView::sensitiveFindPrevious (gboolean sensitive)
{
    m_FindPreviousSensitive = sensitive;
}

void
DumbFindView::setInformationText (const gchar *text)
{
    g_free (m_InformationText);
    m_InformationText = g_strdup (text);
}

////////////////////////////////////////////////////////////////
// Test Only Functions.
////////////////////////////////////////////////////////////////

const gchar *
DumbFindView::getInformationText ()
{
    return m_InformationText;
}

gboolean
DumbFindView::isFindNextSensitive ()
{
    return m_FindNextSensitive;
}

gboolean
DumbFindView::isFindPreviousSensitive ()
{
    return m_FindPreviousSensitive;
}

void
DumbFindView::setTextToFind (const gchar *text)
{
    g_free (m_TextToFind);
    m_TextToFind = g_strdup (text);
    getPresenter ()->textToFindChanged ();
}
