// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Main Presenter Test Fixture.
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

#include <epdfview.h>
#include "DumbDocument.h"
#include "DumbMainView.h"
#include "MainPterTest.h"

using namespace ePDFView;

G_LOCK_EXTERN (JobRender);

// Register the test suite into the `registry'.
CPPUNIT_TEST_SUITE_REGISTRATION (MainPterTest);

///
/// @brief Sets up the environment for each test.
///
void
MainPterTest::setUp ()
{
    m_Document = new DumbDocument ();
    m_MainPter = new MainPter (m_Document);
    m_View = new DumbMainView (m_MainPter);
    m_MainPter->setView (m_View);
    Config::loadFile (FALSE);
    G_LOCK (JobRender);
    JobRender::m_CanProcessJobs = TRUE;
    G_UNLOCK (JobRender);
}

///
/// @brief Cleans up after each test.
///
void
MainPterTest::tearDown ()
{
    G_LOCK (JobRender);
    JobRender::m_CanProcessJobs = FALSE;
    IJob::clearQueue ();
    G_UNLOCK (JobRender);
    // We only need to delete the presenter, as all other
    // classes are deleted by it.
    delete m_MainPter;
    // Drop the current configuration.
    Config::destroy ();
}


///
/// @brief Tests the initial status of the main presenter.
///
/// Initially the presenter hasn't loaded a document, yet, so
/// the main window's title is 'PDF Viewer', the page selection 
/// and zoom actions are insensitive and the documentView has
/// no image yet. Also the side bar won't be displayed, yet.
///
void
MainPterTest::initialStatus ()
{
    CPPUNIT_ASSERT ( m_View->isShown () );
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp ("PDF Viewer", m_View->getTitle ()));
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveReload ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateLeft ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateRight ());
    CPPUNIT_ASSERT (!m_View->isSensitiveSave ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoom ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomOut ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomFit ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomWidth ());
    CPPUNIT_ASSERT (!m_View->isShownIndex ());
    CPPUNIT_ASSERT (m_View->isShownToolbar ());
    CPPUNIT_ASSERT (m_View->isShownStatusbar ());
#if defined (HAVE_CUPS)
    CPPUNIT_ASSERT (!m_View->isSensitivePrint ());
#endif // HAVE_CUPS
}

///
/// @brief Test loading a document.
///
/// Loading a document will set the title to the document's title or the
/// document's file name, if it has no title. Also will sensitive the
/// go to next page, go to last page and all zoom actions. Also will
/// show the document's first page. Since this document don't have
/// outline, the sidebar won't be displayed.
///
void
MainPterTest::loadDocument ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp ("/tmp/test.pdf", m_View->getTitle ()));
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveReload ());
    CPPUNIT_ASSERT (m_View->isSensitiveRotateLeft ());
    CPPUNIT_ASSERT (m_View->isSensitiveRotateRight ());
    CPPUNIT_ASSERT (m_View->isSensitiveSave ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoom ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomOut ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomFit ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomWidth ());
    CPPUNIT_ASSERT (!m_View->isShownIndex ());
#if defined (HAVE_CUPS)
    CPPUNIT_ASSERT (m_View->isSensitivePrint ());
#endif // HAVE_CUPS

    // Now try a document with a title.
    m_Document->setTitle (g_strdup ("Test PDF"));
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("Test PDF", m_View->getTitle ()));
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveReload ());
    CPPUNIT_ASSERT (m_View->isSensitiveRotateLeft ());
    CPPUNIT_ASSERT (m_View->isSensitiveRotateRight ());
    CPPUNIT_ASSERT (m_View->isSensitiveSave ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoom ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomOut ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomFit ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomWidth ());
    CPPUNIT_ASSERT (!m_View->isShownIndex ());
#if defined (HAVE_CUPS)
    CPPUNIT_ASSERT (m_View->isSensitivePrint ());
#endif // HAVE_CUPS
}

///
/// @brief Tests a canceled open file.
///
/// When the user cancels the open dialog, then nothing should change.
///
void
MainPterTest::loadCanceled ()
{
    // returning a NULL file name is what will happen when cancel.
    m_View->setOpenFileName (NULL);
    m_MainPter->openFileActivated ();
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp ("PDF Viewer", m_View->getTitle ()));
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveReload ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateLeft ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateRight ());
    CPPUNIT_ASSERT (!m_View->isSensitiveSave ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoom ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomOut ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomFit ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomWidth ());
    CPPUNIT_ASSERT (!m_View->shownError ());
