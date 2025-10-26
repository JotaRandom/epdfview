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

#if !defined (__IJOB_H__)
#define __IJOB_H__

#if defined DEBUG
#define JOB_NOTIFIER(callback, data) callback (data)
#define JOB_DELETE TRUE
#define JOB_NOTIFIER_END()
#else // !DEBUG
#define JOB_NOTIFIER(callback, data) g_idle_add (callback, data)
#define JOB_DELETE FALSE
#define JOB_NOTIFIER_END() delete job
#endif // !DEBUG

namespace ePDFView
{
    /// @class IJob
    /// @brief Interface for jobs.
    ///
    /// A Job is simply a process that will be processed in background
    /// by the dispatch() function.
    ///
    class IJob
    {
        public:
            /// @brief Destroys all dynamically allocated memory for IJob.
            virtual ~IJob (void) { }

            static void clearQueue (void);
            static gpointer dispatcher (gpointer data); 
            static void init (void);
            static void enqueue (IJob *job);
            
            ///
            /// @brief Runs the job.
            ///
            /// This is called by the dispatcher() function when
            /// the job must start its work. It's the job's entry point.
            ///
            /// @return TRUE if the job must be deleted after the
            ///         call to run(). FALSE if the job will be
            ///         deleted by himself.
            ///
            virtual gboolean run (void) = 0;
            
        protected:
            static GAsyncQueue *m_JobsQueue;

            /// @brief Creates a new IJob object.
            IJob () { }
    };
}

#endif // __IJOB_H__
