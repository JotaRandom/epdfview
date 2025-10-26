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

#include <gdk/gdk.h>
#include "epdfview.h"

using namespace ePDFView;

// Types

///
/// @brief Information to hold while a page is not available.
///
typedef struct
{
    /// The presenter to use to try again.
    PagePter *pter;
    /// The scroll to be used when the page is available.
    PageScroll scroll;
}
PageNotAvailableData;

///
/// @struct _DragInfo
/// @brief Pointer device and page scroll information while dragging the page.
///
/// This structure is used by ePDFView::PagePter when the user is dragging 
/// the page with the a pointer device.
///
struct _DragInfo
{
    /// The X position of the pointer when the drag started.
    gint x;
    /// The Y position of the pointer when the drag started.
    gint y;
    /// The horizontal position of the page view scroll when the drag started.
    int startX;
    /// The vertical position of the page view scroll when the drag started.
    int startY;
};

// Global variables.
static gboolean g_WaitingForPage = FALSE;

///
/// @brief Constructs a new PagePter object.
///
PagePter::PagePter (IDocument *document)
{
    m_LastSelection = NULL;
    m_Document = document;
    m_Document->attach (this);
    m_DragInfo = NULL;
    m_NextPageScroll = PAGE_SCROLL_START;
    m_PageView = NULL;
    m_ScrollMode = PagePterModeScroll;
}

///
/// @brief Destroys all dynamically allocated memory by PagePter.
///
PagePter::~PagePter ()
{
    if (m_LastSelection) {
        cairo_region_destroy(m_LastSelection);
        m_LastSelection = NULL;
    }
    delete m_DragInfo;
    m_Document->detach (this);
}

void
PagePter::setInvertColorToggle(char on) { //krogan
	m_PageView->setInvertColorToggle(on);
}

///
/// @brief Gets the size of the page view.
///
/// @param width The position to save the view's width to.
/// @param height The position to save the view's height to.
///
void
PagePter::getSize (gint *width, gint *height)
{
    getView ().getSize (width, height);
}

///
/// @brief Gets the presenter's view.
///
/// @return The reference to the view that was set with setView().
///
IPageView &
PagePter::getView ()
{
    g_assert (NULL != m_PageView && "Tried to get the view but it's NULL.");

    return (*m_PageView);
}

void
PagePter::onCtrlButton(gint button, gint state,gint x, gint y)
{
	// Use Ctrl-Right click
	if (1 != button)
		return;
	// printf("Returning...\n\n");
	gchar *backsearchCommandLine =
		Config::getConfig().getExternalBacksearchCommandLine();
	
	// printf("Config:\n%s\n", backsearchCommandLine);
	// printf("Tokenizing...\n");
	gchar **backsearchTokens = g_strsplit(backsearchCommandLine," ",-1);
	
	for (gint i=0; backsearchTokens[i] != NULL; i++)
	{
		gchar *token = backsearchTokens[i];
		// printf("Token %d: '%s'\n",i,token);
		
		if (0 == g_strcmp0(token,"%p"))
		{
			g_free(backsearchTokens[i]);
			gint page = m_Document -> getCurrentPageNum();	// get page number
			backsearchTokens[i] = g_strdup_printf("%d",page);
			continue;
		}
		
		if (0 == g_strcmp0(token,"%x"))
		{
			g_free(backsearchTokens[i]);
			backsearchTokens[i] = g_strdup_printf("%d",x);
			continue;
		}
		
		if (0 == g_strcmp0(token,"%y"))
		{
			g_free(backsearchTokens[i]);
			backsearchTokens[i] = g_strdup_printf("%d",y);
			continue;
		}
			
		if (0 == g_strcmp0(token,"%f"))
		{
			g_free(backsearchTokens[i]);
			backsearchTokens[i] = g_strdup(m_Document -> getFileName());
			continue;
		}
	}
	gchar *commandLine = g_strjoinv(" ",backsearchTokens);
	
	// printf("Should be executing:\n%s\n",commandLine);

	GError *error = NULL;
    if ( !g_spawn_command_line_async (commandLine, &error) )
    {
        g_error_free (error);
    }
	
	// Cleanup	
	g_free (commandLine);
	g_strfreev(backsearchTokens);
	g_free(backsearchCommandLine);
	return;
}
///
void
PagePter::tryReShowPage() {
	m_PageView->tryReShowPage();
}

