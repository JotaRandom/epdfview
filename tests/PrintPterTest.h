// ePDFView - Preferences Presenter Test Fixture.
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

#if !defined (__PRINT_PTER_TEST_H__)
#define __PRINT_PTER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

namespace ePDFView
{
    class PrintPterTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE (PrintPterTest);
        CPPUNIT_TEST (initialSensitivity);
        CPPUNIT_TEST (collateSensitivity);
        CPPUNIT_TEST (pageRangeSensitivity);
        CPPUNIT_TEST_SUITE_END ();

        public:
            void setUp (void);
            void tearDown (void);

            void initialSensitivity (void);
            void collateSensitivity (void);
            void pageRangeSensitivity (void);

        protected:
            DumbDocument *m_Document;
            PrintPter *m_PrintPter;
            DumbPrintView *m_View;
    };
}

#endif // !__PRINT_PTER_TEST_H__
