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

#if !defined (__PAGE_PTER_H__)
#define __PAGE_PTER_H__

// Forward declarations.
typedef struct _DragInfo DragInfo;

namespace ePDFView
{
    enum PagePterMode{
        PagePterModeScroll,
        PagePterModeSelectText,
    };

    ///
    /// @class PagePter.
    /// @brief The page presenter.
    ///
    /// This class is the presenter that drives the page view that
    /// shows the current document's page image.
    ///
    class PagePter: public IDocumentObserver
    {
        public:
            PagePter (IDocument *document);
            ~PagePter ();

            void getSize (gint *width, gint *height);
            IPageView &getView (void);
            void onCtrlButton (gint mouseButton, gint mouseState,gint x, gint y);
            void mouseButtonPressed (gint mouseButton, gint mouseState,gint x, gint y);
            void mouseButtonReleased (gint mouseButton);
            void mouseMoved (gint x, gint y);
            void notifyFindChanged (DocumentRectangle *matchRect);
            void notifyLoad (void);
            void notifyPageChanged (gint pageNum);
            void notifyPageRotated (gint rotation);
            void notifyPageZoomed (gdouble zoom);
            void notifyReload (void);
			void tryReShowPage (void);
            static gboolean pageNotAvailable (gpointer user);
            void scrollToNextPage (void);
            void scrollToPreviousPage (void);
            void setNextPageScroll (PageScroll next);
            void setView (IMainView &view);
            void viewResized (gint width, gint height);
            void setMode(PagePterMode mode);
			void setInvertColorToggle(char on);//krogan

        protected:
            /// The document whose page is shown.
            IDocument *m_Document;
            /// Information about dragging the page.
            DragInfo *m_DragInfo;
            /// How to scroll the next page to show.
            PageScroll m_NextPageScroll;
            /// The page view.
            IPageView *m_PageView;
            /// Last text selection
            cairo_region_t *m_LastSelection;
            /// What page presenter must do when user move mouse with button pressed.
            PagePterMode m_ScrollMode;

            void refreshPage (PageScroll pageScroll, gboolean wasZoomed);
    };
}

#endif // !__PAGE_PTER_H__
