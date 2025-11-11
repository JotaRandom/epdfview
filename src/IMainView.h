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

#if !defined (__IMAIN_VIEW_H__)
#define __IMAIN_VIEW_H__


namespace ePDFView
{
    // Forward declarations
    class MainPter;
    class DocumentPage;
    class DocumentOutline;

    ///
    /// @brief Main view's cursor.
    ///
    enum ViewCursor
    {
        /// Normal cursor.
        MAIN_VIEW_CURSOR_NORMAL,
        /// Hour glass wait cursor.
        MAIN_VIEW_CURSOR_WAIT,
        /// Drag cursor.
        MAIN_VIEW_CURSOR_DRAG
    };

    ///
    /// @brief Callback type for file chooser dialogs.
    ///
    /// @param fileName The selected file name, or NULL if cancelled. Caller must free.
    /// @param userData User data passed to the dialog function.
    ///
    typedef void (*FileChooserCallback)(gchar *fileName, gpointer userData);

    ///
    /// @class IMainView
    /// @brief Interface class for the main view.
    ///
    /// This is the base class for all toolkit dependent implementations
    /// of the main view. This is the class responsible to get all user
    /// events and pass them to the presenter.
    ///
    class IMainView
    {
        public:
            ///
            /// @brief Destroys the main window's dynamically allocated memory.
            ///
            virtual ~IMainView (void) {}

            ///
            /// @brief Actives or datives the zoom to fit.
            ///
            /// The view must change the state of the zoom to fit option
            /// to @a active.
            ///
            /// @param active TRUE if the option must be activated, FALSE
            ///               otherwise.
            ///
            virtual void activeZoomFit (gboolean active) = 0;

            ///
            /// @brief Actives or deactivates the zoom to width.
            ///
            /// The view must change the state of the zoom to width option
            /// to @a active.
            ///
            /// @param active TRUE if the option must be activated, FALSE
            ///               otherwise.
            ///
            virtual void activeZoomWidth (gboolean active) = 0;

            ///
            /// @brief Tells if the index is shown.
            ///
            /// @return @c TRUE if the index is currently shown, @c FALSE
            ///         otherwise.
            ///
            virtual gboolean isIndexVisible() const = 0;

            ///
            /// @brief Shows the open file dialog (async version for GTK4).
            ///
            /// GTK4 version: This function returns immediately and calls the
            /// callback when the user selects a file or cancels.
            ///
            /// @param lastFolder The folder that was used the last time.
            /// @param callback Function to call with the result.
            /// @param userData User data to pass to the callback.
            ///
            virtual void openFileDialogAsync (const gchar *lastFolder,
                                             FileChooserCallback callback,
                                             gpointer userData) = 0;

            ///
            /// @brief Shows the open file dialog.
            ///
            /// The view must ask the user which file to open by using
            /// the toolkit's specific open dialog.
            ///
            /// @param lastFolder The last folder used to open a file. This is
            ///                   used to show this folder when the open dialog
            ///                   appears.
            ///
            /// @return A copy of the file name that the user will try to open
            ///         or NULL if the user cancelled the operation.
            ///         This string will be freed by the presenter.
            ///
            virtual gchar *openFileDialog (const gchar *lastFolder) = 0;

            ///
            /// @brief Shows the password dialog.
            ///
            /// This is used when the user wants to open an encrypted file.
            /// The view must ask the password to the user. It will be called
            /// up to three times by the presenter.
            ///
            /// @return A copy of the password entered by the user or NULL
            ///         if the user cancelled the operation.
            ///         This string will be freed by the presenter.
            ///
            virtual gchar *promptPasswordDialog (void) = 0;

            ///
            /// @brief Shows the save file dialog.
            ///
            /// The view must ask the user which file name to use to
            /// save using the toolkit's specific save dialog.
            ///
            /// @param lastFolder The last folder used to save a file. This is
            ///                   used to show this folder when the save dialog
            ///                   appears.
            /// @param fileName The file name to set as the initial name to
            ///                 the save dialog.
            ///
            /// @return A copy of the file name that the user will try to use
            ///         to save or NULL if the user cancelled the operation.
            ///         This string will be freed by the presenter.
            ///
            virtual gchar *saveFileDialog (const gchar *lastFolder,
                                           const gchar *fileName) = 0;

            ///
            /// @brief Changes the sensitivity of the "Find" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Find" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveFind (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Go To First Page" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Go To First Page" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveGoToFirstPage (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Go To Last Page" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Go To Last Page" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveGoToLastPage (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Go To Next Page" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Go To Next Page" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveGoToNextPage (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Go To Page" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Go To Page" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveGoToPage (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Go To Prev. Page" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Go To Previous Page" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveGoToPreviousPage (gboolean sensitive) = 0;

#if defined (HAVE_CUPS)
            ///
            /// @brief Changes the sensitivity of the "Print" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Print" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make the action
            ///                  sensitive (enable) or FALSE to
            ///                  insensitive (disable) it.
            ///
            virtual void sensitivePrint (gboolean sensitive) = 0;
#endif // HAVE_CUPS

