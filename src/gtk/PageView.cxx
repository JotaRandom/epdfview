// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda Gutiérrez
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

///
/// @brief Inverts the colors of a GdkPixbuf
///
/// This function inverts the RGB values of a GdkPixbuf while preserving the alpha channel.
/// @param pb The GdkPixbuf to invert. If NULL or invalid, the function does nothing.
///
static void gdkpixbuf_invert(GdkPixbuf *pb) {
    // Check if the pixbuf is valid
    if (pb == NULL) {
        g_warning("gdkpixbuf_invert: NULL pixbuf provided");
        return;
    }

    // Get pixbuf properties
    int n_channels = gdk_pixbuf_get_n_channels(pb);
    
    // Validate pixbuf format
    if (gdk_pixbuf_get_colorspace(pb) != GDK_COLORSPACE_RGB ||
        gdk_pixbuf_get_bits_per_sample(pb) != 8 ||
        !gdk_pixbuf_get_has_alpha(pb) ||
        n_channels != 4) {
        g_warning("gdkpixbuf_invert: Unsupported pixbuf format");
        return;
    }

    int width = gdk_pixbuf_get_width(pb);
    int height = gdk_pixbuf_get_height(pb);
    int rowstride = gdk_pixbuf_get_rowstride(pb);
    guchar *pixels = gdk_pixbuf_get_pixels(pb);
    
    // Check if we have valid dimensions and pixel data
    if (width <= 0 || height <= 0 || pixels == NULL) {
        g_warning("gdkpixbuf_invert: Invalid pixbuf dimensions or pixel data");
        return;
    }
    
    // Invert each pixel
    for (int y = 0; y < height; y++) {
        guchar *row = pixels + y * rowstride;
        for (int x = 0; x < width; x++) {
            guchar *p = row + x * n_channels;
            p[0] = 255 - p[0];  // Red
            p[1] = 255 - p[1];  // Green
            p[2] = 255 - p[2];  // Blue
            // p[3] is alpha, leave it unchanged
        }
    }
}

