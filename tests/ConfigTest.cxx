// ePDFView - Configuration Test Fixture.
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
#include "ConfigTest.h"

using namespace ePDFView;

// Register the test suite into the `registry'.
CPPUNIT_TEST_SUITE_REGISTRATION (ConfigTest);

///
/// @brief Sets up the environment for each test.
///
/// Since the configuration is a singleton, we don't need to set up
/// it.
///
void
ConfigTest::setUp ()
{
    Config::loadFile (FALSE);
}

///
/// @brief Cleans up after each test.
///
void
ConfigTest::tearDown ()
{
    Config::destroy ();
}

///
/// @brief Checks the defaults values (i.e., no values read from a file)
///
void
ConfigTest::defaultValues ()
{
    Config &config = Config::getConfig ();

    CPPUNIT_ASSERT_EQUAL (0, config.getWindowX ());
    CPPUNIT_ASSERT_EQUAL (0, config.getWindowY ());
    CPPUNIT_ASSERT_EQUAL (600, config.getWindowWidth ());
    CPPUNIT_ASSERT_EQUAL (650, config.getWindowHeight ());
    CPPUNIT_ASSERT (config.showToolbar ());
    CPPUNIT_ASSERT (config.showStatusbar ());
    CPPUNIT_ASSERT_EQUAL ((gchar *)NULL, config.getOpenFileFolder ());
    CPPUNIT_ASSERT_EQUAL ((gchar *)NULL, config.getSaveFileFolder ());
    CPPUNIT_ASSERT (!config.zoomToWidth ());
    CPPUNIT_ASSERT (!config.zoomToFit ());

    gchar *commandLine = config.getExternalBrowserCommandLine ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("firefox %s", commandLine));
    g_free (commandLine);
}

///
/// @brief Check setting and retrieving values related to the main window.
///
void
ConfigTest::windowValues ()
{
    Config &config = Config::getConfig ();

    config.setWindowPos (30, 40);
    config.setWindowSize (100, 90);

    CPPUNIT_ASSERT_EQUAL (30, config.getWindowX ());
    CPPUNIT_ASSERT_EQUAL (40, config.getWindowY ());
    CPPUNIT_ASSERT_EQUAL (100, config.getWindowWidth ());
    CPPUNIT_ASSERT_EQUAL (90, config.getWindowHeight ());    
}

///
/// @brief Checks setting the value for showing the toolbar.
///
void
ConfigTest::showToolbar ()
{
    Config &config = Config::getConfig ();

    config.setShowToolbar (FALSE);
    CPPUNIT_ASSERT ( !config.showToolbar () ); 
    config.setShowToolbar (TRUE);
    CPPUNIT_ASSERT ( config.showToolbar () ); 
}

///
/// @brief Checks setting the value for showing the status bar.
///
void
ConfigTest::showStatusbar ()
{
    Config &config = Config::getConfig ();

    config.setShowStatusbar (FALSE);
    CPPUNIT_ASSERT ( !config.showStatusbar () );
    config.setShowStatusbar (TRUE);
    CPPUNIT_ASSERT ( config.showStatusbar () );
}

///
/// @brief Check setting the current folder for opening files.
///
void
ConfigTest::openCurrentFolder ()
{
    Config &config = Config::getConfig ();

    config.setOpenFileFolder ("/tmp");
    gchar *openFolder = config.getOpenFileFolder ();
    CPPUNIT_ASSERT ( 0 == g_ascii_strcasecmp ("/tmp", openFolder));
    g_free (openFolder);
}

///
/// @brief Check setting the current folder for saving files.
///
void
ConfigTest::saveCurrentFolder ()
{
    Config &config = Config::getConfig ();

    config.setSaveFileFolder ("/home");
    gchar *saveFolder = config.getSaveFileFolder ();
    CPPUNIT_ASSERT ( 0 == g_ascii_strcasecmp ("/home", saveFolder));
    g_free (saveFolder);
}

///
/// @brief Check setting the zoom values.
///
/// Especially check that zoomToWidth() and zoomToFit() can't be
/// set both to true.
///
void
ConfigTest::zoomValues ()
{
    Config &config = Config::getConfig ();
    
    config.setZoomToFit (TRUE);
    CPPUNIT_ASSERT ( !config.zoomToWidth () );
    CPPUNIT_ASSERT ( config.zoomToFit () );
    config.setZoomToFit (FALSE);
    CPPUNIT_ASSERT ( !config.zoomToWidth () );
    CPPUNIT_ASSERT ( !config.zoomToFit () );
    config.setZoomToWidth (TRUE);
    CPPUNIT_ASSERT ( config.zoomToWidth () );
    CPPUNIT_ASSERT ( !config.zoomToFit () );
    config.setZoomToWidth (FALSE);
    CPPUNIT_ASSERT ( !config.zoomToWidth () );
    CPPUNIT_ASSERT ( !config.zoomToFit () );
    config.setZoomToFit (TRUE);
    CPPUNIT_ASSERT ( !config.zoomToWidth () );
    CPPUNIT_ASSERT ( config.zoomToFit () );
    config.setZoomToWidth (TRUE);
    CPPUNIT_ASSERT ( config.zoomToWidth () );
    CPPUNIT_ASSERT ( !config.zoomToFit () );
    config.setZoomToFit (TRUE);
    CPPUNIT_ASSERT ( !config.zoomToWidth () );
    CPPUNIT_ASSERT ( config.zoomToFit () );
    config.setZoomToFit (FALSE);
    CPPUNIT_ASSERT ( !config.zoomToWidth () );
    CPPUNIT_ASSERT ( !config.zoomToFit () );
}

///
/// @brief Checks setting the external browser command line.
///
void
ConfigTest::externalBrowser ()
{
    Config &config = Config::getConfig ();

    config.setExternalBrowserCommandLine ("galeon %s");
    gchar *commandLine = config.getExternalBrowserCommandLine ();
    CPPUNIT_ASSERT ( 0 == g_ascii_strcasecmp ("galeon %s", commandLine));
    g_free (commandLine);

    config.setExternalBrowserCommandLine ("xterm -e lynx %s");
    commandLine = config.getExternalBrowserCommandLine ();
    CPPUNIT_ASSERT ( 0 == g_ascii_strcasecmp ("xterm -e lynx %s", commandLine));
    g_free (commandLine);
}
