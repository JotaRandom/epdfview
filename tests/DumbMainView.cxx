// ePDFView - Dumb Test Main View.
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

#include <epdfview.h>
#include <stdlib.h>
#include "DumbFindView.h"
#include "DumbPageView.h"
#include "DumbPreferencesView.h"
#include "DumbPrintView.h"
#include "DumbMainView.h"

using namespace ePDFView;

////////////////////////////////////////////////////////////////
// Interface Methods
////////////////////////////////////////////////////////////////
DumbMainView::DumbMainView (MainPter *pter):
    IMainView (pter)
{
    m_CurrentPage = 0;
    m_FindView = new DumbFindView ();
    m_NumberOfPages = g_strdup ("of 0");
    m_GoToPageText = g_strdup ("");
    m_OpenFileName = g_strdup ("");
    m_LastOpenFileFolder = NULL;
    m_LastSaveFileFolder = NULL;
    m_Outline = NULL;
    m_PageView = new DumbPageView ();
    m_Password = NULL;
    m_SaveFileName = g_strdup ("");
    m_SensitiveFind = TRUE;
    m_SensitiveGoToFirstPage = TRUE;
    m_SensitiveGoToLastPage = TRUE;
    m_SensitiveGoToNextPage = TRUE;
    m_SensitiveGoToPage = TRUE;
    m_SensitiveGoToPreviousPage = TRUE;
    m_SensitivePrint = TRUE;
    m_SensitiveReload = TRUE;
    m_SensitiveRotateLeft = TRUE;
    m_SensitiveRotateRight = TRUE;
    m_SensitiveSave = TRUE;
    m_SensitiveZoom = TRUE;
    m_SensitiveZoomIn = TRUE;
    m_SensitiveZoomOut = TRUE;
    m_SensitiveZoomFit = TRUE;
    m_SensitiveZoomWidth = TRUE;
    m_Shown = FALSE;
    m_ShownError = FALSE;
    m_ShownIndex = FALSE;
    m_ShownStatusbar = FALSE;
    m_ShownToolbar = FALSE;
    m_Title = g_strdup ("");
    m_TimesShownPassword = 0;
    m_ZoomText = g_strdup ("");
    m_ZoomToFit = FALSE;
    m_ZoomToWidth = FALSE;
}

DumbMainView::~DumbMainView ()
{
    delete m_FindView;
    delete m_PageView;
    g_free (m_GoToPageText);
    g_free (m_NumberOfPages);
    g_free (m_LastOpenFileFolder);
    g_free (m_LastSaveFileFolder);
    g_free (m_OpenFileName);
    g_free (m_Password);
    g_free (m_SaveFileName);
    g_free (m_Title);
    g_free (m_ZoomText);
}

void
DumbMainView::activeZoomFit (gboolean active)
{
    m_ZoomToFit = active;
}

void
DumbMainView::activeZoomWidth (gboolean active)
{
    m_ZoomToWidth = active;
}

IFindView *
DumbMainView::getFindView ()
{
    return m_FindView;
}

IPageView *
DumbMainView::getPageView ()
{
    return m_PageView;
}

IPreferencesView *
DumbMainView::getPreferencesView ()
{
    return new DumbPreferencesView ();
}

#if defined (HAVE_CUPS)
IPrintView *
DumbMainView::getPrintView ()
{
    return new DumbPrintView ();
}
#endif // HAVE_CUPS


gchar *
DumbMainView::openFileDialog (const gchar *lastFolder)
{
    g_free (m_LastOpenFileFolder);
    m_LastOpenFileFolder = g_strdup (lastFolder);
    return g_strdup (m_OpenFileName);
}

gchar *
DumbMainView::promptPasswordDialog (void)
{
    m_TimesShownPassword++;
    return g_strdup (m_Password);
}

gchar *
DumbMainView::saveFileDialog (const gchar *lastFolder)
{
    g_free (m_LastSaveFileFolder);
    m_LastSaveFileFolder = g_strdup (lastFolder);
    return g_strdup (m_SaveFileName);
}

