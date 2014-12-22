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
#include <gtk/gtkstock.h>
#include <gtk/gtkiconfactory.h>
#include <gdk/gdkpixbuf.h>
#include "StockIcons.h"

static const struct 
{
    const char *iconName;
    const char *iconFile;
} g_StockIcons[] =
{
    { EPDFVIEW_STOCK_FIND_NEXT, "stock_find_next_24.png" },
    { EPDFVIEW_STOCK_FIND_PREVIOUS, "stock_find_previous_24.png" },
    { EPDFVIEW_STOCK_ROTATE_LEFT, "stock_rotate-270.png" },
    { EPDFVIEW_STOCK_ROTATE_RIGHT, "stock_rotate-90.png" },  
    { EPDFVIEW_STOCK_ZOOM_WIDTH, "stock_zoom-page-width.png" }
};

static GtkStockItem g_StockItems[] =
{
    { EPDFVIEW_STOCK_FIND_NEXT, N_("Find _Next"), (GdkModifierType)0, 0, PACKAGE },
    { EPDFVIEW_STOCK_FIND_PREVIOUS, N_("Find _Previous"), (GdkModifierType)0, 0, PACKAGE },
    { EPDFVIEW_STOCK_ROTATE_LEFT, N_("Rotate _Left"), (GdkModifierType)0, 0, PACKAGE },
    { EPDFVIEW_STOCK_ROTATE_RIGHT, N_("Rotate _Right"), (GdkModifierType)0, 0, PACKAGE },
    { EPDFVIEW_STOCK_ZOOM_WIDTH, N_("Zoom to _Width"), (GdkModifierType)0, 0, PACKAGE }
};

void
epdfview_stock_icons_init (void)
{
    GtkIconFactory *factory = gtk_icon_factory_new ();
    gtk_icon_factory_add_default (factory);

    // Add the custom stock icons.
    int numIcons = G_N_ELEMENTS (g_StockIcons);
    for ( int iconIndex = 0 ; iconIndex < numIcons ; iconIndex++ )
    {
        gchar *fileName = g_strdup_printf ("%s/pixmaps/%s", DATADIR,
                g_StockIcons[iconIndex].iconFile);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (fileName, NULL);        
        g_free (fileName);
        if ( NULL != pixbuf )
        {
            GtkIconSet *iconSet = gtk_icon_set_new_from_pixbuf (pixbuf);
            gtk_icon_factory_add (factory, g_StockIcons[iconIndex].iconName, 
                                  iconSet);
            gtk_icon_set_unref (iconSet);
        }
    }
    g_object_unref (G_OBJECT (factory));

    gtk_stock_add_static (g_StockItems, G_N_ELEMENTS (g_StockItems));
}
