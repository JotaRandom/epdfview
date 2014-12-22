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
#include "epdfview.h"

using namespace ePDFView;

// Forward declarations.
static void deleteChild (gpointer child, gpointer userData);

///
/// @brief Constructs a new DocumentOutline.
///
DocumentOutline::DocumentOutline ()
{
    m_Destination = 1;
    m_Children = NULL;
    m_LastReturnedChild = NULL;
    m_Parent = NULL;
    m_Title = NULL;
}

///
/// @brief Destroys any dynamically allocated memory for DocumentOutline.
///
DocumentOutline::~DocumentOutline ()
{
    m_Parent = NULL;
    g_free (m_Title);
    /// Delete the children.
    g_list_foreach (m_Children, deleteChild, NULL);
    g_list_free (m_Children);
}

///
/// @brief Adds a new outline's child.
///
/// @param child The new child to add.
///
void
DocumentOutline::addChild (DocumentOutline *child)
{
    m_Children = g_list_append (m_Children, child);
}

///
/// @brief Gets the outline's destination page.
///
/// I'm only using the outlines as a way to go to an specific page, although
/// the PDF Specification says that an outline could open a page from another
/// file, execute an application, etc... I won't use all those features and
/// concentrate only on the pages.
///
/// @return The destination page number for the outline number or 1 if the
///         outline item has no page number (like the root outline item or
///         other kinds of destinations).
///
gint
DocumentOutline::getDestinationPage ()
{
    return m_Destination;
}

///
/// @brief The first children of the outline item.
///
/// @return The pointer to the first children of the outline item, or
///         NULL if the outline has no children.
///
DocumentOutline *
DocumentOutline::getFirstChild ()
{
    m_LastReturnedChild = g_list_first (m_Children);
    if ( NULL != m_LastReturnedChild )
    {
        return (DocumentOutline *)m_LastReturnedChild->data;
    }
    return NULL;
}

///
/// @brief Gets the next outline's child.
///
/// @returns The next child after the one returned by the last call to 
///          getFirstChild() or getNextChild().
///          If no child has been returned yet (i.e., getFirstChild() hasn't
///          been called yet) or there's no next child returns NULL.
///
DocumentOutline *
DocumentOutline::getNextChild ()
{
    m_LastReturnedChild = g_list_next (m_LastReturnedChild);
    if ( NULL != m_LastReturnedChild )
    {
        return (DocumentOutline *)m_LastReturnedChild->data;
    }
    return NULL;
}

///
/// @brief Gets the number of children for this outline.
///
/// @return The number of children this outline has.
///
gint
DocumentOutline::getNumChildren ()
{
    return g_list_length (m_Children);
}

///
/// @brief Gets the outline's title.
///
/// @return The title of the outline item or an empty string if the
///         outline item has no title (like the root outline item).
///
const gchar *
DocumentOutline::getTitle ()
{
    if ( NULL == m_Title )
    {
        return "";
    }
    return m_Title;
}

///
/// @brief Sets the outline item's destination.
///
/// @see getDestination().
///
/// @param destination The page number, starting from 1, that
///                    this outline links to.
///
void
DocumentOutline::setDestination (gint destination)
{
    m_Destination = destination;
}

///
/// @brief Sets the item's parent.
///
/// This is currently not used.
///
/// @param parent The outline item's parent.
///
void
DocumentOutline::setParent (DocumentOutline *parent)
{
    m_Parent = parent;
}

///
/// @brief Sets the outline item's title.
///
/// @param title The title to set to the outline.
///
void
DocumentOutline::setTitle (const gchar *title)
{
    g_assert (NULL != title && "Tried to set a NULL title.");

    g_free (m_Title);
    m_Title = g_strdup (title);
}

///
/// @brief Deletes an outline item's child.
///
/// This function is called by the destructor to delete all children in
/// the m_Children list.
///
/// @param child The data element in the list's item.
/// @param userData The data passed to the g_list_foreach() function.
///                 Currently unused.
///
void
deleteChild (gpointer child, gpointer userData)
{
    g_assert ( NULL != child && "An outline's child is NULL!");
    
    DocumentOutline *outline = (DocumentOutline *)child;
    delete outline;
}
