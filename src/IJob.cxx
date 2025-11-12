// ePDFView - A lightweight PDF Viewer.
// Copyright (C) 2006-2011 Emma's Software.
// Copyright (C) 2014-2025 Pablo Lezaeta
// Copyright (C) 2014 Pedro A. Aranda GutiÃ©rrez

// ePDFView - A lightweight PDF Viewer.
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

#include "epdfview.h"
#include <unistd.h>
#include <stdlib.h>

using namespace ePDFView;

/// The queue of jobs to run in background.
GAsyncQueue *IJob::m_JobsQueue = NULL;

///
/// @brief Clears the list of jobs.
///
/// This is mainly used for the test suites when they are going
/// to delete the document class and don't want to generate segmentation
/// faults. It just pops all queued jobs without run them.
///
void
IJob::clearQueue (void)
{
    IJob *job = NULL;
    do
    {
        job = (IJob *)g_async_queue_try_pop (m_JobsQueue);
    }
    while ( NULL != job );
}

///
/// @brief The job dispatcher.
///
/// This function is the one that will run in a thread. What it does is
/// just pop queued jobs from the queue and then runs them.
///
/// @param data The asynchronous queue where the jobs will be enqueued.
///
gpointer
IJob::dispatcher (gpointer data)
{
    while (true)
    {
        IJob *job = (IJob *)g_async_queue_pop (m_JobsQueue);
        if ( job->run () )
        {
            delete job;
        }
    }
#ifdef _WIN32
    _sleep(0);
#else
    sleep(0);
#endif
}

///
/// @brief Initialises the job dispatcher.
///
/// This function must be the first called before any other jobs-related
/// function, as it does initialised the thread subsystem, created the
/// threaded dispatcher() function and initialised the job queue.
///
void
IJob::init ()
{
    // GLib threads are always available in modern versions (>= 2.32)
    // No need to call g_thread_init() or check g_thread_supported()
    m_JobsQueue = g_async_queue_new ();
    GError *error = NULL;
    if ( NULL == g_thread_create (IJob::dispatcher, NULL, FALSE, &error) )
    {
        g_error ("Couldn't create the dispatcher thread: %s\n", error->message);
    }
}

///
/// @brief Adds a new job to the queue.
///
/// It adds a new job to the queued jobs that will be dispatched by
/// the dispatch() function.
///
/// @param job The job to add to the queue.
///
void
IJob::enqueue (IJob *job)
{
    g_assert ( NULL != job && "Tried to queue a NULL job.");

    g_async_queue_push (m_JobsQueue, (gpointer)job);
}