void
DumbMainView::sensitiveGoToFirstPage (gboolean sensitive)
{
    m_SensitiveGoToFirstPage = sensitive;
}

void
DumbMainView::sensitiveFind (gboolean sensitive)
{
    m_SensitiveFind = sensitive;
}

void
DumbMainView::sensitiveGoToLastPage (gboolean sensitive)
{
    m_SensitiveGoToLastPage = sensitive;
}

void
DumbMainView::sensitiveGoToNextPage (gboolean sensitive)
{
    m_SensitiveGoToNextPage = sensitive;
}

void
DumbMainView::sensitiveGoToPage (gboolean sensitive)
{
    m_SensitiveGoToPage = sensitive;
}

void
DumbMainView::sensitiveGoToPreviousPage (gboolean sensitive)
{
    m_SensitiveGoToPreviousPage = sensitive;
}

void
DumbMainView::sensitiveOpen (gboolean sensitive)
{
}

void
DumbMainView::sensitivePrint (gboolean sensitive)
{
    m_SensitivePrint = sensitive;
}

void
DumbMainView::sensitiveReload (gboolean sensitive)
{
    m_SensitiveReload = sensitive;
}

void
DumbMainView::sensitiveRotateLeft (gboolean sensitive)
{
    m_SensitiveRotateLeft = sensitive;
}

void
DumbMainView::sensitiveRotateRight (gboolean sensitive)
{
    m_SensitiveRotateRight = sensitive;
}

void
DumbMainView::sensitiveSave (gboolean sensitive)
{
    m_SensitiveSave = sensitive;
}

void
DumbMainView::sensitiveZoom (gboolean sensitive)
{
    m_SensitiveZoom = sensitive;
}

void
DumbMainView::sensitiveZoomIn (gboolean sensitive)
{
    m_SensitiveZoomIn = sensitive;
}

void
DumbMainView::sensitiveZoomOut (gboolean sensitive)
{
    m_SensitiveZoomOut = sensitive;
}

void
DumbMainView::sensitiveZoomFit (gboolean sensitive)
{
    m_SensitiveZoomFit = sensitive;
}

void
DumbMainView::sensitiveZoomWidth (gboolean sensitive)
{
    m_SensitiveZoomWidth = sensitive;
}

const gchar *
DumbMainView::getGoToPageText ()
{
    return (const gchar *)m_GoToPageText;
}

const gchar *
DumbMainView::getZoomText ()
{
    return m_ZoomText;
}

void
DumbMainView::setCursor (ViewCursor cursorType)
{
}

void
DumbMainView::setFullScreen (gboolean fullScreen)
{
}

void
DumbMainView::setNumberOfPagesText (const gchar *text)
{
    g_free (m_NumberOfPages);
    m_NumberOfPages = g_strdup (text);
}

void
DumbMainView::setGoToPageText (const char *text)
{
    g_free (m_GoToPageText);
    m_GoToPageText = g_strdup (text);
    m_CurrentPage = atoi(text);
}

void
DumbMainView::setTitle (const gchar *title)
{
    g_free (m_Title);
    m_Title = g_strdup (title);
}

void
DumbMainView::setOutline (DocumentOutline *outline)
{
    m_Outline = outline;
}

void
DumbMainView::setStatusBarText (const gchar *text)
{
}

void
DumbMainView::setZoomText (const gchar *text)
{
    g_free (m_ZoomText);
    m_ZoomText = g_strdup (text);
}

void
DumbMainView::show (void)
{
    m_Shown = TRUE;
}

void
DumbMainView::showErrorMessage (const gchar *title, const gchar *body)
{
    m_ShownError = TRUE;
}

void
DumbMainView::showIndex (gboolean show)
{
    m_ShownIndex = show;
}

void
DumbMainView::showStatusbar (gboolean show)
{
    m_ShownStatusbar = show;
}

