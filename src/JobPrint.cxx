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
#include <stdlib.h>
#include <cups/cups.h>
#include "epdfview.h"

using namespace ePDFView;

// Forward declaration.
static gboolean job_print_end (gpointer data);

///
/// @brief Constructs a new JobPrint object.
///
JobPrint::JobPrint ():
    IJob ()
{
    m_Collate = FALSE;
    m_ColorModel = NULL;
    m_CurrentPage = 1;
    m_Document = NULL;
    m_DocumentCopy = NULL;
    m_NumberOfCopies = 1;
    m_PageHeight = 0.1624f;
    m_PageLayout = PRINT_PAGE_LAYOUT_PLAIN;
    m_PageOrientation = PRINT_PAGE_ORIENTATION_PORTRAIT;
    m_PageRange = NULL;
    m_PageRangeString = NULL;
    m_PageSet = PRINT_ALL_PAGE_SET;
    m_PageWidth = 0.1147f;
    m_PrinterName = NULL;
    m_Resolution = NULL;
    m_TempFileName = NULL;
}

///
/// @brief Deletes all dynamically allocated memory by JobPrint.
///
JobPrint::~JobPrint ()
{
    delete m_DocumentCopy;
    delete[] m_PageRange;
    g_free (m_ColorModel);
    g_free (m_PageRangeString);
    g_free (m_PrinterName);
    g_free (m_Resolution);
    g_free (m_TempFileName);
}

gboolean
JobPrint::getCollate ()
{
    return m_Collate;
}

const gchar *
JobPrint::getColorModel ()
{
    return m_ColorModel;
}

guint
JobPrint::getCurrentPage ()
{
    return m_CurrentPage;
}

guint
JobPrint::getNumberOfCopies ()
{
    return m_NumberOfCopies;
}

IDocument &
JobPrint::getDocument ()
{
    g_assert ( NULL != m_DocumentCopy && "The copy of the document is NULL.");

    return *m_DocumentCopy;
}

gfloat
JobPrint::getPageHeight ()
{
    return m_PageHeight;
}

PrintPageLayout
JobPrint::getPageLayout ()
{
    return m_PageLayout;
}

PrintPageOrientation
JobPrint::getPageOrientation ()
{
    return m_PageOrientation;
}

const gchar *
JobPrint::getPageRangeString ()
{
    return m_PageRangeString;
}

PrintPageSet
JobPrint::getPageSet ()
{
    return m_PageSet;
}

gfloat
JobPrint::getPageWidth ()
{
    return m_PageWidth;
}

const gchar *
JobPrint::getPrinterName ()
{
    return m_PrinterName;
}

const gchar *
JobPrint::getResolution ()
{
    return m_Resolution;
}

const gchar *
JobPrint::getTempFileName ()
{
    return m_TempFileName;
}

///
/// @brief Renders a single page into a PostScript document.
///
gboolean
JobPrint::run ()
{
    // This only will only be executed the first time.
    setUpPrint ();

    guint currentPage = getCurrentPage ();
    getDocument ().outputPostscriptPage (currentPage);

    guint numPages = getDocument ().getNumPages ();
    for (guint pageNum = currentPage ; pageNum < numPages ; ++pageNum )
    {
        if (m_PageRange[pageNum])
        {
            setCurrentPage (pageNum + 1);
            break;
        }
    }

    if ( getCurrentPage () == currentPage )
    {
        JOB_NOTIFIER (job_print_end, this);
    }
    else
    {
        IJob::enqueue (this);
    }

    return JOB_DELETE;
}

void
JobPrint::setCollate (gboolean collate)
{
    m_Collate = collate;
}

void
JobPrint::setColorModel (const gchar *colorModel)
{
    g_free (m_ColorModel);
    m_ColorModel = g_strdup (colorModel);
}

void
JobPrint::setCurrentPage (guint pageNumber)
{
    m_CurrentPage = pageNumber;
}

void
JobPrint::setDocument (IDocument *document)
{
    m_Document = document;
}

void
JobPrint::setNumberOfCopies (guint copies)
{
    m_NumberOfCopies = copies;
}

void
JobPrint::setPrinterName (const gchar *name)
{
    g_free (m_PrinterName);
    m_PrinterName = g_strdup (name);
}

