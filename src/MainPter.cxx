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
#include <unistd.h>
#include "epdfview.h"
#include <stdio.h>
using namespace ePDFView;

#if defined (DEBUG)
G_LOCK_DEFINE_STATIC (fileLoaded);
static volatile gboolean fileLoaded;
G_LOCK_DEFINE_STATIC (fileSaved);
static volatile gboolean fileSaved;
#endif // DEBUG

///
/// @brief Constructs a new main presenter with a default document.
///
MainPter::MainPter ()
{
    MainPter (new PDFDocument ());
}

///
/// @brief Constructs a new main presenter with the give document.
///
/// @param document The document to view. It doesn't have to be loaded,
///                 as it can be used for testing purposes.
///                 This document will be freed by the presenter.
///
MainPter::MainPter (IDocument *document)
{
    g_assert (NULL != document && "Tried to set a NULL document");

    m_Document = document;
    m_Document->attach (this);
    m_View = NULL;
    m_PagePter = NULL;
    m_FindPter = NULL;
    m_PasswordTries = 3;
    m_ReloadPage = 1;
#if defined (DEBUG)
    G_LOCK (fileLoaded);
    fileLoaded = FALSE;
    G_UNLOCK (fileLoaded);
    G_LOCK (fileSaved);
    fileSaved = FALSE;
    G_UNLOCK (fileSaved);
#endif
}

///
/// @brief Deletes all dynamically allocated memory for the presenter.
///
MainPter::~MainPter ()
{
    // Need to delete the page presenter first before deleting
    // the document, because the page presenter will detach itself
    // from the document.
    delete m_PagePter;
    m_Document->detach (this);
    delete m_Document;
    delete m_View;
}

///
/// @brief: return TRUE is a document is loaded
///
gboolean
MainPter::isDocumentLoaded(void)
{
	gboolean result = FALSE;
	if (NULL != m_Document)
		result =  m_Document -> isLoaded();
	// printf("in MainPter::isDocumentLoaded() result = %d", result);
	return result;
}
///
/// @brief Checks the zoom settings.
///
/// This function is called when the page rotation changes or the page
/// number changes to check if the zoom to width or zoom to fit options
/// are on and do again the zoom to width or zoom to fit action.
///
void
MainPter::checkZoomSettings ()
{
    Config &config = Config::getConfig ();
    if ( config.zoomToFit () )
    {
        zoomFit ();
    }
    else if ( config.zoomToWidth () )
    {
        zoomWidth ();
    }
}

