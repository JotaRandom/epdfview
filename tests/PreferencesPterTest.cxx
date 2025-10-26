// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Preferences Presenter Test Fixture.
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
#include "DumbPreferencesView.h"
#include "PreferencesPterTest.h"

using namespace ePDFView;

// Register the test suite into the `registry'.
CPPUNIT_TEST_SUITE_REGISTRATION (PreferencesPterTest);

///
/// @brief Sets up the environment for each test.
///
void
PreferencesPterTest::setUp ()
{
    Config::loadFile (FALSE);
    m_PreferencesPter = new PreferencesPter ();
    m_View = new DumbPreferencesView ();
    m_PreferencesPter->setView (m_View);
}

///
/// @brief Cleans up after each test.
///
void
PreferencesPterTest::tearDown ()
{
    Config::destroy ();
    // Telling the presenter to close will delete the view and the presenter.
    m_PreferencesPter->closeActivated ();
    m_PreferencesPter = NULL;
    m_View = NULL;
}

///
/// @brief Test setting the external browser command line.
///
void
PreferencesPterTest::externalCommandBrowser ()
{
    Config &config = Config::getConfig ();

    gchar *commandLine = config.getExternalBrowserCommandLine ();
    CPPUNIT_ASSERT ( 0 == g_ascii_strcasecmp ("firefox %s", commandLine));
    g_free (commandLine);

    m_View->setBrowserCommandLine ("Eterm links %s");
    m_PreferencesPter->browserCommandLineChanged ();

    commandLine = config.getExternalBrowserCommandLine ();
    CPPUNIT_ASSERT ( 0 == g_ascii_strcasecmp ("Eterm links %s", commandLine));
    g_free (commandLine);
}
