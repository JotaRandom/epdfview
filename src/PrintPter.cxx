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

#include <config.h>
#include <cups/cups.h>
#include <cups/ipp.h>
#include <cups/ppd.h>
#include <locale.h>
#include "epdfview.h"

#if (CUPS_VERSION_MAJOR > 1) || (CUPS_VERSION_MINOR > 5)
#define HAVE_CUPS_1_6 1
#endif

#ifndef HAVE_CUPS_1_6
inline int ippGetInteger (ipp_attribute_t *attr, int element)
{
    return (attr->values[element].integer);
}

inline const char * ippGetString (ipp_attribute_t *attr,
                                  int             element,
                                  const char      **language /*UNUSED*/)
{
    return (attr->values[element].string.text);
}

inline int ippSetOperation (ipp_t *ipp, ipp_op_t op)
{
  if (!ipp)
    return (0);
  ipp->request.op.operation_id = op;
  return (1);
}

inline int ippSetRequestId (ipp_t *ipp, int request_id)
{
    if (!ipp)
        return (0);
    ipp->request.any.request_id = request_id;
    return (1);
}
#endif

using namespace ePDFView;

// Structures
struct _printerAttributes
{
    gchar *location;
    gchar *state;
};

PrintPter::PrintPter (IDocument *document)
{
    m_View = NULL;
    m_Document = document;
}

PrintPter::~PrintPter ()
{
    delete m_View;
}

IPrintView &
PrintPter::getView ()
{
    g_assert ( NULL != m_View && "The print's view is NULL.");

    return *m_View;
}

void
PrintPter::setView (IPrintView *view)
{
    m_View = view;

    // Set the page range and collate options insensitive.
    view->sensitiveCollate (FALSE);
    view->sensitivePageRange (FALSE);

    // Retrieve and set the list of printer.
    listPrinters ();

    getView ().setPresenter (this);
}

void
PrintPter::cancelActivated (void)
{
    delete this;
}

void
PrintPter::numberOfCopiesChanged (void)
{
    IPrintView &view = getView ();
    if ( 1 == view.getNumberOfCopies () )
    {
        view.sensitiveCollate (FALSE);
    }
    else
    {
        view.sensitiveCollate (TRUE);
    }
}

void
PrintPter::pageRangeOptionChanged ()
{
    IPrintView &view = getView ();
    if ( view.isSelectedAllPagesRangeOption () )
    {
        view.sensitivePageRange (FALSE);
    }
    else
    {
        view.sensitivePageRange (TRUE);
    }
}

void
PrintPter::printActivated ()
{
    IPrintView &view = getView ();
    // Get the printer's name.
    char *printerAndInstanceNames = view.getSelectedPrinterName ();
    if ( NULL != printerAndInstanceNames )
    {
        gchar *printerName;
        gchar *slashPosition = g_strrstr (printerAndInstanceNames, "/");
        if ( NULL != slashPosition )
        {
            *slashPosition = '\0';
            printerName = g_strdup (printerAndInstanceNames);
            *slashPosition = '/';
        }
        else
        {
            printerName = g_strdup (printerAndInstanceNames);
        }
        g_free (printerAndInstanceNames);

        float pageWidth;
        float pageHeight;
        char *pageSizeName = view.getPageSize ();
        getPageSizeForPrinter (printerName, pageSizeName,
                               &pageWidth, &pageHeight);
        g_free (pageSizeName);
        // Create the new print job.
        JobPrint *job = new JobPrint ();
        job->setDocument (m_Document);
        job->setPrinterName (printerName);
        g_free (printerName);

        if ( view.isSelectedAllPagesRangeOption () )
        {
            char *pageRange = g_strdup_printf ("1-%d",
                                               m_Document->getNumPages ());
            job->setPageRange (pageRange);
            g_free (pageRange);
        }
        else
        {
            job->setPageRange (view.getPageRange ());
        }

        if ( view.isSelectedOddPageSet () )
        {
            job->setPageSet (PRINT_ODD_PAGE_SET);
        }
        else if ( view.isSelectedEvenPageSet () )
        {
            job->setPageSet (PRINT_EVEN_PAGE_SET);
        }
        else
        {
            job->setPageSet (PRINT_ALL_PAGE_SET);
        }
        job->setNumberOfCopies (view.getNumberOfCopies ());
        job->setCollate (view.isCheckedCollate ());
        job->setPageSize (pageWidth, pageHeight);
        job->setPageOrientation (view.getPageOrientation ());
        job->setPageLayout (view.getPageLayout ());

        gchar *colorModel = view.getColorModel ();
        job->setColorModel (colorModel);
        g_free (colorModel);
        gchar *resolution = view.getResolution ();
        job->setResolution (resolution);
        g_free (resolution);

        IJob::enqueue (job);
    }
    // Once done, it's the same as canceled :-)
    cancelActivated ();
}