#if defined (HAVE_CUPS)
    CPPUNIT_ASSERT (!m_View->isSensitivePrint ());
#endif // HAVE_CUPS
}


///
/// @brief Test a failed load.
///
/// A failed load (anything but a DocumentErrorNone and DocumentErrorEncrypted)
/// is very similar to the load canceled, but an error message is shown 
/// to the user.
///
void
MainPterTest::loadFailed ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_Document->setOpenError (DocumentErrorDamaged);
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp ("PDF Viewer", m_View->getTitle ()));
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveReload ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateLeft ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateRight ());
    CPPUNIT_ASSERT (!m_View->isSensitiveSave ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoom ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomOut ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomFit ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomWidth ());
    CPPUNIT_ASSERT (m_View->shownError ());
#if defined (HAVE_CUPS)
    CPPUNIT_ASSERT (!m_View->isSensitivePrint ());
#endif // HAVE_CUPS
}

///
/// @brief Test a canceled password.
///
/// A canceled password is more or less the same as canceling the loading
/// of a file. It's when the user tried to open an encrypted file, but when 
/// the password is prompted, then it cancels :-)
/// It should happen the same as when canceling the open file dialog.
///
void
MainPterTest::canceledPassword ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_View->setPassword (NULL);
    m_Document->setOpenError (DocumentErrorEncrypted);
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp ("PDF Viewer", m_View->getTitle ()));
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveReload ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateLeft ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateRight ());
    CPPUNIT_ASSERT (!m_View->isSensitiveSave ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoom ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomOut ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomFit ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomWidth ());
    CPPUNIT_ASSERT (!m_View->shownError ());
#if defined (HAVE_CUPS)
    CPPUNIT_ASSERT (!m_View->isSensitivePrint ());
#endif // HAVE_CUPS
}

///
/// @brief Test a bad password.
///
/// This test is very similar to the previous, but this time the user
/// enters a bad password, trying to guess which is the correct password.
/// The presenter gives up after three times and shows an error message.
///
void
MainPterTest::badPassword ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_View->setPassword ("badpassword");
    m_Document->setTestPassword ("goodpassword");
    m_Document->setOpenError (DocumentErrorEncrypted);
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp ("PDF Viewer", m_View->getTitle ()));
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveReload ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateLeft ());
    CPPUNIT_ASSERT (!m_View->isSensitiveRotateRight ());
    CPPUNIT_ASSERT (!m_View->isSensitiveSave ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoom ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomOut ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomFit ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomWidth ());
    CPPUNIT_ASSERT (m_View->shownError ());
    CPPUNIT_ASSERT_EQUAL (3, m_View->countTimesShownPasswordPrompt ());
#if defined (HAVE_CUPS)
    CPPUNIT_ASSERT (!m_View->isSensitivePrint ());
#endif // HAVE_CUPS
}

///
/// @brief Test a good password.
///
/// This test just check that giving the correct password opens the
/// file correctly.
///
void
MainPterTest::goodPassword ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_View->setPassword ("goodpassword");
    m_Document->setTestPassword ("goodpassword");
    m_Document->setOpenError (DocumentErrorEncrypted);
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp ("/tmp/test.pdf", m_View->getTitle ()));
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveReload ());
    CPPUNIT_ASSERT (m_View->isSensitiveRotateLeft ());
    CPPUNIT_ASSERT (m_View->isSensitiveRotateRight ());
    CPPUNIT_ASSERT (m_View->isSensitiveSave ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoom ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomOut ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomFit ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomWidth ());
    CPPUNIT_ASSERT (!m_View->shownError ());
    CPPUNIT_ASSERT_EQUAL (1, m_View->countTimesShownPasswordPrompt ());
#if defined (HAVE_CUPS)
    CPPUNIT_ASSERT (m_View->isSensitivePrint ());
#endif // HAVE_CUPS
}

