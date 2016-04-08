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

#if !defined (__CONFIG_H__)
#define __CONFIG_H__

namespace ePDFView
{
    ///
    /// @class Config
    /// @brief Configuration.
    ///
    /// The configuration class tries to follow the singleton patter but with
    /// a variation: the application can destroy the created instance by 
    /// calling Config::destroy(). This, along with the Config::loadFile() 
    /// function, are used almost only for testing.
    ///
    class Config
    {
        public:
            static void destroy (void);
            static Config &getConfig (void);
            static void loadFile (gboolean load);

            ~Config (void);

            gchar *getExternalBrowserCommandLine (void);
			gchar *getExternalBacksearchCommandLine (void);
            gchar *getOpenFileFolder (void);
            gchar *getSaveFileFolder (void);
            gint getWindowHeight (void);
            gint getWindowWidth (void);
            gint getWindowX (void);
            gint getWindowY (void);
            gboolean showMenubar (void); //krogan
            gboolean invertToggle (void); //krogan
            gboolean showStatusbar (void);
            gboolean showToolbar (void);
            gboolean zoomToFit (void);
            gboolean zoomToWidth (void);
            void save(void);
            void setExternalBrowserCommandLine (const gchar *commandLine);
			void setExternalBacksearchCommandLine (const gchar *commandLine);
			void setOpenFileFolder (const gchar *folder);
            void setSaveFileFolder (const gchar *folder);
            void setShowMenubar (gboolean show); //krogan
            void setInvertToggle (gboolean on); //krogan
            void setShowStatusbar (gboolean show);
            void setShowToolbar (gboolean show);
            void setWindowSize (gint width, gint height);
            void setWindowPos (gint x, gint y);
            void setZoomToFit (gboolean active);
            void setZoomToWidth (gboolean active);

        protected:
            /// The configuration values.
            GKeyFile *m_Values;
            
            Config (void);

            ///
            /// @brief Constructs a Config object as a copy of another.
            ///
            /// This is defined as protected to prevent instantiate
            /// a new Config object besides the one created when calling 
            /// Config::getConfig().
            ///
            /// @param config The Config object to copy.
            ///
            Config (Config &config) { }

            /// The global configuration object that is returned by
            /// ePDFView::Config::getConfig() function.
            static Config *m_Config;
            /// This is for test only purposes. If this variables is
            /// set to false, the Config class doesn't load
            /// the configuration values from the file and doesn't save
            /// it when destroyed.
            static gboolean m_LoadFile;

            gboolean getBoolean (const gchar *group, const gchar *key,
                                 gboolean defaultValue);
            gint getInteger (const gchar *group, const gchar *key, 
                             gint defaultValue);
            gchar *getString (const gchar *group, const gchar *key, 
                              const gchar *defaultValue);
    };
}

#endif // !__CONFIG_H__