///
/// @brief Initialises the view with the initial application's state.
///
/// Normally that will make all action except "Open File" insensitive, will
/// set the window's title and tell the main view to show itself.
///
void
MainPter::setInitialState ()
{
    g_assert ( NULL != m_Document && "The document is NULL.");

    IMainView &view = getView ();
    Config &config = Config::getConfig ();
    gboolean showSidebar = FALSE;
    if ( m_Document->isLoaded () )
    {
        if ( 0 == g_ascii_strcasecmp ("", m_Document->getTitle ()) )
        {
            view.setTitle (m_Document->getFileName ());
        }
        else
        {
            view.setTitle (m_Document->getTitle ());
        }
        setZoomText (m_Document->getZoom ());
        view.sensitiveFind (TRUE);
        view.sensitiveGoToPage (TRUE);
        view.sensitiveReload (TRUE);
        view.sensitiveRotateRight (TRUE);
        view.sensitiveRotateLeft (TRUE);
        view.sensitiveSave (TRUE);
        view.sensitiveZoom (TRUE);
        view.sensitiveZoomIn (TRUE);
        view.sensitiveZoomOut (TRUE);
        view.sensitiveZoomFit (TRUE);
        view.sensitiveZoomWidth (TRUE);
        view.activeZoomFit (config.zoomToFit ());
        view.activeZoomWidth (config.zoomToWidth ());
#if defined (HAVE_CUPS)
        view.sensitivePrint (TRUE);
#endif // HAVE_CUPS
        view.sensitiveFullScreen (TRUE);

        checkZoomSettings ();

        // Check if we should see the outlines.
        showSidebar = (0 < m_Document->getOutline ()->getNumChildren () &&
                       PageModeOutlines == m_Document->getPageMode ());
        // IDocument::getOutline is set if and only if m_Document is
        // loaded.
        view.setOutline (m_Document->getOutline ());
    }
    else
    {
        view.setTitle (_("PDF Viewer"));
        setZoomText (1.0);
        view.sensitiveFind (FALSE);
        view.sensitiveGoToFirstPage (FALSE);
        view.sensitiveGoToLastPage (FALSE);
        view.sensitiveGoToNextPage (FALSE);
        view.sensitiveGoToPage (FALSE);
        view.sensitiveGoToPreviousPage (FALSE);
        view.sensitiveReload (FALSE);
        view.sensitiveRotateRight (FALSE);
        view.sensitiveRotateLeft (FALSE);
        view.sensitiveSave (FALSE);
        view.sensitiveZoom (FALSE);
        view.sensitiveZoomIn (FALSE);
        view.sensitiveZoomOut (FALSE);
        view.sensitiveZoomFit (FALSE);
        view.sensitiveZoomWidth (FALSE);
        showSidebar = FALSE;
#if defined (HAVE_CUPS)
        view.sensitivePrint (FALSE);
#endif // HAVE_CUPS
        view.sensitiveFullScreen (FALSE);
    }

    // Sensitive the open file action.
    view.sensitiveOpen (TRUE);
    // Show the toolbar,menu and status bar depending on the configuration.
	// printf("menubar is %s\n", config.showMenubar() ? "on" : "off");
    view.showMenubar (config.showMenubar ()); //krogan wuz here
    view.invertToggle (config.invertToggle ()); //krogan
    //(config.invertToggle == TRUE) ? m_PagePter->:;
    view.showToolbar (config.showToolbar ());
    view.showStatusbar (config.showStatusbar ());
    view.showIndex (showSidebar);
    // Remove the status bar text.
    view.setStatusBarText (NULL);
    // Show the normal cursor.
    view.setCursor (MAIN_VIEW_CURSOR_NORMAL);
    // Show the first page *AFTER* showing the sidebar because 
    // the view change the row to the first outline that doesn't need
    // to be the first page. If we don't do this, the first page pointed
    // by the first outline is selected and we don't want this.
    view.show ();
}

///
/// @brief Sets the view's state while loading or reloading a document.
///
/// This function tells the main view how to set the sensitivity of the
/// widgets, the cursor and the status bar text when loading or reloading
/// a file.
///
/// @param fileName The file name that is loading or reloading.
/// @param reload TRUE if we are reloading or FALSE otherwise.
///
void
MainPter::setOpenState (const gchar *fileName, gboolean reload)
{
    // Insensitive the open, reload, page navigation, zoom and rotate.
    setZoomText (1.0);
    IMainView &view = getView ();
    view.sensitiveFind (FALSE);
    view.sensitiveGoToFirstPage (FALSE);
    view.sensitiveGoToLastPage (FALSE);
    view.sensitiveGoToNextPage (FALSE);
    view.sensitiveGoToPage (FALSE);
    view.sensitiveGoToPreviousPage (FALSE);
    view.sensitiveReload (FALSE);
    view.sensitiveRotateRight (FALSE);
    view.sensitiveRotateLeft (FALSE);
    view.sensitiveSave (FALSE);
    view.sensitiveZoom (FALSE);
    view.sensitiveZoomIn (FALSE);
    view.sensitiveZoomOut (FALSE);
    view.sensitiveZoomFit (FALSE);
    view.sensitiveZoomWidth (FALSE);
    view.sensitiveFullScreen (FALSE);
    view.sensitiveOpen (FALSE);
#if defined (HAVE_CUPS)
    view.sensitivePrint (FALSE);
#endif // HAVE_CUPS
    // Show the text on the status bar.
    gchar *statusText = NULL;
    if ( reload )
    {
        statusText = g_strdup_printf (_("Reloading file %s..."), fileName);
    }
    else
    {
        statusText = g_strdup_printf (_("Loading file %s..."), fileName);
    }
    view.setStatusBarText (statusText);
    g_free (statusText);
    // Set the wait cursor.
    view.setCursor (MAIN_VIEW_CURSOR_WAIT);
    // Set the number of times we can try the password.
    m_PasswordTries = 3;
}

