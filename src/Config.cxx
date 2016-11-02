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
#include <glib/gstdio.h>
#include "epdfview.h"

using namespace ePDFView;

// Constants
static const gchar *DEFAULT_EXTERNAL_BROWSER_COMMAND_LINE = "x-www-browser %s";
static const gchar *DEFAULT_EXTERNAL_BACKSEARCH_COMMAND_LINE = "epdfsync -l %p %x %y %f";
static const gchar *DEFAULT_OPEN_FILE_FOLDER = NULL;
static const gchar *DEFAULT_SAVE_FILE_FOLDER = NULL;
static const gboolean DEFAULT_SHOW_MENUBAR = TRUE; //krogan edit
static const gboolean DEFAULT_INVERT_TOGGLE = FALSE; //krogan edit
static const gboolean DEFAULT_SHOW_STATUSBAR = TRUE;
static const gboolean DEFAULT_SHOW_TOOLBAR = FALSE;
static const gint DEFAULT_WINDOW_HEIGHT = 650;
static const gint DEFAULT_WINDOW_WIDTH = 600;
static const gint DEFAULT_WINDOW_X = 0;
static const gint DEFAULT_WINDOW_Y = 0;
static const gboolean DEFAULT_ZOOM_TO_FIT = FALSE;
static const gboolean DEFAULT_ZOOM_TO_WIDTH = FALSE;

// Static member attributes.
Config *Config::m_Config = NULL;
gboolean Config::m_LoadFile = TRUE;

// Forward declarations.
gchar *getConfigFileName (void);

///
/// @brief Constructs a new Config object.
///
/// It creates and load from a file the configuration, unless the
/// Config::loadFile() was called with FALSE.
///
Config::Config ()
{
    m_Values = g_key_file_new ();
    if ( m_LoadFile )
    {
        gchar *configFile = getConfigFileName ();
        GError *error = NULL;
        if ( !g_key_file_load_from_file (m_Values, configFile, 
                                         G_KEY_FILE_NONE, &error ) )
        {
            g_warning ("Couldn't load config file '%s': %s\n", configFile,
                       error->message);
            g_error_free (error);
        }
        g_free (configFile);
    }
}

///
/// @brief Destroys all dynamically allocated memory for Config.
///
Config::~Config ()
{
    g_key_file_free (m_Values);
}

///
/// @brief Destroys the configuration object.
///
/// This function is mainly used by the test suite to delete the 
/// singleton config object between test cases.
///
void
Config::destroy ()
{
    delete m_Config;
    m_Config = NULL;
}

///
/// @brief Gets the configuration object.
///
/// This is the main "entry point" for the configuration. All classes that
/// want to get configuration must call this function to retrieve the
/// configuration object. The first time this function is called creates
/// the object.
///
/// @return The reference to the configuration object.
///
Config &
Config::getConfig ()
{
    if ( NULL == m_Config )
    {
        m_Config = new Config;
    }

    g_assert (NULL != m_Config && "The configuration is NULL.");
    return *m_Config;
}

///
/// @brief Gets a boolean configuration option.
///
/// @param group Configuration group where the key belongs.
/// @param key The key name to retrieve its value.
/// @param defaultValue The default value to return if @a key doesn't exists.
///
/// @return The value of the @a key in @a group or @a defaultValue if @a key
///         doesn't exists.
///
gboolean
Config::getBoolean (const gchar *group, const gchar *key, gboolean defaultValue)
{
    gboolean value = defaultValue;

    if ( g_key_file_has_key (m_Values, group, key, NULL) )
    {
        GError *error = NULL;
        gboolean savedValue = 
            g_key_file_get_boolean (m_Values, group, key, &error);
        if ( NULL == error )
        {
            value = savedValue;
        }
        else
        {
            g_warning ("Error reading key '%s' from group '%s': %s\n", 
                       key, group, error->message);
            g_error_free (error);
        }
    }

    return value;
}

///
/// @brief Gets the command line to use to execute the external browser.
///
/// The external browser is used, for example, when the user clicks on
/// an external hyperlink in the document. The command line to execute
/// is given by this function and it should contains a %s in the place
/// where the URL should go.
///
/// @return The command line to use to execute the external browser. The
///         user must free this pointer.
///
gchar *
Config::getExternalBrowserCommandLine ()
{
    return getString ("command line", "browser",
                      DEFAULT_EXTERNAL_BROWSER_COMMAND_LINE);
}

/// paag
/// @brief Gets the command line to use to execute the external backsearch.
///
/// The external backsearch is used, when the PDF has an associated synctex.gz
/// file and you want to get pack to the LaTEX code with Ctrl.Click
/// %p will be replaced by the page
/// *x, %y the position in the page
/// %f the PDF file name
/// these parameters will be then fed to synctex to do the backsearch
/// Doing the actual search and opening up the editor is done by the scriot
/// NOT by epdfview
///
/// @return The command line to use to execute the backsearch script
///
 
gchar *
Config::getExternalBacksearchCommandLine ()
{
	return getString ("command line", "backsearch",
					  DEFAULT_EXTERNAL_BACKSEARCH_COMMAND_LINE);
}
 
