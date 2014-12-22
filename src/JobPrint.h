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

#if !defined (__JOB_PRINT_H__)
#define __JOB_PRINT_H__

namespace ePDFView
{
    ///
    /// @enum PrintPageLayout
    ///
    enum PrintPageLayout
    {
        PRINT_PAGE_LAYOUT_PLAIN,
        PRINT_PAGE_LAYOUT_2IN1,
        PRINT_PAGE_LAYOUT_4IN1,
        PRINT_PAGE_LAYOUT_6IN1
    };

    ///
    /// @enum PrintPageOrientation.
    ///
    enum PrintPageOrientation
    {
        PRINT_PAGE_ORIENTATION_PORTRAIT,
        PRINT_PAGE_ORIENTATION_LANDSCAPE
    };

    ///
    /// @enum PrintPageSet
    ///
    enum PrintPageSet
    {
        PRINT_ALL_PAGE_SET,
        PRINT_EVEN_PAGE_SET,
        PRINT_ODD_PAGE_SET
    };

    ///
    /// @class JobPrint
    /// @brief A background job that prints the document.
    ///
    /// This class converts the document to postscript and then
    /// uses CUPS to print the converted document.
    ///
    class JobPrint: public IJob
    {
        public:
            JobPrint (void);
            ~JobPrint (void);

            gboolean getCollate (void);
            const gchar *getColorModel (void);
            IDocument &getDocument (void);
            guint getNumberOfCopies (void);
            PrintPageLayout getPageLayout (void);
            PrintPageOrientation getPageOrientation (void);
            const gchar *getPrinterName (void);
            const gchar *getResolution (void);
            const gchar *getTempFileName (void);
            virtual gboolean run (void);
            void setCollate (gboolean collate);
            void setColorModel (const gchar *colorModel);
            void setDocument (IDocument *document);
            void setNumberOfCopies (guint copies);
            void setPageLayout (PrintPageLayout layout);
            void setPageOrientation (PrintPageOrientation orientation);
            void setPageRange (const gchar *range);
            void setPageSet (PrintPageSet set);
            void setPageSize (gfloat pageWidth, gfloat pageHeight);
            void setPrinterName (const gchar *name);
            void setResolution (const gchar *resolution);
            void setUpPrint (void);

        protected:
            gboolean m_Collate;
            gchar *m_ColorModel;
            guint m_CurrentPage;
            IDocument *m_Document;
            IDocument *m_DocumentCopy;
            guint m_NumberOfCopies;
            gfloat m_PageHeight;
            PrintPageLayout m_PageLayout;
            PrintPageOrientation m_PageOrientation;
            gboolean *m_PageRange;
            gchar *m_PageRangeString;
            PrintPageSet m_PageSet;
            gchar *m_Resolution;
            gfloat m_PageWidth;
            gchar *m_PrinterName;
            gchar *m_TempFileName;

            guint getCurrentPage (void);
            gfloat getPageHeight (void);
            const gchar *getPageRangeString (void);
            PrintPageSet getPageSet (void);
            gfloat getPageWidth (void);
            void setCurrentPage (guint pageNumber);
            void setTempFileName (const gchar *fileName);
            guint setUpPageRange (void);
    };
}

#endif // __JOB_PRINT_H__