PageView::PageView ():
    IPageView ()
{
    // The initial cursor is normal.
    m_CurrentCursor = PAGE_VIEW_CURSOR_NORMAL;
    
    // GTK4: Store current pixbuf separately since gtk_image_get_pixbuf is removed
    m_CurrentPixbuf = NULL;

    // The current zoom level
    m_ZoomLevel = 1.0;

    // Create the scrolled window where the page image will be.
    m_PageScroll = gtk_scrolled_window_new();
    
    // Configure scrolled window to allow both horizontal and vertical scrolling
    m_EventBox = m_PageScroll;
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(m_PageScroll),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
    
    // Configure scrolled window properties for better zoom behavior
    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(m_PageScroll), 10);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(m_PageScroll), 10);
    gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(m_PageScroll), FALSE);
    gtk_widget_set_hexpand(m_PageScroll, TRUE);
    gtk_widget_set_vexpand(m_PageScroll, TRUE);

    // Create a container box to hold the image
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    
    // Create the actual page image
    m_PageImage = gtk_picture_new();
    
    // Configure the image to maintain its size and not be constrained by the window
    gtk_widget_set_hexpand(m_PageImage, FALSE);
    gtk_widget_set_vexpand(m_PageImage, FALSE);
    gtk_widget_set_halign(m_PageImage, GTK_ALIGN_START);
    gtk_widget_set_valign(m_PageImage, GTK_ALIGN_START);
    
    // Add padding around the image
    gtk_widget_set_margin_start(m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_end(m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_top(m_PageImage, PAGE_VIEW_PADDING);
    gtk_widget_set_margin_bottom(m_PageImage, PAGE_VIEW_PADDING);
    
    // Add the image to the box and the box to the scrolled window
    gtk_box_append(GTK_BOX(box), m_PageImage);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(m_PageScroll), box);
    
    // Set a minimum size for the scrolled window
    gtk_widget_set_size_request(m_PageScroll, 100, 100);

    // In GTK4, widgets are visible by default - no need for gtk_widget_show_all
    
    // Initialize zoom level to 1.0 (100%)
    m_ZoomLevel = 1.0;
    
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
        // Return reasonable default size if widget not ready
        *width = 800;
        *height = 600;
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
    if (m_CurrentPixbuf == NULL) {
        return;
    }

    // Freeze the scrolled window to prevent flickering
    g_object_freeze_notify(G_OBJECT(m_PageScroll));
    
    // Get current adjustments before any changes
    GtkAdjustment *hAdjustment = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(m_PageScroll));
    GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(m_PageScroll));
    
    // Get current scroll positions as ratios (0.0 to 1.0)
    gdouble hratio = 0.0, vratio = 0.0;
    gdouble hpage_size = gtk_adjustment_get_page_size(hAdjustment);
    gdouble vpage_size = gtk_adjustment_get_page_size(vAdjustment);
    gdouble hadj_upper = gtk_adjustment_get_upper(hAdjustment);
    gdouble vadj_upper = gtk_adjustment_get_upper(vAdjustment);
    gdouble hvalue = gtk_adjustment_get_value(hAdjustment);
    gdouble vvalue = gtk_adjustment_get_value(vAdjustment);
    
    // Calculate scroll position ratios if we have scrollable content
    if (hadj_upper > hpage_size && (hadj_upper - hpage_size) > 0) {
        hratio = hvalue / (hadj_upper - hpage_size);
        hratio = CLAMP(hratio, 0.0, 1.0);
    }
    if (vadj_upper > vpage_size && (vadj_upper - vpage_size) > 0) {
        vratio = vvalue / (vadj_upper - vpage_size);
        vratio = CLAMP(vratio, 0.0, 1.0);
    }
    
    // Calculate new dimensions with zoom and padding
    gint newWidth = (gint)(width * m_ZoomLevel) + (2 * PAGE_VIEW_PADDING);
    gint newHeight = (gint)(height * m_ZoomLevel) + (2 * PAGE_VIEW_PADDING);
    
    // Ensure minimum size to prevent rendering issues
    newWidth = MAX(newWidth, 10);
    newHeight = MAX(newHeight, 10);
    
    // Get the viewport size
    gint viewport_width, viewport_height;
    gtk_widget_get_size_request(GTK_WIDGET(m_PageScroll), &viewport_width, &viewport_height);
    if (viewport_width <= 0 || viewport_height <= 0) {
        viewport_width = gtk_widget_get_allocated_width(GTK_WIDGET(m_PageScroll));
        viewport_height = gtk_widget_get_allocated_height(GTK_WIDGET(m_PageScroll));
    }
    
    // Scale the pixbuf to the new size
    GdkPixbuf *scaledPage = gdk_pixbuf_scale_simple(m_CurrentPixbuf,
                                                   (gint)(width * m_ZoomLevel),
                                                   (gint)(height * m_ZoomLevel),
                                                   GDK_INTERP_BILINEAR);
    
    if (scaledPage != NULL) {
        // Create a texture from the scaled pixbuf
        GdkTexture *texture = gdk_texture_new_for_pixbuf(scaledPage);
        
        // Set the texture to the picture
        gtk_picture_set_paintable(GTK_PICTURE(m_PageImage), GDK_PAINTABLE(texture));
        
        // Set the size request to the scaled size
        gtk_widget_set_size_request(m_PageImage, newWidth, newHeight);
        
        // Update the adjustments to match the new content size
        gtk_adjustment_configure(hAdjustment,
                                gtk_adjustment_get_value(hAdjustment),  // value
                                0.0,                                   // lower
                                (gdouble)newWidth,                     // upper
                                viewport_width * 0.1,                  // step_increment
                                viewport_width * 0.9,                  // page_increment
                                MIN(viewport_width, newWidth));         // page_size
        
        gtk_adjustment_configure(vAdjustment,
                                gtk_adjustment_get_value(vAdjustment),  // value
                                0.0,                                   // lower
                                (gdouble)newHeight,                    // upper
                                viewport_height * 0.1,                 // step_increment
                                viewport_height * 0.9,                 // page_increment
                                MIN(viewport_height, newHeight));       // page_size
        
        // Calculate and set the new scroll positions
        if (newWidth > viewport_width) {
            gdouble new_hvalue = hratio * (newWidth - viewport_width);
            gtk_adjustment_set_value(hAdjustment, CLAMP(new_hvalue, 0, newWidth - viewport_width));
        } else {
            gtk_adjustment_set_value(hAdjustment, 0);
        }
        
        if (newHeight > viewport_height) {
            gdouble new_vvalue = vratio * (newHeight - viewport_height);
            gtk_adjustment_set_value(vAdjustment, CLAMP(new_vvalue, 0, newHeight - viewport_height));
        } else {
            gtk_adjustment_set_value(vAdjustment, 0);
        }
        
        // Force the scrolled window to update its scrollbars
        gtk_widget_queue_resize(GTK_WIDGET(m_PageScroll));
        
        // Force a redraw of the scrolled window
        gtk_widget_queue_draw(m_PageScroll);
        
        // Clean up
        g_object_unref(texture);
        g_object_unref(scaledPage);
    } else {
        // If scaling failed, log the error
        g_warning("Failed to scale pixbuf for resizing");
    }
    
    // Always thaw the scrolled window
    g_object_thaw_notify(G_OBJECT(m_PageScroll));
}