void
PrintPter::printerSelectionChanged ()
{
    IPrintView &view = getView ();

    gchar *printerAndInstanceNames = view.getSelectedPrinterName ();

    if ( NULL != printerAndInstanceNames )
    {
        cups_dest_t *destinations;
        guint numDestinations = cupsGetDests (&destinations);
        if ( NULL == destinations )
        {
            g_free (printerAndInstanceNames);
            return;
        }
        gchar *slashPosition = g_strrstr (printerAndInstanceNames, "/");
        gchar *printerName = NULL;
        gchar *instanceName = NULL;
        if ( slashPosition != NULL )
        {
            *slashPosition = '\0';
            printerName = g_strdup (printerAndInstanceNames);
            instanceName = g_strdup (slashPosition + 1);
            *slashPosition = '/';
        }
        else
        {
            printerName = g_strdup (printerAndInstanceNames);
        }
        cups_dest_t *destination = cupsGetDest (printerName, instanceName,
                                                numDestinations, destinations);
        if ( NULL == destinations )
        {
            g_free (printerName);
            cupsFreeDests (numDestinations, destinations);
            return;
        }

        const gchar *printerPPDName = cupsGetPPD (printerName);
        if ( NULL != printerPPDName )
        {
            ppd_file_t *printerPPD = ppdOpenFile (printerPPDName);
            if ( NULL != printerPPD )
            {
                ppdMarkDefaults (printerPPD);
                cupsMarkOptions (printerPPD, destination->num_options,
                                 destination->options);

                // Get the page sizes.
                ppd_option_t *pageSizeOption =
                    ppdFindOption (printerPPD, "PageSize");
                view.clearPageSizeList ();
                int pageSizeToSelect = 0;
                if ( 0 != pageSizeOption )
                {
                    ppd_choice_t *pageSizeChoice = pageSizeOption->choices;
                    for ( int currentSize = 0 ;
                         currentSize < pageSizeOption->num_choices ;
                         ++currentSize, ++pageSizeChoice )
                    {
                        const gchar *sizeName = pageSizeChoice->text;
                        const gchar *sizeValue = pageSizeChoice->choice;

                        view.addPageSize (_(sizeName), sizeValue);
                        if ( pageSizeChoice->marked )
                        {
                            pageSizeToSelect = currentSize;
                        }
                    }
                }
                else
                {
                    view.addPageSize (_("A4"), "A4");
                }
                view.selectPageSize (pageSizeToSelect);

                // Get the resolutions.
                ppd_option_t *resolutionOption =
                    ppdFindOption (printerPPD, "Resolution");
                view.clearResolutionList ();
                int resolutionToSelect = 0;
                if ( 0 != resolutionOption )
                {
                    ppd_choice_t *resolutionChoice = resolutionOption->choices;
                    for ( int currentRes = 0 ;
                            currentRes < resolutionOption->num_choices ;
                            ++currentRes, ++resolutionChoice )
                    {
                        const gchar *resName = resolutionChoice->text;
                        const gchar *resValue = resolutionChoice->choice;

                        view.addResolution (_(resName), resValue);
                        if ( resolutionChoice->marked )
                        {
                            resolutionToSelect = currentRes;
                        }
                    }
                }
                else
                {
                    view.addResolution (_("300 DPI"), "300x300dpi");
                }
                view.selectResolution (resolutionToSelect);

                // Get the color models.
                ppd_option_t *colorModelOption =
                    ppdFindOption (printerPPD, "ColorModel");
                view.clearColorModelList ();
                int colorModelToSelect = 0;
                if ( NULL != colorModelOption )
                {
                    ppd_choice_t *colorModelChoice = colorModelOption->choices;
                    for ( int currentColor = 0 ;
                          currentColor < colorModelOption->num_choices ;
                          ++currentColor, ++colorModelChoice )
                    {
                        const gchar *colorName = colorModelChoice->text;
                        const gchar *colorValue = colorModelChoice->choice;

                        view.addColorModel (_(colorName), colorValue);
                        if ( colorModelChoice->marked )
                        {
                            colorModelToSelect = currentColor;
                        }
                    }
                }
                else
                {
                    view.addColorModel (_("Grayscale"), "Gray");
                }
                view.selectColorModel (colorModelToSelect);
                ppdClose (printerPPD);
                if (printerPPDName != NULL)
                    unlink(printerPPDName);
            }
        }
        cupsFreeDests (numDestinations, destinations);
        g_free (printerName);
        g_free (instanceName);
        g_free (printerAndInstanceNames);
    }
}

