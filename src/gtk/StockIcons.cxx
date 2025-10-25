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
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "StockIcons.h"

// GTK4 stock icon definitions moved to StockIcons.h

// Custom ePDFView icons that need to be loaded from files
static const struct 
{
    const char *iconName;
    const char *iconFile;
} g_CustomIcons[] =
{
    { EPDFVIEW_STOCK_FIND_NEXT, "stock_find_next_24.png" },
    { EPDFVIEW_STOCK_FIND_PREVIOUS, "stock_find_previous_24.png" },
    { EPDFVIEW_STOCK_ROTATE_LEFT, "stock_rotate-270.png" },
    { EPDFVIEW_STOCK_ROTATE_RIGHT, "stock_rotate-90.png" },  
    { EPDFVIEW_STOCK_ZOOM_WIDTH, "stock_zoom-page-width.png" }
};

void
epdfview_stock_icons_init (void)
{
    // In GTK4, we add the data directory to the icon theme search path
    // This allows the icon theme to find our custom icons
    GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display (gdk_display_get_default ());
    
    // Add both system and local pixmaps directories
    // This handles both /usr/share and /usr/local/share installations
    const gchar *search_paths[] = {
        "/usr/share/pixmaps",           // System location
        "/usr/local/share/pixmaps",     // Local installation
        DATADIR "/pixmaps",             // Configured prefix
        DATADIR,                        // Fallback to data dir
        NULL
    };
    
    for (int i = 0; search_paths[i] != NULL; i++) {
        if (g_file_test(search_paths[i], G_FILE_TEST_IS_DIR)) {
            gtk_icon_theme_add_search_path (icon_theme, search_paths[i]);
        }
    }
    
    // The custom icons will be automatically found by their names when requested
    // No need to manually load them - GTK4 icon theme handles this
}