void
PageView::setZoom (gdouble zoom)
{
    // Only update if the zoom level has actually changed
    if (ABS(zoom - m_ZoomLevel) > 0.001 && zoom > 0.05)  // Minimum zoom level of 5%
    {
        // Store the old zoom level for calculations
        gdouble oldZoom = m_ZoomLevel;
        
        // Update the zoom level
        m_ZoomLevel = zoom;
        
        // If we have a current pixbuf, resize the page to apply the new zoom
        if (m_CurrentPixbuf != NULL)
        {
            // Get the original dimensions of the pixbuf (without any zoom)
            gint origWidth = gdk_pixbuf_get_width(m_CurrentPixbuf);
            gint origHeight = gdk_pixbuf_get_height(m_CurrentPixbuf);
            
            // Resize the page with the new zoom level
            resizePage(origWidth, origHeight);
            
            // Notify any listeners about the zoom change
            if (m_Pter != NULL) {
                m_Pter->notifyPageZoomed(m_ZoomLevel);
            }
        }
    }
}

void
PageView::setCursor (PageCursor cursorType)
{
    if (cursorType != m_CurrentCursor)
    {
        const char *cursor_name = NULL;
        switch (cursorType)
        {
            case PAGE_VIEW_CURSOR_NORMAL:
                cursor_name = "default";
                break;
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
    gtk_picture_set_paintable (GTK_PICTURE (m_PageImage), NULL);
    
    // Release old pixbuf if any
    if (m_CurrentPixbuf != NULL)
    {
        g_object_unref (m_CurrentPixbuf);
    }
    
    // Get new pixbuf and store it
    m_CurrentPixbuf = getPixbufFromPage (page);
    
    if (m_CurrentPixbuf == NULL) {
        g_warning("PageView::showPage: Failed to create pixbuf from page!");
        return;
    }
    
    g_message("PageView::showPage: Created pixbuf %dx%d", 
              gdk_pixbuf_get_width(m_CurrentPixbuf),
              gdk_pixbuf_get_height(m_CurrentPixbuf));
    
    // Invert colors if needed
    if (invertColorToggle && m_CurrentPixbuf != NULL) {
        gdkpixbuf_invert(m_CurrentPixbuf);
    }
    
    // GTK4: Convert pixbuf to texture for display
    GdkTexture *texture = gdk_texture_new_for_pixbuf (m_CurrentPixbuf);
    if (texture == NULL) {
        g_warning("PageView::showPage: Failed to create texture from pixbuf!");
        return;
    }
    
    gtk_picture_set_paintable (GTK_PICTURE (m_PageImage), GDK_PAINTABLE (texture));
    g_message("PageView::showPage: Successfully set image from texture");
    g_object_unref (texture);
    
    // GTK4: Force widget visibility and request proper size
    gtk_widget_set_visible(m_PageImage, TRUE);
    
    // GTK4: Get pixbuf dimensions for debugging
    gint pixbuf_width = gdk_pixbuf_get_width(m_CurrentPixbuf);
    gint pixbuf_height = gdk_pixbuf_get_height(m_CurrentPixbuf);
    
    // GTK4: Configure image to display at natural size
    gtk_widget_set_halign(m_PageImage, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(m_PageImage, GTK_ALIGN_CENTER);
    
    // GTK4: Force the widget to request its natural size
    gtk_widget_set_size_request(m_PageImage, -1, -1);
    
    gtk_widget_queue_resize(m_PageImage);
    gtk_widget_queue_draw(m_PageImage);
    
    // Also ensure parent scrolled window is visible
    gtk_widget_set_visible(m_PageScroll, TRUE);
    
    // Debug: Check widget state
    g_message("PageView::showPage: Image widget visible=%d, width=%d, height=%d, mapped=%d, size_req=%dx%d",
              gtk_widget_get_visible(m_PageImage),
              gtk_widget_get_width(m_PageImage),
              gtk_widget_get_height(m_PageImage),
              gtk_widget_get_mapped(m_PageImage),
              pixbuf_width, pixbuf_height);
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
    
    // Only handle size changes if the allocation is valid
    if (allocation->width <= 1 || allocation->height <= 1) {
        return;
    }

    // Get the page view instance
    PageView *view = (PageView *)g_object_get_data(G_OBJECT(widget), "page-view-instance");
    if (view == NULL) {
        return;
    }

    // Get scrollbar sizes
    gint vScrollSize = 0;
    gint hScrollSize = 0;
    page_view_get_scrollbars_size(widget, &vScrollSize, &hScrollSize);

    // Calculate available size for the page
    gint width = MAX(1, allocation->width - vScrollSize);
    gint height = MAX(1, allocation->height - hScrollSize);

    // Only notify the presenter if we have a significant size change
    static gint last_width = 0;
    static gint last_height = 0;
    
    if (abs(width - last_width) > 5 || abs(height - last_height) > 5) {
        last_width = width;
        last_height = height;
        
        PagePter *pter = (PagePter *)data;
        if (pter != NULL) {
            pter->viewResized(width, height);
        }
        
        // Force an update of the scrollbars
        if (view != NULL) {
            gint pixbuf_width, pixbuf_height;
            view->getSize(&pixbuf_width, &pixbuf_height);
            view->resizePage(pixbuf_width, pixbuf_height);
        }
    }
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
