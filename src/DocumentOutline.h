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

#if !defined (__DOCUMENT_OUTLINE_H__)
#define __DOCUMENT_OUTLINE_H__

namespace ePDFView
{
    ///
    /// @class DocumentOutline
    /// @brief Stores the document's outline title, page num. and its children.
    ///
    /// Some documents have an index with them. This index can be used to
    /// help the user navigate through the document, similar to a table of
    /// contents.
    ///
    /// Each node contains a title, a destination page an possibly children
    /// nodes. The only node that have an empty name and a 0 page number is the
    /// top level DocumentOutline, that is only used as a container for all 
    /// other DocumentOutline objects.
    ///
    class DocumentOutline
    {
        public:
            DocumentOutline (void);
            ~DocumentOutline (void);

            void addChild (DocumentOutline *child);
            gint getDestinationPage (void);
            DocumentOutline *getFirstChild (void);
            DocumentOutline *getNextChild (void);
            gint getNumChildren (void);
            const gchar *getTitle (void);
            void setParent (DocumentOutline *parent);
            void setTitle (const gchar *title);
            void setDestination (gint destination);

        protected:
            /// The list of this outline's children.
            GList *m_Children;
            /// The page number this outline points to.
            gint m_Destination;
            /// @brief This is used to know which child to return when calling
            /// the DocumentOutline::getNextChild() function.
            GList *m_LastReturnedChild;
            /// The outline's parent outline.
            DocumentOutline *m_Parent;
            /// The outline's name or title.
            gchar *m_Title;
    };
}

#endif // !__DOCUMENT_OUTLINE_H__
