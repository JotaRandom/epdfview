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

#if !defined (__MAIN_VIEW_H__)
#define __MAIN_VIEW_H__


namespace ePDFView
{
    // Forward declarations
    class FindView;
    class MainPter;
    class DocumentPage;
    class PageView;

    class MainView: public IMainView
    {
        public:
            MainView (MainPter *pter);
            ~MainView ();

            void activeZoomFit (gboolean active);
            void activeZoomWidth (gboolean active);
            void activePageModeScroll (gboolean active);
            void activePageModeText (gboolean active);
            gboolean isIndexVisible () const;
            gchar *openFileDialog (const gchar *lastFolder);
            gchar *promptPasswordDialog (void);
            gchar *saveFileDialog (const gchar *lastFolder,
                                   const gchar *fileName);
            void sensitiveFind (gboolean sensitive);
            void sensitiveGoToFirstPage (gboolean sensitive);
            void sensitiveGoToLastPage (gboolean sensitive);
            void sensitiveGoToNextPage (gboolean sensitive);
            void sensitiveGoToPage (gboolean sensitive);
            void sensitiveGoToPreviousPage (gboolean sensitive);
            void sensitiveReload (gboolean sensitive);
            void sensitiveOpen (gboolean sensitive);
#if defined (HAVE_CUPS)
            void sensitivePrint (gboolean sensitive);
#endif // HAVE_CUPS
            void sensitiveRotateLeft (gboolean sensitive);
            void sensitiveRotateRight (gboolean sensitive);
            void sensitiveSave (gboolean sensitive);
            void sensitiveZoom (gboolean sensitive);
            void sensitiveZoomIn (gboolean sensitive);
            void sensitiveZoomOut (gboolean sensitive);
            void sensitiveZoomFit (gboolean sensitive);
            void sensitiveZoomWidth (gboolean sensitive);
            void sensitiveFullScreen (gboolean sensitive);
            void setCurrentPageGoto (gint number);
            void setNumberOfPages (gint number);
            void setOutline (DocumentOutline *outline);
            void setPageMode (PageModeType mode);
            void setWindow (GtkWindow *window);
            void setZoomFactor (gfloat zoomFactor);
            void show (void);
            void showErrorMessage (const gchar *title, const gchar *body);
            void showIndex (gboolean show);
            void showMenubar (gboolean show);
            void showStatusbar (gboolean show);
            void showToolbar (gboolean show);
            void toggleInvertColors (void);
            
            // GTK4: Get the main window for application registration
            GtkWidget *getMainWindow () { return m_MainWindow; }
            
            void setCursor (ViewCursor cursorType);
            void setFullScreen (gboolean fullScreen);
            void setNumberOfPagesText (const gchar *text);
            void setGoToPageText (const gchar *text);
            void setStatusBarText (const gchar *text);
            void setZoomText (const gchar *text);
            const gchar *getGoToPageText (void);
            void copyTextToClibboard(const gchar* text);
            IFindView *getFindView (void);
            IPageView *getPageView (void);
            IPreferencesView *getPreferencesView (void);
#if defined (HAVE_CUPS)
            IPrintView *getPrintView (void);
#endif // HAVE_CUPS
            const gchar *getZoomText (void);
            void setTitle (const gchar *title);
            void invertToggle (gboolean show);

        protected:
            GtkWidget *m_CurrentPage;
            GtkWidget *m_CurrentPageToolItem;
            GtkWidget *m_CurrentZoom;
            GtkWidget *m_CurrentZoomToolItem;
            FindView *m_FindView;
            GtkWidget *m_MainWindow;
            GtkWidget *m_MainBox;
            GtkWidget *m_NumberOfPages;
            GtkTreeStore *m_Outline;
            PageView *m_PageView;
            GtkWidget *m_Sidebar;
            GtkWidget *m_StatusBar;
            GtkWidget *m_TreeIndex;
            GSimpleActionGroup *m_ActionGroup;
            GtkWidget *m_MenuBar;
            GtkWidget *m_ToolBar;

            void createCurrentPage (void);
            void createCurrentZoom (void);
            GtkWidget *createPageView (void);
            void createActions (void);
            void createMenuBar (void);
            void createToolBar (void);
            void setMainWindowIcon (void);
            void setOutlineChildren (DocumentOutline *outline, 
                                     GtkTreeIter *rootIter);
    };
}

#endif // !__MAIN_VIEW_H__
