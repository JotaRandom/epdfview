// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - Test Util Functions.
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

#include <glib.h>
#include "Utils.h"

///
/// @brief Returns the path to the test data file.
///
/// It assumes that TEST_DIR is defined to the relative
/// tests directory and that the checks are executed from the
/// test build dir.
///
gchar *
getTestFile (const gchar *fileName)
{
    gchar *currentDir = g_get_current_dir ();
    gchar *filePath = g_build_filename (currentDir, TEST_DIR, fileName, NULL);
    g_free (currentDir);

    return filePath;
}
