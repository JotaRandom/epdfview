// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - PDF Document Test Fixture.
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

#if !defined (__PDF_DOCUMENT_TEST_H__)
#define __PDF_DOCUMENT_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

namespace ePDFView
{
    class PDFDocumentTest: public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE (PDFDocumentTest);
        CPPUNIT_TEST (emptyDocument);
        CPPUNIT_TEST (fileNotFound);
        CPPUNIT_TEST (invalidFile);
        CPPUNIT_TEST (encryptedFile);
        CPPUNIT_TEST (validFile);
        CPPUNIT_TEST (relativePath);
        CPPUNIT_TEST (pageChange);
        CPPUNIT_TEST (pageRotate);
        CPPUNIT_TEST (pageZoom);
        CPPUNIT_TEST (pageRender);
        CPPUNIT_TEST (pageLinks);
        CPPUNIT_TEST (pageFindText);
        CPPUNIT_TEST_SUITE_END ();

        public:
            void setUp (void);
            void tearDown (void);

            void emptyDocument (void);
            void fileNotFound (void);
            void invalidFile (void);
            void encryptedFile (void);
            void validFile (void);
            void relativePath (void);
            void pageChange (void);
            void pageRotate (void);
            void pageZoom (void);
            void pageRender (void);
            void pageLinks (void);
            void pageFindText (void);
            
        private:
            PDFDocument *m_Document;
            DumbDocumentObserver *m_Observer;
    };
}

#endif // !__PDF_DOCUMENT_TEST_H__