///
/// @brief Test the last folder used to open a file.
///
/// When the presenter wants to open a file, it should pass to the
/// view the last folder that was used to open a file, so the
/// view can show it up when opening the dialog.
///
void
MainPterTest::openLastFolder ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT (NULL == m_View->getLastOpenFileFolder ());


    m_View->setOpenFileName ("/usr/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("/tmp", 
                                             m_View->getLastOpenFileFolder ()));

    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("/usr", 
                                             m_View->getLastOpenFileFolder ()));
}

///
/// @brief Test saving a document.
///
/// Saving a document will call the saveFile() function of the document's class.
///
void
MainPterTest::saveDocument ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    m_View->setSaveFileName ("/tmp/copy.pdf");
    m_MainPter->saveFileActivated ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("/tmp/copy.pdf",
                                             m_Document->getSavedFileName ()));
}

///
/// @brief Tests a canceled save file.
///
/// When the user cancels the save dialog, then nothing should be saved.
///
void
MainPterTest::saveCanceled ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    // returning a NULL file name is what will happen when cancel.
    m_View->setSaveFileName (NULL);
    m_MainPter->saveFileActivated ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("",
                                             m_Document->getSavedFileName ()));
}

///
/// @brief Test the last folder used to save a file.
///
/// When the presenter wants to save a file, it should pass to the
/// view the last folder that was used to save a file, so the
/// view can show it up when saving the dialog.
///
void
MainPterTest::saveLastFolder ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    m_View->setSaveFileName ("/tmp/copy.pdf");
    m_MainPter->saveFileActivated ();
    m_MainPter->waitForFileSaved ();
    CPPUNIT_ASSERT (NULL == m_View->getLastSaveFileFolder ());


    m_View->setSaveFileName ("/usr/copy.pdf");
    m_MainPter->saveFileActivated ();
    m_MainPter->waitForFileSaved ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("/tmp",
                                             m_View->getLastSaveFileFolder ()));

    m_MainPter->saveFileActivated ();
    m_MainPter->waitForFileSaved ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("/usr",
                                             m_View->getLastSaveFileFolder ()));

}


///
/// @brief Test page navigation.
///
/// I'll load a 4-pages document and will try to go to the first, last, next, 
/// previous and also on a page using the entry on the toolbar.
///
void
MainPterTest::pageNavigation ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_Document->setNumPages (4);
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    // Check that sets the correct current page.
    CPPUNIT_ASSERT_EQUAL (1, m_View->getCurrentPage ());
    // Going to the next page should make all actions sensitive.
    m_MainPter->goToNextPageActivated ();
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPreviousPage ());
    // Going to the last will make some insensitive.
    m_MainPter->goToLastPageActivated ();
    CPPUNIT_ASSERT_EQUAL (4, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPreviousPage ());
    // Again to the last won't harm (just in case...) nor going next.
    m_MainPter->goToLastPageActivated ();
    m_MainPter->goToNextPageActivated ();
    CPPUNIT_ASSERT_EQUAL (4, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPreviousPage ());
    // But going to the previous will change something.
    m_MainPter->goToPreviousPageActivated ();
    CPPUNIT_ASSERT_EQUAL (3, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPreviousPage ());
    // Now, to the first again.
    m_MainPter->goToFirstPageActivated ();
    CPPUNIT_ASSERT_EQUAL (1, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
    // Check sane behaviour.
    m_MainPter->goToFirstPageActivated ();
    m_MainPter->goToPreviousPageActivated ();
    CPPUNIT_ASSERT_EQUAL (1, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());
}

///
/// @brief Test the page navigation using the entry on the toolbar.
///
/// The application has a text entry that the user can use to to a 
/// page just by writing the number. Any non number after the first number
/// character will be discarded.
///
void
MainPterTest::pageNavigationEntry ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_Document->setNumPages (4);
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    // Check that sets the correct current page.
    CPPUNIT_ASSERT_EQUAL (1, m_View->getCurrentPage ());

    // Now try again using the page entry on toolbar.
    m_View->setGoToPageText ("2 of 4");
    m_MainPter->goToPageActivated ();
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPreviousPage ());

    m_View->setGoToPageText ("4 of 4");
    m_MainPter->goToPageActivated ();
    CPPUNIT_ASSERT_EQUAL (4, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPreviousPage ());

    m_View->setGoToPageText ("1 of 4");
    m_MainPter->goToPageActivated ();
    CPPUNIT_ASSERT_EQUAL (1, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());

    // Invalid values. Remains to the same page.
    m_View->setGoToPageText ("123123 of 1");
    m_MainPter->goToPageActivated ();
    CPPUNIT_ASSERT_EQUAL (1, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());

    m_View->setGoToPageText ("0 of 12");
    m_MainPter->goToPageActivated ();
    CPPUNIT_ASSERT_EQUAL (1, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (!m_View->isSensitiveGoToPreviousPage ());

    m_MainPter->goToNextPageActivated ();
    m_View->setGoToPageText ("Jejej :-)");
    m_MainPter->goToPageActivated ();
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT (m_View->isSensitiveGoToFirstPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToLastPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToNextPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPage ());
    CPPUNIT_ASSERT (m_View->isSensitiveGoToPreviousPage ());
}