///
/// @brief Tells the view which text to show in the zoom entry.
///
/// @param zoom The current document's zoom value.
///
void
MainPter::setZoomText (gdouble zoom)
{
    gchar *zoomText = g_strdup_printf ("%.3g", zoom * 100.0);
    getView ().setZoomText (zoomText);
    g_free (zoomText);
}

///
/// @brief Gets the presenter's view.
///
/// @return The reference to the view that was set with setView().
///
IMainView &
MainPter::getView ()
{
    g_assert (NULL != m_View && "Tried to get the view but it's NULL.");

    return (*m_View);
}

///
/// @brief Sets the presenter's view.
///
/// @param view The view that will receive the user events for the presenter.
///             This view will be deleted by the presenter.
///
void
MainPter::setView (IMainView *view)
{
    g_assert (NULL != view && "Tried to set a NULL view.");

    // First create the page presenter.
    delete m_PagePter;
    m_PagePter = new PagePter (m_Document);
    m_PagePter->setView (*view);

    delete m_View;
    m_View = view;
    // Now that the view is set, it's time to set the initial 
    // state.
    setInitialState ();
}

///
/// @brief An URL link in the about box has been activated.
///
/// @param link The URL of the link activated.
///
void
MainPter::aboutBoxLinkActivated (const gchar *link)
{
    gchar *browserCommandLine =
        Config::getConfig ().getExternalBrowserCommandLine ();
    gchar *commandLine = g_strdup_printf (browserCommandLine, link);
    g_free (browserCommandLine);

    GError *error = NULL;
    if ( !g_spawn_command_line_async (commandLine, &error) )
    {
        g_error_free (error);
    }
    g_free (commandLine);
}

///
/// @brief The "Find" action was activated.
///
void
MainPter::findActivated ()
{
    if ( NULL == m_FindPter )
    {
        m_FindPter = new FindPter (m_Document);
    }
    m_FindPter->setView (getView ().getFindView ());
}

///
/// @brief The "Full Screen" action was activated.
///
/// @param show TRUE if Full Screen is active, FALSE otherwise.
///
void
MainPter::fullScreenActivated (gboolean active)
{
	if (NULL != m_Document)
		getView ().setFullScreen (active);
}

///
/// @brief The "Go To First Page" action was activated.
///
void
MainPter::goToFirstPageActivated ()
{
    g_assert (NULL != m_Document &&
              "Tried to go to the first page of a NULL document.");

    m_PagePter->setNextPageScroll (PAGE_SCROLL_START);
    m_Document->goToFirstPage ();
}

///
/// @brief The "Go To Last Page" action was activated.
///
void
MainPter::goToLastPageActivated ()
{
    g_assert (NULL != m_Document && 
              "Tried to go to the last page of a NULL document.");

    m_PagePter->setNextPageScroll (PAGE_SCROLL_START);
    m_Document->goToLastPage ();
}

///
/// @brief The "Go To Next Page" action was activated.
///
void
MainPter::goToNextPageActivated ()
{
    g_assert (NULL != m_Document && 
              "Tried to go to the next page of a NULL document.");

    if ( m_Document->getCurrentPageNum () < m_Document->getNumPages () )
    {
        m_PagePter->setNextPageScroll (PAGE_SCROLL_START);
        m_Document->goToNextPage ();
    }
}

///
/// @brief The "Go To Page" action was activated.
///
/// This action is activated when the user presses the Enter key on
/// the displayed current page number. The user can change the value,
/// and the page will change to that value.
///
void
MainPter::goToPageActivated ()
{
    g_assert (NULL != m_Document &&
              "Tried to go to the a page of a NULL document.");

    // First try to get the page number from the view.
    const gchar *goToPageText = getView ().getGoToPageText ();
    if ( NULL != goToPageText )
    {
        // Try to read as much as possible.
        int pageNum = atoi (goToPageText);
        // If the page number is too high, the Document class will
        // remain to the same page page. The same if it's too low.
        m_PagePter->setNextPageScroll (PAGE_SCROLL_START);
        m_Document->goToPage (pageNum);

        // Set the text for the current page.
        gint newPageNum = m_Document->getCurrentPageNum ();
        gint totalPages = m_Document->getNumPages ();
        gchar *goToPageText = g_strdup_printf ("%d", newPageNum);
        getView ().setGoToPageText (goToPageText);
        g_free (goToPageText);
        gchar *totalPagesText = g_strdup_printf (_("of %d"), totalPages);
        getView ().setNumberOfPagesText (totalPagesText);
        g_free (totalPagesText);
    }
}


