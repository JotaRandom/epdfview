// ePDFView - Page Presenter Test Fixture.
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
#include "DumbDocument.h"
#include "DumbMainView.h"
#include "DumbPageView.h"
#include "PagePterTest.h"

using namespace ePDFView;

// Register the test suite into the `registry'.
CPPUNIT_TEST_SUITE_REGISTRATION (PagePterTest);

///
/// @brief Sets up the environment for each test.
///
void
PagePterTest::setUp ()
{
    Config::loadFile (FALSE);

    m_Document = new DumbDocument ();
    m_MainPter = new MainPter (m_Document);
    m_MainView = new DumbMainView (m_MainPter);
    // We must set the main view to the presenter or it will try to
    // use it anyway.
    m_MainPter->setView (m_MainView);
    m_PagePter = new PagePter (m_Document);
    m_PagePter->setView (*m_MainView);
    m_PageView = (DumbPageView *)m_MainView->getPageView ();

    m_Document->loadFile ("/tmp/test1.pdf", NULL, NULL);
}

///
/// @brief Cleans up after each test.
///
void
PagePterTest::tearDown ()
{
    // We need to delete this page presenter, because we created it ourselves
    // and the main presenter doesn't know a thing about it. The main
    // presenter will destroy its own page presenter though.
    delete m_PagePter;
    delete m_MainPter;
    
    Config::destroy ();
}

///
/// @brief Tests page's zoom to width
///
void
PagePterTest::pageZoomWidth ()
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, m_Document->getZoom (), 0.0001);

    // OK, now try to zoom width. Since rotation is 0
    // the zoom level should be 75/100 = 0.75
    gint width;
    gint height;
    m_PageView->getSize (&width, &height);
    m_Document->zoomToWidth (width);
    Config::getConfig ().setZoomToWidth (TRUE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.75, m_Document->getZoom (), 0.0001);

    // Now change the view size. We do this calling the page presenter's
    // function instead of the view, because the view here don't have anything
    // to do.
    // The new zoom should be 50/100 = 0.5
    m_PagePter->viewResized (50, 50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.5, m_Document->getZoom (), 0.0001);
    
    m_PagePter->viewResized (75, 50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.75, m_Document->getZoom (), 0.0001);

    // If we desactive the zoom to width, no resizing changes the
    // zoom.
    Config::getConfig ().setZoomToWidth (FALSE);
    m_PagePter->viewResized (50, 50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.75, m_Document->getZoom (), 0.0001);
    m_PagePter->viewResized (100, 50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.75, m_Document->getZoom (), 0.0001);

    // Now rotate and try again.
    m_Document->rotateRight (); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ());    
    // Since rotation is now 90 the zoom level should be 75/250 = 0.3
    m_Document->zoomToWidth (width);
    Config::getConfig ().setZoomToWidth (TRUE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
    // Resizing. 50/250 = 0.2
    m_PagePter->viewResized (50, 50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.2, m_Document->getZoom (), 0.0001);
}

///
/// @brief Tests page's zoom to fit
///
void
PagePterTest::pageZoomFit ()
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, m_Document->getZoom (), 0.0001);

    // OK, now try to zoom to fit. Since rotation is 0
    // the zoom level should be 50/250 = 0.2
    gint width;
    gint height;
    m_PageView->getSize (&width, &height);
    m_Document->zoomToFit (width, height);
    Config::getConfig ().setZoomToFit (TRUE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.2, m_Document->getZoom (), 0.0001);

    // Resizing the view size. 100/250 = 0.4
    m_PagePter->viewResized (100, 100);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.4, m_Document->getZoom (), 0.0001);
    m_PagePter->viewResized (100, 50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.2, m_Document->getZoom (), 0.0001);

    // Desactivating the zoom to fit.
    Config::getConfig ().setZoomToFit (FALSE);
    m_PagePter->viewResized (100, 100);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.2, m_Document->getZoom (), 0.0001);
    m_PagePter->viewResized (100, 50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.2, m_Document->getZoom (), 0.0001);

    // Now rotate and try again.
    m_Document->rotateRight (); 
    CPPUNIT_ASSERT_EQUAL (90, m_Document->getRotation ());
    
    // Since rotation is now 90 the zoom level should be 75/250 = 0.3
    m_Document->zoomToFit (width, height);
    Config::getConfig ().setZoomToFit (TRUE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.3, m_Document->getZoom (), 0.0001);
    // 100/250 = 0.4
    m_PagePter->viewResized (100, 100);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.4, m_Document->getZoom (), 0.0001);
}

///
/// @brief Test dragging the page with the left mouse button.
///
void
PagePterTest::pageDrag ()
{
    // Moving the mouse without pressing the first mouse button, doesn't
    // change the current scrolling.
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, m_PageView->getHorizontalScroll (),
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, m_PageView->getVerticalScroll (),
                                  0.0001);
    m_PagePter->mouseMoved (10, 10);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, m_PageView->getHorizontalScroll (),
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, m_PageView->getVerticalScroll (),
                                  0.0001);
    m_PagePter->mouseMoved (15, 20);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, m_PageView->getHorizontalScroll (),
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, m_PageView->getVerticalScroll (),
                                  0.0001);

    // Now press the first mouse button and move a little the pointer.
    // The presenter should tell the view the difference between the position
    // where the drag started and the current position when the mouse moves.
    m_PagePter->mouseButtonPressed (1, 15, 20);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, m_PageView->getHorizontalScroll (),
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, m_PageView->getVerticalScroll (),
                                  0.0001);
    m_PagePter->mouseMoved (20, 21);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (5.0, m_PageView->getHorizontalScroll (),
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, m_PageView->getVerticalScroll (),
                                  0.0001);
    m_PagePter->mouseMoved (16, 26);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, m_PageView->getHorizontalScroll (),
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (6.0, m_PageView->getVerticalScroll (),
                                  0.0001);
    m_PagePter->mouseMoved (10, 13);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (-5.0, m_PageView->getHorizontalScroll (),
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (-7.0, m_PageView->getVerticalScroll (),
                                  0.0001);

    // Releasing the first mouse button stops the drag and doesn't change
    // the scroll.
    m_PagePter->mouseButtonReleased (1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (-5.0, m_PageView->getHorizontalScroll (), 
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (-7.0, m_PageView->getVerticalScroll (),
                                  0.0001);
    m_PagePter->mouseMoved (20, 21);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (-5.0, m_PageView->getHorizontalScroll (),
                                  0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (-7.0, m_PageView->getVerticalScroll (),
                                  0.0001);
}