///
/// @brief Test the rotation of the page.
///
/// This is a very simple test about page rotation.
///
void
MainPterTest::pageRotate ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0, m_Document->getRotation ());

    m_MainPter->rotateRightActivated (); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ());

    m_MainPter->rotateLeftActivated (); 
    CPPUNIT_ASSERT_EQUAL (0, m_Document->getRotation ());
}

///
/// @brief Tests page's zoom.
///
/// The DumbDocument has a fixed size of 100x250 pixels and the DumbMainView
/// has a fixed pageView of 75x50. We will make sure that getting to the max
/// ZoomIn insensitives the ZoomIn button. The same for the ZoomOut.
///
///
void
MainPterTest::pageZoomInAndOut ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, m_Document->getZoom (), 0.0001);

    m_MainPter->zoomInActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.2, m_Document->getZoom (), 0.0001);

    m_MainPter->zoomOutActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, m_Document->getZoom (), 0.0001);

    CPPUNIT_ASSERT (m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomOut ());
    // Make sure we go to the last zoom level.
    for (int count = 0 ; count < 20 ; count++)
    {
        m_MainPter->zoomInActivated ();
    }
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (m_View->isSensitiveZoomOut ());
    for (int count = 0 ; count < 40 ; count ++)
    {
        m_MainPter->zoomOutActivated ();
    }
    CPPUNIT_ASSERT (m_View->isSensitiveZoomIn ());
    CPPUNIT_ASSERT (!m_View->isSensitiveZoomOut ());
}

///
/// @brief Tests page's zoom to width
///
void
MainPterTest::pageZoomWidth ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();

    m_MainPter->zoomWidthActivated (TRUE);
    CPPUNIT_ASSERT (m_View->isZoomToWidthActive ());
    m_MainPter->zoomWidthActivated (FALSE);
    CPPUNIT_ASSERT (!m_View->isZoomToWidthActive ());
    m_MainPter->zoomWidthActivated (TRUE);
    CPPUNIT_ASSERT (m_View->isZoomToWidthActive ());
    // Changing any of the zoom in or zoom out features should make the
    // zoom to width not being active.
    m_MainPter->zoomInActivated ();
    CPPUNIT_ASSERT (!m_View->isZoomToWidthActive ());
    m_MainPter->zoomWidthActivated (TRUE);
    m_MainPter->zoomOutActivated ();
    CPPUNIT_ASSERT (!m_View->isZoomToWidthActive ());
}

///
/// @brief Tests page's zoom to fit
///
void
MainPterTest::pageZoomFit ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();

    m_MainPter->zoomFitActivated (TRUE);
    CPPUNIT_ASSERT (m_View->isZoomToFitActive ());
    m_MainPter->zoomFitActivated (FALSE);
    CPPUNIT_ASSERT (!m_View->isZoomToFitActive ());
    m_MainPter->zoomFitActivated (TRUE);
    CPPUNIT_ASSERT (m_View->isZoomToFitActive ());
    // Changing any of the zoom in or zoom out features should make the
    // zoom to fit not being active.
    m_MainPter->zoomInActivated ();
    CPPUNIT_ASSERT (!m_View->isZoomToFitActive ());
    m_MainPter->zoomFitActivated (TRUE);
    m_MainPter->zoomOutActivated ();
    CPPUNIT_ASSERT (!m_View->isZoomToFitActive ());
}

