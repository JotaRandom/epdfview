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

#if !defined (__JOB_RENDER_H__)
#define __JOB_RENDER_H__

namespace ePDFView
{
    ///
    /// @class JobRender
    /// @brief A background job that renders a page.
    ///
    /// This class is used to render a single page. Render means not only
    /// get the image but also the links on that page.
    ///
    class JobRender: public IJob
    {
        public:
            JobRender ();
            ~JobRender ();

            gboolean run (void);

            guint32 getAge (void);
            IDocument *getDocument (void);
            DocumentPage *getPageImage (void);
            gint getPageNumber (void);
            void setAge (guint32 age);
            void setDocument (IDocument *document);
            void setPageNumber (gint pageNumber);

            static gboolean m_CanProcessJobs;
            static void setMinAge (guint32 age);

        protected:
            /// The job's age.
            guint32 m_Age;
            /// The class to use to render the page.
            IDocument *m_Document;
            static guint32 m_MinAge;
            /// The page's rendered image.
            DocumentPage *m_PageImage;
            /// The page's number to render.
            gint m_PageNumber;

    };
}

#endif // !__JOB_RENDER_H__