            ///
            /// @brief Changes the sensitivity of the "Open" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Open" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveOpen (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Reload" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Reload" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveReload (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Rotate Left" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Rotate Left" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveRotateLeft (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Rotate Right" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Rotate Right" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveRotateRight (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Save a Copy" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Save a Copy" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action or FALSE to insensitive (disable)
            ///                  it.
            ///
            virtual void sensitiveSave (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Zoom" entry.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Zoom" entry
            /// on the toolbar.
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the entry or FALSE to insensitive (disable) it.
            ///
            virtual void sensitiveZoom (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Zoom In" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Zoom In" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveZoomIn (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Zoom Out" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Zoom Out" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveZoomOut (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Zoom to Fit" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Zoom to Fit" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveZoomFit (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Zoom to Width" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Zoom to Width" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveZoomWidth (gboolean sensitive) = 0;
            ///
            /// @brief Changes the sensitivity of the "Full Screen" action.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Full Screen" action
            /// (both on the menu and the toolbar or any other place).
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the action (enable) or FALSE to 
            ///                  insensitive (disable) it.
            ///
            virtual void sensitiveFullScreen (gboolean sensitive) = 0;

            ///
            /// @brief Sets the page view's cursor.
            ///
            /// The main view must show the cursor as given by @a cursorType
            /// for the page's view.
            ///
            /// @param cursorType The type of cursor to show.
            ///
            virtual void setCursor (ViewCursor cursorType) = 0;

            ///
            /// @brief Sets the view to full screen.
            ///
            /// @param fullScreen @a TRUE if the view should go full screen,
            ///                   @a FALSE otherwise.
            ///
            virtual void setFullScreen (gboolean fullScreen) = 0;

            ///
            /// @brief Sets the text for the number of pages.
            ///
            /// The view displays the number of pages separately
            /// of the current page. This function should set the
            /// text to display in this separate control.
            ///
            /// @param text The text to set as the number of pages.
            ///
            virtual void setNumberOfPagesText (const gchar *text) = 0;

            ///
            /// @brief Sets the text of the status bar.
            ///
            /// The view must show the text at @a text unless it's NULL,
            /// which means show nothing.
            ///
            /// @param text The text to shown on the status bar or NULL if
            ///             no text should be shown on the status bar.
            ///
            virtual void setStatusBarText (const gchar *text) = 0;

            ///
            /// @brief Sets the current zoom text.
            ///
            /// The current zoom text is displayed on the tool bar and
            /// the user can change it in order to specify a concrete
            /// zoom level.
            ///
            /// @param text The text to show in the current zoom entry.
            ///
            virtual void setZoomText (const gchar *text) = 0;

            ///
            /// @brief Makes the main view to show itself.
            ///
            /// From the main view's creation until the presenter calls
            /// this function, the main view must remain hidden.
            ///
            virtual void show (void) = 0;

            ///
            /// @brief Shows an error message dialog.
            ///
            /// Shows to the user a dialog with an error message. Use
            /// the toolkit's dialog feature to show it.
            ///
            /// Some environments uses the @a title and @a body as the
            /// dialog's primary and secondary texts respectively. The
            /// presenter doesn't care.
            ///
            /// @param title The error dialog's title.
            /// @param body The error dialog's body (the error message.)
            ///
            virtual void showErrorMessage (const gchar *title,
                                           const gchar *body) = 0;

            ///
            /// @brief Shows the document's index.
            ///
            /// The presenter will call it just before to show the main window,
            /// to let the view know if it must show the index or not
            /// when the main window will be displayed.
            ///
            /// After that, the presenter can call it to show or hide the
            /// index when the user toggles the "Show Index" option.
            ///
            /// @param show Set to TRUE if the index should be shown. FALSE
            ///             otherwise.
            ///
            virtual void showIndex (gboolean show) = 0;

            /// krogan
            /// @brief Shows the menu bar.
            ///
            /// The view must show the menu bar or hide it depending
            /// on the @a show parameter.
            ///
            /// @param show TRUE if the menu bar must be shown, 
            ///             FALSE otherwise.
            ///
            virtual void showMenubar (gboolean show) = 0;
            
            /// krogan
            /// @brief Toggles color inversion
            ///
            /// The view must show the status bar or hide it depending
            /// on the @a show parameter.
            ///
            /// @param show TRUE if invert color toggle on, 
            ///             FALSE otherwise.
            ///
            virtual void invertToggle (gboolean show) = 0;

            ///
            /// @brief Shows the status bar.
            ///
            /// The view must show the status bar or hide it depending
            /// on the @a show parameter.
            ///
            /// @param show TRUE if the status bar must be shown, 
            ///             FALSE otherwise.
            ///
            virtual void showStatusbar (gboolean show) = 0;

            ///
            /// @brief Shows the tool bar.
            ///
            /// The view must show the toolbar or hide it depending
            /// on the @a show parameter.
            ///
            /// @param show TRUE if the toolbar must be shown, FALSE otherwise.
            ///
            virtual void showToolbar (gboolean show) = 0;

            ///
            /// @brief Sets the "Go To Page" text.
            ///
            /// The view must keep a text entry (edit control) that has the 
            /// current shown page number. The presenter will update this 
            /// text every time the page changes by calling this function.
            ///
            /// @param text The text to set to the "Go To Page" entry.
            ///
            virtual void setGoToPageText (const gchar *text) = 0;

            ///
            /// @brief Gets the text at "Go To Page".
            ///
            /// Gets the text that is displayed on the "Go To Page" entry.
            /// This is normally used when the user activated it (pressed
            /// the Enter key) and the presenter must try to go to the page
            /// the user entered.
            ///
            /// @return The text at "Go To Page". The presenter won't free
            ///         this pointer.
            ///
            virtual const gchar *getGoToPageText (void) = 0;

            ///
            /// @brief Gets the find's view.
            ///
            /// When the main view is created it also creates the find
            /// view, although hidden.
            ///
            /// This view is then retrieved by the MainPter when the
            /// find bar must be shown, and is passed to the FindPter
            /// that will control it.
            ///
            /// @return The find's view. The returned pointer will be
            ///         freed by the main view.
            virtual IFindView *getFindView (void) = 0;

            ///
            /// @brief Gets the page's view.
            ///
            /// When the main view is created besides creating all its
            /// controls, it creates the page view because.
            ///
            /// This created view is then retrieved by the soon-to-be-presenter
            /// class when it is created. This way the presenter knows nothing
            /// about how to create the view (i.e., toolkit agnostic.)
            ///
            /// @return The page's view. The returned pointer will be freed
            ///         by the main view.
            ///
            virtual IPageView *getPageView (void) = 0;

            ///
            /// @brief Gets the preferences' view.
            ///
            /// The preferences view is created each time the presenter
            /// calls this function, with the main view as its parent window.
            ///
            /// @return The new preferences view. The returned pointer
            ///         must be freed by the caller.
            ///
            virtual IPreferencesView *getPreferencesView (void) = 0;

#if defined (HAVE_CUPS)
            ///
            /// @brief Gets the print's view.
            ///
            /// The print view is created each time the presenter
            /// class this function, with the main view as its parent window.
            ///
            /// @return The new print view. The returned pointer must be
            ///         freed by the caller.
            ///
            virtual IPrintView *getPrintView (void) = 0;
#endif // HAVE_CUPS

            ///
            /// @brief Gets the text from the zoom entry.
            ///
            /// Gets the text inside the "Current Zoom" entry on the
            /// tool bar.
            ///
            /// @return The text inside the "Current Zoom" entry.
            ///
            virtual const gchar *getZoomText (void) = 0;

            ///
            /// @brief Sets the main view's title.
            ///
            /// This is called by the presenter when a new document has been
            /// loaded. Normally it will put the document's file name or the
            /// document's title (if it has).
            ///
            /// The view just has to set the window's title to @a title.
            ///
            /// @param title The title to set to the main window.
            ///
            virtual void setTitle (const gchar *title) = 0;

            ///
            /// @brief Sets the document's outline.
            ///
            /// The presenter will call this function regardless if the
            /// sidebar is shown or not.
            ///
            /// The view must make a tree view from the document's outline
            /// and show it when the sidebar is shown.
            ///
            /// Also, when one of these outlines is activated (the user
            /// clicks on one of them) the view must pass the pointer to the
            /// activated outline when calling MainPter::outlineActivated().
            ///
            /// @param outline The root outline to set. It can have no
            ///                children.
            ///
            virtual void setOutline (DocumentOutline *outline) = 0;

            virtual void copyTextToClibboard(const gchar* text) = 0;
            virtual void activePageModeScroll (gboolean active) = 0;
            virtual void activePageModeText (gboolean active) = 0;

        protected:
            ///
            /// @brief Constructs a new main view.
            ///
            /// This constructor is keep protected because this class is an
            /// interface (abstract class) that can't be instantiated.
            ///
            /// @param pter The main presenter that will drive the view.
            ///
            IMainView (MainPter *pter)
            {
                g_assert ( NULL != pter && "Tried to set a NULL presenter." );
                m_Pter = pter;
            }

            /// The view's presenter.
            MainPter *m_Pter;
    };
}

#endif // !__IMAIN_VIEW_H__
