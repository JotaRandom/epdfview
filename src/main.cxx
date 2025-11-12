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
    // Initialize background job dispatcher (queue + thread)
    IJob::init();
    appData->mainPter = new MainPter (appData->document);
    
    // Create the main view
    appData->mainView = new MainView (appData->mainPter);
    
    // Let the presenter know which is its view
    appData->mainPter->setView (appData->mainView);
    
    // GTK4: Register the window with the application (prevents immediate exit)
    gtk_application_add_window (app, GTK_WINDOW (appData->mainView->getMainWindow()));
    
    // Setup keyboard shortcuts
    const char *quit_accels[] = { "<Control>Q", NULL };
    gtk_application_set_accels_for_action (app, "win.quit", quit_accels);
    
    const char *open_accels[] = { "<Control>O", NULL };
    gtk_application_set_accels_for_action (app, "win.open-file", open_accels);
    
    const char *find_accels[] = { "<Control>F", NULL };
    gtk_application_set_accels_for_action (app, "win.find", find_accels);
    
    const char *reload_accels[] = { "<Control>R", NULL };
    gtk_application_set_accels_for_action (app, "win.reload-file", reload_accels);
    
    const char *print_accels[] = { "<Control>P", NULL };
    gtk_application_set_accels_for_action (app, "win.print", print_accels);
    
    const char *fullscreen_accels[] = { "F11", NULL };
    gtk_application_set_accels_for_action (app, "win.fullscreen", fullscreen_accels);
    
    // Navigation shortcuts
    const char *next_page_accels[] = { "Page_Down", "<Control>Page_Down", NULL };
    gtk_application_set_accels_for_action (app, "win.go-next", next_page_accels);
    
    const char *prev_page_accels[] = { "Page_Up", "<Control>Page_Up", NULL };
    gtk_application_set_accels_for_action (app, "win.go-previous", prev_page_accels);
    
    const char *first_page_accels[] = { "Home", "<Control>Home", NULL };
    gtk_application_set_accels_for_action (app, "win.go-first", first_page_accels);
    
    const char *last_page_accels[] = { "End", "<Control>End", NULL };
    gtk_application_set_accels_for_action (app, "win.go-last", last_page_accels);
    
    // Zoom shortcuts
    const char *zoom_in_accels[] = { "<Control>plus", "<Control>equal", "<Control>KP_Add", NULL };
    gtk_application_set_accels_for_action (app, "win.zoom-in", zoom_in_accels);
    
    const char *zoom_out_accels[] = { "<Control>minus", "<Control>KP_Subtract", NULL };
    gtk_application_set_accels_for_action (app, "win.zoom-out", zoom_out_accels);
    
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
}static void
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
    (void)bindtextdomain (PACKAGE, LOCALEDIR);
    (void)bind_textdomain_codeset (PACKAGE, "UTF-8");
    (void)textdomain (PACKAGE);
    
    // Initialise the working thread.
    IJob::init ();
    
    // Create the GTK4 application
    // Note: G_APPLICATION_NON_UNIQUE allows multiple instances
    // G_APPLICATION_HANDLES_COMMAND_LINE allows us to handle command line args
    GtkApplication *app = gtk_application_new ("org.emma-soft.epdfview",
                                               G_APPLICATION_NON_UNIQUE);
    
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
