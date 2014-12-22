// ePDFView - PDF Document Test Fixture.
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
#include "Utils.h"
#include "DumbDocumentObserver.h"
#include "PDFDocumentTest.h"

using namespace ePDFView;
    
G_LOCK_EXTERN (JobRender);

// Constant for checking doubles.
static const double DELTA = 0.0001;

// Register the test suite into the `registry'.
CPPUNIT_TEST_SUITE_REGISTRATION (PDFDocumentTest);

///
/// @brief Sets up the environment for each test.
///
void
PDFDocumentTest::setUp ()
{
    m_Document = new PDFDocument ();
    m_Observer = new DumbDocumentObserver ();
    m_Document->attach (m_Observer);
    G_LOCK (JobRender);
    JobRender::m_CanProcessJobs = TRUE;
    G_UNLOCK (JobRender);
}

///
/// @brief Cleans up after each test.
///
void
PDFDocumentTest::tearDown ()
{
    G_LOCK (JobRender);
    JobRender::m_CanProcessJobs = FALSE;
    IJob::clearQueue ();
    G_UNLOCK (JobRender);
    m_Document->detach (m_Observer);
    delete m_Observer;
    delete m_Document;
}

///
/// @brief Test the data of an empty (not loaded) PDF document.
///
/// When a document has been created but still don't have a document
/// loaded, then the document gives some initial data. This can happen
/// when the first application's window is open but no PDF has been loaded 
/// yet.
///
void
PDFDocumentTest::emptyDocument ()
{
    CPPUNIT_ASSERT (!m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getFileName ()));
    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp ("", m_Document->getTitle ()));
    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp ("", m_Document->getAuthor ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getSubject ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getKeywords ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getCreator ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getProducer ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getFormat ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("No", m_Document->getLinearized ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getCreationDate ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getModifiedDate ()));
    CPPUNIT_ASSERT_EQUAL (PageModeUnset, m_Document->getPageMode ());
    CPPUNIT_ASSERT_EQUAL (PageLayoutUnset, m_Document->getPageLayout ());
    CPPUNIT_ASSERT_EQUAL (0, m_Document->getNumPages ());
    CPPUNIT_ASSERT_EQUAL (0, m_Document->getCurrentPageNum ());
}

///
/// @brief Tests that loading a file which does not exist fails.
///
/// Loading a PDF can fail under three conditions: the file is not found,
/// the file is not a PDF (or a malformed PDF file) and the file is encrypted
/// but not password has been supplied.
///
/// This test just check that loading a not existent file fails.
///
void
PDFDocumentTest::fileNotFound (void)
{
    gchar *testFile = getTestFile ("noFile.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedError ());
    CPPUNIT_ASSERT (!m_Document->isLoaded ());

    gchar *documentError = IDocument::getErrorMessage(DocumentErrorOpenFile);
    gchar *errorMessage = g_strdup_printf (
            "Failed to load document '%s'.\n%s\n", testFile, documentError);
    g_free(documentError);
    const GError *error = m_Observer->getLoadError ();
    DocumentError errorCode = (DocumentError)error->code;
    CPPUNIT_ASSERT_EQUAL (DocumentErrorOpenFile, errorCode);
    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp (errorMessage, 
                                                 error->message));
    g_free (errorMessage);
    g_free (testFile);
}

///
/// @brief Test that loading an invalid file fails.
///
/// This test checks the second load failure: the file is not a PDF or 
/// is malformed.
///
void
PDFDocumentTest::invalidFile (void)
{
    gchar *testFile = getTestFile ("PDFDocumentTest.cxx");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedError ());
    CPPUNIT_ASSERT (!m_Document->isLoaded ());

    gchar *documentError = IDocument::getErrorMessage(DocumentErrorDamaged);
    gchar *errorMessage = g_strdup_printf (
            "Failed to load document '%s'.\n%s\n", testFile, documentError);
    g_free(documentError);
    const GError *error = m_Observer->getLoadError ();
    DocumentError errorCode = (DocumentError)error->code;
    CPPUNIT_ASSERT_EQUAL (DocumentErrorDamaged, errorCode);
    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp (errorMessage, 
                                                 error->message));
    g_free (testFile);
    g_free (errorMessage);
}

