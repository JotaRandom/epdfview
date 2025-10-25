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
#include <stdlib.h>
#include <gettext.h>
#include <locale.h>
#include <gtk/gtk.h>
#ifndef _WIN32
#include <glib-unix.h>
#endif
#include "epdfview.h"
#include "gtk/MainView.h"

// GTK4 initialization - no compatibility layer needed

#ifdef _WIN32
#include <io.h>
#include <conio.h>
#define _WIN32_WINNT 0x0500
#include <windows.h>
#endif

using namespace ePDFView;

// Application data structure
struct AppData
{
    GtkApplication *app;
    gchar *fileToOpen;
    MainPter *mainPter;
    PDFDocument *document;
    MainView *mainView;
};

static int
loadFileFromCommandLine (gpointer data)
{
    AppData *appData = static_cast<AppData *> (data);

    appData->mainPter->setOpenState (appData->fileToOpen, FALSE);
    appData->document->load (appData->fileToOpen, NULL);

    return FALSE;
}

#ifndef _WIN32
static gboolean
handleReloadSignal(gpointer data)
{
    MainPter *mainPter = static_cast<MainPter *> (data);

	mainPter->reloadActivated ();
	return TRUE;
}
#endif

// GTK4 Application callbacks
static void
on_activate (GtkApplication *app, gpointer user_data)
{
    AppData *appData = static_cast<AppData *> (user_data);
    
    // Create the main presenter and document
    appData->document = new PDFDocument;
    appData->mainPter = new MainPter (appData->document);
    
    // Create the main view
    appData->mainView = new MainView (appData->mainPter);
    
    // Let the presenter know which is its view
    appData->mainPter->setView (appData->mainView);
    
    // Show the main window (GTK4: windows are hidden by default)
    appData->mainView->show ();
    
    // If we have a file to open, schedule it
    if (appData->fileToOpen != NULL)
    {
        g_idle_add (loadFileFromCommandLine, appData);
    }
    
#ifndef _WIN32
    g_unix_signal_add(SIGHUP, handleReloadSignal, appData->mainPter);
#endif
}

static void
on_shutdown (GtkApplication *app, gpointer user_data)
{
    AppData *appData = static_cast<AppData *> (user_data);
    
    // Delete the main presenter (which also deletes the view)
    if (appData->mainPter != NULL)
    {
        delete appData->mainPter;
        appData->mainPter = NULL;
    }
    
    // Save the configuration
    Config::getConfig().save ();
}

int
main (int argc, char **argv)
{
#ifdef _WIN32

    if (fileno (stdout) != -1 &&
 	  _get_osfhandle (fileno (stdout)) != -1)
    {
      /* stdout is fine, presumably redirected to a file or pipe */
    }
    else
    {
      typedef BOOL (* WINAPI AttachConsole_t) (DWORD);

      AttachConsole_t p_AttachConsole =
        (AttachConsole_t) GetProcAddress (GetModuleHandle ("kernel32.dll"), "AttachConsole");

      if (p_AttachConsole != NULL && p_AttachConsole (ATTACH_PARENT_PROCESS))
      {
          freopen ("CONOUT$", "w", stdout);
          dup2 (fileno (stdout), 1);
          freopen ("CONOUT$", "w", stderr);
          dup2 (fileno (stderr), 2);

      }
    }
#endif

    // Enable NLS.
    setlocale (LC_ALL, "");
    bindtextdomain (PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (PACKAGE, "UTF-8");
    textdomain (PACKAGE);
    
    // Initialise the working thread.
    IJob::init ();
    
    // Create the GTK4 application
    GtkApplication *app = gtk_application_new ("org.emma-soft.epdfview",
                                               G_APPLICATION_HANDLES_OPEN);
    
    // Set application name
    g_set_application_name (_("PDF Viewer"));
    
    // Initialize application data
    AppData appData = { 0 };
    appData.app = app;
    appData.fileToOpen = NULL;
    appData.mainPter = NULL;
    appData.document = NULL;
    appData.mainView = NULL;
    
    // Check if we have a file to open from command line
    if (argc > 1)
    {
        appData.fileToOpen = g_strdup (argv[1]);
    }
    
    // Connect application signals
    g_signal_connect (app, "activate", G_CALLBACK (on_activate), &appData);
    g_signal_connect (app, "shutdown", G_CALLBACK (on_shutdown), &appData);
    
    // Run the application
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    
    // Cleanup
    if (appData.fileToOpen != NULL)
    {
        g_free (appData.fileToOpen);
    }
    g_object_unref (app);
    
    return status;
}