void
JobPrint::setPageLayout (PrintPageLayout layout)
{
    m_PageLayout = layout;
}

void
JobPrint::setPageOrientation (PrintPageOrientation orientation)
{
    m_PageOrientation = orientation;
}

void
JobPrint::setPageRange (const gchar *range)
{
    g_free (m_PageRangeString);
    m_PageRangeString = g_strdup (range);
}

void
JobPrint::setPageSet (PrintPageSet set)
{
    m_PageSet = set;
}

void
JobPrint::setPageSize (float pageWidth, float pageHeight)
{
    m_PageWidth = pageWidth;
    m_PageHeight = pageHeight;
}

void
JobPrint::setResolution (const gchar *resolution)
{
    g_free (m_Resolution);
    m_Resolution = g_strdup (resolution);
}

guint
JobPrint::setUpPageRange ()
{
    // The pages range are in the format:
    //  "a-b,c,d-e,f,g,h,i-j"
    //
    // What I will do here is traversing the string and looking for the
    // following characters:
    //      '-'             -   Tells a range of pages.
    //      ','             -   Tells a single page.
    //      Digit           -   Just accumule them.
    //      Any non-digit   -   The number has finished.
    //
    // We assume the following:
    //      1) There's at least one character string.
    //      2) The string ends with the \0 character.

    enum
    {
        COPY_SINGLE,
        COPY_RANGE
    } currentOperation = COPY_SINGLE;

    const gint numPages = getDocument().getNumPages () - 1;
    const gchar *stringStart = getPageRangeString ();
    const gchar *stringEnd = stringStart;
    guint pages[2] = {0, 0};
    guint currentPageNum = 0;

    do
    {
        if ( !g_ascii_isdigit (*stringEnd) )
        {
            // No more digits, get the number we have.
            gchar *number = g_strndup (stringStart, stringEnd - stringStart);
            if ( NULL != number )
            {
                pages[currentPageNum] = CLAMP (atoi (number) - 1, 0, numPages);
                g_free (number);
            }
            else
            {
                pages[currentPageNum] = 0;
            }
            stringStart = stringEnd;

            // Now check what should we do with out digit.
            switch (*stringEnd)
            {
                case '-':
                    // It's a range. Must get the second number.
                    currentPageNum = 1;
                    currentOperation = COPY_RANGE;
                    stringStart++;
                    break;

                case ',':
                default:
                    // In any other case, just copy the single number of the
                    // range and start again.
                    if ( COPY_RANGE == currentOperation )
                    {
                        const int lastPage = MAX (pages[0], pages[1]);
                        const int firstPage = MIN (pages[0], pages[1]);
                        for (int pageNum = firstPage ; pageNum <= lastPage ;
                             ++pageNum )
                        {
                            m_PageRange[pageNum] = TRUE;
                        }
                    }
                    else
                    {
                        m_PageRange[pages[0]] = TRUE;
                    }
                    currentPageNum = 0;
                    currentOperation = COPY_SINGLE;
                    stringStart++;
            }
        }
        ++stringEnd;
    }
    while ( '\0' != *stringEnd );

    gchar *lastDigit = g_strndup (stringStart, stringEnd - stringStart);
    if ( NULL != lastDigit )
    {
        pages[currentPageNum] = CLAMP (atoi (lastDigit) - 1, 0, numPages);
        g_free (lastDigit);
    }
    else
    {
        pages[currentPageNum] = 0;
    }

    if ( COPY_RANGE == currentOperation )
    {
        const int lastPage = MAX (pages[0], pages[1]);
        const int firstPage = MIN (pages[0], pages[1]);
        for (int pageNum = firstPage ; pageNum <= lastPage ; ++pageNum )
        {
            m_PageRange[pageNum] = TRUE;
        }
    }
    else
    {
        m_PageRange[pages[0]] = TRUE;
    }

    // Now check the page range set.
    PrintPageSet pageSet = getPageSet ();
    gint totalNumberOfPages = 0;
    for ( gint pageNum = 0; pageNum <= numPages ; ++pageNum )
    {
        // Since the range is from 0 to numPages - 1, the checks for
        // even or odd are reversed.
        if ( pageNum % 2 == 0 && PRINT_EVEN_PAGE_SET == pageSet ||
             pageNum % 2 != 0 && PRINT_ODD_PAGE_SET == pageSet )
        {
            m_PageRange[pageNum] = FALSE;
        }

        if ( m_PageRange[pageNum] )
        {
            totalNumberOfPages++;
        }
    }

    return totalNumberOfPages;
}

