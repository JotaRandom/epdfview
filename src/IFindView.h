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

#if !defined (__IFIND_VIEW_H__)
#define __IFIND_VIEW_H__

namespace ePDFView
{
    // Forward declarations.
    class FindPter;

    ///
    /// @class IFindView
    /// @brief Interface for the find view.
    ///
    /// The find view is the view that will ask to the user the text to
    /// find in the dialog and also the "find progress dialog", which
    /// tells the user where is looking for at the moment.
    ///
    class IFindView
    {
        public:
            ///
            /// @brief Destroys all allocated memory for IFindView.
            ///
            virtual ~IFindView (void)
            {
            }

            ///
            /// @brief Gets the view's presenter.
            ///
            /// @return The presenter that is controlling the view.
            ///
            FindPter *getPresenter (void)
            {
                return m_Pter;
            }

            ///
            /// @brief Sets the view's presenter.
            ///
            /// After settings the presenter, the view can show the
            /// find bar.
            ///
            /// @param pter The presenter that will control the view.
            ///
            virtual void setPresenter (FindPter *pter)
            {
                m_Pter = pter;
            }

            ///
            /// @brief Gets the text to find.
            ///
            /// The view must get the text to find from the find dialog
            /// and return it.
            ///
            /// @return The text to find entered by the user.
            ///
            virtual const gchar *getTextToFind (void) = 0;

            ///
            /// @brief Hides the view.
            ///
            /// The view must be hidden when the presenter calls this member
            /// and also the text of the find entry must be set to an empty
            /// string.
            ///
            virtual void hide (void) = 0;

            ///
            /// @brief Sensitives the Find Next button.
            ///
            /// @param sensitive TRUE to sensitive the button, FALSE
            ///                  to insensitive it.
            ///
            virtual void sensitiveFindNext (gboolean sensitive) = 0;

            ///
            /// @brief Sensitives the Find Next button.
            ///
            /// @param sensitive TRUE to sensitive the button, FALSE
            ///                  to insensitive it.
            ///
            virtual void sensitiveFindPrevious (gboolean sensitive) = 0;

            ///
            /// @brief Sets the information text.
            ///
            /// The view must set a label to the specified text to
            /// show it to the user. The text to show is something like
            /// "Searching on page %d of %d" or "Not Found!", etc...
            ///
            /// @param text The text to show.
            ///
            virtual void setInformationText (const gchar *text) = 0;

        protected:
            /// The presenter that controls the view.
            FindPter *m_Pter;

            ///
            /// @brief Constructs a new IFindView object.
            ///
            IFindView (void)
            {
                m_Pter = NULL;
            }
    };
}

#endif // !__IFIND_VIEW_H__
