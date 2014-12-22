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

#if !defined (__E_PDF_VIEW_H__)
#define __E_PDF_VIEW_H__

#include <config.h>
#include <gettext.h>
#include <glib.h>

#include <Config.h>

#include <DocumentRectangle.h>
#include <IDocumentLink.h>
#include <DocumentLinkGoto.h>
#include <DocumentLinkUri.h>
#include <DocumentOutline.h>
#include <DocumentPage.h>
#include <IDocumentObserver.h>
#include <IDocument.h>
#include <PDFDocument.h>

#include <IJob.h>
#include <JobFind.h>
#include <JobLoad.h>
#include <JobPrint.h>
#include <JobRender.h>
#include <JobSave.h>
#if defined (HAVE_CUPS)
#endif // HAVE_CUPS

#include <IFindView.h>
#include <IPageView.h>
#include <IPreferencesView.h>
#if defined (HAVE_CUPS)
#include <IPrintView.h>
#endif // HAVE_CUPS
#include <IMainView.h>
#include <FindPter.h>
#include <PagePter.h>
#include <PreferencesPter.h>
#if defined (HAVE_CUPS)
#include <PrintPter.h>
#endif // HAVE_CUPS
#include <MainPter.h>

#endif //!__E_PDF_VIEW_H__
