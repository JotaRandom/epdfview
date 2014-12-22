// ePDFView - Dumb Test Preferences View.
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
#include "DumbPreferencesView.h"

using namespace ePDFView;

DumbPreferencesView::DumbPreferencesView ():
    IPreferencesView ()
{
    m_BrowserCommandLine = g_strdup ("");
}

DumbPreferencesView::~DumbPreferencesView ()
{
    g_free (m_BrowserCommandLine);
}

const gchar *
DumbPreferencesView::getBrowserCommandLine ()
{
    return m_BrowserCommandLine;
}

////////////////////////////////////////////////////////////////
// Test Only Functions.
////////////////////////////////////////////////////////////////

void
DumbPreferencesView::setBrowserCommandLine (const gchar *commandLine)
{
    g_free (m_BrowserCommandLine);
    m_BrowserCommandLine = g_strdup (commandLine);
}
