// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Preferences Presenter Test Fixture.
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
#include "DumbPrintView.h"
#include "PrintPterTest.h"

using namespace ePDFView;

// Register the test suite into the `registry'.
CPPUNIT_TEST_SUITE_REGISTRATION (PrintPterTest);

///
/// @brief Sets up the environment for each test.
///
void
PrintPterTest::setUp ()
{
    Config::loadFile (FALSE);
    m_Document = new DumbDocument ();
    m_PrintPter = new PrintPter (m_Document);
    m_View = new DumbPrintView ();
    m_PrintPter->setView (m_View);
}

///
/// @brief Cleans up after each test.
///
void
PrintPterTest::tearDown ()
{
    Config::destroy ();
    // Telling the presenter to cancel will delete the view and the
    // presenter.
    if ( NULL != m_PrintPter )
    {
        m_PrintPter->cancelActivated ();
        m_PrintPter = NULL;
        m_View = NULL;
    }
    delete m_Document;
}

///
/// @brief Check the initial's dialog sensitivity.
///
void
PrintPterTest::initialSensitivity ()
{
    // By default "All Pages" range is selected, so the
    // custom page ranges is insensitive.
    CPPUNIT_ASSERT ( !m_View->isSensitivePageRange ());
    // By default the number of copies is 1, so the collate
    // option is insensitive.
    CPPUNIT_ASSERT ( !m_View->isSensitiveCollate ());
}

///
/// @brief Check "Collate"'s sensitivity.
///
/// The collate option is always sensitive unless the number
/// of copies is 1.
///
void
PrintPterTest::collateSensitivity ()
{
    m_View->setNumberOfCopies (1);
    m_PrintPter->numberOfCopiesChanged ();
    CPPUNIT_ASSERT ( !m_View->isSensitiveCollate ());

    m_View->setNumberOfCopies (10);
    m_PrintPter->numberOfCopiesChanged ();
    CPPUNIT_ASSERT ( m_View->isSensitiveCollate ());

    m_View->setNumberOfCopies (234);
    m_PrintPter->numberOfCopiesChanged ();
    CPPUNIT_ASSERT ( m_View->isSensitiveCollate ());

    m_View->setNumberOfCopies (1);
    m_PrintPter->numberOfCopiesChanged ();
    CPPUNIT_ASSERT ( !m_View->isSensitiveCollate ());

    m_View->setNumberOfCopies (32);
    m_PrintPter->numberOfCopiesChanged ();
    CPPUNIT_ASSERT ( m_View->isSensitiveCollate ());
}

///
/// @brief Check "Page Range"'s sensitivity.
///
/// The page range entry is only sensitive when the "Range:" radio
/// button is selected.
///
void
PrintPterTest::pageRangeSensitivity ()
{
    m_View->selectAllPagesRangeOption ();
    m_PrintPter->pageRangeOptionChanged ();
    CPPUNIT_ASSERT ( !m_View->isSensitivePageRange ());

    m_View->selectCustomPagesRangeOption ();
    m_PrintPter->pageRangeOptionChanged ();
    CPPUNIT_ASSERT ( m_View->isSensitivePageRange ());

    m_View->selectAllPagesRangeOption ();
    m_PrintPter->pageRangeOptionChanged ();
    CPPUNIT_ASSERT ( !m_View->isSensitivePageRange ());

    m_View->selectCustomPagesRangeOption ();
    m_PrintPter->pageRangeOptionChanged ();
    CPPUNIT_ASSERT ( m_View->isSensitivePageRange ());
}
