// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda Gutiérrez

#ifndef __PAGE_VIEW_H__
#define __PAGE_VIEW_H__

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <utility>  // for std::move

// GLib types
typedef int gint;
typedef double gdouble;
typedef int64_t gint64;
typedef char gchar;

// Forward declarations for ePDFView namespace
namespace ePDFView {
    class DocumentPage;
    class DocumentRectangle;
    class PagePter;
    class IPageView;
    
    // Use enum instead of enum class for forward compatibility
    enum PageCursor;
    enum PageScroll;
    
    // Zoom cache entry structure
    struct ZoomCacheEntry {
        GdkPixbuf *pixbuf{nullptr};
        gdouble zoomLevel{0.0};
        gint width{0};
        gint height{0};
        gint64 lastAccess{0}; // For LRU eviction
        
        ZoomCacheEntry() = default;
        
        ZoomCacheEntry(GdkPixbuf *pb, gdouble zoom, gint w, gint h, gint64 access);
        ~ZoomCacheEntry();
        
        // Disable copy
        ZoomCacheEntry(const ZoomCacheEntry&) = delete;
        ZoomCacheEntry& operator=(const ZoomCacheEntry&) = delete;
        
        // Enable move
        ZoomCacheEntry(ZoomCacheEntry&& other) noexcept;
        ZoomCacheEntry& operator=(ZoomCacheEntry&& other) noexcept;
    };
}

// Include IPageView after forward declarations
#include "IPageView.h"

namespace ePDFView {
    class PageView: public IPageView
    {
    private:
        // Zoom cache entry structure
        struct ZoomCacheEntry {
            GdkPixbuf *pixbuf{nullptr};
            gdouble zoomLevel{0.0};
            gint width{0};
            gint height{0};
            gint64 lastAccess{0}; // For LRU eviction
            
            ZoomCacheEntry() = default;
            
            ZoomCacheEntry(GdkPixbuf *pb, gdouble zoom, gint w, gint h, gint64 access)
                : pixbuf(pb ? GDK_PIXBUF(g_object_ref(pb)) : nullptr),
                  zoomLevel(zoom),
                  width(w),
                  height(h),
                  lastAccess(access) 
            {}
            
            ~ZoomCacheEntry() {
                if (pixbuf) {
                    g_object_unref(pixbuf);
                    pixbuf = nullptr;
                }
            }
            
            // Disable copy
            ZoomCacheEntry(const ZoomCacheEntry&) = delete;
            ZoomCacheEntry& operator=(const ZoomCacheEntry&) = delete;
            
            // Enable move
            ZoomCacheEntry(ZoomCacheEntry&& other) noexcept 
                : pixbuf(other.pixbuf),
                  zoomLevel(other.zoomLevel),
                  width(other.width),
                  height(other.height),
                  lastAccess(other.lastAccess) {
                other.pixbuf = nullptr;
            }
            
            ZoomCacheEntry& operator=(ZoomCacheEntry&& other) noexcept {
                if (this != &other) {
                    // Clean up existing resources
                    if (pixbuf) {
                        g_object_unref(pixbuf);
                    }
                    
                    // Transfer ownership
                    pixbuf = other.pixbuf;
                    zoomLevel = other.zoomLevel;
                    width = other.width;
                    height = other.height;
                    lastAccess = other.lastAccess;
                    
                    // Reset source
                    other.pixbuf = nullptr;
                }
                return *this;
            }
        };
        
        // Cache of rendered pages at different zoom levels
        std::vector<ZoomCacheEntry> m_ZoomCache;
        static constexpr gint MAX_ZOOM_CACHE_ENTRIES = 5; // Keep last 5 zoom levels in cache
        gint64 m_CacheAccessCounter{0};
        
        // Cache management methods
        void clearZoomCache();
        GdkPixbuf* getFromZoomCache(gdouble zoom, gint width, gint height);
        void addToZoomCache(GdkPixbuf *pixbuf, gdouble zoom, gint width, gint height);
        GdkPixbuf *getPixbufFromPage(DocumentPage *page);
        
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
            void setZoom(gdouble zoom);
            GdkPixbuf *getCurrentPixbuf(void) { return m_CurrentPixbuf; }

        protected:
			DocumentPage *lastPageShown;
			PageScroll lastScroll;
			char hasShownAPage;
        
            PageCursor m_CurrentCursor;
            GtkWidget *m_EventBox;
            GtkWidget *m_PageImage;
            GtkWidget *m_PageScroll;
            GdkPixbuf *m_CurrentPixbuf; // GTK4: Store current pixbuf separately
            GdkPixbuf *m_CurrentScaledPixbuf; // GTK4: Store scaled pixbuf for drawing
            gdouble m_ZoomLevel;
            
            char invertColorToggle; // krogan edit
    };
}

#endif // !__PAGE_VIEW_H__
