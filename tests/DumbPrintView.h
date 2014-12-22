// ePDFView - Dumb Test Preferences View.
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

#if !defined (__DUMB_PRINT_VIEW_H__)
#define __DUMB_PRINT_VIEW_H__

namespace ePDFView
{
    class DumbPrintView: public IPrintView
    {
        public:
            DumbPrintView ();
            virtual ~DumbPrintView (void);

            virtual void addColorModel (const gchar *name, const gchar *value);
            virtual void addPageSize (const gchar *name, const gchar *value);
            virtual void addPrinter (const gchar *name, int jobs,
                                     const gchar *state, const gchar *location);
            virtual void addResolution (const gchar *name, const gchar *value);
            virtual void clearColorModelList (void);
            virtual void clearPageSizeList (void);
            virtual void clearResolutionList (void);
            virtual guint getNumberOfCopies (void);
            virtual PrintPageLayout getPageLayout (void);
            virtual PrintPageOrientation getPageOrientation (void);
            virtual const gchar *getPageRange (void);
            virtual gchar *getColorModel (void);
            virtual gchar *getPageSize (void);
            virtual gchar *getResolution (void);
            virtual gchar *getSelectedPrinterName (void);
            virtual gboolean isCheckedCollate (void);
            virtual gboolean isSelectedAllPagesRangeOption (void);
            virtual gboolean isSelectedEvenPageSet (void);
            virtual gboolean isSelectedOddPageSet (void);
            virtual void selectColorModel (guint colorModelIndex);
            virtual void selectPageSize (guint pageSizeIndex);
            virtual void selectPrinter (guint printerIndex);
            virtual void selectResolution (guint resolutionIndex);
            virtual void sensitiveCollate (gboolean sensitive);
            virtual void sensitivePageRange (gboolean sensitive);
            virtual void sensitivePrintButton (gboolean sensitive);

            // Test only functions.
            gboolean isSensitiveCollate (void);
            gboolean isSensitivePageRange (void);
            void selectAllPagesRangeOption (void);
            void selectCustomPagesRangeOption (void);
            void setNumberOfCopies (unsigned int copies);

        protected:
            gboolean m_AllPagesRangeOptionSelected;
            guint m_NumberOfCopies;
            gboolean m_SensitiveCollate;
            gboolean m_SensitivePageRange;
    };
}

#endif // !__DUMB_PRINT_VIEW_H__