///
/// @brief Gets an integer configuration option.
///
/// @param group Configuration group where the key belongs.
/// @param key The key name to retrieve its value.
/// @param defaultValue The default value to return if @a key doesn't exists.
///
/// @return The value of the @a key in @a group or @a defaultValue if @a key
///         doesn't exists.
///
gint
Config::getInteger (const gchar *group, const gchar *key, gint defaultValue)
{
    gint value = defaultValue;

    if ( g_key_file_has_key (m_Values, group, key, NULL) )
    {
        GError *error = NULL;
        gint savedValue = g_key_file_get_integer (m_Values, group, key, &error);
        if ( NULL == error )
        {
            value = savedValue;
        }
        else
        {
            g_warning ("Error reading key '%s' from group '%s': %s\n", 
                       key, group, error->message);
            g_error_free (error);
        }        
    }

    return value;
}

///
/// @brief Gets the last folder used to open a file.
///
/// @return The path to the last folder that was used to open a file.
///         This string must be freed using g_free(). NULL if no folder
///         has been used yet, which means to use the current working directory.
///
gchar *
Config::getOpenFileFolder ()
{
    return getString ("open dialog", "folder", DEFAULT_OPEN_FILE_FOLDER);
}

///
/// @brief Gets the last folder used to save a file.
///
/// @return The path to the last folder that was used to save a file.
///         This string must be freed using g_free(). NULL if no folder
///         has been used yet, which means to use the current working directory.
///
gchar *
Config::getSaveFileFolder ()
{
    return getString ("save dialog", "folder", DEFAULT_SAVE_FILE_FOLDER);
}

///
/// @brief Gets an string configuration option.
///
/// @param group Configuration group where the key belongs.
/// @param key The key name to retrieve its value.
/// @param defaultValue The default value to return if @a key doesn't exists.
///
/// @return The value of the @a key in @a group or @a defaultValue if @a key
///         doesn't exists.
///
gchar *
Config::getString (const gchar *group, const gchar *key, 
                   const gchar *defaultValue)
{
    gchar *value = g_strdup (defaultValue);
    if ( g_key_file_has_key (m_Values, group, key, NULL) )
    {
        GError *error = NULL; 
        gchar *savedValue = 
            g_key_file_get_string (m_Values, group, key, &error);
        if ( NULL == error )
        {
            g_free (value);
            value = savedValue;
        }
        else
        {
            g_warning ("Error reading key '%s' from group '%s': %s\n", 
                       key, group, error->message);
            g_error_free (error);
        }
    }

    return value;
}

///
/// @brief Gets the main window's height.
///
/// @return The saved height value of the main window.
///
gint
Config::getWindowHeight ()
{
    return getInteger ("main window", "height", DEFAULT_WINDOW_HEIGHT);
}

///
/// @brief Gets the main window's width.
///
/// @return The saved width value of the main window.
///
gint
Config::getWindowWidth ()
{
    return getInteger ("main window", "width", DEFAULT_WINDOW_WIDTH);
}

///
/// @brief Gets main window's X position.
///
/// @return The saved X position value of the main window.
///
gint
Config::getWindowX ()
{    
    return getInteger ("main window", "x", DEFAULT_WINDOW_X);
}

///
/// @brief Gets main window's Y position.
///
/// @return the saved Y position value of the main window.
///
gint
Config::getWindowY ()
{
    return getInteger ("main window", "y", DEFAULT_WINDOW_Y);
}

///
/// @brief Sets if load the configuration value.
///
/// This is mainly for testing only purposes. If it's called before
/// the first call to getConfig(), then the configuration file is not 
/// loaded.
///
/// @param load Set to TRUE to load the file, to FALSE to prevent loading it.
///
void
Config::loadFile (gboolean load)
{
    m_LoadFile = load;
}

///
/// @brief Save the current configuration to a file.
///
void
Config::save ()
{
    gsize length = 0;
    gchar *contents = g_key_file_to_data (m_Values, &length, NULL);
    gchar *configFileName = getConfigFileName ();
    FILE *configFile = g_fopen (configFileName, "w+t");
    if ( NULL != configFile )
    {
        fwrite (contents, sizeof (gchar), length, configFile);
        fclose (configFile);
    }
    g_free (contents);
    g_free (configFileName);
}

///
/// @brief Saves the command line to use to execute the external browser.
///
/// @param commandLine the browser's command line to save.
///
void
Config::setExternalBrowserCommandLine (const gchar *commandLine)
{
    g_key_file_set_string (m_Values, "command line", "browser", commandLine);
}

/// paag
/// @brief Saves the command line to use to execute the external backsearch script
///
/// @param commandLine the backsearch command line to save.
///
void
Config::setExternalBacksearchCommandLine (const gchar *commandLine)
{
	g_key_file_set_string (m_Values, "command line", "backsearch", commandLine);
}

///
/// @brief Saves the last folder used to open a file.
///
/// @param folder The path to the last folder used.
///
void
Config::setOpenFileFolder (const gchar *folder)
{
    g_key_file_set_string (m_Values, "open dialog", "folder", folder);
}

