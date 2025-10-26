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

#if !defined (__IDOCUMENT_OBSERVER_H__)
#define __IDOCUMENT_OBSERVER_H__

namespace ePDFView
{
    /// @class IDocumentObserver
    /// @brief Interface for IDocument's observers.
    ///
    /// A document observer is just a class that receives notifications
    /// of the document when it changes its state.
    ///
    class IDocumentObserver
    {
        public:
            /// @brief Destroys all dynamically allocated memory.
            virtual ~IDocumentObserver (void) { }

            ///
            /// @brief A find result has been changed.
            ///
            /// This is called every time the highligthed find result
            /// is changed.
            ///
            /// @param matchRect The rectangle of the highligthed result.
            ///
            virtual void notifyFindChanged (DocumentRectangle *matchRect) { }

            ///
            /// @brief A find has been finished.
            ///
            /// This is called when the find is finished (i.e., no
            /// results.)
            ///
            virtual void notifyFindFinished (void) { }

            ///
            /// @brief A find has been started.
            ///
            /// This function is called when a new find has been started.
            /// This is mainly for test purposes.
            ///
            virtual void notifyFindStarted (void) { }

            ///
            /// @brief The document has been loaded.
            ///
            /// This function is called when the document has been
            /// loaded successfully.
            ///
            virtual void notifyLoad (void) { }

            ///
            /// @brief The document couldn't be loaded due an error.
            ///
            /// This function is called when the document couldn't be
            /// loaded because of an error. The error message is
            /// passed as a parameter.
            ///
            /// @param fileName The file name that was trying to load.
            /// @param error The error code and message.
            ///
            virtual void notifyLoadError (const gchar *fileName,
                    const GError *error) { }

            ///
            /// @brief The document is encrypted.
            ///
            /// This function is called when the document couldn't be
            /// loaded because it's encrypted and no password or an invalid
            /// password was supplied.
            ///
            /// @param fileName The file name that was trying to load.
            /// @param reload TRUE if it was trying to reload the file, FALSE
            ///               if it was trying to load a new file.
            /// @param error The error message and code that the operation
            ///              produced.
            virtual void notifyLoadPassword (const gchar *fileName,
                                             gboolean reload,
                                             const GError *error) { }

            ///
            /// @brief The current page has been changed.
            ///
            /// This function is called when the current page has been
            /// replaced by a new one.
            /// 
            /// @param pageNum The number of the new page.
            ///
            virtual void notifyPageChanged (gint pageNum) { }

            ///
            /// @brief The current page has been rotated.
            ///
            /// This function is called when the current page has been
            /// rotated.
            ///
            /// @param rotation The rotation angle in degrees.
            ///
            virtual void notifyPageRotated (gint rotation) { }

            ///
            /// @brief The current page has been scaled.
            ///
            /// This function is called when the current page zoom level
            /// has been changed.
            ///
            /// @param zoom The current scale.
            ///
            virtual void notifyPageZoomed (gdouble zoom) { }

            ///
            /// @brief The document has been reloaded.
            ///
            /// This function is called when the document has been reloaded
            /// successfully.
            ///
            virtual void notifyReload (void) { }

            ///
            /// @brief The document has been saved.
            ///
            /// This function is called when a document's copy has been
            /// saved successfully.
            ///
            virtual void notifySave (void) { }

            ///
            /// @brief The document couldn't be saved due an error.
            ///
            /// This function is called when a document's copy couldn't be
            /// saved because of an error. The error message is
            /// passed as a parameter.
            ///
            /// @param error The error code and message.
            ///
            virtual void notifySaveError (const GError *error) { }

            ///
            /// @brief Someone select text in document.
            ///
            /// This function called when someone select some text
            /// in document.
            ///
            /// @param text Selected text.
            ///
            virtual void notifyTextSelected (const gchar* text) { }

        protected:
            ///
            /// @brief Constructs a new IDocumentObserver object.
            IDocumentObserver (void) { }
    };
}

#endif // !__IDOCUMENT_OBSERVER_H__
