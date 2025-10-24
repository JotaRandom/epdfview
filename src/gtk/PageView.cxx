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

    // Create the scrolled window where the page image will be.
    m_PageScroll = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_PageScroll),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);

    // The actual page image.
    m_PageImage = gtk_image_new ();
    gtk_widget_set_margin_start (m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_end (m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_top (m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_bottom (m_PageImage, PAGE_VIEW_PADDING);

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

    gint vScrollSize = 0;
    gint hScrollSize = 0;
    page_view_get_scrollbars_size (m_PageScroll, &vScrollSize, &hScrollSize);
    *width = gtk_widget_get_allocated_width (m_PageScroll) - vScrollSize;
    *height = gtk_widget_get_allocated_height (m_PageScroll) - hScrollSize;
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
    gdouble docY2 = docY1 + vAdjustment->page_size;

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
            CLAMP (vAdjustment->value + dy,
                   vAdjustment->lower,
                   vAdjustment->upper - vAdjustment->page_size));
}

void
PageView::resizePage (gint width, gint height)
{
    GdkPixbuf *originalPage = gtk_image_get_pixbuf (GTK_IMAGE (m_PageImage));
    if ( NULL != originalPage )
    {
        GdkPixbuf *scaledPage = gdk_pixbuf_scale_simple (originalPage,
                                                         width, height,
                                                         GDK_INTERP_NEAREST);
        if ( NULL != scaledPage )
        {
            gtk_image_set_from_pixbuf (GTK_IMAGE (m_PageImage), scaledPage);
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
            gtk_widget_set_cursor_from_name (m_EventBox, cursor_name);
        }
        
        m_CurrentCursor = cursorType;
    }
}

void
PageView::setPresenter (PagePter *pter)
{
    IPageView::setPresenter (pter);

    // When resizing.
    g_signal_connect (G_OBJECT (m_PageScroll), "size-allocate",
                      G_CALLBACK (page_view_resized_cb), pter);
    // When scrolling.
    g_signal_connect (G_OBJECT (m_PageScroll), "scroll-event",
                      G_CALLBACK (page_view_scrolled_cb), pter);
    
#ifdef GTK_4_0
// Use GTK4 API
    // GTK4 uses event controllers for key events
    GtkEventController *key_controller = gtk_event_controller_key_new();
    gtk_widget_add_controller(m_PageScroll, key_controller);
    g_signal_connect(key_controller, "key-press",
                    G_CALLBACK(page_view_keypress_cb), pter);

    // GTK4 uses gesture controllers for mouse events
    GtkGesture *press_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(press_gesture), 0); // Any button
    gtk_widget_add_controller(m_EventBox, GTK_EVENT_CONTROLLER(press_gesture));
    g_signal_connect(press_gesture, "pressed", 
                    G_CALLBACK(page_view_button_press_cb), this);
    g_signal_connect(press_gesture, "released", 
                    G_CALLBACK(page_view_button_release_cb), this);
                    
    GtkEventController *motion_controller = gtk_event_controller_motion_new();
    gtk_widget_add_controller(m_EventBox, motion_controller);
    g_signal_connect(motion_controller, "motion", 
                    G_CALLBACK(page_view_mouse_motion_cb), this);
#else
    // For GTK3 and earlier, use traditional event connections
    g_signal_connect (G_OBJECT (m_PageScroll), "key-press-event",
                      G_CALLBACK (page_view_keypress_cb), pter);
                      
    // Add events to the event box.
    gtk_widget_add_events (m_EventBox, 
            GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | 
            GDK_POINTER_MOTION_MASK);
    g_signal_connect (G_OBJECT (m_EventBox), "button-press-event",
                      G_CALLBACK (page_view_button_press_cb), this);
    g_signal_connect (G_OBJECT (m_EventBox), "button-release-event",
                      G_CALLBACK (page_view_button_release_cb), this);
    g_signal_connect (G_OBJECT (m_EventBox), "motion-notify-event",
                      G_CALLBACK (page_view_mouse_motion_cb), this);
#endif
}

