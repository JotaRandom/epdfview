// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Test Suite.
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

#include <stdlib.h>
#include <glib-object.h>
#include <IJob.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

using namespace CPPUNIT_NS;

int
main (int argc, char **argv)
{
    // We must initialize glib't types.
    g_type_init ();

    // Initialize the job's queue.
    ePDFView::IJob::init ();
    
    // Get the top level suite from the registry.
    Test *suite = TestFactoryRegistry::getRegistry ().makeTest ();

    // Add the test suite to the list of tests to run.
    TextUi::TestRunner runner;
    runner.addTest (suite);

    // Change the default outputter to a compiler error format outputter.
    runner.setOutputter (new CompilerOutputter (&runner.result (), std::cerr));

    // Run the tests.
    bool wasSuccessful = runner.run ();

    return wasSuccessful ? EXIT_SUCCESS : EXIT_FAILURE;
}
