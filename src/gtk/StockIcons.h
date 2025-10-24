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

#if !defined (__STOCK_ICONS_H__)
#define __STOCK_ICONS_H__

#include <glib.h>

G_BEGIN_DECLS

// GTK4 stock icon replacements - use named icons instead of deprecated stock icons
#define GTK_STOCK_ABOUT "help-about"
#define GTK_STOCK_ADD "list-add"
#define GTK_STOCK_CANCEL "process-stop"
#define GTK_STOCK_CLOSE "window-close"
#define GTK_STOCK_DIALOG_AUTHENTICATION "dialog-password"
#define GTK_STOCK_FIND "edit-find"
#define GTK_STOCK_FULLSCREEN "view-fullscreen"
#define GTK_STOCK_GO_BACK "go-previous"
#define GTK_STOCK_GO_FORWARD "go-next"
#define GTK_STOCK_GO_TO_FIRST "go-first"
#define GTK_STOCK_GO_TO_LAST "go-last"
#define GTK_STOCK_OK "dialog-ok"
#define GTK_STOCK_OPEN "document-open"
#define GTK_STOCK_PREFERENCES "preferences-system"
#define GTK_STOCK_PRINT "document-print"
#define GTK_STOCK_SAVE "document-save"
#define GTK_STOCK_SAVE_AS "document-save-as"
#define GTK_STOCK_ZOOM_100 "zoom-original"
#define GTK_STOCK_ZOOM_FIT "zoom-fit-best"
#define GTK_STOCK_ZOOM_IN "zoom-in"
#define GTK_STOCK_ZOOM_OUT "zoom-out"

// ePDFView stock icons.
#define EPDFVIEW_STOCK_FIND_NEXT        "epdfview_find-next"
#define EPDFVIEW_STOCK_FIND_PREVIOUS    "epdfview_find-previous"
#define EPDFVIEW_STOCK_ROTATE_LEFT      "epdfview_rotate-left"
#define EPDFVIEW_STOCK_ROTATE_RIGHT     "epdfview_rotate-right"
#define EPDFVIEW_STOCK_ZOOM_WIDTH       "epdfview_zoom-width"

void epdfview_stock_icons_init (void);

G_END_DECLS

#endif // !__STOCK_ICONS_H__
