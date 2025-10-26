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

#if !defined (__IPREFERENCES_VIEW_H__)
#define __IPREFERENCES_VIEW_H__

namespace ePDFView
{
    // Forward declarations.
    class PreferencesPter;

    ///
    /// @class IPreferencesView
    /// @brief Interface for the preferences view.
    ///
    class IPreferencesView
    {
        public:
            ///
            /// @brief Destroys all allocated memory for IPreferencesView.
            ///
            virtual ~IPreferencesView (void)
            {
            }

            ///
            /// @brief Gets the view's presenter.
            ///
            /// @return The presenter that is controlling the view.
            ///
            PreferencesPter *getPresenter (void)
            {
                return m_Pter;
            }

            ///
            /// @brief Sets the view's presenter.
            ///
            /// After setting the presenter, the view can show the
            /// preferences dialog.
            ///
            /// @param pter The presenter that will control the view.
            ///
            virtual void setPresenter (PreferencesPter *pter)
            {
                m_Pter = pter;
            }

            virtual const gchar *getBrowserCommandLine (void) = 0;
            virtual const gchar *getBacksearchCommandLine (void) = 0;

        protected:
            /// The presenter that controls the view.
            PreferencesPter *m_Pter;

            ///
            /// @brief Constructs a new IPresenterView object.
            ///
            IPreferencesView (void)
            {
                m_Pter = NULL;
            }
    };
}

#endif // !__IPREFERENCES_VIEW_H__
