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

#if !defined (__PRINT_VIEW_H__)
#define __PRINT_VIEW_H__

namespace ePDFView
{
    class PrintView: public IPrintView
    {
        public:
            PrintView (GtkWindow *parent);
            virtual ~PrintView (void);

            virtual void setPresenter (PrintPter *pter);

            virtual void addColorModel (const gchar *name,
                                        const gchar *value);
            virtual void addPageSize (const gchar *name,
                                      const gchar *value);
            virtual void addPrinter (const gchar *name, gint jobs,
                                     const gchar *state,
                                     const gchar *location);
            virtual void addResolution (const gchar *name,
                                        const gchar *value);
            virtual void clearColorModelList (void);
            virtual void clearPageSizeList (void);
            virtual void clearResolutionList (void);
            virtual guint getNumberOfCopies (void);
            virtual gchar *getColorModel (void);
            virtual PrintPageLayout getPageLayout (void);
            virtual PrintPageOrientation getPageOrientation (void);
            virtual const gchar *getPageRange (void);
            virtual gchar *getPageSize (void);
            virtual gchar *getResolution (void);
            virtual gchar *getSelectedPrinterName (void);
            virtual gboolean isCheckedCollate (void);
            virtual gboolean isSelectedAllPagesRangeOption (void);
            virtual gboolean isSelectedEvenPageSet (void);
            virtual gboolean isSelectedOddPageSet (void);
            virtual void selectColorModel (guint colorModeIndex);
            virtual void selectPageSize (guint pageSizeIndex);
            virtual void selectPrinter (guint printerIndex);
            virtual void selectResolution (guint resolutionIndex);
            virtual void sensitiveCollate (gboolean sensitive);
            virtual void sensitivePageRange (gboolean sensitive);
            virtual void sensitivePrintButton (gboolean sensitive);

        protected:
            GtkWidget *m_AllPagesRangeOption;
            GtkWidget *m_Collate;
            GtkListStore *m_ColorModel;
            GtkWidget *m_ColorModelView;
            GtkWidget *m_CustomPagesRangeOption;
            GtkWidget *m_EvenPageSet;
            GtkListStore *m_Layout;
            GtkWidget *m_LayoutView;
            GtkWidget *m_NumberOfCopies;
            GtkWidget *m_OddPageSet;
            GtkListStore *m_Orientation;
            GtkWidget *m_OrientationView;
            GtkWidget *m_PageRange;
            GtkListStore *m_PageSize;
            GtkWidget *m_PageSizeView;
            GtkWidget *m_PrintDialog;
            GtkListStore *m_PrinterList;
            GtkWidget *m_PrinterListView;
            GtkListStore *m_Resolution;
            GtkWidget *m_ResolutionView;
            GtkWidget *scrollBox;

            void addOptionToList (GtkListStore *optionList,
                                  const gchar *name, const gchar *value);
            void getOptionFromComboBox (GtkWidget *comboBox, gpointer value);

            GtkWidget *createJobTab (void);
            GtkWidget *createPaperTab (void);
            GtkWidget *createPrinterTab (void);

            void createColorModelListModel (void);
            void createLayoutListModel (void);
            void createOrientationListModel (void);
            void createPageSizeListModel (void);
            void createPrinterListModel (void);
            void createResolutionListModel (void);
    };
}

#endif // __PRINT_VIEW_H__