///
/// @brief The "Go To Previous Page" action was activated.
///
void
MainPter::goToPreviousPageActivated ()
{
    g_assert (NULL != m_Document && 
              "Tried to go to the previous page of a NULL document.");

    if ( 1 < m_Document->getCurrentPageNum () )
    {
        m_PagePter->setNextPageScroll (PAGE_SCROLL_END);
        m_Document->goToPreviousPage ();
    }
}

///
/// @brief The Open File action was activated.
///
/// This means that the user wants to open a new file. The presenter
/// asks the view to show the Open File dialog and return a file name.
/// Then we'll try to open the file and if it's correct we'll show it, 
/// otherwise it will request the view to show an error dialog or a 
/// password dialog, if the document is encrypted.
///
void
MainPter::openFileActivated ()
{
    Config &config = Config::getConfig ();
    gchar *lastFolder = config.getOpenFileFolder ();
    IMainView &view = getView ();
    gchar *fileName = view.openFileDialog (lastFolder);
    g_free (lastFolder);
    if ( NULL != fileName )
    {
        gchar *dirName = g_path_get_dirname (fileName);
        config.setOpenFileFolder (dirName);
        g_free (dirName);
        // Open the file.
        setOpenState (fileName, FALSE);
        m_Document->load (fileName, NULL);
        g_free (fileName);
    }
}

///
/// @brief The user activated an outline.
///
/// When the sidebar is shown and the user clicks on a outline, the view
/// should send to the presenter which outline has been activated, so the
/// presenter can change the page to the pointed by the outline.
///
/// @param outline The outline that has been activated.
void
MainPter::outlineActivated (DocumentOutline *outline)
{
    g_assert (NULL != outline && "The outline activated is NULL.");

    m_PagePter->setNextPageScroll (PAGE_SCROLL_START);
    m_Document->goToPage (outline->getDestinationPage ());
}

#if defined (HAVE_CUPS)
///
/// @brief The "print" action was activated.
///
void
MainPter::printActivated ()
{
    PrintPter *print = new PrintPter (m_Document);
    IPrintView *view = getView ().getPrintView ();
    if ( NULL != view )
    {
        print->setView (view);
    }
    else
    {
        delete print;
    }
    // There's no need to keep a pointer to the presenter because
    // it will destroy itself when the dialog is closed.
}
#endif // HAVE_CUPS

///
/// @brief The "Preferences" was activated.
///
void
MainPter::preferencesActivated ()
{
    PreferencesPter *preferences = new PreferencesPter ();
    IPreferencesView *view = getView ().getPreferencesView ();
    if ( NULL != view )
    {
        preferences->setView (view);
    }
    else
    {
        delete preferences;
    }
    // There's no need to keep a pointer of the preferences presenter because
    // when the dialog is closed or the application is finished it will
    // destroy itself.
}

///
/// @brief The "Reload" was activated.
///
/// Reloading is like opening the same file but the current page, rotation
/// and zoom are maintained.
///
void
MainPter::reloadActivated ()
{
	if(m_Document->isLoaded ()==TRUE) {
		// Reload
		m_ReloadPage = m_Document->getCurrentPageNum();
		setOpenState (m_Document->getFileName (), TRUE);
		m_Document->reload ();
	}
}

///
/// @brief The "Rotate Left" was activated.
///
void
MainPter::rotateLeftActivated ()
{
    g_assert ( NULL != m_Document && "Tried to rotate a NULL document.");

    m_Document->rotateLeft ();
    checkZoomSettings ();
}

///
/// @brief The "Rotate Right" was activated.
///
void
MainPter::rotateRightActivated ()
{
    g_assert ( NULL != m_Document && "Tried to rotate a NULL document.");

    m_Document->rotateRight ();
    checkZoomSettings ();
}

