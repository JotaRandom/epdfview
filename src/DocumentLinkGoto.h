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

#if !defined (__DOCUMENT_LINK_GOTO_H__)
#define __DOCUMENT_LINK_GOTO_H__

namespace ePDFView
{
    class DocumentLinkGoto: public IDocumentLink
    {
        public:
            DocumentLinkGoto (gdouble x1, gdouble y1, gdouble x2, gdouble y2,
                              gint destinationPage);
            virtual ~DocumentLinkGoto (void);

            virtual void activate (IDocument *document);

        protected:
            /// The number of the link's destination page.
            gint m_DestinationPage;
    };
}

#endif // !__DOCUMENT_LINK_GOTO_H__