void
PageView::scrollPage (gdouble scrollX, gdouble scrollY, gint dx, gint dy)
{
    /* if the page cannot scroll and i'm dragging bottom to up, or left to right, 
     i will go to the next page. viceversa previous page */
    GtkAdjustment *hAdjustment = gtk_scrolled_window_get_hadjustment (
            GTK_SCROLLED_WINDOW (m_PageScroll));
            
#ifdef GTK_4_0
    // GTK4 uses getter functions instead of direct struct access
    gdouble hPageSize = gtk_adjustment_get_page_size(hAdjustment);
    gdouble hLower = gtk_adjustment_get_lower(hAdjustment);
    gdouble hUpper = gtk_adjustment_get_upper(hAdjustment);
    
    // In GTK4, we need to get allocation differently
    int width, height;
    gtk_widget_get_size_request(m_PageImage, &width, &height);
    gdouble hAdjValue = hPageSize * (gdouble)dx / width;
    
    gtk_adjustment_set_value(hAdjustment,
            CLAMP(scrollX - hAdjValue, hLower, hUpper - hPageSize));

    GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment(
            GTK_SCROLLED_WINDOW(m_PageScroll));
    gdouble vPageSize = gtk_adjustment_get_page_size(vAdjustment);
    gdouble vLower = gtk_adjustment_get_lower(vAdjustment);
    gdouble vUpper = gtk_adjustment_get_upper(vAdjustment);
    
    gdouble vAdjValue = vPageSize * (gdouble)dy / height;
    
    gtk_adjustment_set_value(vAdjustment,
            CLAMP(scrollY - vAdjValue, vLower, vUpper - vPageSize));
    
    /* if the page cannot scroll and i'm dragging bottom to up, or left to right, 
       I will go to the next page. viceversa previous page */
    if ((scrollY == (vUpper - vPageSize) && dy < (-SCROLL_PAGE_DRAG_LENGTH)) ||
        (scrollX == (hUpper - hPageSize) && dx < (-SCROLL_PAGE_DRAG_LENGTH)))
#else
    gdouble hAdjValue = hAdjustment->page_size *
        (gdouble)dx / m_PageImage->allocation.width;
    gtk_adjustment_set_value (hAdjustment,
            CLAMP (scrollX - hAdjValue,
                   hAdjustment->lower,
                   hAdjustment->upper - hAdjustment->page_size));

    GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment (
            GTK_SCROLLED_WINDOW (m_PageScroll));
    gdouble vAdjValue = vAdjustment->page_size *
        (gdouble)dy / m_PageImage->allocation.height;
    gtk_adjustment_set_value (vAdjustment,
            CLAMP (scrollY - vAdjValue,
                   vAdjustment->lower,
                   vAdjustment->upper - vAdjustment->page_size));
    
    /* if the page cannot scroll and i'm dragging bottom to up, or left to right, 
       I will go to the next page. viceversa previous page */
    if ( (scrollY == (vAdjustment->upper - vAdjustment->page_size) &&
                dy < (-SCROLL_PAGE_DRAG_LENGTH) ) ||
        (scrollX == (hAdjustment->upper - hAdjustment->page_size) &&
         dx < (-SCROLL_PAGE_DRAG_LENGTH)) )
#endif
    {
        m_Pter->scrollToNextPage();
        m_Pter->mouseButtonReleased(1);
    }
#if GTK_CHECK_VERSION(4,0,0)
    else if((scrollY == vLower && dy > SCROLL_PAGE_DRAG_LENGTH) ||
            (scrollX == hLower && dx > SCROLL_PAGE_DRAG_LENGTH))
#else
    else if((scrollY == vAdjustment->lower && dy > SCROLL_PAGE_DRAG_LENGTH) ||
        (scrollX == hAdjustment->lower && dx > SCROLL_PAGE_DRAG_LENGTH))
#endif
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
	
    gtk_image_set_from_pixbuf (GTK_IMAGE (m_PageImage), NULL);
    GdkPixbuf *pixbuf = getPixbufFromPage (page);
    
    if(invertColorToggle) { gdkpixbuf_invert(pixbuf); }
    
    gtk_image_set_from_pixbuf (GTK_IMAGE (m_PageImage), pixbuf);
    g_object_unref (pixbuf);
    // Set the vertical scroll to the specified.
    if ( PAGE_SCROLL_NONE != scroll )
    {
        GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment (
                                    GTK_SCROLLED_WINDOW (m_PageScroll));
        if ( PAGE_SCROLL_START == scroll )
        {
            gtk_adjustment_set_value (adjustment, adjustment->lower);
        }
        else if ( PAGE_SCROLL_END == scroll )
        {
            gtk_adjustment_set_value (adjustment, adjustment->upper);
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
    // I need to make the changes to the original Pixbuf that the GtkImage
    // controls shows.
    // Since a Pixbuf is a client-side buffer, it's not drawable and the
    // gdk functions to draw using pango won't accept it.
    // The solution I found is to render the pixbuf to a pixmap, which is
    // server-side and therefore drawable, make the modifications
    // and set the modified pixbuf again to the GtkImage control.
    // 
    GdkPixbuf *originalPage = gtk_image_get_pixbuf (GTK_IMAGE (m_PageImage));
    if ( NULL != originalPage )
    {
        gint width = gdk_pixbuf_get_width (originalPage);
        gint height = gdk_pixbuf_get_height (originalPage);

        PangoLayout *layout =
            gtk_widget_create_pango_layout (m_PageImage, text);

        // I want the text to be half the page's width.
        // Since I don't know the font's size I just set the font
        // size to 1pt.
        PangoFontDescription *fontDescription = pango_font_description_new ();
        pango_font_description_set_size (fontDescription, PANGO_SCALE);
        pango_layout_set_font_description (layout, fontDescription);
        // The I get the logical rectangle that this text would
        // extent to if it was rendered using 1pt and calculate the
        // size in points it would take to make the width fill the
        // page's half width.
        PangoRectangle logicalRectangle;
        pango_layout_get_pixel_extents (layout, NULL, &logicalRectangle);
        gint targetWidth = MAX (width / 2, 1);
        gdouble realSize = 
            (gdouble)targetWidth / (gdouble)logicalRectangle.width;
        pango_font_description_set_size (fontDescription,
                                         (gint)realSize * PANGO_SCALE);        
        pango_layout_set_font_description (layout, fontDescription);
        pango_layout_get_pixel_extents (layout, NULL, &logicalRectangle);

        // Once the font are set up, I just need to render 
        // the pixbuf to the pixmap (copying from client-side to 
        // server size) and draw the layout.
        GdkPixmap *pixmap;
        GdkBitmap *mask;
        gdk_pixbuf_render_pixmap_and_mask (originalPage,
                                           &pixmap, &mask, 0);
        gdk_draw_layout (pixmap, 
                         m_PageImage->style->black_gc, 
                         (width - logicalRectangle.width) / 2, 
                         height / 4 - logicalRectangle.height / 2,
                         layout);
        pango_font_description_free (fontDescription);
        g_object_unref (layout);
        // Then I just copy back the image to a client-size pixbuf.
        GdkPixbuf *modifiedPage = 
            gdk_pixbuf_get_from_drawable (NULL,
                                          pixmap,
                                          NULL,
                                          0, 0,
                                          0, 0,
                                          width, height);
        g_object_unref (pixmap);
        if ( NULL != mask )
        {
            g_object_unref (mask);
        }

        gtk_image_set_from_pixbuf (GTK_IMAGE (m_PageImage), modifiedPage);
        g_object_unref (modifiedPage);
    } 
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
    GdkPixbuf *page = gtk_image_get_pixbuf (GTK_IMAGE (m_PageImage));
    if ( NULL != page )
    {
        horizontalPadding =
            (m_PageImage->allocation.width - gdk_pixbuf_get_width (page)) / 2;
        verticalPadding =
            (m_PageImage->allocation.height - gdk_pixbuf_get_height (page)) / 2;
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

    gint borderWidth = widget->style->xthickness;
    gint borderHeight = widget->style->ythickness;
    if ( GTK_SHADOW_NONE !=
            gtk_scrolled_window_get_shadow_type (GTK_SCROLLED_WINDOW (widget)) )
    {
        borderWidth += widget->style->xthickness;
        borderHeight += widget->style->ythickness;
    }

    gint scrollBarSpacing = 0;
    gtk_widget_style_get (widget, "scrollbar-spacing", &scrollBarSpacing, NULL);

    GtkWidget *vScrollBar = GTK_SCROLLED_WINDOW (widget)->vscrollbar;
    *width = vScrollBar->allocation.width +
             (PAGE_VIEW_PADDING + borderWidth) * 2 + scrollBarSpacing;

    GtkWidget *hScrollBar = GTK_SCROLLED_WINDOW (widget)->hscrollbar;
    *height = hScrollBar->allocation.height +
              (PAGE_VIEW_PADDING + borderHeight) * 2 + scrollBarSpacing;
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
page_view_scrolled_cb (GtkWidget *widget, GdkEventScroll *event, gpointer data)
{
    g_assert ( NULL != data && "The data parameter is NULL.");

    PagePter *pter = (PagePter *)data;
    GtkAdjustment *adjustment = 
        gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (widget));
    gdouble position = gtk_adjustment_get_value (adjustment);
    if ( GDK_SCROLL_UP == event->direction && position == adjustment->lower )
    {
        pter->scrollToPreviousPage ();
        return TRUE;
    }
    else if ( GDK_SCROLL_DOWN == event->direction &&
              position == ( adjustment->upper - adjustment->page_size) )
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
    GtkAdjustment *vadjustment = 
        gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (widget));
    gdouble vposition = gtk_adjustment_get_value (vadjustment);

    if ( state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK) )
    {
        return FALSE;
    }

    switch ( keyval )
    {
        case GDK_Left:
        case GDK_KP_Left:
        case GDK_h:
            if ( hposition == hadjustment->lower )
            {
                pter->scrollToPreviousPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_STEP_LEFT;
            horizontal = TRUE;
            break;

        case GDK_Right:
        case GDK_KP_Right:
        case GDK_l:
            if ( hposition == ( hadjustment->upper - hadjustment->page_size) )
            {
                pter->scrollToNextPage ();
                return TRUE;
            }
            horizontal = TRUE;
            direction = GTK_SCROLL_STEP_RIGHT;
            break;

        case GDK_Up:
        case GDK_KP_Up:
        case GDK_k:
            if ( vposition == vadjustment->lower )
            {
                pter->scrollToPreviousPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_STEP_UP;
            break;

        case GDK_Down:
        case GDK_KP_Down:
        case GDK_j:
            if ( vposition == ( vadjustment->upper - vadjustment->page_size) )
            {
                pter->scrollToNextPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_STEP_DOWN;
            break;

        case GDK_Page_Up:
        case GDK_KP_Page_Up:
            if ( vposition == vadjustment->lower )
            {
                pter->scrollToPreviousPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_PAGE_UP;
            break;

        case GDK_space:
        case GDK_KP_Space:
        case GDK_Page_Down:
        case GDK_KP_Page_Down:
            if ( vposition == ( vadjustment->upper - vadjustment->page_size) )
            {
                pter->scrollToNextPage ();
                return TRUE;
            }
            direction = GTK_SCROLL_PAGE_DOWN;
            break;

       case GDK_Home:
       case GDK_KP_Home:
            direction = GTK_SCROLL_START;
            break;

       case GDK_End:
       case GDK_KP_End:
            direction = GTK_SCROLL_END;
            break;

       case GDK_Return:
       case GDK_KP_Enter:
            pter->scrollToNextPage ();
            direction = GTK_SCROLL_START;
            break;

       case GDK_BackSpace:
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