void
PrintPter::getPageSizeForPrinter (const gchar *printerName,
                                  const gchar *pageSizeName,
                                  float *pageWidth, float *pageHeight)
{
    g_assert (NULL != pageWidth && "Tried to save the page width to NULL.");
    g_assert (NULL != pageHeight && "Tried to save the page height to NULL.");

    // Set the initial value to an A4 page size (size is in 1/72 inches).
    *pageWidth = 0.1147f;
    *pageHeight = 0.1624f;

    if ( NULL != printerName )
    {
        const gchar *printerPPDName = cupsGetPPD (printerName);
        if ( NULL != printerPPDName )
        {
            ppd_file_t *printerPPD = ppdOpenFile (printerPPDName);
            if ( NULL != printerPPD )
            {
                for ( int currentSize = 0 ;
                      currentSize < printerPPD->num_sizes ;
                      ++currentSize )
                {
                    ppd_size_t *size = &printerPPD->sizes[currentSize];
                    if ( 0 == g_ascii_strcasecmp (pageSizeName, size->name) )
                    {
                        *pageWidth = size->width;
                        *pageHeight = size->length;
                        break;
                    }
                }
                ppdClose (printerPPD);
            }
        }
    }
}

printerAttributes *
PrintPter::getPrinterAttributes (const gchar *printerName)
{
    // Create and empty attributes structure.
    printerAttributes *attributes = new printerAttributes ();
    attributes->state = NULL;
    attributes->location = NULL;

    // The attributes to request from the server.
    const gchar *attributesToRequest[] =
    {
        "printer-state",
        "printer-location"
    };

#if (CUPS_VERSION_MAJOR > 1) || (CUPS_VERSION_MINOR >= 7)
    http_t *http = httpConnect2(cupsServer(), ippPort(), NULL, AF_UNSPEC, HTTP_ENCRYPTION_IF_REQUESTED, 1, 30000, NULL);
#else
    http_t *http = httpConnect (cupsServer (), ippPort ());
#endif
    if ( NULL == http )
    {
        printf ("No http!\n");
        return attributes;
    }

    ipp_t *request = ippNew ();

    ippSetOperation(request, IPP_GET_PRINTER_ATTRIBUTES);
    ippSetRequestId(request, 1);

    ippAddString (request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
                  "attributes-charset", NULL, "utf-8");
    ippAddString (request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
                  "attributes-natural-language", NULL,
                  setlocale (LC_MESSAGES, NULL));
    ippAddStrings (request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD,
                   "requested-attributes", G_N_ELEMENTS (attributesToRequest),
                   NULL, attributesToRequest);
    gchar *uri = g_strdup_printf ("ipp://localhost/printers/%s", printerName);
    ippAddString (request, IPP_TAG_OPERATION, IPP_TAG_URI,
                  "printer-uri", NULL, uri);

    ipp_t *answer = cupsDoRequest (http, request, "/");
    if ( NULL != answer )
    {
        // Get the state.
        ipp_attribute_t *state =
            ippFindAttribute (answer, "printer-state", IPP_TAG_ZERO);
        if ( NULL != state )
        {
            switch (ippGetInteger (state, 0))
            {
                case IPP_PRINTER_IDLE:
                    attributes->state = g_strdup (_("Idle"));
                    break;
                case IPP_PRINTER_STOPPED:
                    attributes->state = g_strdup (_("Stopped"));
                    break;
                case IPP_PRINTER_PROCESSING:
                    attributes->state = g_strdup (_("Processing"));
                    break;
                default:
                    attributes->state = g_strdup (_("Unknown"));
                    break;
            }
        }

        // Get the location.
        ipp_attribute_t *location =
            ippFindAttribute (answer, "printer-location", IPP_TAG_ZERO);
        if ( NULL != location )
        {
            attributes->location = g_strdup (ippGetString (location, 0, NULL));
        }

        ippDelete (answer);
    }
    else
    {
        printf ("No answer!\n");
    }

    httpClose (http);
    g_free (uri);
    return attributes;
}

