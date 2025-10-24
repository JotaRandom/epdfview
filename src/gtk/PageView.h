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

#if !defined (__PAGE_VIEW_H__)
#define __PAGE_VIEW_H__

namespace ePDFView
{
    class PageView: public IPageView
    {
        public:
            PageView (void);
            ~PageView (void);

            gdouble getHorizontalScroll (void);
            GtkWidget *getTopWidget (void);
            void getPagePosition (gint widgetX, gint widgetY,
                                  gint *pageX, gint *pageY);
            void getSize (gint *width, gint *height);
            gdouble getVerticalScroll (void);
            void makeRectangleVisible (DocumentRectangle &rect, gdouble scale);
            void resizePage (gint width, gint height);
            void scrollPage (gdouble scrollX, gdouble scrollY,
                             gint dx, gint dy);
            void setCursor (PageCursor cursorType);
            void setPresenter (PagePter *pter);
            
            void showPage (DocumentPage *page, PageScroll scroll);
            void tryReShowPage (void);
            
            void showText (const gchar *text);
            
            void setInvertColorToggle(char on); // krogan edit

        protected:
			DocumentPage *lastPageShown;
			PageScroll lastScroll;
			char hasShownAPage;
        
            PageCursor m_CurrentCursor;
            GtkWidget *m_EventBox;
            GtkWidget *m_PageImage;
            GtkWidget *m_PageScroll;
            GdkPixbuf *m_CurrentPixbuf; // GTK4: Store current pixbuf separately

            GdkPixbuf *getPixbufFromPage (DocumentPage *page);
            
            char invertColorToggle; // krogan edit
    };
}

#endif // !__PAGE_VIEW_H__