///
/// @brief Checks rotation and zoom.
///
/// This tests that rotation while the zoom to width or zoom to
/// fit are activated also changed the zoom level.
///
void
MainPterTest::pageZoomAndRotate ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, m_Document->getZoom (), 0.0001);

    // The document is 100x250 and the page view 75x50.
    // ZoomToWidth 75/100 = 0.75
    m_MainPter->zoomWidthActivated (TRUE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.75, m_Document->getZoom (), 0.0001);
    // Rotate left. 75/250 = 0.3
    m_MainPter->rotateLeftActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
    m_MainPter->rotateRightActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.75, m_Document->getZoom (), 0.0001);
    m_MainPter->rotateRightActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
    m_MainPter->rotateLeftActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.75, m_Document->getZoom (), 0.0001);

    // Now try the same with zoom to fit.
    // 50/250 = 0.2
    m_MainPter->zoomFitActivated (TRUE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.2, m_Document->getZoom (), 0.0001);
    // Rotate left, 75/250 = 0.3
    m_MainPter->rotateLeftActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
    m_MainPter->rotateRightActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.2, m_Document->getZoom (), 0.0001);
    m_MainPter->rotateRightActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
    m_MainPter->rotateLeftActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.2, m_Document->getZoom (), 0.0001);
}

///
/// @brief Test the current zoom level indicator.
///
/// The main view has an entry in the tool bar that tells the user
/// which is the current document's zoom level. Also, when the user
/// enter a new zoom level and presses ENTER, the new zoom level is
/// used.
///
void
MainPterTest::pageZoomIndicator ()
{
    // When the open is first opened, the zoom is 100%.
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("100%", m_View->getZoomText ()));

    // Try changing a little the zoom level.
    m_MainPter->zoomInActivated ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("120%", m_View->getZoomText ()));
    m_MainPter->zoomInActivated ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("144%", m_View->getZoomText ()));
    m_MainPter->zoomOutActivated ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("120%", m_View->getZoomText ()));
    m_MainPter->zoomOutActivated ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("100%", m_View->getZoomText ()));
    m_MainPter->zoomOutActivated ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("83.3%", m_View->getZoomText ()));

    // Now try to set the zoom level by hand. If the zoom level is 
    // clamped 400% and 5.41%.
    m_View->setZoomText ("50%");
    m_MainPter->zoomActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.5, m_Document->getZoom (), 0.0001);
    m_View->setZoomText ("1000%");
    m_MainPter->zoomActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (4.0, m_Document->getZoom (), 0.0001);
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("400%", m_View->getZoomText ()));
    m_View->setZoomText ("1");
    m_MainPter->zoomActivated ();
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0541, m_Document->getZoom (), 0.0001);
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("5.41%", m_View->getZoomText ()));
}

///
/// @brief Test to reload a normal document.
///
/// A "normal" document is a non encrypted document. The document
/// should then be viewed at the same page, scale an rotation.
///
void
MainPterTest::reloadNormal ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();

    m_MainPter->goToNextPageActivated ();
    m_MainPter->rotateRightActivated (); 
    m_MainPter->zoomWidthActivated (TRUE);
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ()); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);

    // Reload the document.
    m_MainPter->reloadActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("/tmp/test.pdf", m_View->getTitle ()));
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ()); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
}

///
/// @brief Tries to reload an encrypted file.
///
/// Reloading an encrypted file won't ask for the password unless it changed, 
/// but here we assume that don't
///
void
MainPterTest::reloadEncrypted ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_View->setPassword ("goodpassword");
    m_Document->setTestPassword ("goodpassword");
    m_Document->setOpenError (DocumentErrorEncrypted);
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("/tmp/test.pdf", m_View->getTitle ()));
    
    m_MainPter->goToNextPageActivated ();
    m_MainPter->rotateRightActivated (); 
    m_MainPter->zoomWidthActivated (TRUE);
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ()); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);

    // Reload the document.
    m_View->setPassword ("badpassword");
    m_Document->setTestPassword ("goodpassword");
    m_Document->setOpenError (DocumentErrorEncrypted);
    m_MainPter->reloadActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("/tmp/test.pdf", m_View->getTitle ()));
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ()); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
    CPPUNIT_ASSERT (!m_View->shownError ());
    CPPUNIT_ASSERT_EQUAL (0, m_View->countTimesShownPasswordPrompt ());
}

