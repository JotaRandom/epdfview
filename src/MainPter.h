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

#if !defined (__MAIN_PTER_H__)
#define __MAIN_PTER_H__

namespace ePDFView
{
    // Forward declarations.
    class IDocument;
    class DocumentPage;

    ///
    /// @class MainPter
    /// @brief Main window presenter.
    ///
    /// The presenter is the one responsible of the application's behaviour.
    ///
    /// This class will decide what the main window should show to the user
    /// and how to react to main window's user events.
    ///
    class MainPter: public IDocumentObserver
    {
        public:
            MainPter (void);
            MainPter (IDocument *document);
            ~MainPter (void);

            void setPageMode(PagePterMode mode);

            void setInitialState (void);
            void setOpenState (const gchar *fileName, gboolean reload);
            IMainView &getView (void);
            void setView (IMainView *view);

            void aboutBoxLinkActivated (const gchar *link);
            void findActivated (void);
            void fullScreenActivated (gboolean active);
            void goToFirstPageActivated (void);
            void goToLastPageActivated (void);
            void goToNextPageActivated (void);
            void goToPageActivated (void);
            void goToPreviousPageActivated (void);
            gboolean openDocument (const gchar *fileName,
                                   const gchar *oldPassword, GError **error);
            void openFileActivated (void);
            void outlineActivated (DocumentOutline *outline);
            void preferencesActivated (void);
#if defined (HAVE_CUPS)
            void printActivated (void);
#endif // HAVE_CUPS
            void reloadActivated (void);
            void rotateLeftActivated (void);
            void rotateRightActivated (void);
            void saveFileActivated (void);
            void showIndexActivated (gboolean show);
            void showMenubarActivated (gboolean show); //krogan
            void invertToggleActivated (gboolean on); //krogan
            void showStatusbarActivated (gboolean show);
            void showToolbarActivated (gboolean show);
            void zoomActivated (void);
            void zoomFitActivated (gboolean active);
            void zoomInActivated (void);
            void zoomOutActivated (void);
            void zoomWidthActivated (gboolean active);

            void notifyLoad (void);
            void notifyLoadError (const gchar *fileName, const GError *error);
            void notifyLoadPassword (const gchar *fileName, gboolean reload,
                                     const GError *error);
            void notifyPageChanged (gint pageNum);
            void notifyPageRotated (gint rotation);
            void notifyPageZoomed (gdouble zoom);
            void notifyReload (void);
            void notifySave (void);
            void notifySaveError (const GError *error);
            void notifyTextSelected (const gchar* text);

#if defined (DEBUG)
            void waitForFileLoaded (void);
            void waitForFileSaved (void);
#endif // DEBUG

        protected:
            /// The document that it's showing.
            IDocument *m_Document;
            /// The presenter of the find bar.
            FindPter *m_FindPter;
            /// The presenter of the view that shows the current page.
            PagePter *m_PagePter;
            /// The number of times the password has been tried for a document.
            gint m_PasswordTries;
            /// The page to move once the document is reloaded.
            gint m_ReloadPage;
            /// The main view.
            IMainView *m_View;

            void checkZoomSettings (void);
            void setZoomText (gdouble zoom);
            void zoomFit (void);
            void zoomWidth (void);
    };
}

#endif // !__MAIN_PTER_H__
