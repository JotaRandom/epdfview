// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - A lightweight PDF Viewer.
// 
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

#if !defined (__JOB_LOAD_H__)
#define __JOB_LOAD_H__

namespace ePDFView
{
    // Forward declarations.
    class IDocument;
   
    ///
    /// @class JobLoad
    /// @brief A background job that loads a file.
    ///
    /// This class is used to load and reload the PDF files.
    ///
    class JobLoad: public IJob
    {
        public:
            JobLoad (void);
            ~JobLoad (void);

            IDocument &getDocument (void);
            GError *getError (void);
            const gchar *getFileName (void);
            const gchar *getPassword (void);
            gboolean isReloading (void);
            gboolean run (void);
            void setDocument (IDocument *document);
            void setError (GError *error);
            void setFileName (const gchar *fileName);
            void setPassword (const gchar *password);
            void setReload (gboolean reload);

        protected:
            /// The document to notify when loaded or on error.
            IDocument *m_Document;
            /// The error produced when loading.
            GError *m_Error;
            /// The file name to load or reload.
            gchar *m_FileName;
            /// The password to use when loading the file.
            gchar *m_Password;
            /// Tells if we are reloading or loading from new.
            gboolean m_Reload;
    };
}

#endif // __JOB_LOAD_H__