///
/// @brief The Save File action was activated.
///
/// This means that the user wants to save a copy of the document.
/// The presenter asks the view to show the Save File dialog and return a 
/// file name.
/// Then it will try to save the document and if there's any error it will 
/// request the view to show an error dialog.
///
void
MainPter::saveFileActivated ()
{
    Config &config = Config::getConfig ();
    gchar *lastFolder = config.getSaveFileFolder ();
    IMainView &view = getView ();
    gchar *originalFileName = g_path_get_basename (m_Document->getFileName ());
    gchar *fileName = view.saveFileDialog (lastFolder, originalFileName);
    g_free (originalFileName);
    g_free (lastFolder);
    if ( NULL != fileName )
    {

        // Show on the status bar that we are saving the copy.
        gchar *statusText = g_strdup_printf (_("Saving document to %s..."),
                                             fileName);
        view.setStatusBarText (statusText);
        g_free (statusText);
        gchar *dirName = g_path_get_dirname (fileName);
        config.setSaveFileFolder (dirName);
        g_free (dirName);
        // Save the document.
        m_Document->save (fileName);
        g_free (fileName);
    }
}


///
/// @brief The "Show Index" was activated.
///
/// @param show TRUE if the index should be shown, FALSE otherwise.
///
void
MainPter::showIndexActivated (gboolean show)
{
    getView ().showIndex (show);
}

/// krogan custom edit
/// @brief The "Show Menubar" was activated.
///
/// @param show TRUE if show Menubar is active, FALSE otherwise.
///
void
MainPter::showMenubarActivated (gboolean show)
{
    Config::getConfig().setShowMenubar (show);
    getView ().showMenubar (show);
}

/// krogan custom edit
/// @brief The "Invert Toggle" was activated.
///
/// @param show TRUE if inversion toggle active, FALSE otherwise.
///
void
MainPter::invertToggleActivated (gboolean invert)
{
    Config::getConfig().setInvertToggle (invert);
    getView ().invertToggle (invert);
    m_PagePter->setInvertColorToggle(invert==TRUE);
    m_PagePter->tryReShowPage();
}

///
/// @brief The "Show Statusbar" was activated.
///
/// @param show TRUE if show Statusbar is active, FALSE otherwise.
///
void
MainPter::showStatusbarActivated (gboolean show)
{
    Config::getConfig().setShowStatusbar (show);
    getView ().showStatusbar (show);
}

///
/// @brief The "Show Toolbar" was activated.
///
/// @param show TRUE if show toolbar is active, FALSE otherwise.
///
void
MainPter::showToolbarActivated (gboolean show)
{
    Config::getConfig().setShowToolbar (show);
    getView ().showToolbar (show);
}

///
/// @brief The user entered a zoom value.
///
void
MainPter::zoomActivated ()
{
    IMainView &view = getView ();
    const gchar *zoomText = view.getZoomText ();
    if ( NULL != zoomText )
    {
        Config &config = Config::getConfig ();
        config.setZoomToFit (FALSE);
        config.setZoomToWidth (FALSE);

        view.activeZoomFit (FALSE);
        view.activeZoomWidth (FALSE);

        gdouble zoom = atof (zoomText);
        m_Document->setZoom (zoom / 100.0);
    }
}

///
/// @brief Performs the "Zoom Fit Best".
///
/// Gets the width of the current page view and then calls the zoom to fit
/// function from document.
///
void
MainPter::zoomFit ()
{
    gint width;
    gint height;
    m_PagePter->getSize (&width, &height);
    m_Document->zoomToFit (width, height);
}

///
/// @brief The "Zoom Fit Best" was activated.
///
/// @param active TRUE if the Zoom fit best is active, FALSE otherwise.
///
void
MainPter::zoomFitActivated (gboolean active)
{
    g_assert ( NULL != m_Document && "Tried to zoom fit a NULL document.");

    Config &config = Config::getConfig ();
    IMainView &view = getView ();
    if ( active && !config.zoomToFit () )
    {
        config.setZoomToFit (TRUE);
        view.activeZoomWidth (FALSE);
        zoomFit ();
    }
    else if ( config.zoomToFit () )
    {
        view.activeZoomFit (TRUE);
    }
}

void
MainPter::setPageMode (PagePterMode mode)
{
    m_PagePter->setMode(mode);
}

