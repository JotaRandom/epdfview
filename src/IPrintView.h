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

#if !defined (__IPRINT_VIEW_H__)
#define __IPRINT_VIEW_H__

namespace ePDFView
{
    // Forward declarations.
    class PrintPter;

    ///
    /// @class IPrintView
    /// @brief Interface for the print view.
    ///
    class IPrintView
    {
        public:
            ///
            /// @brief Destroys all allocated memory for IPrint.
            ///
            virtual ~IPrintView (void)
            {
            }

            ///
            /// @brief Gets the view's presenter.
            ///
            /// @return The presenter that is controlling the view.
            ///
            PrintPter *getPresenter (void)
            {
                return m_Pter;
            }

            ///
            /// @brief Sets the view's presenter.
            ///
            /// After setting the presenter, the view can show the
            /// print dialog.
            ///
            /// @param pter The presenter that will control the view.
            ///
            virtual void setPresenter (PrintPter *pter)
            {
                m_Pter = pter;
            }

            ///
            /// @brief Adds a new color model to the list.
            ///
            /// @param name The possibly translated name of the color model.
            /// @param value The value the ePDFView internally uses to know
            ///              which colorModel is.
            ///
            virtual void addColorModel (const gchar *name,
                                        const gchar *value) = 0;
 

            ///
            /// @brief Adds a new page size to the list.
            ///
            /// @param name The possibly translated name of the page size.
            /// @param value The value the ePDFView internally uses to know
            ///              which page size is.
            ///
            virtual void addPageSize (const gchar *name,
                                      const gchar *value) = 0;

            ///
            /// @brief Adds a new printer to the list of printer.
            ///
            /// The presenter will get the list of printers from the
            /// system and call this function for each one it finds.
            ///
            /// @param name The name of the printer.
            /// @param job The number of jobs the printer currently has active.
            /// @param state The current printer's state.
            /// @param location The printer's location.
            ///
            virtual void addPrinter (const gchar *name, gint jobs,
                                     const gchar *state,
                                     const gchar *location) = 0;

            ///
            /// @brief Adds a new resolution to the list.
            ///
            /// @param name The possibly translated name of the resolution.
            /// @param value The value the ePDFView internally uses to know
            ///              which resolution is.
            ///
            virtual void addResolution (const gchar *name,
                                        const gchar *value) = 0;

            ///
            /// @brief Deletes all color models from the list of color modes.
            ///
            virtual void clearColorModelList (void) = 0;

            ///
            /// @brief Deletes all sizes from the list of page sizes.
            ///
            virtual void clearPageSizeList (void) = 0;

            ///
            /// @brief Deletes all resolutions from the list of resolutions.
            ///
            virtual void clearResolutionList (void) = 0;

            ///
            /// @brief Gets the currently selected color model.
            ///
            /// @return The value of the color model that is selected.
            ///
            virtual gchar *getColorModel (void) = 0;

            ///
            /// @brief Gets the number of copies to do.
            ///
            /// @return The number of copies of the document to print. The
            ///         range is between 1 and 999.
            ///
            virtual guint getNumberOfCopies (void) = 0;

            ///
            /// @brief Gets the page layout.
            ///
            /// @return The selected page layout's value.
            ///
            virtual PrintPageLayout getPageLayout (void) = 0;

            ///
            /// @brief Gets the page orientation.
            ///
            /// @return The selected page orientation's value.
            ///
            virtual PrintPageOrientation getPageOrientation (void) = 0;

            ///
            /// @brief Gets the entred page range.
            ///
            /// @return The entered text in the page range entry.
            ///
            virtual const gchar *getPageRange (void) = 0;

            ///
            /// @brief Gets the currently selected page size.
            ///
            /// @return The name of the page size that is selected. This name
            ///         is the @a value parameter of the addPageSize()
            ///         function.
            ///
            virtual gchar *getPageSize (void) = 0;

            ///
            /// @brief Gets the currently selected resolution.
            ///
            /// @return The value of the resolution that is selected.
            ///         This value is the @a value parameter of the
            ///         addResolution() function.
            ///
            virtual gchar *getResolution (void) = 0;

            ///
            /// @brief Gets the name of the currently selected printer.
            ///
            /// @return The name of the printer that is currently selected.
            ///         The name returned is the same as the name passed
            ///         to the view using the addPrinter() function.
            ///
            virtual gchar *getSelectedPrinterName (void) = 0;

            ///
            /// @brief Checks if the "Collate" option is checked.
            ///
            /// @return TRUE if the "Collate" option is checked.
            ///         FALSE otherwise.
            ///
            virtual gboolean isCheckedCollate (void) = 0;

            ///
            /// @brief Checks if the "All Pages" range option is selected.
            ///
            /// @return TRUE if the range's "All Pages" option is selected.
            ///         FALSE otherwise.
            ///
            virtual gboolean isSelectedAllPagesRangeOption (void) = 0;

            ///
            /// @brief Checks if the "Even Page" set option is selected.
            ///
            /// @return TRUE if the set's "Even Page" option is selected.
            ///         FALSE otherwise.
            ///
            virtual gboolean isSelectedEvenPageSet (void) = 0;

            ///
            /// @brief Checks if the "Odd Page" set option is selected.
            ///
            /// @return TRUE if the set's "Odd Page" option is selected.
            ///         FALSE otherwise.
            ///
            virtual gboolean isSelectedOddPageSet (void) = 0;

            ///
            /// @brief Select a color model.
            ///
            /// @param colorModelIndex The index of the color model to select.
            ///
            virtual void selectColorModel (guint colorModelIndex) = 0;

            ///
            /// @brief Selects a page size.
            ///
            /// @param pageSizeIndex The index of the page size to select.
            ///
            virtual void selectPageSize (guint pageSizeIndex) = 0;

            ///
            /// @brief Selects a printer from the list.
            ///
            /// @param printerIndex The index of the printer to select.
            ///
            virtual void selectPrinter (guint printerIndex) = 0;

            ///
            /// @brief Selects a page size.
            ///
            /// @param resolutionIndex The index of the resolution to select.
            ///
            virtual void selectResolution (guint resolutionIndex) = 0;

            ///
            /// @brief Changes the sensitivity of the "Collate" option.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Collate" option.
            ///
            /// @param sensitive Set to TRUE to make sensitive (enable) the
            ///                  option or FALSE to insensitive (disable) it.
            ///
            virtual void sensitiveCollate (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Page Range" entry.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Page Range" entry.
            ///
            /// @param sensitive Set to TRUE if need to make sensitive (enable)
            ///                  the entry or FALSE to insensitive (disable) it.
            ///
            virtual void sensitivePageRange (gboolean sensitive) = 0;

            ///
            /// @brief Changes the sensitivity of the "Print" button.
            ///
            /// The view must change the sensitivity (it's called enabled or
            /// disabled on some toolkits) of the "Print" button.
            ///
            /// @param sensitive Set to TRUE if need to sensitive (enable) the
            ///                  button or FALSE to insensitive (disable) it.
            ///
            virtual void sensitivePrintButton (gboolean sensitive) = 0;


        protected:
            /// The presenter that controls the view.
            PrintPter *m_Pter;

            ///
            /// @brief Constructs a new IPrintView object.
            ///
            IPrintView (void)
            {
                m_Pter = NULL;
            }
    };
}

#endif // !__IPRINT_VIEW_H__
