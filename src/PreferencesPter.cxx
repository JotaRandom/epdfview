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

#include <config.h>
#include "epdfview.h"

using namespace ePDFView;

PreferencesPter::PreferencesPter ()
{
    m_View = NULL;
}

PreferencesPter::~PreferencesPter ()
{
    delete m_View;
}

IPreferencesView &
PreferencesPter::getView ()
{
    g_assert ( NULL != m_View && "The preferences' view is NULL.");

    return *m_View;
}

void
PreferencesPter::setView (IPreferencesView *view)
{
    m_View = view;
    getView ().setPresenter (this);
}

void
PreferencesPter::browserCommandLineChanged ()
{
    const gchar *commandLine = getView ().getBrowserCommandLine ();
    Config::getConfig ().setExternalBrowserCommandLine (commandLine);
}

void
PreferencesPter::backsearchCommandLineChanged ()
{
    const gchar *commandLine = getView ().getBacksearchCommandLine ();
    Config::getConfig ().setExternalBacksearchCommandLine (commandLine);
}

void
PreferencesPter::closeActivated ()
{
    delete this;
}
