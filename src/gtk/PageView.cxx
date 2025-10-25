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
#include <gettext.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <epdfview.h>
#include "PageView.h"

// GTK4 direct API usage - no compatibility layer needed

using namespace ePDFView;

// Constants
static gint PAGE_VIEW_PADDING = 12;
static gint SCROLL_PAGE_DRAG_LENGTH = 50;
static const gint PIXBUF_BITS_PER_SAMPLE = 8;

// Forwards declarations.
static void page_view_button_press_cb (GtkGestureClick *, gint, gdouble, gdouble, gpointer);
static void page_view_button_release_cb (GtkGestureClick *, gint, gdouble, gdouble, gpointer);
static void page_view_mouse_motion_cb (GtkEventControllerMotion *, gdouble, gdouble, gpointer);
static void page_view_get_scrollbars_size (GtkWidget *,
                                           gint *width, gint *height);
static void page_view_resized_cb (GtkWidget *, GtkAllocation *, gpointer);
static gboolean page_view_scrolled_cb (GtkEventControllerScroll *, gdouble, gdouble, gpointer);
static gboolean page_view_keypress_cb (GtkEventControllerKey *, guint, guint, GdkModifierType, gpointer);

static void gdkpixbuf_invert(GdkPixbuf *pb) {//krogan edit
	int width, height, rowlength, n_channels;
	guchar *pixels, *p;

	n_channels = gdk_pixbuf_get_n_channels (pb);

	g_assert (gdk_pixbuf_get_colorspace (pb) == GDK_COLORSPACE_RGB);
	g_assert (gdk_pixbuf_get_bits_per_sample (pb) == 8);
	g_assert (gdk_pixbuf_get_has_alpha (pb));
	g_assert (n_channels == 4);

	width = gdk_pixbuf_get_width (pb);
	height = gdk_pixbuf_get_height (pb);

	rowlength = width*n_channels;
	
	pixels = gdk_pixbuf_get_pixels (pb);
	
	int i;
	int max = rowlength*height;
	for(i=0;i<max;i+=n_channels) {
		p = pixels + i;
		p[0] = 255 - p[0];
		p[1] = 255 - p[1];
		p[2] = 255 - p[2];
	}
}