void
JobPrint::setTempFileName (const gchar *fileName)
{
    g_free (m_TempFileName);
    m_TempFileName = g_strdup (fileName);
}

void
JobPrint::setUpPrint ()
{
    if ( NULL != m_Document )
    {
        // Get a *copy* of the document. We don't want to open
        // a new document while printing, do we?
        m_DocumentCopy = m_Document->copy ();
        m_Document = NULL;

        // This array tells us which pages to export to postscript.
        int numPages = getDocument ().getNumPages ();
        m_PageRange = new gboolean[numPages];
        for ( int currentPage = 0 ; currentPage < numPages ; ++currentPage )
        {
            m_PageRange[currentPage] = FALSE;
        }
        if ( NULL == getPageRangeString () )
        {
            gchar *pageRange = g_strdup_printf ("1-%d", numPages);
            setPageRange (pageRange);
            g_free (pageRange);
        }
        // Now get the real range to print.
        gint numPagesToPrint = setUpPageRange ();

        // Set the current page to the first page that must be rendered.
        for (int currentPage = 0 ; currentPage < numPages ; ++currentPage )
        {
            if ( m_PageRange[currentPage] )
            {
                setCurrentPage (currentPage + 1);
                break;
            }
        }

        // Create a temporary file to store the postscript
        gchar tempFileName[1024];
        int cupsFile = cupsTempFd (tempFileName, 1024);
        setTempFileName (tempFileName);
        getDocument ().outputPostscriptBegin (tempFileName, numPagesToPrint,
                                              getPageWidth (),
                                              getPageHeight ());
        close (cupsFile);
    }
}

////////////////////////////////////////////////////////////////
// Static threaded functions.
////////////////////////////////////////////////////////////////

gboolean
job_print_end (gpointer data)
{
    g_assert (NULL != data && "The data parameter is NULL.");

    JobPrint *job = (JobPrint *)data;
    job->getDocument().outputPostscriptEnd ();

    // Add Cups options.
    cups_option_t *options;
    gint numOptions = 0;

    gchar *numCopies = g_strdup_printf ("%d", job->getNumberOfCopies ());
    numOptions = cupsAddOption ("copies", numCopies, numOptions, &options);
    g_free (numCopies);

    gchar *collate = g_strdup (job->getCollate() ? "True" : "False");
    numOptions = cupsAddOption ("Collate", collate, numOptions, &options);
    g_free (collate);

    gchar *orientation = NULL;
    if ( PRINT_PAGE_ORIENTATION_LANDSCAPE == job->getPageOrientation () )
    {
        orientation = g_strdup_printf ("4");
    }
    else
    {
        orientation = g_strdup_printf ("3");
    }
    numOptions = cupsAddOption ("orientation-requested", orientation,
                                 numOptions, &options);
    g_free (orientation);

    gchar *layout = NULL;
    switch (job->getPageLayout ())
    {
        case PRINT_PAGE_LAYOUT_2IN1:
            layout = g_strdup_printf ("%d", 2);
            break;

        case PRINT_PAGE_LAYOUT_4IN1:
            layout = g_strdup_printf ("%d", 4);
            break;

        case PRINT_PAGE_LAYOUT_6IN1:
            layout = g_strdup_printf ("%d", 6);
            break;

        default:
            layout = g_strdup_printf ("%d", 1);
    }
    numOptions = cupsAddOption ("number-up", layout, numOptions, &options);
    g_free (layout);
    numOptions = cupsAddOption ("ColorModel", job->getColorModel (),
                                numOptions, &options);
    numOptions = cupsAddOption ("resolution", job->getResolution (),
                                numOptions, &options);

    cupsPrintFile (job->getPrinterName (), job->getTempFileName (),
                   job->getDocument ().getFileName (), numOptions, options);

    cupsFreeOptions (numOptions, options);

    JOB_NOTIFIER_END ();

    return FALSE;
}
