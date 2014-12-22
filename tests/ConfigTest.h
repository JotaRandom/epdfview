// ePDFView - Configuration Test Fixture.
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

#if !defined (__CONFIG_TEST_H__)
#define __CONFIG_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

namespace ePDFView
{
    class ConfigTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE (ConfigTest);
        CPPUNIT_TEST (defaultValues);
        CPPUNIT_TEST (windowValues);
        CPPUNIT_TEST (showToolbar);
        CPPUNIT_TEST (showStatusbar);
        CPPUNIT_TEST (openCurrentFolder);
        CPPUNIT_TEST (saveCurrentFolder);
        CPPUNIT_TEST (zoomValues);
        CPPUNIT_TEST (externalBrowser);
        CPPUNIT_TEST_SUITE_END ();

        public:
            void setUp (void);
            void tearDown (void);

            void defaultValues (void);
            void windowValues (void);
            void showToolbar (void);
            void showStatusbar (void);
            void openCurrentFolder (void);
            void saveCurrentFolder (void);
            void zoomValues (void);
            void externalBrowser (void);
    };
}

#endif // __CONFIG_TEST_H__
