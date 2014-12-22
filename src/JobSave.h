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

#if !defined (__JOB_SAVE_H__)
#define __JOB_SAVE_H__

namespace ePDFView
{
    // Forward declarations.
    class IDocument;

    ///
    /// @class JobSave
    /// @brief A background job that saves a document's copy to a file.
    ///
    /// This class is used to save copies of PDF files.
    ///
    class JobSave: public IJob
    {
        public:
            JobSave (void);
            ~JobSave (void);

            IDocument &getDocument (void);
            GError *getError (void);
            const gchar *getFileName (void);
            gboolean run (void);
            void setDocument (IDocument *document);
            void setError (GError *error);
            void setFileName (const gchar *fileName);

        protected:
            /// The document to notify when loaded or on error.
            IDocument *m_Document;
            /// The error produced when loading.
            GError *m_Error;
            /// The file name to load or reload.
            gchar *m_FileName;
    };
}

#endif // __JOB_SAVE_H__