///
/// @brief The "Zoom In" was activated.
///
void
MainPter::zoomInActivated ()
{
    g_assert ( NULL != m_Document && "Tried to zoom in a NULL document.");

    Config &config = Config::getConfig ();
    config.setZoomToFit (FALSE);
    config.setZoomToWidth (FALSE);

    IMainView &view = getView ();
    view.activeZoomFit (FALSE);
    view.activeZoomWidth (FALSE);

    m_Document->zoomIn ();
}

///
/// @brief The "Zoom Out" was activated.
///
void
MainPter::zoomOutActivated ()
{
    g_assert ( NULL != m_Document && "Tried to zoom out a NULL document.");

    Config &config = Config::getConfig ();
    config.setZoomToFit (FALSE);
    config.setZoomToWidth (FALSE);

    IMainView &view = getView ();
    view.activeZoomFit (FALSE);
    view.activeZoomWidth (FALSE);

    m_Document->zoomOut ();
}

///
/// @brief Performs the "Zoom Fit Width".
///
/// Gets the width of the current page view and then calls the zoom to width
/// function from document.
///
void
MainPter::zoomWidth ()
{
    gint width;
    gint height;
    m_PagePter->getSize (&width, &height);
    m_Document->zoomToWidth (width);
}

///
/// @brief The "Zoom Fit Width" was activated.
///
/// @param active TRUE if the zoom to width option is activated,
///               FALSE otherwise.
///
void
MainPter::zoomWidthActivated (gboolean active)
{
    g_assert ( NULL != m_Document && "Tried to zoom width a NULL document.");

    Config &config = Config::getConfig ();
    IMainView &view = getView ();
    if ( active && !config.zoomToWidth () )
    {
        config.setZoomToWidth (TRUE);
        view.activeZoomFit (FALSE);
        view.activeZoomWidth (TRUE);
        zoomWidth ();
    }
    else if ( config.zoomToWidth () )
    {
        view.activeZoomWidth (TRUE);
    }
}

///
/// @brief Opens a document.
///
/// This function is called both by openFileActivated() and reloadActivated
/// to open a document.
///
/// @param fileName If be the name of the document to open. If it's
///                 NULL then that means the user didn't wanted to
///                 open a file, so this function won't do anything.
/// @param oldPassword This is only used when reloading. It is the last
///                    password that was used to open a file, and will be
///                    used to open the file the first try before asking
///                    to the user. For opening a new file, just set to NULL.
/// @param error Where to save the error if there's one.
///
gboolean
MainPter::openDocument (const gchar *fileName, const gchar *oldPassword,
                        GError **error)
{
    g_assert ( NULL != m_Document && "The document is NULL.");

    // if fileName is NULL, then the user cancelled the operation.
    // I don't need to do anything in this case. I'm only interested when
    // the user tried to open a file.
    if ( NULL != fileName )
    {
        return m_Document->loadFile (fileName, oldPassword, error);
    }
    return TRUE;
}

void
MainPter::notifyLoad ()
{
    setInitialState ();
    m_Document->goToFirstPage ();
    // This way will inform all observers even if the page doesn't
    // change.
    m_Document->notifyPageChanged ();
#if defined (DEBUG)
    G_LOCK (fileLoaded);
    fileLoaded = TRUE;
    G_UNLOCK (fileLoaded);
#endif // DEBUG
}

void
MainPter::notifyLoadError (const gchar *fileName, const GError *error)
{
    gchar *errorTitle = g_strdup_printf ("%s \"%s\"", _("Error Loading File"), fileName);
    getView ().showErrorMessage (errorTitle, error->message);
    g_free (errorTitle);
    setInitialState ();
#if defined (DEBUG)
    G_LOCK (fileLoaded);
    fileLoaded = TRUE;
    G_UNLOCK (fileLoaded);
#endif // DEBUG
}

void
MainPter::notifyLoadPassword (const gchar *fileName, gboolean reload, const GError *error)
{
    if ( 0 < m_PasswordTries )
    {
        m_PasswordTries--;
        gchar *password = getView ().promptPasswordDialog ();
        if ( password != NULL )
        {
            if ( reload )
            {
                m_Document->setPassword (password);
                m_Document->reload ();
            }
            else
            {
                m_Document->load (fileName, password);
            }
        }
#if defined (DEBUG)
        else
        {
            G_LOCK (fileLoaded);
            fileLoaded = TRUE;
            G_UNLOCK (fileLoaded);
        }
#endif // DEBUG
    }
    else
    {
        getView ().showErrorMessage (_("Error Loading File"),
                _("The password you have supplied is not a valid password "
                  "for this file."));
        setInitialState ();
#if defined (DEBUG)
        G_LOCK (fileLoaded);
        fileLoaded = TRUE;
        G_UNLOCK (fileLoaded);
#endif // DEBUG
    }
}

