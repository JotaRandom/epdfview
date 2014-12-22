// ePDFView - Document Outline Test Fixture.
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
#include "DocumentOutlineTest.h"

using namespace ePDFView;

// Register the test suite into the `registry'.
CPPUNIT_TEST_SUITE_REGISTRATION (DocumentOutlineTest);

///
/// @brief Sets up the environment for each test.
///
void
DocumentOutlineTest::setUp ()
{
    m_Document = new PDFDocument ();
}

///
/// @brief Cleans up after each test.
///
void
DocumentOutlineTest::tearDown ()
{
    delete m_Document;
}

///
/// @brief Check the initial document's outline.
///
/// Initially, the document doesn't had outline as it hasn't been loaded.
///
void
DocumentOutlineTest::initialStatus ()
{
    CPPUNIT_ASSERT ( !m_Document->isLoaded ());
    CPPUNIT_ASSERT ( NULL == m_Document->getOutline ());
}

///
/// @brief Test a file without outline.
///
/// When a loaded document doesn't have an outline, the DocumentOutline's
/// is empty.
///
void
DocumentOutlineTest::noOutline ()
{
    gchar *testFile = getTestFile ("test2.pdf");
    CPPUNIT_ASSERT (m_Document->loadFile (testFile, NULL, NULL));
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    DocumentOutline *outline = m_Document->getOutline ();
    CPPUNIT_ASSERT ( NULL != outline );
    CPPUNIT_ASSERT_EQUAL (0, outline->getNumChildren ());
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("", outline->getTitle ()));
    CPPUNIT_ASSERT_EQUAL (1, outline->getDestinationPage ());
    CPPUNIT_ASSERT (NULL == outline->getFirstChild ());
    CPPUNIT_ASSERT (NULL == outline->getNextChild ());

    g_free (testFile);
}

///
/// @brief Test a file that has outline.
///
/// The document I've gone test has the following outline:
///
///  *
///  |
///  +- Test PDF 1 / page 3
///  |
///  +- Table of Contents / page 4
///  |
///  +- Chapter 1. First Chapter / page 5
///     |
///     + First Section / page 5
///
void
DocumentOutlineTest::hasOutline ()
{
    gchar *testFile = getTestFile ("test1.pdf");
    CPPUNIT_ASSERT (m_Document->loadFile (testFile, NULL, NULL));
    CPPUNIT_ASSERT (m_Document->isLoaded ());
    DocumentOutline *outline = m_Document->getOutline ();
    CPPUNIT_ASSERT ( NULL != outline );
    // The root outline must be the same empty one, except it has children.
    CPPUNIT_ASSERT_EQUAL (3, outline->getNumChildren ());
    CPPUNIT_ASSERT (0 == g_ascii_strcasecmp ("", outline->getTitle ()));
    CPPUNIT_ASSERT_EQUAL (1, outline->getDestinationPage ());

    {
        DocumentOutline *child = outline->getFirstChild();
        CPPUNIT_ASSERT ( NULL != child );
        CPPUNIT_ASSERT_EQUAL (0, child->getNumChildren ());
        CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp ("Test PDF 1", 
                                                     child->getTitle ()));
        CPPUNIT_ASSERT_EQUAL (3, child->getDestinationPage ());
    }
    
    {
        DocumentOutline *child = outline->getNextChild();
        CPPUNIT_ASSERT ( NULL != child );
        CPPUNIT_ASSERT_EQUAL (0, child->getNumChildren ());
        CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp ("Table of Contents", 
                                                     child->getTitle ()));
        CPPUNIT_ASSERT_EQUAL (4, child->getDestinationPage ());
    }

    {
        DocumentOutline *child = outline->getNextChild();
        CPPUNIT_ASSERT ( NULL != child );
        CPPUNIT_ASSERT_EQUAL (1, child->getNumChildren ());
        CPPUNIT_ASSERT_EQUAL (0,
                              g_ascii_strcasecmp ("Chapter 1. First Chapter",
                                                  child->getTitle ()));
        CPPUNIT_ASSERT_EQUAL (5, child->getDestinationPage ());

        DocumentOutline *greatChild = child->getFirstChild ();
        CPPUNIT_ASSERT ( NULL != greatChild );
        CPPUNIT_ASSERT_EQUAL (0, greatChild->getNumChildren ());
        CPPUNIT_ASSERT_EQUAL (0, g_ascii_strcasecmp ("First Section",
                                                     greatChild->getTitle ()));
        CPPUNIT_ASSERT_EQUAL (5, greatChild->getDestinationPage ());
    }

    // After the last child, it should return NULL.
    CPPUNIT_ASSERT (NULL == outline->getNextChild ());

    // Try again the "no outlined" file.
    noOutline ();

    g_free (testFile);
}
