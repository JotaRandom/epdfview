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

#include <gio/gio.h>

// Define GType for our custom data structures
G_BEGIN_DECLS

typedef struct _OptionData OptionData;
struct _OptionData {
    gchar *name;
    gchar *value;
};

GType option_data_get_type(void);
#define OPTION_DATA_TYPE (option_data_get_type())

// Function declarations for OptionData
OptionData *option_data_copy(const OptionData *data);
void option_data_free(OptionData *data);

typedef struct _PrinterData PrinterData;
struct _PrinterData {
    gchar *name;
    gchar *state;
    gint jobs;
    gchar *location;
};

GType printer_data_get_type(void);
#define PRINTER_DATA_TYPE (printer_data_get_type())

// Function declarations for PrinterData
PrinterData *printer_data_copy(const PrinterData *data);
void printer_data_free(PrinterData *data);

// Print page layout options
typedef enum {
    PRINT_PAGE_LAYOUT_1IN1 = 0,
    PRINT_PAGE_LAYOUT_2IN1,
    PRINT_PAGE_LAYOUT_4IN1,
    PRINT_PAGE_LAYOUT_6IN1
} PrintPageLayout;

// Print page orientation options
typedef enum {
    PRINT_PAGE_ORIENTATION_PORTRAIT = 0,
    PRINT_PAGE_ORIENTATION_LANDSCAPE
} PrintPageOrientation;

G_END_DECLS

namespace ePDFView
{
    // Column indices for the printer list
    enum {
        PRINTER_LIST_NAME_COLUMN,
        PRINTER_LIST_STATE_COLUMN,
        PRINTER_LIST_JOBS_COLUMN,
        PRINTER_LIST_LOCATION_COLUMN,
        PRINTER_LIST_N_COLUMNS
    };

    // Column indices for the option list
    enum {
        OPTION_LIST_LABEL_COLUMN,
        OPTION_LIST_VALUE_COLUMN,
        OPTION_LIST_N_COLUMNS
    };
    
    // For backward compatibility
    static constexpr gint printOptionLabelColumn = OPTION_LIST_LABEL_COLUMN;
    static constexpr gint printerListNameColumn = PRINTER_LIST_NAME_COLUMN;

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
            GListStore *m_ColorModel;
            GtkWidget *m_ColorModelView;
            GtkWidget *m_CustomPagesRangeOption;
            GtkWidget *m_EvenPageSet;
            GListStore *m_Layout;
            GtkWidget *m_LayoutView;
            GtkWidget *m_NumberOfCopies;
            GtkWidget *m_OddPageSet;
            GListStore *m_Orientation;
            GtkWidget *m_OrientationView;
            GtkWidget *m_PageRange;
            GListStore *m_PageSize;
            GtkWidget *m_PageSizeView;
            GtkWidget *m_PrintDialog;
            GListStore *m_PrinterList;
            GtkWidget *m_PrinterListView;
            GListStore *m_Resolution;
            GtkWidget *m_ResolutionView;
            GtkWidget *scrollBox;
            PrintPter *m_Presenter;

            // Structure to hold option data for GListStore
            struct OptionData {
                gchar *name;
                gchar *value;
            };
            
            // Structure to hold printer data for GListStore
            struct PrinterData {
                gchar *name;
                gchar *state;
                gint jobs;
                gchar *location;
            };
            
            void addOptionToList (GListStore *optionList,
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