void
PrintPter::listPrinters (void)
{
    IPrintView &view = getView ();

    cups_dest_t *destinations;
    int numDestinations = cupsGetDests (&destinations);
    // For now we don't have any printer selected.
    int printerToSelect = -1;

    for ( int currentDestination = 0 ; currentDestination < numDestinations ;
          ++currentDestination )
    {
        // Get the printer name and the local instance, if it has any.
        gchar *printerName = destinations[currentDestination].name;
        gchar *printerAndInstanceNames = NULL;
        if ( NULL != destinations[currentDestination].instance )
        {
            printerAndInstanceNames = g_strdup_printf ("%s/%s",
                    destinations[currentDestination].name,
                    destinations[currentDestination].instance );
        }
        else
        {
            printerAndInstanceNames =
                g_strdup (destinations[currentDestination].name);
        }
        // Get the number of jobs the printer currently has.
        cups_job_t *destinationJobs;
        int numJobs = cupsGetJobs (&destinationJobs, printerName, 1, 0);
        cupsFreeJobs (numJobs, destinationJobs);
        // Get the state and location.
        printerAttributes *attributes = getPrinterAttributes (printerName);
        // Set all this data to the view.
        view.addPrinter (printerAndInstanceNames, numJobs,
                               attributes->state, attributes->location);
        g_free (printerAndInstanceNames);
        g_free (attributes->location);
        g_free (attributes->state);
        delete attributes;

        // If the printer is the default and we don't have any printer
        // selected, then we should select it.
        if ( -1 == printerToSelect &&
             destinations[currentDestination].is_default )
        {
            printerToSelect = currentDestination;
        }
    }
    cupsFreeDests (numDestinations, destinations);

    // If not printer is available, insensitive the print button.
    if ( 0 == numDestinations )
    {
        view.sensitivePrintButton (FALSE);
    }
    // Otherwise select a printer: the previously used, the default or 
    // the first.
    else
    {
        if ( -1 == printerToSelect )
        {
            view.selectPrinter (0);
        }
        else
        {
            view.selectPrinter (printerToSelect);
        }
        // The selection has been changed before the
        // view could connect the signals, so here we'll set the selected
        // printer's page sizes "by hand".
        printerSelectionChanged ();
    }
}