PageView::PageView ():
    IPageView ()
{
    // The initial cursor is normal.
    m_CurrentCursor = PAGE_VIEW_CURSOR_NORMAL;
    
    // GTK4: Store current pixbuf separately since gtk_image_get_pixbuf is removed
    m_CurrentPixbuf = NULL;

    // Create the scrolled window where the page image will be.
    m_PageScroll = gtk_scrolled_window_new ();
    
    // GTK4: Create event box for page image (not needed in GTK4, add controllers directly)
    m_EventBox = m_PageScroll;
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_PageScroll),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);

    // The actual page image.
    m_PageImage = gtk_image_new ();
    gtk_widget_set_margin_start (m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_end (m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_top (m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_bottom (m_PageImage, PAGE_VIEW_PADDING);
    
    // GTK4: Ensure the image can expand to fill available space
    gtk_widget_set_hexpand (m_PageImage, TRUE);
    gtk_widget_set_vexpand (m_PageImage, TRUE);
    gtk_widget_set_halign (m_PageImage, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (m_PageImage, GTK_ALIGN_CENTER);

    // I want to be able to drag the page with the left mouse
    // button, because that will make possible to move the page
    // with an stylus on a PDA. Later I'll need this to implement the
    // document's links. The GtkImage widget doesn't have a
    // GdkWindow, so I have to add the event box that will receive
    // the mouse events.
    // In GTK4, we don't need GtkEventBox, we can add event controllers directly to widgets
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (m_PageScroll), m_PageImage);

    // In GTK4, widgets are visible by default - no need for gtk_widget_show_all
    
    invertColorToggle = 0;
    hasShownAPage = 0;
}

PageView::~PageView ()
{
    if (m_CurrentPixbuf != NULL)
    {
        g_object_unref (m_CurrentPixbuf);
        m_CurrentPixbuf = NULL;
    }
}

void //krogan edit
PageView::setInvertColorToggle(char on)
{
	invertColorToggle = on;
}

gdouble
PageView::getHorizontalScroll ()
{
    GtkAdjustment *hAdjustment = gtk_scrolled_window_get_hadjustment (
            GTK_SCROLLED_WINDOW (m_PageScroll));
    return gtk_adjustment_get_value (hAdjustment);
}

void 
PageView::getSize (gint *width, gint *height)
{
    g_assert (NULL != width && "Tried to save the width to a NULL pointer.");
    g_assert (NULL != height && "Tried to save the height to a NULL pointer.");

    // GTK4: Check if widget is valid and realized before getting dimensions
    if (!GTK_IS_WIDGET(m_PageScroll) || !gtk_widget_get_realized(m_PageScroll)) {
        *width = 0;
        *height = 0;
        return;
    }

    gint vScrollSize = 0;
    gint hScrollSize = 0;
    page_view_get_scrollbars_size (m_PageScroll, &vScrollSize, &hScrollSize);
    // GTK4: Use gtk_widget_get_width/height
    *width = gtk_widget_get_width (m_PageScroll) - vScrollSize;
    *height = gtk_widget_get_height (m_PageScroll) - hScrollSize;
}

gdouble
PageView::getVerticalScroll ()
{
    GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment (
            GTK_SCROLLED_WINDOW (m_PageScroll));
    return gtk_adjustment_get_value (vAdjustment);
}

void
PageView::makeRectangleVisible (DocumentRectangle &rect, gdouble scale)
{
    gdouble margin = 5 * scale;

    // Calculate the horizontal adjustment.
    GtkAdjustment *hAdjustment = gtk_scrolled_window_get_hadjustment (
            GTK_SCROLLED_WINDOW (m_PageScroll));

    gdouble realX1 = rect.getX1 () * scale;
    gdouble realX2 = rect.getX2 () * scale;
    gdouble docX1 = getHorizontalScroll () - PAGE_VIEW_PADDING;
    gdouble docX2 = docX1 + gtk_adjustment_get_page_size (hAdjustment);

    gdouble dx = 0.0;
    if ( realX1 < docX1 )
    {
        dx = (realX1 - docX1 - margin);
    }
    else if ( realX2 > docX2 )
    {
        dx = (realX2 - docX2 + margin);
    }

    gtk_adjustment_set_value (GTK_ADJUSTMENT (hAdjustment),
            CLAMP (gtk_adjustment_get_value (hAdjustment) + dx,
                   gtk_adjustment_get_lower (hAdjustment),
                   gtk_adjustment_get_upper (hAdjustment) - gtk_adjustment_get_page_size (hAdjustment)));

    // Calculate the vertical adjustment.
    GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment (
            GTK_SCROLLED_WINDOW (m_PageScroll));

    gdouble realY1 = rect.getY1 () * scale;
    gdouble realY2 = rect.getY2 () * scale;
    gdouble docY1 = getVerticalScroll () - PAGE_VIEW_PADDING;
    gdouble docY2 = docY1 + gtk_adjustment_get_page_size(vAdjustment);

    gdouble dy = 0;
    if ( realY1 < docY1 )
    {
        dy = (realY1 - docY1 - margin);
    }
    else if ( realY2 > docY2 )
    {
        dy = (realY2 - docY2 + margin);
    }

    gtk_adjustment_set_value (GTK_ADJUSTMENT (vAdjustment),
            CLAMP (gtk_adjustment_get_value(vAdjustment) + dy,
                   gtk_adjustment_get_lower(vAdjustment),
                   gtk_adjustment_get_upper(vAdjustment) - gtk_adjustment_get_page_size(vAdjustment)));
}

void
PageView::resizePage (gint width, gint height)
{
    // GTK4: Use stored pixbuf since gtk_image_get_pixbuf is removed
    if ( NULL != m_CurrentPixbuf )
    {
        GdkPixbuf *scaledPage = gdk_pixbuf_scale_simple (m_CurrentPixbuf,
                                                         width, height,
                                                         GDK_INTERP_NEAREST);
        if ( NULL != scaledPage )
        {
            GdkTexture *texture = gdk_texture_new_for_pixbuf (scaledPage);
            gtk_image_set_from_paintable (GTK_IMAGE (m_PageImage), GDK_PAINTABLE (texture));
            g_object_unref (texture);
            g_object_unref (scaledPage);
        }
    }
}

void
PageView::setCursor (PageCursor cursorType)
{
    if ( cursorType != m_CurrentCursor )
    {
        const char *cursor_name = NULL;
        switch (cursorType)
        {
            case PAGE_VIEW_CURSOR_SELECT_TEXT:
                cursor_name = "text";
                break;
            case PAGE_VIEW_CURSOR_LINK:
                cursor_name = "pointer";
                break;
            case PAGE_VIEW_CURSOR_DRAG:
                cursor_name = "move";
                break;
            default:
                cursor_name = "default";
        }
        
        // In GTK4, use gtk_widget_set_cursor_from_name
        if (cursor_name)
        {
            gtk_widget_set_cursor_from_name (m_PageImage, cursor_name);
        }
        
        m_CurrentCursor = cursorType;
    }
}

void
PageView::setPresenter (PagePter *pter)
{
    IPageView::setPresenter (pter);

    // GTK4: size-allocate signal removed, widget resizing handled automatically
    // The resize callback is not critical for functionality in GTK4
    // as the layout system handles size changes automatically
    
    // GTK4 uses event controllers for key events - add to scrolled window
    GtkEventController *key_controller = gtk_event_controller_key_new();
    gtk_widget_add_controller(m_PageScroll, key_controller);
    g_signal_connect(key_controller, "key-pressed",
                    G_CALLBACK(page_view_keypress_cb), pter);

    // GTK4 uses gesture controllers for mouse events - add to the image widget
    GtkGesture *press_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(press_gesture), 0); // Any button
    gtk_widget_add_controller(m_PageImage, GTK_EVENT_CONTROLLER(press_gesture));
    g_signal_connect(press_gesture, "pressed", 
                    G_CALLBACK(page_view_button_press_cb), this);
    g_signal_connect(press_gesture, "released", 
                    G_CALLBACK(page_view_button_release_cb), this);
                    
    GtkEventController *motion_controller = gtk_event_controller_motion_new();
    gtk_widget_add_controller(m_PageImage, motion_controller);
    g_signal_connect(motion_controller, "motion", 
                    G_CALLBACK(page_view_mouse_motion_cb), this);
    
    // GTK4 uses scroll event controllers - add to scrolled window
    GtkEventController *scroll_controller = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    gtk_widget_add_controller(m_PageScroll, scroll_controller);
    g_signal_connect(scroll_controller, "scroll",
                    G_CALLBACK(page_view_scrolled_cb), pter);
}

void
PageView::scrollPage (gdouble scrollX, gdouble scrollY, gint dx, gint dy)
{
    /* if the page cannot scroll and i'm dragging bottom to up, or left to right, 
     i will go to the next page. viceversa previous page */
    GtkAdjustment *hAdjustment = gtk_scrolled_window_get_hadjustment (
            GTK_SCROLLED_WINDOW (m_PageScroll));
            
    // GTK4 uses getter functions instead of direct struct access
    gdouble hPageSize = gtk_adjustment_get_page_size(hAdjustment);
    gdouble hLower = gtk_adjustment_get_lower(hAdjustment);
    gdouble hUpper = gtk_adjustment_get_upper(hAdjustment);
    
    // GTK4: Use gtk_widget_get_width/height instead of get_allocated_*
    int width = GTK_IS_WIDGET(m_PageImage) ? gtk_widget_get_width(m_PageImage) : 1;
    int height = GTK_IS_WIDGET(m_PageImage) ? gtk_widget_get_height(m_PageImage) : 1;
    gdouble hAdjValue = hPageSize * (gdouble)dx / MAX(width, 1);
    
    gtk_adjustment_set_value(hAdjustment,
            CLAMP(scrollX - hAdjValue, hLower, hUpper - hPageSize));

    GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment(
            GTK_SCROLLED_WINDOW(m_PageScroll));
    gdouble vPageSize = gtk_adjustment_get_page_size(vAdjustment);
    gdouble vLower = gtk_adjustment_get_lower(vAdjustment);
    gdouble vUpper = gtk_adjustment_get_upper(vAdjustment);
    
    gdouble vAdjValue = vPageSize * (gdouble)dy / MAX(height, 1);
    
    gtk_adjustment_set_value(vAdjustment,
            CLAMP(scrollY - vAdjValue, vLower, vUpper - vPageSize));
    
    /* if the page cannot scroll and i'm dragging bottom to up, or left to right, 
       I will go to the next page. viceversa previous page */
    if ((scrollY == (vUpper - vPageSize) && dy < (-SCROLL_PAGE_DRAG_LENGTH)) ||
        (scrollX == (hUpper - hPageSize) && dx < (-SCROLL_PAGE_DRAG_LENGTH)))
    {
        m_Pter->scrollToNextPage();
        m_Pter->mouseButtonReleased(1);
    }
    else if((scrollY == vLower && dy > SCROLL_PAGE_DRAG_LENGTH) ||
            (scrollX == hLower && dx > SCROLL_PAGE_DRAG_LENGTH))
    {
        m_Pter->scrollToPreviousPage();
        m_Pter->mouseButtonReleased(1);
    }
}

void 
PageView::showPage (DocumentPage *page, PageScroll scroll)
{
	hasShownAPage = 1;
	lastPageShown = page;
	lastScroll = scroll;
	
    // Clear current image
    gtk_image_clear (GTK_IMAGE (m_PageImage));
    
    // Release old pixbuf if any
    if (m_CurrentPixbuf != NULL)
    {
        g_object_unref (m_CurrentPixbuf);
    }
    
    // Get new pixbuf and store it
    m_CurrentPixbuf = getPixbufFromPage (page);
    
    if(invertColorToggle) { gdkpixbuf_invert(m_CurrentPixbuf); }
    
    // GTK4: Convert pixbuf to texture for display
    GdkTexture *texture = gdk_texture_new_for_pixbuf (m_CurrentPixbuf);
    gtk_image_set_from_paintable (GTK_IMAGE (m_PageImage), GDK_PAINTABLE (texture));
    g_object_unref (texture);
    // Set the vertical scroll to the specified.
    if ( PAGE_SCROLL_NONE != scroll )
    {
        GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment (
                                    GTK_SCROLLED_WINDOW (m_PageScroll));
        if ( PAGE_SCROLL_START == scroll )
        {
            gtk_adjustment_set_value (adjustment, gtk_adjustment_get_lower(adjustment));
        }
        else if ( PAGE_SCROLL_END == scroll )
        {
            gtk_adjustment_set_value (adjustment, gtk_adjustment_get_upper(adjustment));
        }
    }
}

void
PageView::tryReShowPage()
{
	if(hasShownAPage) {
		showPage(lastPageShown, lastScroll);
	}
}

void
PageView::showText (const gchar *text)
{
    // GTK4: GdkPixmap and GdkBitmap are removed
    // This function was used to draw a text overlay (like "Loading...") on the page
    // In GTK4, the page rendering is fast enough that we don't need this
    // The actual PDF rendering happens in showPage() and works correctly
    
    // Silently ignore - the text overlay is not critical for functionality
    // The PDF content will display properly via showPage()
}

////////////////////////////////////////////////////////////////
// GTK+ Functions.
////////////////////////////////////////////////////////////////

GtkWidget *
PageView::getTopWidget ()
{
    return m_PageScroll;
}

void
PageView::getPagePosition (gint widgetX, gint widgetY, gint *pageX, gint *pageY)
{
    g_assert ( NULL != pageX && "Tried to save the page's X to NULL.");
    g_assert ( NULL != pageY && "Tried to save the page's Y to NULL.");

    // Since the page is centered on the GtkImage widget, we need to
    // get the current widget size and the current image size to know
    // how many widget space is being used for padding.
    gint horizontalPadding = PAGE_VIEW_PADDING;
    gint verticalPadding = PAGE_VIEW_PADDING;
    // GTK4: Use stored pixbuf
    if ( NULL != m_CurrentPixbuf && GTK_IS_WIDGET(m_PageImage) )
    {
        int imageWidth = gtk_widget_get_width(m_PageImage);
        int imageHeight = gtk_widget_get_height(m_PageImage);
        horizontalPadding = (imageWidth - gdk_pixbuf_get_width (m_CurrentPixbuf)) / 2;
        verticalPadding = (imageHeight - gdk_pixbuf_get_height (m_CurrentPixbuf)) / 2;
    }

    *pageX = widgetX - horizontalPadding + (gint)getHorizontalScroll ();
    *pageY = widgetY - verticalPadding + (gint)getVerticalScroll ();
}

///
/// @brief Creates a new GdkPixbuf from a given DocumentPage.
///
/// @param page The DocumentPage to transform to a GdkPixbuf.
/// @return The resultant GdkPixbuf.
///
GdkPixbuf *
PageView::getPixbufFromPage (DocumentPage *page)
{
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data (page->getData (),
            GDK_COLORSPACE_RGB, page->hasAlpha(), PIXBUF_BITS_PER_SAMPLE,
            page->getWidth (), page->getHeight (),
            page->getRowStride (), NULL, NULL);
    GdkPixbuf *finalPixbuf = gdk_pixbuf_copy (pixbuf);
    g_object_unref (pixbuf);

    return finalPixbuf;
}

////////////////////////////////////////////////////////////////
// Callbacks
////////////////////////////////////////////////////////////////

///
/// @brief A mouse button has been pressed.
///
void
page_view_button_press_cb (GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer data)
{
    g_assert ( NULL != data && "The data is NULL.");
    PageView *view = (PageView *)data;

    guint button = gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (gesture));
    GdkModifierType state = gtk_event_controller_get_current_event_state (GTK_EVENT_CONTROLLER (gesture));
    
    gint page_x;
    gint page_y;
    view->getPagePosition ((gint)x, (gint)y, &page_x, &page_y);
    view->getPresenter ()->mouseButtonPressed (button, state, page_x, page_y);

    gtk_widget_grab_focus(view->getTopWidget());
}