///
/// @brief A mouse button has been pressed.
///
/// This is called by the view when any mouse or pointer device button
/// has been pressed.
///
/// @param button The button that has been pressed.
/// @param state  The state of the keyboard (i.e. Ctrl pressed, etc.)
/// @param x The X coordinate relative to the top-left corner of the document
///          when the button was pressed.
/// @param y The Y coordinate relative to the top-left corner of the document
///          when the button was pressed.
///
void
PagePter::mouseButtonPressed (gint button, gint state,gint x, gint y)
{
	//
	// This is new
	//
	if (0 != (state & GDK_CONTROL_MASK))
	{
		onCtrlButton(button, state,x , y);
		return;
	}

	
    if ( 1 == button )
    {
        if ( m_Document->hasLinkAtPosition (x, y) )
        {
            m_Document->activateLinkAtPosition (x, y);
        }
        else
        {
            m_DragInfo = new DragInfo;
            m_DragInfo->x = x;
            m_DragInfo->y = y;

            IPageView &view = getView ();
            m_DragInfo->startX = x;
            m_DragInfo->startY = y;
            if(m_ScrollMode == PagePterModeScroll)
                view.setCursor (PAGE_VIEW_CURSOR_DRAG);
            else
                view.setCursor (PAGE_VIEW_CURSOR_SELECT_TEXT);
        }
    }
}

///
/// @brief A mouse button has been released.
///
/// This is called by the view when a previously pressed mouse button is
/// now released.
///
/// @param button The mouse button that has been released.
///
void
PagePter::mouseButtonReleased (gint button)
{
    if ( 1 == button )
    {
        if(m_LastSelection)
            cairo_region_destroy(m_LastSelection);
        m_LastSelection = NULL;

        if ( m_Document->isLoaded() &&
             m_ScrollMode == PagePterModeSelectText &&
             NULL != m_DragInfo )
        {
            DocumentRectangle rect(m_DragInfo->startX, m_DragInfo->startY,
                                   m_DragInfo->x, m_DragInfo->y);
            m_Document->setTextSelection(&rect);
        }

        delete m_DragInfo;
        m_DragInfo = NULL;

        refreshPage(PAGE_SCROLL_NONE, FALSE);

        getView ().setCursor (PAGE_VIEW_CURSOR_NORMAL);
    }
}

///
/// @brief The mouse has been moved.
///
/// This is called by the view when the mouse or the pointer device has been
/// moved.
///
/// @param x The X coordinate relative to the top-left border of the document
///          where the cursor is now.
/// @param y The Y coordinate relative to the top-left border of the document
///          where the cursor is now.
///
void
PagePter::mouseMoved (gint x, gint y)
{
    IPageView &view = getView ();
    if ( NULL == m_DragInfo )
    {
        if ( m_Document->hasLinkAtPosition (x, y) )
        {
            view.setCursor (PAGE_VIEW_CURSOR_LINK);
        }
        else
        {
            view.setCursor (PAGE_VIEW_CURSOR_NORMAL);
        }
    }
    else{
        m_DragInfo->x = x;
        m_DragInfo->y = y;  

        if(m_ScrollMode == PagePterModeScroll){
            view.scrollPage (view.getHorizontalScroll (), view.getVerticalScroll (),
                             x - m_DragInfo->startX, y - m_DragInfo->startY);
        }
        else{
            if(!m_Document->isLoaded())
                return ;
            
            DocumentRectangle rect(m_DragInfo->startX, m_DragInfo->startY,
                                          m_DragInfo->x, m_DragInfo->y);

            cairo_region_t *region = m_Document->getTextRegion (&rect);
            if (NULL == region)
                return;

            if( !m_LastSelection || 
                !cairo_region_equal(m_LastSelection, region)
            ){
                if(m_LastSelection) {
                    cairo_region_destroy(m_LastSelection);
                }
                m_LastSelection = cairo_region_copy(region);
                DocumentPage *page = m_Document->getCurrentPage();
                if ( NULL != page )
                    refreshPage (PAGE_SCROLL_NONE, FALSE);
            }
            
            // Free the temporary region returned by getTextRegion
            cairo_region_destroy(region);
        }
    }
}

void
PagePter::notifyFindChanged (DocumentRectangle *matchRect)
{
    refreshPage (PAGE_SCROLL_NONE, FALSE);
    if ( NULL != matchRect )
    {
        getView ().makeRectangleVisible (*matchRect, m_Document->getZoom ());
    }
}

void
PagePter::notifyLoad ()
{
    g_WaitingForPage = FALSE;
    refreshPage (PAGE_SCROLL_START, FALSE);
}

void
PagePter::notifyPageChanged (gint pageNum)
{
    g_WaitingForPage = FALSE;
    refreshPage (m_NextPageScroll, FALSE);
}

void
PagePter::notifyPageRotated (gint rotation)
{
    g_WaitingForPage = FALSE;
    refreshPage (PAGE_SCROLL_START, FALSE);
}

void
PagePter::notifyPageZoomed (gdouble zoom)
{
    g_WaitingForPage = FALSE;
    refreshPage (PAGE_SCROLL_NONE, TRUE);
}

