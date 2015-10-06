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

#if !defined (__IPAGE_VIEW_H__)
#define __IPAGE_VIEW_H__

namespace ePDFView
{
    // Forward declarations.
    class PagePter;

    ///
    /// @brief The possible cursor for the page view.
    ///
    enum PageCursor
    {
        /// Normal cursor.
        PAGE_VIEW_CURSOR_NORMAL,
        /// Drag cursor.
        PAGE_VIEW_CURSOR_DRAG,
        /// Text selection cursor.
        PAGE_VIEW_CURSOR_SELECT_TEXT,
        /// Link cursor.
        PAGE_VIEW_CURSOR_LINK
    };
    
    ///
    /// @brief How to show the new page.
    ///
    enum PageScroll
    {
        /// No scroll.
        PAGE_SCROLL_NONE,
        /// Scroll to the beginning of the page.
        PAGE_SCROLL_START,
        /// Scroll to the bottom of the page.
        PAGE_SCROLL_END
    };
  
    ///
    /// @class IPageView.
    /// @brief Interface for the page view.
    ///
    /// The page view is the responsible to show the page's image,
    /// handle the scroll of it and the links. It will also tell
    /// the available width and height space when doing a zoom to fit or
    /// zoom to width.
    ///
    class IPageView
    {
        public:
            ///
            /// @brief Destroys all dynamically allocated memory.
            ///
            virtual ~IPageView (void) { }

            ///
            /// @brief Sets the view's presenter.
            ///
            /// @param pter The presenter that controls the view and the
            ///             view sends notices to.
            ///
            virtual void setPresenter (PagePter *pter) { m_Pter = pter; }

            ///
            /// @brief Gets the view's presenter.
            ///
            /// @return The presenter that controls the view or NULL if
            ///         it was not set.
            ///
            PagePter *getPresenter (void) { return m_Pter; }

            /// @brief Gets the horizontal scroll position.
            ///
            /// @return The position of the horizontal scroll bar.
            virtual gdouble getHorizontalScroll (void) = 0;

            ///
            /// @brief Gets the size of the document's view.
            ///
            /// The view must provide to the presenter the current size
            /// the part destined to show the page has.
            ///
            /// This is used when the user requests "Zoom to Fit" or 
            /// "Zoom to Width", so don't user the current's page size. Give
            /// the allocated space for viewing the page (for example the
            /// Scrolled Window's size in a GTK application).
            ///
            /// @param width The location to save the page view's width.
            /// @param height The location to save the page view's height.
            virtual void getSize (gint *width, gint *height) = 0;

            /// @brief Gets the vertical scroll position.
            ///
            /// @return The position of the vertical scroll bar.
            virtual gdouble getVerticalScroll (void) = 0;

            ///
            /// @brief Makes a document rectangle visible.
            ///
            /// When the presenter calls this function, the view must make
            /// sure that the rectangle is visible. If the rectangle is
            /// already visible, then do nothing. Otherwise, scroll the
            /// page to be able to view the rectangle.
            ///
            /// @param rect The rectangle to make visible to the user.
            /// @param scale The scale that the page is currently shown,
            ///              since the rectangle's coordinates are unscaled.
            ///
            virtual void makeRectangleVisible (DocumentRectangle &rect,
                                               gdouble scale) = 0;

            ///
            /// @brief Resize the current page.
            ///
            /// When the document's scale is changed the view is instructed
            /// to resize the current page when the cached page image is
            /// not ready yet. The view should resize the current image using
            /// a fast algorithm, without caring too much about the final
            /// result.
            ///
            /// @param width The new width of the page.
            /// @param height The new height of the page.
            ///
            virtual void resizePage (gint width, gint height) = 0;
            
            ///
            /// @brief Sets the page scroll.
            ///
            /// The view should scroll the page when the presenter calls
            /// this, because it's dragging the page with the mouse.
            ///
            /// @param scrollX The drag X start position of the scroll bar.
            /// @param scrollY The drag Y start position of the scroll bar.
            /// @param dx The difference in the x-axis between the drag
            ///           start position and the current mouse position.
            /// @param dy The difference in the y-axis between the drag
            ///           start position and the current mouse position.
            ///
            virtual void scrollPage (gdouble scrollX, gdouble scrollY,
                                     gint dx, gint dy) = 0;
            
            ///
            /// @brief Sets the cursor for the page view.
            ///
            /// The view must change the cursor for the page image.
            /// If the cursor to be set is the same as the current cursor,
            /// the view can ignore the petition.
            ///
            /// @param cursorType The cursor to set.
            ///
            virtual void setCursor (PageCursor cursorType) = 0;
            
            ///
            /// @brief Shows a document's page.
            ///
            /// The view must show the @a page the the presenter gives to it.
            /// Of course the view can use scroll bars if the page doesn't fit
            /// the current view's size, but MUST NOT change the page's size.
            ///
            /// The view must make a copy of the image, because the page
            /// can be deleted after this call.
            ///
            /// @param page The document's page to show.
            /// @param scroll Tells the main view how to scroll the
            ///               new page.
            ///
            virtual void showPage (DocumentPage *page, PageScroll scroll) = 0;
            virtual void tryReShowPage () = 0;
            
            virtual void setInvertColorToggle (char on) = 0; // krogan

            ///
            /// @brief Shows text on the page.
            ///
            /// This is only used when the page is still loading but the
            /// presenter wants to show something on the user. The presenter
            /// passes a text string to show during the rendering of the
            /// current page. The view should put this string at the middle
            /// of the currently shown page.
            ///
            /// @param text The text to show on the middle of the page.
            ///
            virtual void showText (const gchar *text) = 0;
            
            
        protected:
            ///
            /// @brief Creates a new IPageView object.
            ///
            IPageView (void) { m_Pter = NULL; }
            
            /// @brief The view's presenter.
            PagePter *m_Pter;
    };
}

#endif // !__IPAGE_VIEW_H__
