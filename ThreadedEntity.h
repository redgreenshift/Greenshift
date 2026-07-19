#include "Project Greenshift.h"
/*
 *  Copyright (C) 2001-2026 Jared Ivey
 *
 *  This file is part of Project Greenshift
 *
 *  OSI Certified Open Source Software
 *
 *  Project Greenshift is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; version 2 only.
 *
 *  Project Greenshift is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

 /****************************************************************************
  *
  * ThreadedEntity - abstract class for multithreading
  *
  ****************************************************************************/

#ifndef _ThreadedEntity_H_
#define _ThreadedEntity_H_

#ifndef STRICT
#define STRICT 1
#endif
#include <windows.h>

  /*
  THREAD_PRIORITY_LOWEST
  THREAD_PRIORITY_BELOW_NORMAL
  THREAD_PRIORITY_NORMAL
  THREAD_PRIORITY_ABOVE_NORMAL
  THREAD_PRIORITY_HIGHEST
  */

class ThreadedEntity
{
public:
	ThreadedEntity(const int iThreadPriority =
		THREAD_PRIORITY_NORMAL);
	virtual             ~ThreadedEntity();

	virtual int         ThreadProcedure(void* pData) = 0;

	int                 BaseThreadProcedure(void* pData = NULL);

	bool                IsActive(void);
	bool                IsBusy(void);

	void                TerminateThread(void);

protected:
	inline void         EnterCS(void) { EnterCriticalSection(&m_CritSect); };
	inline void         LeaveCS(void) { LeaveCriticalSection(&m_CritSect); };
	inline void         SetActive(const bool bActive)
	{
		EnterCS();     m_bActive = bActive;    LeaveCS();
	};

	DWORD               Resume(void);
	DWORD               Suspend(void);

	bool                ResumeAndWaitUntilThreadHasInitialized(void);
	void        ThreadHasInitialized(const bool bSuccessfullyInitialized);

private:
	bool                m_bInitialized; /* true after thread has initialized */
	bool                m_bActive;      /* true after thread has initialized
										 * false after returned         */
	bool                m_bBusy;        /* true while thread is running
										 * false while suspended        */
	CRITICAL_SECTION    m_CritSect;     /* the thread safety mechanism  */
	HANDLE              m_hThread;      /* handle for the thread        */
	DWORD               m_dwThreadID;   /* the ID for the thread        */
};


#endif  /* _ThreadedEntity_H_ */

