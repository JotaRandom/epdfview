// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Page Presenter Test Fixture.
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

#if !defined (__PAGE_PTER_TEST_H__)
#define __PAGE_PTER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

namespace ePDFView
{
    class PagePterTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE (PagePterTest);
        CPPUNIT_TEST (pageZoomWidth);
        CPPUNIT_TEST (pageZoomFit);
        CPPUNIT_TEST (pageDrag);
        CPPUNIT_TEST_SUITE_END ();

        public:
            void setUp (void);
            void tearDown (void);

            void pageZoomWidth (void);
            void pageZoomFit (void);
            void pageDrag (void);

        protected:
            DumbDocument *m_Document;
            MainPter *m_MainPter;
            DumbMainView *m_MainView;
            PagePter *m_PagePter;
            DumbPageView *m_PageView;
    };
}

#endif // !__PAGE_PTER_TEST_H__