void
PagePter::notifyReload ()
{
    g_WaitingForPage = FALSE;
    refreshPage (PAGE_SCROLL_NONE, FALSE);
}

gboolean
PagePter::pageNotAvailable (gpointer user)
{
    g_assert (NULL != user && "The data parameter in NULL.");

    PageNotAvailableData *data = (PageNotAvailableData *)user;
    if ( g_WaitingForPage )
    {
        data->pter->refreshPage (data->scroll, FALSE);
        return TRUE;
    }
    delete data;
    return FALSE;
}

///
/// @brief Refreshes the current page.
///
/// This function is called when the presenter needs to show a document's
/// page. It only gets the rendered page from the document and then
/// pass the resultant DocumentPage to the view.
///
/// @param pageScroll This will be passed to the main view to let it know how
///                   to show the scroll this new page.
/// @param wasZoomed If this is set to TRUE then it means that the page
///                  has been refreshed because its scaled has changed.
///
void
PagePter::refreshPage (PageScroll pageScroll, gboolean wasZoomed)
{
    g_assert (NULL != m_Document && 
              "Tried to show a page from a NULL document.");
    
    if ( m_Document->isLoaded () )
    {
        IPageView &view = getView ();
        DocumentPage *documentPage = m_Document->getCurrentPage ();
        if ( NULL != documentPage )
        {
            g_WaitingForPage = FALSE;
            if ( NULL != m_LastSelection )
                documentPage->setSelection(m_LastSelection);
            view.showPage (documentPage, pageScroll);
        }
        else
        {
            if ( !g_WaitingForPage )
            {
                documentPage = m_Document->getEmptyPage ();
                if ( wasZoomed )
                {
                    view.resizePage (documentPage->getWidth (),
                                     documentPage->getHeight ());
                }
                else
                {
                    view.showPage (documentPage, pageScroll);
                    delete documentPage;
                    view.showText (_("Loading..."));
                }

                g_WaitingForPage = TRUE;
                PageNotAvailableData *data = new PageNotAvailableData;
                data->pter = this;
                data->scroll = pageScroll;
                g_idle_add (PagePter::pageNotAvailable, data);
            }
        }
    }
}

///
/// @brief The user tried to scroll to the next page.
///
/// This is called when the view sees that the user tried to use
/// the mouse wheel to scroll down to the next page.
///
/// It just changes the document to the next page.
///
void
PagePter::scrollToNextPage ()
{
    setNextPageScroll (PAGE_SCROLL_START);
    m_Document->goToNextPage ();
}

///
/// @brief The user tried to scroll to the previous page.
///
/// This is called when the view sees that the user tried to use
/// the mouse wheel to scroll up to the previous page.
///
/// It just changes the document to the previous page.
///
void
PagePter::scrollToPreviousPage ()
{
    setNextPageScroll (PAGE_SCROLL_END);
    m_Document->goToPreviousPage ();
}

///
/// @brief Sets how to scroll the next time that the page must be refreshed.
///
/// A lot of times when the page has to be refreshed it must be scrolled
/// in some way, for example when the next page is shown we want to scroll
/// to the start of the page to continue reading.
///
/// @param next The next scroll to do when the page is refreshed.
///
void
PagePter::setNextPageScroll (PageScroll next)
{
    m_NextPageScroll = next;
}

///
/// @brief Sets the presenter's view.
///
/// The page presenter only "controls" the page image from the main
/// view, but this part is also created by the main view. The page presenter
/// requests the main view class to get the IPageView that it created.
///
/// @param view The main view to get the IPageView from.
///
void
PagePter::setView (IMainView &view)
{
    m_PageView = view.getPageView ();
    m_PageView->setPresenter (this);
}

///
/// @brief Sets the page mouse scroll mode
///
/// @param mode Scroll mode to set.
///
void
PagePter::setMode (PagePterMode mode)
{
    this->m_ScrollMode = mode;
}

///
/// @brief The page view has been resized.
///
/// @param width The new with of the page view.
/// @param height The new height of the page view.
///
void
PagePter::viewResized (gint width, gint height)
{
    static gint lastWidth = -1;
    static gint lastHeight = -1;

    if ( !m_Document->isLoaded() || 
         ( lastWidth == width && lastHeight == height) )
    {
        return;
    }
    
    lastWidth = width;
    lastHeight = height;
    
    Config &config = Config::getConfig ();

    if (config.zoomToFit()) {
        // Fit to page
        m_Document->zoomToFit(width, height);
    } else if (config.zoomToWidth()) {
        // Fit to width while maintaining aspect ratio
        gdouble pageWidth, pageHeight;
        m_Document->getPageSize(&pageWidth, &pageHeight);
        if (pageWidth > 0) {
            gdouble scale = (gdouble)width / pageWidth;
            m_Document->setZoom(scale);
        }
    }
}