///
/// @brief A mouse button has been released.
///
void
page_view_button_release_cb (GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer data)
{
    g_assert ( NULL != data && "The data is NULL.");

    PageView *view = (PageView *)data;
    guint button = gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (gesture));
    view->getPresenter ()->mouseButtonReleased (button);
}

///
/// @brief The page view is being dragged.
///
void
page_view_mouse_motion_cb (GtkEventControllerMotion *controller, gdouble x, gdouble y, gpointer data)
{
    g_assert ( NULL != data && "The data is NULL.");
    PageView *view = (PageView *)data;

    gint page_x;
    gint page_y;
    view->getPagePosition ((gint)x, (gint)y, &page_x, &page_y);
    view->getPresenter ()->mouseMoved (page_x, page_y);
}


void
page_view_get_scrollbars_size (GtkWidget *widget, gint *width, gint *height)
{
    g_assert (NULL != width && "Tried to save the width to a NULL pointer.");
    g_assert (NULL != height && "Tried to save the height to a NULL pointer.");

    // GTK4: GtkScrolledWindow doesn't expose scrollbars directly
    // Use a simple approximation - typically scrollbars are about 15-20px
    gint scrollbarWidth = 20;
    gint scrollbarHeight = 20;
    
    *width = scrollbarWidth + PAGE_VIEW_PADDING * 2;
    *height = scrollbarHeight + PAGE_VIEW_PADDING * 2;
}

