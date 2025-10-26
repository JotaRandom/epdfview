// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Find Presenter Test Fixture.
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
#include "DumbDocumentObserver.h"
#include "DumbFindView.h"
#include "Utils.h"
#include "FindPterTest.h"

using namespace ePDFView;

G_LOCK_EXTERN (JobRender);

// Register the test suite into the `registry'.
CPPUNIT_TEST_SUITE_REGISTRATION (FindPterTest);

///
/// @brief Sets up the environment for each test.
///
void
FindPterTest::setUp ()
{
    m_Document = new PDFDocument ();
    m_Observer = new DumbDocumentObserver ();
    m_Document->attach (m_Observer);
    gchar *testFile = getTestFile ("test1.pdf");
    m_Document->load (testFile, NULL);
    while ( !m_Observer->loadFinished () ) { }
    m_FindPter = new FindPter (m_Document);
    m_View = new DumbFindView ();
    m_FindPter->setView (m_View);

    G_LOCK (JobRender);
    JobRender::m_CanProcessJobs = TRUE;
    G_UNLOCK (JobRender);
}

///
/// @brief Cleans up after each test.
///
void
FindPterTest::tearDown ()
{
    G_LOCK (JobRender);
    JobRender::m_CanProcessJobs = FALSE;
    IJob::clearQueue ();
    G_UNLOCK (JobRender);

    // Deleting the FindPter will not delete the DumbFindView.
    delete m_FindPter;
    m_Document->detach (m_Observer);
    delete m_Observer;
    delete m_Document;
    delete m_View;
}

///
/// @brief Checks the find view's controls sensitivity.
///
/// The Find Next and Find Previous buttons will be insensitive while the text
/// is empty (i.e., nothing to search), but will become sensitive when
/// there's text to search.
///
void
FindPterTest::viewSensitivity ()
{
    CPPUNIT_ASSERT (NULL != m_View->getPresenter ());

    CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp ("", m_View->getTextToFind ()));
    CPPUNIT_ASSERT (!m_View->isFindNextSensitive ());
    CPPUNIT_ASSERT (!m_View->isFindPreviousSensitive ());

    m_View->setTextToFind ("f");
    while ( m_Observer->isStillSearching () ) { }
    CPPUNIT_ASSERT (m_View->isFindNextSensitive ());
    CPPUNIT_ASSERT (m_View->isFindPreviousSensitive ());

    m_View->setTextToFind ("");
    CPPUNIT_ASSERT (!m_View->isFindNextSensitive ());
    CPPUNIT_ASSERT (!m_View->isFindPreviousSensitive ());
}

///
/// @brief Checks looking for an inexistent text.
///
/// If the text to search doesn't exist on the document, then the
/// find dialog will receive the text "Not found!" and all observers will
/// receive a notifyFindFinished() event.
///
void
FindPterTest::textDoesNotExist ()
{
    m_View->setTextToFind ("AnStupidLongTextThatShouldntAppearInTheDocument");
    while ( m_Observer->isStillSearching () ) { }
    const gchar *infoText = m_View->getInformationText ();
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("No Results Found!", infoText));
}

///
/// @brief Tests the Find Next button.
///
/// When a search starts it tried to find the next text in the document.
/// If the user presses the Find Next button, it will try to find the next
/// found text in the document, searching forward.
///
void
FindPterTest::findNext ()
{
    m_View->setTextToFind ("first");   
    volatile gboolean stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    
    // The 'first' text appears twice in page 4 and also twice in
    // page 5. So the first time it will set a rect on page 4.
    CPPUNIT_ASSERT_EQUAL (4, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (82.0000, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (137.1100, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (100.3400, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (146.1100, rect->getY2 (), 0.0001);
    }
   
    m_Observer->notifyFindStarted ();
    m_FindPter->findNextActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (4, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (96.0000, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (148.1100, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (114.3400, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (157.1100, rect->getY2 (), 0.0001);
    }
   
    m_Observer->notifyFindStarted ();
    m_FindPter->findNextActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (200.5943, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (84.5980, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (254.5158, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (107.6147, rect->getY2 (), 0.0001);
    }
    
    m_Observer->notifyFindStarted ();
    m_FindPter->findNextActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (164.1700, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (121.1020, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (180.2800, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (130.1020, rect->getY2 (), 0.0001);
    }
   
    m_Observer->notifyFindStarted ();
    m_FindPter->findNextActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (72.0000, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (143.1745, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (116.9349, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (162.3553, rect->getY2 (), 0.0001);
    }
   
    // Clicking after the last result will start again.
    m_Observer->notifyFindStarted ();
    m_FindPter->findNextActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (4, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (82.0000, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (137.1100, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (100.3400, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (146.1100, rect->getY2 (), 0.0001);
    }
}

///
/// @brief Tests the Find Previous button.
///
/// When a search starts it tried to find the next text in the document.
/// If the user presses the Find Previous button, it will try to find the
/// previous found text in the document, searching backwards.
///
void
FindPterTest::findPrevious ()
{   
    m_View->setTextToFind ("first");
    volatile gboolean stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    
    // The 'first' text appears twice in page 4 and also twice in
    // page 5. So the first time it will set a rect on page 4.
    CPPUNIT_ASSERT_EQUAL (4, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (82.0000, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (137.1100, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (100.3400, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (146.1100, rect->getY2 (), 0.0001);
    }
    
    m_Observer->notifyFindStarted ();
    m_FindPter->findPreviousActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (72.0000, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (143.1745, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (116.9349, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (162.3553, rect->getY2 (), 0.0001);
    }
   
    m_Observer->notifyFindStarted ();
    m_FindPter->findPreviousActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (164.1700, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (121.1020, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (180.2800, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (130.1020, rect->getY2 (), 0.0001);
    }

    m_Observer->notifyFindStarted ();
    m_FindPter->findPreviousActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (200.5943, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (84.5980, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (254.5158, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (107.6147, rect->getY2 (), 0.0001);
    }
  
    m_Observer->notifyFindStarted ();
    m_FindPter->findPreviousActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (4, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (96.0000, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (148.1100, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (114.3400, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (157.1100, rect->getY2 (), 0.0001);
    }

    m_Observer->notifyFindStarted ();
    m_FindPter->findNextActivated ();
    stillSearching = TRUE;
    while ( stillSearching ) {stillSearching = m_Observer->isStillSearching ();}
    CPPUNIT_ASSERT_EQUAL (5, m_Observer->getCurrentPage ());
    {
        DocumentRectangle *rect = m_Observer->getFindMatchRect ();
        CPPUNIT_ASSERT (NULL != rect);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (200.5943, rect->getX1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (84.5980, rect->getY1 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (254.5158, rect->getX2 (), 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL (107.6147, rect->getY2 (), 0.0001);
    }
}