void
DumbMainView::showToolbar (gboolean show)
{
    m_ShownToolbar = show;
}

////////////////////////////////////////////////////////////////
// Test Methods
////////////////////////////////////////////////////////////////

gint
DumbMainView::countTimesShownPasswordPrompt ()
{
    return m_TimesShownPassword;
}

gint
DumbMainView::getCurrentPage ()
{
    return m_CurrentPage;
}

const gchar *
DumbMainView::getLastOpenFileFolder ()
{
    return m_LastOpenFileFolder;
}

const gchar *
DumbMainView::getLastSaveFileFolder ()
{
    return m_LastSaveFileFolder;
}


DocumentOutline *
DumbMainView::getOutline ()
{
    return m_Outline;
}

const gchar *
DumbMainView::getTitle ()
{
    return m_Title;
}

gboolean
DumbMainView::isSensitiveFind ()
{
    return m_SensitiveFind;
}

gboolean
DumbMainView::isSensitiveGoToFirstPage ()
{
    return m_SensitiveGoToFirstPage;
}

gboolean
DumbMainView::isSensitiveGoToLastPage ()
{
    return m_SensitiveGoToLastPage;
}

gboolean
DumbMainView::isSensitiveGoToNextPage ()
{
    return m_SensitiveGoToNextPage;
}

gboolean
DumbMainView::isSensitiveGoToPage ()
{
    return m_SensitiveGoToPage;
}

gboolean
DumbMainView::isSensitiveGoToPreviousPage ()
{
    return m_SensitiveGoToPreviousPage;
}

gboolean
DumbMainView::isSensitivePrint ()
{
    return m_SensitivePrint;
}

gboolean
DumbMainView::isSensitiveReload ()
{
    return m_SensitiveReload;
}

gboolean
DumbMainView::isSensitiveRotateLeft ()
{
    return m_SensitiveRotateLeft;
}

gboolean
DumbMainView::isSensitiveRotateRight ()
{
    return m_SensitiveRotateRight;
}

gboolean
DumbMainView::isSensitiveSave ()
{
    return m_SensitiveSave;
}

gboolean
DumbMainView::isSensitiveZoom ()
{
    return m_SensitiveZoom;
}

gboolean
DumbMainView::isSensitiveZoomIn ()
{
    return m_SensitiveZoomIn;
}

gboolean
DumbMainView::isSensitiveZoomOut ()
{
    return m_SensitiveZoomOut;
}

gboolean
DumbMainView::isSensitiveZoomFit ()
{
    return m_SensitiveZoomFit;
}

gboolean
DumbMainView::isSensitiveZoomWidth ()
{
    return m_SensitiveZoomWidth;
}

gboolean
DumbMainView::isShown ()
{
    return m_Shown;
}

gboolean
DumbMainView::isShownIndex ()
{
    return m_ShownIndex;
}

gboolean
DumbMainView::isShownStatusbar ()
{
    return m_ShownStatusbar;
}

gboolean
DumbMainView::isShownToolbar ()
{
    return m_ShownToolbar;
}

gboolean
DumbMainView::isZoomToFitActive ()
{
    return m_ZoomToFit;
}

gboolean
DumbMainView::isZoomToWidthActive ()
{
    return m_ZoomToWidth;
}

void
DumbMainView::setOpenFileName (const gchar *fileName)
{
    g_free (m_OpenFileName);
    m_OpenFileName = g_strdup (fileName);
}

void
DumbMainView::setPassword (const gchar *password)
{
    g_free (m_Password);
    m_Password = g_strdup (password);
    m_TimesShownPassword = 0;
}

void
DumbMainView::setSaveFileName (const gchar *fileName)
{
    g_free (m_SaveFileName);
    m_SaveFileName = g_strdup (fileName);
}

gboolean
DumbMainView::shownError ()
{
    gboolean shown = m_ShownError;
    // It's like the user clicked the "OK" button. No shown anymore, until
    // the next error.
    m_ShownError = FALSE;

    return shown;
}