///
/// @brief Saves the last folder used to osave a file.
///
/// @param folder The path to the last folder used.
///
void
Config::setSaveFileFolder (const gchar *folder)
{
    g_key_file_set_string (m_Values, "save dialog", "folder", folder);
}

/// krogan custom edit
/// @brief Save if show the menu bar.
///
/// @param show TRUE to show the status bar, FALSE otherwise.
///
void
Config::setShowMenubar (gboolean show)
{
    g_key_file_set_boolean (m_Values, "main window", "showMenubar", show);
}

/// krogan custom edit
/// @brief Save if invert page colors.
///
/// @param show TRUE to show the status bar, FALSE otherwise.
///
void
Config::setInvertToggle (gboolean on)
{
    g_key_file_set_boolean (m_Values, "main window", "invertToggle", on);
}

///
/// @brief Save if show the status bar.
///
/// @param show TRUE to show the status bar, FALSE otherwise.
///
void
Config::setShowStatusbar (gboolean show)
{
    g_key_file_set_boolean (m_Values, "main window", "showStatusbar", show);
}

///
/// @brief Saves if show the tool bar.
///
/// @param show TRUE to show the tool bar, FALSE otherwise.
///
void
Config::setShowToolbar (gboolean show)
{
    g_key_file_set_boolean (m_Values, "main window", "showToolbar", show);
}

///
/// @brief Saves the current window's size.
///
/// @param width The window's width.
/// @param height The window's height.
///
void
Config::setWindowSize (gint width, gint height)
{
    g_key_file_set_integer (m_Values, "main window", "width", width);
    g_key_file_set_integer (m_Values, "main window", "height", height);
}

///
/// @brief Saves the current window's position.
///
/// @param x The X position of the window.
/// @param y The Y position of the window.
///
void
Config::setWindowPos (gint x, gint y)
{
    g_key_file_set_integer (m_Values, "main window", "x", x);
    g_key_file_set_integer (m_Values, "main window", "y", y);
}

///
/// @brief Sets the zoom to fit option.
///
/// @param activate Set to TRUE to activate the zoom to fit option. FALSE
///        otherwise.
///
void
Config::setZoomToFit (gboolean activate)
{
    g_key_file_set_boolean (m_Values, "main window", "zoomToFit", activate);
    g_key_file_set_boolean (m_Values, "main window", "zoomToWidth", 
            !activate && zoomToWidth ());
}

///
/// @brief Sets the zoom to with option.
///
/// @param activate Set to TRUE to activate the zoom to width option. FALSE
///        otherwise.
///
void
Config::setZoomToWidth (gboolean activate)
{
    g_key_file_set_boolean (m_Values, "main window", "zoomToFit",
            !activate && zoomToFit ());
    g_key_file_set_boolean (m_Values, "main window", "zoomToWidth", activate);
}

///krogan custom edit
/// @brief Gets if show the menu bar.
///
/// @return TRUE if the menu bar should be shown, FALSE otherwise.
///
gboolean
Config::showMenubar ()
{
    return getBoolean ("main window", "showMenubar", DEFAULT_SHOW_MENUBAR);
}

///krogan custom edit
/// @brief Gets if show the menu bar.
///
/// @return TRUE if the menu bar should be shown, FALSE otherwise.
///
gboolean
Config::invertToggle ()
{
    return getBoolean ("main window", "invertToggle", DEFAULT_INVERT_TOGGLE);
}

///
/// @brief Gets if show the status bar.
///
/// @return TRUE if the status bar should be shown, FALSE otherwise.
///
gboolean
Config::showStatusbar ()
{
    return getBoolean ("main window", "showStatusbar", DEFAULT_SHOW_STATUSBAR);
}

///
/// @brief Gets if show the tool bar
///
/// @return TRUE if the tool bar should be shown, FALSE otherwise.
///
gboolean
Config::showToolbar ()
{
    return getBoolean ("main window", "showToolbar", DEFAULT_SHOW_TOOLBAR);
}

///
/// @brief Gets the zoom to fit option.
///
/// @return TRUE if the zoom to fit option is activated. FALSE otherwise.
///
gboolean
Config::zoomToFit ()
{
    return getBoolean ("main window", "zoomToFit", DEFAULT_ZOOM_TO_FIT);
}

///
/// @brief Gets the zoom to width option.
///
/// @return TRUE if the zoom to width option is activated. FALSE otherwise.
///
gboolean
Config::zoomToWidth ()
{
    return getBoolean ("main window", "zoomToWidth", DEFAULT_ZOOM_TO_WIDTH);
}

///
/// @brief Gets the configuration file name.
///
/// This function creates the directory where the configuration file should
/// go, if it doesn't exist yet.
///
/// @return The full path to the configuration file.
///
gchar *
getConfigFileName ()
{
    gchar *configDir = 
        g_build_filename (g_get_user_config_dir (), PACKAGE, NULL);
    g_mkdir_with_parents (configDir, 0700);
    gchar *configFile = g_build_filename (configDir, "main.conf", NULL);
    g_free (configDir);

    return configFile;
}
