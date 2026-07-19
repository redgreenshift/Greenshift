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

#include "ThreadedEntity.h"


  /****************************************************************************
   *
   * ThreadedEntityThreadProc - thing that gets the thread going
   *
   ****************************************************************************/
DWORD WINAPI ThreadedEntityThreadProc(LPVOID lpVoid)
{
	if (lpVoid != NULL)
		return ((ThreadedEntity*)lpVoid)->BaseThreadProcedure();
	else
		return 1;
}

#if EXTREME_DEBUGGING
#include <ddraw.h>
#include "DXError.h"
#endif


/****************************************************************************
 *
 * BaseThreadProcedure -
 *
 ****************************************************************************/
int    ThreadedEntity::BaseThreadProcedure(void* pData)
{
	int iRet;

	iRet = ThreadProcedure(pData); /* the upcall to the derived class */

	EnterCS();
	m_bActive = false;  /* no longer active */
	m_bBusy = true;   /* no longer accepting work */
	LeaveCS();

#if EXTREME_DEBUGGING
	if (iRet != SUCCESS)
	{
		DumpToFile("error.txt", "Not sure which, but one of the following two errors occured:", "\n");
		DumpToFile("error.txt", "\tERROR: ", "");
		DumpToFile("error.txt", ErrorString(iRet), "\n");
		DumpToFile("error.txt", "\tERROR: ", "");
		DumpToFile("error.txt", DXError(iRet), "\n");
	}
#endif

	return iRet;
}



/****************************************************************************
 *
 * ThreadedEntity - abstract class for multithreading
 *
 ****************************************************************************/
ThreadedEntity::ThreadedEntity(const int iThreadPriority) :
	m_bInitialized(false),
	m_bActive(false),
	m_bBusy(false)
{
	m_hThread = NULL;

	InitializeCriticalSection(&m_CritSect);

	m_hThread = CreateThread(
		NULL,
		0,
		ThreadedEntityThreadProc,
		this,
		CREATE_SUSPENDED,
		&m_dwThreadID);

	SetThreadPriority(m_hThread, iThreadPriority);

	/*     DuplicateHandle( GetCurrentProcess(), GetCurrentThread(),
		GetCurrentProcess(), &m_hMainThread, DUPLICATE_SAME_ACCESS, false, 0 );*/
}



/****************************************************************************
 *
 * ~ThreadedEntity - abstract class for multithreading
 *
 ****************************************************************************/
ThreadedEntity::~ThreadedEntity()
{
	/*
	 * kill the thread if it's still running
	 */
	TerminateThread();
	DeleteCriticalSection(&m_CritSect);
}


/****************************************************************************
 *
 * TerminateThread - ensure thread is halted
 *
 ****************************************************************************/
void    ThreadedEntity::TerminateThread(void)
{
	if (m_hThread != NULL)
	{
		EnterCS();

		::TerminateThread(m_hThread, 0);

		CloseHandle(m_hThread);

		m_hThread = NULL;
		m_dwThreadID = 0;

		LeaveCS();
	}
}




/****************************************************************************
 *
 * ResumeAndWaitUntilThreadHasInitialized - the thread sets this when initialization is complete
 *
 * the thread should be in the critical section during initialization... I think
 *
 ****************************************************************************/
bool    ThreadedEntity::ResumeAndWaitUntilThreadHasInitialized(void)
{
	bool bInitialized = false;

	Resume();

	while (!bInitialized)
	{
		EnterCS();

		bInitialized = m_bInitialized;

		LeaveCS();
	}

	return bInitialized;
}

/****************************************************************************
 *
 * ThreadHasInitialized - the thread calls this when initialization is complete
 *
 ****************************************************************************/
void    ThreadedEntity::ThreadHasInitialized(const bool bSuccessfullyInitialized)
{
	EnterCS();

	m_bInitialized = true;
	m_bActive = bSuccessfullyInitialized;

	LeaveCS();
}



/****************************************************************************
 *
 * Resume - resume decrements the suspend counter,
 *          *only* resumes if counter is zero.
 *          If multiple Suspend's are issued, multiple Resume's are
 *          necessary to start the thread running again.
 *
 *          "If the return value is 0, the specified thread was not suspended.
 *           If the return value is 1, the specified thread was suspended but
 *           was restarted. If the return value is greater than 1, the
 *           specified thread is still suspended."
 *
 ****************************************************************************/
DWORD    ThreadedEntity::Resume(void)
{
	DWORD    dwRet = -1;  /* error return if thread doesn't exist */

	if (m_hThread != NULL)
	{
		EnterCS();
		m_bBusy = true; /* thread is busy once resumed - if the ResumeThread
						 * call does restart it, then m_bBusy should be true
						 * That's why it's necessary to set m_bBusy *before*
						 * calling ResumeThread.
						 */
		dwRet = ResumeThread(m_hThread);

		/*
		 * now if the thread is running, all is good,
		 * but if the thread isn't running,
		 * no harm done, just make m_bBusy false.
		 *
		 * The only "bad" thing that could have happened is if another thread
		 * asked if it was safe to give work to the thread.  The answer would
		 * be an incorrect "no, I'm busy", but that's not the end of the world.
		 * It's better than an incorrect "yes, I can accept work"... I think.
		 */
		if (dwRet > 1) /* if the thread was suspended multiple times, it's still suspended */
			m_bBusy = false; /* therefore not busy */
		else
			m_bBusy = true;
		LeaveCS();
	}

	return dwRet;
}



/****************************************************************************
 *
 * Suspend - suspend the thread, *always* suspends the thread
 *
 ****************************************************************************/
DWORD    ThreadedEntity::Suspend(void)
{
	DWORD    dwRet = -1;  /* error return if thread doesn't exist */

	if (m_hThread != NULL)
	{
		EnterCS();
		m_bBusy = false;
		LeaveCS();

		dwRet = SuspendThread(m_hThread);
	}

	return dwRet;
}


/****************************************************************************
 *
 * IsActive - returns the status of the thread, determines if it was returned
 *
 ****************************************************************************/
bool    ThreadedEntity::IsActive(void)
{
	bool bActive;

	EnterCS();
	bActive = m_bActive;
	LeaveCS();

	return bActive;
}


/****************************************************************************
 *
 * IsBusy - returns the status of the thread, determines if it is working on something
 *
 ****************************************************************************/
bool    ThreadedEntity::IsBusy(void)
{
	bool bBusy;

	EnterCS();
	bBusy = m_bBusy;
	LeaveCS();

	return bBusy;
}