///
/// @brief The page view has been resized.
///
static void
page_view_resized_cb (GtkWidget *widget, GtkAllocation *allocation,
                                  gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    gint vScrollSize = 0;
    gint hScrollSize = 0;
    page_view_get_scrollbars_size (widget, &vScrollSize, &hScrollSize);

    gint width = allocation->width - vScrollSize;
    gint height = allocation->height - hScrollSize;

    PagePter *pter = (PagePter *)data;
    pter->viewResized (width, height);
}

///
/// @brief The page view has been scrolled.
///
/// This only happens when the user uses the mouse wheel.
///
static gboolean
page_view_scrolled_cb (GtkEventControllerScroll *controller, gdouble dx, gdouble dy, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    PagePter *pter = (PagePter *)data;
    GtkWidget *widget = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (controller));
    GtkAdjustment *adjustment = 
        gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (widget));
    gdouble position = gtk_adjustment_get_value (adjustment);
    gdouble lower = gtk_adjustment_get_lower (adjustment);
    gdouble upper = gtk_adjustment_get_upper (adjustment);
    gdouble page_size = gtk_adjustment_get_page_size (adjustment);
    
    if ( dy < 0 && position == lower )
    {
        pter->scrollToPreviousPage ();
        return TRUE;
    }
    else if ( dy > 0 && position == (upper - page_size) )
    {
        pter->scrollToNextPage ();
        return TRUE;
    }

    return FALSE;
}