void
MainPter::notifyPageChanged (gint pageNum)
{
    IMainView &view = getView ();
    // Set the text for the current page.
    gint totalPages = m_Document->getNumPages ();
    gchar *goToPageText = g_strdup_printf ("%d", pageNum);
    view.setGoToPageText (goToPageText);
    g_free (goToPageText);
    gchar *totalPagesText = g_strdup_printf(_("of %d"), totalPages);
    view.setNumberOfPagesText (totalPagesText);
    g_free (totalPagesText);

    // Set the page navigation sensitivity.
    gboolean documentLoaded = m_Document->isLoaded ();
    gint numPages = m_Document->getNumPages ();
    view.sensitiveGoToFirstPage (documentLoaded && 1 < pageNum );
    view.sensitiveGoToPreviousPage (documentLoaded && 1 < pageNum );
    view.sensitiveGoToLastPage (documentLoaded && numPages > pageNum);
    view.sensitiveGoToNextPage (documentLoaded && numPages > pageNum);

    checkZoomSettings ();
}

void
MainPter::notifyPageRotated (gint rotation)
{
    IMainView &view = getView ();
    // Set the zoom sensitivity.
    view.sensitiveZoomIn (m_Document->canZoomIn ());
    view.sensitiveZoomOut (m_Document->canZoomOut ());
}

void
MainPter::notifyPageZoomed (gdouble zoom)
{
    IMainView &view = getView ();
    // Show the new zoom.
    setZoomText (zoom);
    // Set the zoom sensitivity.
    view.sensitiveZoomIn (m_Document->canZoomIn ());
    view.sensitiveZoomOut (m_Document->canZoomOut ());
}

void
MainPter::notifyReload ()
{
    gboolean showIndex = getView ().isIndexVisible();
    setInitialState ();
    getView ().showIndex(showIndex);
    m_Document->goToPage(m_ReloadPage);
    m_Document->notifyPageChanged();
#if defined (DEBUG)
    G_LOCK (fileLoaded);
    fileLoaded = TRUE;
    G_UNLOCK (fileLoaded);
#endif // DEBUG
}

void
MainPter::notifySave ()
{
    // Remove the status text.
    getView ().setStatusBarText (NULL);
#if defined (DEBUG)
    G_LOCK (fileSaved);
    fileSaved = TRUE;
    G_UNLOCK (fileSaved);
#endif // DEBUG
}

void
MainPter::notifySaveError (const GError *error)
{
    getView ().showErrorMessage (_("Error Saving File"), error->message);
#if defined (DEBUG)
    G_LOCK (fileSaved);
    fileSaved = TRUE;
    G_UNLOCK (fileSaved);
#endif // DEBUG
}

#if defined (DEBUG)
///
/// @brief Waits until a file is loaded.
///
/// This is *only* useful for testing, don't use it.
///
void
MainPter::waitForFileLoaded ()
{
    volatile gboolean end = FALSE;
    do
    {
        G_LOCK (fileLoaded);
        end = fileLoaded;
        G_UNLOCK (fileLoaded);
    }
    while ( !end );
    G_LOCK (fileLoaded);
    fileLoaded = FALSE;
    G_UNLOCK (fileLoaded);
}

///
/// @brief Waits until a file is saved.
///
/// This is *only* useful for testing, don't use it.
///
void
MainPter::waitForFileSaved ()
{
    volatile gboolean end = FALSE;
    do
    {
        G_LOCK (fileSaved);
        end = fileSaved;
        G_UNLOCK (fileSaved);
    }
    while ( !end );
    G_LOCK (fileSaved);
    fileSaved = FALSE;
    G_UNLOCK (fileSaved);
}
#endif // DEBUG

void 
MainPter::notifyTextSelected (const gchar* text)
{
    getView ().copyTextToClibboard(text);
}