///
/// @brief Tries to reload an encrypted file whose password changed.
///
/// Reloading an encrypted file won't ask for the password unless it changed.
/// If the password changed, then the document starts on the first page.
///
void
MainPterTest::reloadChangedPassword ()
{
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_View->setPassword ("goodpassword");
    m_Document->setTestPassword ("goodpassword");
    m_Document->setOpenError (DocumentErrorEncrypted);
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("/tmp/test.pdf", m_View->getTitle ()));
    
    m_MainPter->goToNextPageActivated ();
    m_MainPter->rotateRightActivated (); 
    m_MainPter->zoomWidthActivated (TRUE);
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ()); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);

    // Reload the document.
    m_View->setPassword ("newpassword");
    m_Document->setTestPassword ("newpassword");
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp("goodpassword", m_Document->getPassword ()));
    m_Document->setOpenError (DocumentErrorEncrypted);
    m_MainPter->reloadActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT_EQUAL (0,
            g_ascii_strcasecmp("newpassword", m_Document->getPassword ()));
    CPPUNIT_ASSERT_EQUAL (0, 
            g_ascii_strcasecmp ("/tmp/test.pdf", m_View->getTitle ()));
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ()); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ()); 
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
    CPPUNIT_ASSERT (!m_View->shownError ());
    CPPUNIT_ASSERT_EQUAL (1, m_View->countTimesShownPasswordPrompt ());
}

///
/// @brief Checks that the index is shown when a document has outline.
/// 
/// When the presenter needs to show the outline, it just passes the root
/// outline item to the view, and the view must then render it whatever it
/// can (a GtkTreeView in GTK, etc...)
///
void
MainPterTest::showIndex ()
{
    DocumentOutline *outline = new DocumentOutline ();
    DocumentOutline *child = new DocumentOutline ();
    child->setParent (outline);
    child->setTitle ("Outline 1");
    child->setDestination (2);
    outline->addChild (child);
    m_Document->setPageMode (PageModeOutlines);
    m_Document->setOutline (outline);    
    m_Document->setNumPages (4);
    
    m_View->setOpenFileName ("/tmp/test.pdf");
    m_MainPter->openFileActivated ();
    m_MainPter->waitForFileLoaded ();
    CPPUNIT_ASSERT (m_View->isShownIndex ());
    CPPUNIT_ASSERT (outline == m_View->getOutline ());

    // When the user does click on a outline item, the view
    // sends to the presenter the outline that has been activated.
    // Then the presenter goes to that page.
    m_MainPter->outlineActivated (child);
    CPPUNIT_ASSERT_EQUAL (2, m_View->getCurrentPage ());

    // When the user clicks on the "Show Index" option, the
    // index should hide. Another click and the index should
    // appear.
    m_MainPter->showIndexActivated (FALSE);
    CPPUNIT_ASSERT (!m_View->isShownIndex ());
    m_MainPter->showIndexActivated (TRUE);
    CPPUNIT_ASSERT (m_View->isShownIndex ());
}

///
/// @brief Checks showing and hidding the tool bar and status bar.
///
void
MainPterTest::showToolAndStatusBars ()
{
    m_MainPter->showToolbarActivated (FALSE);
    m_MainPter->showStatusbarActivated (FALSE);
    CPPUNIT_ASSERT (!m_View->isShownToolbar ());
    CPPUNIT_ASSERT (!m_View->isShownStatusbar ());
    m_MainPter->showToolbarActivated (FALSE);
    m_MainPter->showStatusbarActivated (TRUE);
    CPPUNIT_ASSERT (!m_View->isShownToolbar ());
    CPPUNIT_ASSERT (m_View->isShownStatusbar ());
    m_MainPter->showToolbarActivated (TRUE);
    m_MainPter->showStatusbarActivated (FALSE);
    CPPUNIT_ASSERT (m_View->isShownToolbar ());
    CPPUNIT_ASSERT (!m_View->isShownStatusbar ());
    m_MainPter->showToolbarActivated (TRUE);
    m_MainPter->showStatusbarActivated (TRUE);
    CPPUNIT_ASSERT (m_View->isShownToolbar ());
    CPPUNIT_ASSERT (m_View->isShownStatusbar ());
}