///
/// @brief Test to load an encrypted file using invalid passwords.
///
/// This is the last test for loading an invalid file. This test tries
/// to open twice the same encrypted file. The first time with a NULL password
/// (as unencrypted) and then using an invalid password (the real password
/// is 'testpasswd').
///
void
PDFDocumentTest::encryptedFile (void)
{
    gchar *testFile = getTestFile ("test_encrypted.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedPassword ());
    CPPUNIT_ASSERT (!m_Document->isLoaded ());

    gchar *documentError = IDocument::getErrorMessage(DocumentErrorEncrypted);
    gchar *errorMessage = g_strdup_printf (
            "Failed to load document '%s'.\n%s\n", testFile, documentError);
    g_free(documentError);
    const GError *error = m_Observer->getLoadError ();
    DocumentError errorCode = (DocumentError)error->code;
    CPPUNIT_ASSERT_EQUAL (DocumentErrorEncrypted, errorCode);
    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp (errorMessage, 
                                                 error->message));
    g_free (errorMessage);
    
    // Now try with an invalid password...
    m_Document->load (testFile, "invalidpasswd");
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedPassword ());
    CPPUNIT_ASSERT (!m_Document->isLoaded ());

    documentError = IDocument::getErrorMessage(DocumentErrorEncrypted);
    errorMessage = g_strdup_printf (
            "Failed to load document '%s'.\n%s\n", testFile, documentError);
    g_free(documentError);
    error = m_Observer->getLoadError ();
    errorCode = (DocumentError)error->code;
    CPPUNIT_ASSERT_EQUAL (DocumentErrorEncrypted, errorCode);
    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp (errorMessage, 
                                                 error->message));
    g_free (errorMessage);

    // And now, just to test that it can load the pdf using the 
    // correct password.
    m_Document->load (testFile, "testpasswd");
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp (testFile, m_Document->getFileName ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("testpasswd", m_Document->getPassword ()));

    g_free (testFile);
}

