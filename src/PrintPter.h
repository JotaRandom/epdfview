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

#if !defined (__PRINT_PTER_H__)
#define __PRINT_PTER_H__

// Forward declarations.
typedef struct _printerAttributes printerAttributes;

namespace ePDFView
{
    ///
    /// @class PrintPter
    /// @brief Print Presenter.
    ///
    class PrintPter
    {
        public:
            PrintPter (IDocument *document);
            ~PrintPter (void);

            IPrintView &getView (void);
            void setView (IPrintView *view);

            void cancelActivated (void);
            void numberOfCopiesChanged (void);
            void pageRangeOptionChanged (void);
            void printActivated (void);
            void printerSelectionChanged (void);

        protected:
            IDocument *m_Document;
            IPrintView *m_View;

            void getPageSizeForPrinter (const gchar *printerName,
                                        const gchar *sizeName,
                                        float *pageWidth, float *pageHeight);
            printerAttributes *getPrinterAttributes (const gchar *printerName);
            void listPrinters (void);
    };
}

#endif // !__PRINT_PTER_H__
