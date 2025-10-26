// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Dumb Test Main View.
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

#if !defined(__DUMB_MAIN_VIEW_H__)
#define __DUMB_MAIN_VIEW_H__

namespace ePDFView
{
    // Forward declarations.
    class DumbFindView;
    class DumbPageView;

    class DumbMainView: public IMainView
    {
        public:
            // Interface methods.
            DumbMainView (MainPter *pter);
            ~DumbMainView ();

            void activeZoomFit (gboolean active);
            void activeZoomWidth (gboolean active);
            IFindView *getFindView (void);
            IPageView *getPageView (void);
#if defined (HAVE_CUPS)
            IPrintView *getPrintView (void);
#endif // HAVE_CUPS
            IPreferencesView *getPreferencesView (void);
            const gchar *getZoomText (void);
            gchar *openFileDialog (const gchar *lastFolder);
            gchar *promptPasswordDialog (void);
            char *saveFileDialog (const gchar *lastFolder);
            void sensitiveFind (gboolean sensitive);
            void sensitiveGoToFirstPage (gboolean sensitive);
            void sensitiveGoToLastPage (gboolean sensitive);
            void sensitiveGoToNextPage (gboolean sensitive);
            void sensitiveGoToPage (gboolean sensitive);
            void sensitiveGoToPreviousPage (gboolean sensitive);
            void sensitiveOpen (gboolean sensitive);
            void sensitivePrint (gboolean sensitive);
            void sensitiveReload (gboolean sensitive);
            void sensitiveRotateLeft (gboolean sensitive);
            void sensitiveRotateRight (gboolean sensitive);
            void sensitiveSave (gboolean sensitive);
            void sensitiveZoom (gboolean sensitive);
            void sensitiveZoomIn (gboolean sensitive);
            void sensitiveZoomOut (gboolean sensitive);
            void sensitiveZoomFit (gboolean sensitive);
            void sensitiveZoomWidth (gboolean sensitive);
            const gchar *getGoToPageText (void);
            void setCursor (ViewCursor cursorType);
            void setFullScreen (gboolean fullScreen);
            void setNumberOfPagesText (const gchar *text);
            void setGoToPageText (const gchar *text);
            void setTitle (const gchar *title);
            void setOutline (DocumentOutline *outline);
            void setStatusBarText (const gchar *text);
            void setZoomText (const gchar *text);
            void show (void);
            void showErrorMessage (const gchar *title, const gchar *body);
            void showIndex (gboolean show);
            void showStatusbar (gboolean show);
            void showToolbar (gboolean show);

            // Methods for test purposes.
            gint countTimesShownPasswordPrompt (void);
            gint getCurrentPage (void);
            const gchar *getLastOpenFileFolder (void);
            const gchar *getLastSaveFileFolder (void);
            DocumentOutline *getOutline (void);
            const gchar *getTitle (void);
            gboolean isShown (void);
            gboolean isSensitiveFind (void);
            gboolean isSensitiveGoToFirstPage (void);
            gboolean isSensitiveGoToLastPage (void);
            gboolean isSensitiveGoToNextPage (void);
            gboolean isSensitiveGoToPage (void);
            gboolean isSensitiveGoToPreviousPage (void);
            gboolean isSensitivePrint (void);
            gboolean isSensitiveReload (void);
            gboolean isSensitiveRotateLeft (void);
            gboolean isSensitiveRotateRight (void);
            gboolean isSensitiveSave (void);
            gboolean isSensitiveZoom (void);
            gboolean isSensitiveZoomIn (void);
            gboolean isSensitiveZoomOut (void);
            gboolean isSensitiveZoomFit (void);
            gboolean isSensitiveZoomWidth (void);
            gboolean isShownIndex (void);
            gboolean isShownStatusbar (void);
            gboolean isShownToolbar (void);
            gboolean isZoomToFitActive (void);
            gboolean isZoomToWidthActive (void);
            void setOpenFileName (const gchar *fileName);
            void setPassword (const gchar *password);
            void setSaveFileName (const gchar *fileName);
            gboolean shownError (void);

        protected:
            gint m_CurrentPage;
            DumbFindView *m_FindView;
            gchar *m_NumberOfPages;
            gchar *m_GoToPageText;
            gchar *m_LastOpenFileFolder;
            gchar *m_LastSaveFileFolder;
            gchar *m_OpenFileName;
            DocumentOutline *m_Outline;
            DumbPageView *m_PageView;
            gchar *m_Password;
            gchar *m_SaveFileName;
            gboolean m_SensitiveFind;
            gboolean m_SensitiveGoToFirstPage;
            gboolean m_SensitiveGoToLastPage;
            gboolean m_SensitiveGoToNextPage;
            gboolean m_SensitiveGoToPage;
            gboolean m_SensitiveGoToPreviousPage;
            gboolean m_SensitivePrint;
            gboolean m_SensitiveReload;
            gboolean m_SensitiveRotateLeft;
            gboolean m_SensitiveRotateRight;
            gboolean m_SensitiveSave;
            gboolean m_SensitiveZoom;
            gboolean m_SensitiveZoomIn;
            gboolean m_SensitiveZoomOut;
            gboolean m_SensitiveZoomFit;
            gboolean m_SensitiveZoomWidth;
            gboolean m_Shown;
            gboolean m_ShownError;
            gboolean m_ShownIndex;
            gboolean m_ShownStatusbar;
            gboolean m_ShownToolbar;
            gint m_TimesShownPassword;
            gchar *m_Title;
            gchar *m_ZoomText;
            gboolean m_ZoomToFit;
            gboolean m_ZoomToWidth;
    };
}

#endif // !__DUMB_MAIN_VIEW_H__
