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

#if !defined (__MAIN_PTER_TEST_H__)
#define __MAIN_PTER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

namespace ePDFView
{
    class MainPterTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE (MainPterTest);
        CPPUNIT_TEST (initialStatus);
        CPPUNIT_TEST (loadDocument);
        CPPUNIT_TEST (loadCanceled);
        CPPUNIT_TEST (loadFailed);
        CPPUNIT_TEST (canceledPassword);
        CPPUNIT_TEST (badPassword);
        CPPUNIT_TEST (goodPassword);
        CPPUNIT_TEST (openLastFolder);
        CPPUNIT_TEST (saveDocument);
        CPPUNIT_TEST (saveCanceled);
        CPPUNIT_TEST (saveLastFolder);
        CPPUNIT_TEST (pageNavigation);
        CPPUNIT_TEST (pageNavigationEntry);
        CPPUNIT_TEST (pageRotate);
        CPPUNIT_TEST (pageZoomInAndOut);
        CPPUNIT_TEST (pageZoomWidth);
        CPPUNIT_TEST (pageZoomFit);
        CPPUNIT_TEST (pageZoomAndRotate);
        CPPUNIT_TEST (pageZoomIndicator);
        CPPUNIT_TEST (reloadNormal);
        CPPUNIT_TEST (reloadEncrypted);
        CPPUNIT_TEST (reloadChangedPassword);
        CPPUNIT_TEST (showIndex);
        CPPUNIT_TEST (showToolAndStatusBars);
        CPPUNIT_TEST_SUITE_END();

        public:
            void setUp (void);
            void tearDown (void);

            void initialStatus (void);
            void loadDocument (void);
            void loadCanceled (void);
            void loadFailed (void);
            void canceledPassword (void);
            void badPassword (void);
            void goodPassword (void);
            void openLastFolder (void);
            void saveDocument (void);
            void saveCanceled (void);
            void saveLastFolder (void);
            void pageNavigation (void);
            void pageNavigationEntry (void);
            void pageRotate (void);
            void pageZoomInAndOut (void);
            void pageZoomWidth (void);
            void pageZoomFit (void);
            void pageZoomAndRotate (void);
            void pageZoomIndicator (void);
            void reloadNormal (void);
            void reloadEncrypted (void);
            void reloadChangedPassword (void);
            void showIndex (void);
            void showToolAndStatusBars (void);

        private:
            DumbMainView *m_View;
            DumbDocument *m_Document;
            MainPter *m_MainPter;
    };
}

#endif // !__MAIN_PTER_TEST_H__