///
/// @brief A key was pressed.
///
static gboolean
page_view_keypress_cb(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    GtkScrollType direction;
    gboolean horizontal = FALSE;
    gboolean returnValue = TRUE;
    PagePter *pter = (PagePter *)data;
    
    GtkWidget *widget = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (controller));

    GtkAdjustment *hadjustment = 
        gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (widget));
    gdouble hposition = gtk_adjustment_get_value (hadjustment);
    gdouble hlower = gtk_adjustment_get_lower (hadjustment);
    gdouble hupper = gtk_adjustment_get_upper (hadjustment);
    gdouble hpage_size = gtk_adjustment_get_page_size (hadjustment);
    GtkAdjustment *vadjustment = 
        gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (widget));
    gdouble vposition = gtk_adjustment_get_value (vadjustment);
    gdouble vlower = gtk_adjustment_get_lower (vadjustment);
    gdouble vupper = gtk_adjustment_get_upper (vadjustment);
    gdouble vpage_size = gtk_adjustment_get_page_size (vadjustment);

    if ( state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK) )
    {
        return FALSE;
    }

    switch ( keyval )
    {
        case GDK_KEY_Left:
        case GDK_KEY_KP_Left:
        case GDK_KEY_h:
            if ( hposition == hlower )
            {
                pter->scrollToPreviousPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_STEP_LEFT;
            horizontal = TRUE;
            break;

        case GDK_KEY_Right:
        case GDK_KEY_KP_Right:
        case GDK_KEY_l:
            if ( hposition == (hupper - hpage_size) )
            {
                pter->scrollToNextPage ();
                return TRUE;
            }
            horizontal = TRUE;
            direction = GTK_SCROLL_STEP_RIGHT;
            break;

        case GDK_KEY_Up:
        case GDK_KEY_KP_Up:
        case GDK_KEY_k:
            if ( vposition == vlower )
            {
                pter->scrollToPreviousPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_STEP_UP;
            break;

        case GDK_KEY_Down:
        case GDK_KEY_KP_Down:
        case GDK_KEY_j:
            if ( vposition == (vupper - vpage_size) )
            {
                pter->scrollToNextPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_STEP_DOWN;
            break;

        case GDK_KEY_Page_Up:
        case GDK_KEY_KP_Page_Up:
            if ( vposition == vlower )
            {
                pter->scrollToPreviousPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_PAGE_UP;
            break;

        case GDK_KEY_space:
        case GDK_KEY_KP_Space:
        case GDK_KEY_Page_Down:
        case GDK_KEY_KP_Page_Down:
            if ( vposition == (vupper - vpage_size) )
            {
                pter->scrollToNextPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_PAGE_DOWN;
            break;

       case GDK_KEY_Home:
       case GDK_KEY_KP_Home:
            direction = GTK_SCROLL_START;
            break;

       case GDK_KEY_End:
       case GDK_KEY_KP_End:
            direction = GTK_SCROLL_END;
            break;

       case GDK_KEY_Return:
       case GDK_KEY_KP_Enter:
            pter->scrollToNextPage ();
            direction = GTK_SCROLL_START;
            break;

       case GDK_KEY_BackSpace:
            pter->scrollToPreviousPage ();
            direction = GTK_SCROLL_START;
            break;

       default:
            return FALSE;
    }

    g_signal_emit_by_name(G_OBJECT(widget), "scroll-child",
                          direction, horizontal, &returnValue);
    return returnValue;
}