///
/// @brief Test to open a valid file.
///
/// This test will open a valid file and will try to read it's values.
/// Unfortunately I'm unable to produce a file with all fields, so some
/// of them will stay empty as in the first test.
///
void
PDFDocumentTest::validFile ()
{
    gchar *testFile = getTestFile ("test2.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp (testFile, m_Document->getFileName ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("Lorem Ipsum Dolor Site Amet", 
                                m_Document->getTitle ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("Jordi Fita", m_Document->getAuthor ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("Test PDF File", m_Document->getSubject ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("Lorem Ipsum Dolor Sit Amet Consectetuer", 
                                m_Document->getKeywords ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("PDFCreator Version 0.9.0", 
                                m_Document->getCreator ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("AFPL Ghostscript 8.53", 
                                m_Document->getProducer ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("PDF-1.3", m_Document->getFormat ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("No", m_Document->getLinearized ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("2006-10-04 23:12:27", 
                                m_Document->getCreationDate ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("2006-10-04 23:12:27", 
                                m_Document->getModifiedDate ()));
    CPPUNIT_ASSERT_EQUAL (PageModeUnset, m_Document->getPageMode ());
    CPPUNIT_ASSERT_EQUAL (PageLayoutUnset, m_Document->getPageLayout ());
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getNumPages ());
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());

    // Another test file.
    g_free (testFile);
    testFile = getTestFile ("test1.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp (testFile, m_Document->getFileName ()));
    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp ("", m_Document->getTitle ()));
    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp ("", m_Document->getAuthor ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getSubject ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getKeywords ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getCreator ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("FOP 0.20.5", m_Document->getProducer ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("PDF-1.3", m_Document->getFormat ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("No", m_Document->getLinearized ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getCreationDate ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("", m_Document->getModifiedDate ()));
    CPPUNIT_ASSERT_EQUAL (PageModeOutlines, m_Document->getPageMode ());
    CPPUNIT_ASSERT_EQUAL (PageLayoutUnset, m_Document->getPageLayout ());
    CPPUNIT_ASSERT_EQUAL (5, m_Document->getNumPages ());
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());

    g_free (testFile);
}

///
/// @brief Test loading a relative path filename.
///
/// Poppler's glib wrapper needs an URI instead of just a filename, but
/// in order to build it I need an absolute path. This check test that
/// giving a relative path converts it to absolute and loads the file
/// properly.
///
void
PDFDocumentTest::relativePath ()
{
    m_Document->load (TEST_DIR "test1.pdf", NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp (TEST_DIR "test1.pdf", 
                                m_Document->getFileName ()));
}

///
/// @brief Test changing pages.
///
/// The IDocument class also has the information about the current page
/// that's shown, so we must change it through its methods. This test just
/// checks that the page is changed correctly.
///
void
PDFDocumentTest::pageChange ()
{
    // First try with a 2 page document.
    gchar *testFile = getTestFile ("test2.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getNumPages ());
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    m_Document->goToNextPage ();
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (2, m_Observer->getCurrentPage ());
    // Don't let the page go after the last page.
    m_Document->goToNextPage ();
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (2, m_Observer->getCurrentPage ());
    // Now go backwards.
    m_Document->goToPreviousPage ();
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    m_Document->goToPreviousPage ();
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    // First and last pages.
    m_Document->goToLastPage ();
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (2, m_Observer->getCurrentPage ());
    m_Document->goToFirstPage ();
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    // Now, let's go to an specific page. If the page is beyond the
    // last page or before the first page then the current page
    // doesn't change.
    m_Document->goToPage (2);
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (2, m_Observer->getCurrentPage ());
    m_Document->goToPage (1);
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    m_Document->goToPage (13);
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    m_Document->goToPage (0);
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());

    // Let's try with a 5 page document.
    g_free (testFile);
    testFile = getTestFile ("test1.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (5, m_Document->getNumPages ());
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    m_Document->goToNextPage ();
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (2, m_Observer->getCurrentPage ());
    m_Document->goToNextPage ();
    CPPUNIT_ASSERT_EQUAL (3, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (3, m_Observer->getCurrentPage ());
    m_Document->goToPage (5);
    CPPUNIT_ASSERT_EQUAL (5, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    // Don't let the page go after the last page.
    m_Document->goToNextPage ();
    CPPUNIT_ASSERT_EQUAL (5, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    // Now go backwards.
    m_Document->goToPreviousPage ();
    CPPUNIT_ASSERT_EQUAL (4, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (4, m_Observer->getCurrentPage ());
    m_Document->goToPreviousPage ();
    CPPUNIT_ASSERT_EQUAL (3, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (3, m_Observer->getCurrentPage ());
    m_Document->goToPage (1);
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    m_Document->goToPreviousPage ();
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    // First and last pages.
    m_Document->goToLastPage ();
    CPPUNIT_ASSERT_EQUAL (5, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    m_Document->goToFirstPage ();
    CPPUNIT_ASSERT_EQUAL (1, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (1, m_Observer->getCurrentPage ());
    // Now, let's go to an specific page. If the page is beyond the
    // last page, then the current page will be the last. On the other
    // hand, if the page is less than the first (< 1) then the current
    // page will be the first.
    m_Document->goToPage (3);
    CPPUNIT_ASSERT_EQUAL (3, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (3, m_Observer->getCurrentPage ());
    m_Document->goToPage (2);
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (2, m_Observer->getCurrentPage ());
    m_Document->goToPage (13);
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (2, m_Observer->getCurrentPage ());
    m_Document->goToPage (0);
    CPPUNIT_ASSERT_EQUAL (2, m_Document->getCurrentPageNum ());
    CPPUNIT_ASSERT_EQUAL (2, m_Observer->getCurrentPage ());

    g_free (testFile);
}

///
/// @brief Test rotating a page.
///
/// It's possible to rotate a page, but only can rotate in 90 degree
/// step (0, 90, 180 and 270) in either side (left or right).
///
void
PDFDocumentTest::pageRotate ()
{
    gchar *testFile = getTestFile ("test2.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (0, m_Document->getRotation ());
    m_Document->rotateRight ();
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ());
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    m_Document->rotateLeft ();
    CPPUNIT_ASSERT_EQUAL (0, m_Document->getRotation ());
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    m_Document->rotateRight ();
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    m_Document->rotateRight ();
    CPPUNIT_ASSERT_EQUAL (180, m_Document->getRotation ());
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    m_Document->rotateRight ();
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    m_Document->rotateRight ();
    CPPUNIT_ASSERT_EQUAL (0, m_Document->getRotation ());
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    m_Document->rotateLeft ();
    CPPUNIT_ASSERT_EQUAL (270, m_Document->getRotation ());
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    
    g_free (testFile);
    testFile = getTestFile ("test1.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_EQUAL (0, m_Document->getRotation ());
    m_Document->rotateRight ();
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    m_Document->rotateRight ();
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    m_Document->rotateLeft ();
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ());
    CPPUNIT_ASSERT (m_Observer->notifiedRotation ());
    
    g_free (testFile);
}

///
/// @brief Test the document's zoom.
///
/// The IDocument class is also responsible of the page's zoom level. It can
/// Zoom In, Zoom Out, Zoom to Width and Zoom to Fit.
///
/// Currently the Zoom In uses a factor of ~1.2 and the ZoomOut of 
/// ~(1.0/1.2)= ~0.8333. The max scale is 4.0 and the 
///
void
PDFDocumentTest::pageZoom ()
{
    // On this document only try Zoom In and Zoom Out. 
    gchar *testFile = getTestFile ("test2.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0f, m_Document->getZoom (), DELTA);
    m_Document->zoomIn ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.2f, m_Document->getZoom (), DELTA);
    CPPUNIT_ASSERT (m_Observer->notifiedZoom ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.2f, m_Observer->getZoom (), DELTA);
    m_Document->zoomIn ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.44f, m_Document->getZoom (), DELTA);
    CPPUNIT_ASSERT (m_Observer->notifiedZoom ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.44f, m_Observer->getZoom (), DELTA);
    m_Document->zoomOut ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.2f, m_Document->getZoom (), DELTA);
    CPPUNIT_ASSERT (m_Observer->notifiedZoom ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.2f, m_Observer->getZoom (), DELTA);
    m_Document->zoomIn ();
    m_Document->zoomIn ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.728f, m_Document->getZoom (), DELTA);
    CPPUNIT_ASSERT (m_Observer->notifiedZoom ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.728f, m_Observer->getZoom (), DELTA);

    // On this document we'll use the zoom to fit and zoom to width.
    // This document is A4 in size (595 x 842).
    g_free (testFile);
    testFile = getTestFile ("test1.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0f, m_Document->getZoom (), DELTA);
    m_Document->zoomToWidth (200);    
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3361f, m_Document->getZoom (), DELTA);
    CPPUNIT_ASSERT (m_Observer->notifiedZoom ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3361f, m_Observer->getZoom (), DELTA);
    m_Document->zoomToFit (100, 50);    
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0593f, m_Document->getZoom (), DELTA);
    CPPUNIT_ASSERT (m_Observer->notifiedZoom ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0593f, m_Observer->getZoom (), DELTA);
    m_Document->zoomToFit (50, 100);    
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0840f, m_Document->getZoom (), DELTA);
    CPPUNIT_ASSERT (m_Observer->notifiedZoom ());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0840f, m_Observer->getZoom (), DELTA);

    g_free (testFile);
}

///
/// @brief Test rendering a page.
///
/// Rendering a page just means get the pixels of the page's image. Initially
/// I though to use a GdkPixbuf, but since I don't want to have Gtk+-2 as
/// a core dependence (I think it will be possible to make a Qt shell or 
/// whatever on the future), so rendering a page gives a DocumentPage object.
///
/// I can only test the height, width and row stride (bytes between rows)
/// of the DocumentPage class, as I don't know of a way to test if an image
/// is rendered correctly pixel by pixel. I will just test that the image is
/// not filled with 0x00 (initial state...)
///
void
PDFDocumentTest::pageRender ()
{
    gchar *testFile = getTestFile ("test2.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    // Since the document is 595x842 but rotated 90 degrees, 
    // and I request a zoom to width (300) the page will have a zoom of 
    // 0.3562, which means that the final image will be 300x212 pixels. 
    // As *every* page uses 3 bytes (RGB) x 8 bits per pixel, 
    // the image is 190800 bytes long.
    m_Document->rotateRight ();
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ());
    m_Document->zoomToWidth (300); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3562f, m_Document->getZoom (), DELTA);
    DocumentPage *page = NULL;
    while ( NULL == page || (DocumentPage *)0xdeadbeef == page )
    {
        page = m_Document->getCurrentPage ();
    }
    CPPUNIT_ASSERT (NULL != page);
    CPPUNIT_ASSERT ((DocumentPage *)0xdeadbeef != page);
    CPPUNIT_ASSERT_EQUAL (300, page->getWidth ());
    CPPUNIT_ASSERT_EQUAL (212, page->getHeight ());
    // The row stride is the bytes between rows in the page's pixels.
    // 300 (Width) * 3 (RGB) in this case.
    CPPUNIT_ASSERT_EQUAL (900, page->getRowStride ());
    // Test only if the data is not 0x00.
    gint imageSize = page->getRowStride () * page->getHeight ();
    guchar *testData = new guchar[imageSize];
    memset (testData, 0, imageSize);
    CPPUNIT_ASSERT (0 != memcmp (testData, page->getData (), imageSize));
    delete[] testData;

    g_free (testFile);
}

///
/// @brief Tests the document's link.
///
/// If a page has a links, we should be able to ask the document if the
/// mouse pointer is over any link (to be able to change the cursor) and to
/// tell the document to change the current page for the page a links points
/// to.
///
/// All this should happen even if the page is resized but will keep things
/// simple for now and only for unrotated documents.
///
void
PDFDocumentTest::pageLinks ()
{
    gchar *testFile = getTestFile ("test1.pdf");
    m_Document->load (testFile, NULL);
    g_free (testFile);
    while ( !m_Observer->loadFinished () ) { }
    CPPUNIT_ASSERT (m_Observer->notifiedLoaded ());
    CPPUNIT_ASSERT (m_Document->isLoaded ());

    // In this test PDF document there are two links on page 4.
    // Both links points to page 5 and are situated at:
    // ((72.00, 147.11), (134.50, 137.11))
    // ((96.00, 158.11), (145.17, 148.11))
    //
    // First try with an unscaled document.
    m_Document->goToPage (4);
    DocumentPage *page = NULL;
    while ( NULL == page || (DocumentPage *)0xdeadbeef == page )
    {
        page = m_Document->getCurrentPage ();
    }
    CPPUNIT_ASSERT (!m_Document->hasLinkAtPosition (70, 140));
    CPPUNIT_ASSERT (!m_Document->hasLinkAtPosition (76, 40));
    CPPUNIT_ASSERT (!m_Document->hasLinkAtPosition (136, 139));
    CPPUNIT_ASSERT (!m_Document->hasLinkAtPosition (130, 160));
    CPPUNIT_ASSERT (m_Document->hasLinkAtPosition (76, 139));
    CPPUNIT_ASSERT (m_Document->hasLinkAtPosition (100, 150));
    CPPUNIT_ASSERT (!m_Document->hasLinkAtPosition (90, 151));
    CPPUNIT_ASSERT (!m_Document->hasLinkAtPosition (100, 130));
    CPPUNIT_ASSERT (!m_Document->hasLinkAtPosition (110, 160));
    CPPUNIT_ASSERT (!m_Document->hasLinkAtPosition (150, 152));

    // Active a link and let see how the page is changed.
    m_Document->activateLinkAtPosition (76, 139);
    CPPUNIT_ASSERT_EQUAL (5, m_Document->getCurrentPageNum ());

    // Now try the same but with an scaled page. (x1.2)
    // With this scale the links are now at:
    // ((86.40, 176.53), (160.80, 164.53))
    // ((115.2, 189.73), (174.20, 177.73))
    //
    m_Document->zoomIn ();
    m_Document->goToPage (4);
    page = NULL;
    while ( NULL == page || (DocumentPage *)0xdeadbeef == page )
    {
        page = m_Document->getCurrentPage ();
    }
    // Just check the first link and activate the second.
    CPPUNIT_ASSERT (m_Document->hasLinkAtPosition (87, 175));
    m_Document->activateLinkAtPosition (174, 177);
    CPPUNIT_ASSERT_EQUAL (5, m_Document->getCurrentPageNum ());
}

///
/// @brief Tests finding text in a page.
///
void
PDFDocumentTest::pageFindText ()
{
    gchar *testFile = getTestFile ("test1.pdf");
    m_Document->load (testFile, NULL);
    g_free (testFile);
    while ( !m_Observer->loadFinished () ) { }

    // If a text doesn't exist on a page the list of found text is
    // empty.
    CPPUNIT_ASSERT (NULL == m_Document->findTextInPage (4, "nothing"));
    // On the other hand, if something is found the list is filled with
    // the rectangles the text is found.
    GList *results = m_Document->findTextInPage (4, "first");
    CPPUNIT_ASSERT (NULL != results);
    // In this case it should have found 2 results.
    CPPUNIT_ASSERT_EQUAL ((guint)2, g_list_length (results));

    GList *firstResult = g_list_first (results);
    {
        DocumentRectangle *rect = (DocumentRectangle *)firstResult->data;
        CPPUNIT_ASSERT_DOUBLES_EQUAL (82.00, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (137.11, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (100.34, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (146.11, rect->getY2 (), 0.0001);
    }
    GList *secondResult = g_list_next (firstResult);
    {
        DocumentRectangle *rect = (DocumentRectangle *)secondResult->data;
        CPPUNIT_ASSERT_DOUBLES_EQUAL (96.00, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (148.11, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (114.34, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (157.11, rect->getY2 (), 0.0001);
    }
    g_list_free (results);
}
