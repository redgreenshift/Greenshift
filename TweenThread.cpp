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
  * TweenThread
  *
  ****************************************************************************/

#include "TweenThread.h"


#if EXTREME_DEBUGGING
  //HighResolutionTimer hrTweenTimer;
#endif

/****************************************************************************
 *
 * TweenThread - constructor
 *
 ****************************************************************************/
TweenThread::TweenThread(const TWEENDESCRIPTION& tween_description) :
	ThreadedEntity(THREAD_PRIORITY_BELOW_NORMAL),
	m_dwFrameSize(tween_description.dwFrameSize),
	m_dwMaximumFrames(tween_description.dwMaximumFrames),
	m_dwDefaultFrames(tween_description.dwDefaultFrames),
	m_dwPlainFrameRepeatValue(tween_description.dwPlainFrameRepeatValue),
	m_dwTweenFrameRepeatValue(tween_description.dwTweenFrameRepeatValue)
{
	m_pObject1 = NULL;
	m_pObject2 = NULL;

	m_pUnalignedBuffer = NULL;

	m_state = TweenState::Serving_NoPendingWork;
	m_dwCurrentFrame = 0;
	m_dwPlainRepetition = 0;
	m_dwTweenRepetition = 0;

	m_dwNumPlainFrames = 0;
	m_dwNumTweenFrames = 0;
	m_dwNumPlainFramesThreadside = 0;
	m_dwNumTweenFramesThreadside = 0;

	m_pPlainFrames = NULL;
	m_pTweenFrames = NULL;
	m_pPlainFramesThreadside = NULL;
	m_pTweenFramesThreadside = NULL;

}


/****************************************************************************
 *
 * ~TweenThread - destructor
 *
 ****************************************************************************/
TweenThread::~TweenThread()
{
	SAFE_DELETE_ARRAY(m_pUnalignedBuffer);
}




/****************************************************************************
 *
 * Initialize - the ONE time the main thread is allowed to call PrerenderFrames
 *
 ****************************************************************************/
error_t    TweenThread::Initialize(void* pObject1)
{
	error_t err;
	void** pPageTable;
	DWORD   dwBytesToAlign = 16;
	DWORD   dwPageSize = m_dwFrameSize;
	DWORD   dwNumPages = 2 + 2 * m_dwMaximumFrames;
	/*    DWORD   dwNumPages     = 4 * m_dwMaximumFrames; /* one frameset for each of:
															PlainFrames
															TweenFrames
															PlainFramesThreadside
															TweenFramesThreadside
														 */

	//char    strTmp[256];

	/*
	* what if I just go ahead and allocate all the buffers? as one chunk?
	*/

	err = ContiguousAlignedMemory::Allocate(&m_pUnalignedBuffer,
		&pPageTable,
		dwNumPages,
		dwPageSize,
		dwBytesToAlign);

	//int ret = snprintf( strTmp, _countof(strTmp), "%d", m_dwFrameSize );
	//if (ret < 0 || (size_t)ret >= _countof(strTmp)) return FAILURE;
	//MessageBoxA( NULL, strTmp, "ack!", MB_OK );

	if (err != SUCCESS)
		return err;

	/*
		m_pPlainFrames           = &pPageTable[m_dwMaximumFrames*0];
		m_pTweenFrames           = &pPageTable[m_dwMaximumFrames*1];
		m_pTweenFramesThreadside = &pPageTable[m_dwMaximumFrames*2];
		m_pPlainFramesThreadside = &pPageTable[m_dwMaximumFrames*3];/**/
	m_pTweenFrames = &pPageTable[0];
	m_pPlainFrames = &pPageTable[m_dwMaximumFrames];
	m_pTweenFramesThreadside = &pPageTable[m_dwMaximumFrames + 1];
	m_pPlainFramesThreadside = &pPageTable[m_dwMaximumFrames * 2];

	//PrerenderFrames( 1, m_pPlainFrames, pObject1, NULL );
	ExperimentalPrerenderFrame(m_pPlainFrames[0], pObject1);

	return SUCCESS;
}


/****************************************************************************
 *
 * ScheduleTween - queues and shedules background work to precalculate
 * interpolated frames between Object1 and Object2.
 *
 ****************************************************************************/
error_t TweenThread::ScheduleTween(const DWORD dwFrames,
	void* pObject1,
	void* pObject2)
{
	error_t err = SUCCESS;
	/*
	 * add to delta queue
	 */
	if (IsBusy())
		err = ERR_THREADBUSY;
	else
	{
#if EXTREME_DEBUGGING
		//hrTweenTimer.Start();
#endif

		/*
		 * SOMETHING is deadlocking, so it's better safe than sorry
		 */
		EnterCS();
		if (m_state != TweenState::Serving_NoPendingWork)
			err = ERR_THREADBUSY;
		else
		{
			m_state = TweenState::Serving_AwaitingTweenData;  /* tween when ready */
			m_dwNumPlainFramesThreadside = 1;
			m_pObject2 = pObject2;

			m_dwNumTweenFramesThreadside = (dwFrames == 0)
				? m_dwDefaultFrames : dwFrames;
			m_pObject1 = pObject1;
		}
		LeaveCS();

		if (err == SUCCESS)
			Resume();  /* wake up the thread so it can do some work */
	}

	return err;
}





/****************************************************************************
 *
 * GetFrame - hands back Field frames.
 *
 ****************************************************************************/
void* TweenThread::GetFrame(void)
{
	TweenState state;

	EnterCS();
	state = m_state;
	LeaveCS();

	switch (state)
	{
	case TweenState::Serving_NoPendingWork:	// Just serving the source. No pending tasks in flight.

		// I think I was allowing future designing of Fields that change over time, in
		// addition to the pre-existing work to interpolate or "tween" between two Fields.
		// That's why I have m_dwCurrentFrame++ iterating across m_pPlainFrames.
		//
		// To make it even more complex, I also can repeat each frame multiple times,
		// so the transition takes longer.That part is m_dwPlainRepetition++

		// Keep handing out the frame(s) for Field1 (This is the "current" field).
		// There is no pending work for the Background thread to process. This is a
		// steady state, just handing out frames for Field1 over and over.

		if (m_dwPlainRepetition++ < m_dwPlainFrameRepeatValue
			&& m_dwCurrentFrame < m_dwNumPlainFrames)
			return m_pPlainFrames[m_dwCurrentFrame];
		else
			m_dwPlainRepetition = 1; /* might be off by one */

		if (m_dwCurrentFrame >= m_dwNumPlainFrames)
			m_dwCurrentFrame = 0;

		return m_pPlainFrames[m_dwCurrentFrame++];

	case TweenState::Serving_AwaitingTweenData:	// The waiting phase; we are still serving our current field while we wait to sync/receive the next delta field data!

		// This is the "holding pattern" where we finish serving the source field while
		// waiting for the background thread to finish calculating the next field, and
		// interpolation frames in between. Once worker completes or it's time to
		// transition, we swap and go to state 2 (Tween).

		// Another Field (Field2) has been queued for the background thread
		// to process (the BG thread generates/precalculates the frames between the two
		// Fields). Keep handing out frames for Field1 until the background
		// work is done. The first part of this m_state==1 handler is the
		// **same** as the m_state==0 handler. This is to ensure that any "time dependent"
		// DeltaFields finish rendering all the frames before starting to hand back
		// the "tween" frames between DeltaField1 and DeltaField2.
		//
		// This is a "holding pattern" state, Waiting for the background work to be done.
		// Once the work is done, we swap the buffers(the buffers from the background
		// where the "tween" frames were precalculated m_pPlainFramesThreadside /
		// m_pTweenFramesThreadside and the foreground buffers m_pTweenFrames /
		// m_pPlainFrames so that frames from the freshly calculated buffers will now
		// be handed out by GetFrame), and then transition to m_state = 2.

		// Why do we have state 0 and 1? To distinguish between nothing queued, and Awaiting

		if (m_dwPlainRepetition++ < m_dwPlainFrameRepeatValue
			&& m_dwCurrentFrame < m_dwNumPlainFrames)
			return m_pPlainFrames[m_dwCurrentFrame];
		else
			m_dwPlainRepetition = 1; /* might be off by one */


		if (m_dwCurrentFrame < m_dwNumPlainFrames)
			return m_pPlainFrames[m_dwCurrentFrame++];

		/*
		 * if the thread's busy, the tween frames aren't done
		 */
		if (IsBusy())
		{
			m_dwCurrentFrame = 0;  /* thread's not done */
			return m_pPlainFrames[m_dwCurrentFrame++];
		}
		else  /* thread's done calculating */
		{
#if EXTREME_DEBUGGING
			//float nSeconds;
			//hrTimer.Stop();

			//hrTimer.Time(&nSeconds);

			//DumpToFile( "tweenlog.txt", "Tween timer: ", hrTweenTimer.Seconds(), "\n" );
#endif
			/*
			 * need a safety mechanism that works both ways:
			 *   thread done... main thread received the data
			 */

			 /* swap the frame number variables */
			SwapDWORD(&m_dwNumPlainFrames, &m_dwNumPlainFramesThreadside);

			/* swap the pixel map variables */
			SwapPointers(&m_pPlainFrames, &m_pPlainFramesThreadside);

			/* swap the tween frame number variables */
			SwapDWORD(&m_dwNumTweenFrames, &m_dwNumTweenFramesThreadside);

			/* swap the tween pixel map variables */
			SwapPointers(&m_pTweenFrames, &m_pTweenFramesThreadside);
		}

		EnterCS();
		m_state = TweenState::Tweening;
		LeaveCS();
		/* FALLTHROUGH */
		__fallthrough;

	case TweenState::Tweening:	// The interpolation/tween motion itself between two fields.

		// We move between fields. Once finished, we return to a "new" steady-state serving of the current field (or reset the cycle).

		// now we use the tween frames, handing them out so the caller can gradually
		// transition between Field1 and Field2. Once all frames are handed out,
		// proceed back to the steady state m_state=0 and then we HOLD there,
		// since there are no more Fields queued for precalculation.

		if (m_dwTweenRepetition++ < m_dwTweenFrameRepeatValue
			&& m_dwCurrentFrame < m_dwNumTweenFrames)
			return m_pTweenFrames[m_dwCurrentFrame];
		else
			m_dwTweenRepetition = 1; /**/

		if (m_dwCurrentFrame < m_dwNumTweenFrames)
			return m_pTweenFrames[m_dwCurrentFrame++];

		EnterCS();
		m_state = TweenState::Serving_NoPendingWork;
		LeaveCS();
		m_dwCurrentFrame = 1; /*
							  * the fallthrough does frame 0,
							  * so let frame 1 go next
							  */
		/* FALLTHROUGH */
		__fallthrough;

	default:
		return m_pPlainFrames[0];
	}
}



/****************************************************************************
 *
 * Everything below is strictly THREAD side code.  The main thread should not
 * call any of these once the thread has been started.
 *
 ****************************************************************************/


 /****************************************************************************
  *
  * ThreadProcedure - entry point for threaded activity
  *
  ****************************************************************************/
int    TweenThread::ThreadProcedure(void* pData)
{
	enum class WorkType {
		None = 0,							// No Work To Do
		RenderOneFrame = 1,					// don't tween, just prerender a single frame
		RenderTwoAndInterpolateFrames = 2,	// prerender the final frame, and then tween the current and final frames
	} task = WorkType::None; // number of frames to calculate


	while (true)
	{
		EnterCS();
		if ( //IsBusy() /* why do I need this?  of COURSE it's busy */
			//&&
			m_pObject1 != NULL)
		{
			if (m_pObject2 != NULL)
				task = WorkType::RenderTwoAndInterpolateFrames;
			else
				task = WorkType::RenderOneFrame;

		}
		else
		{
			task = WorkType::None;
			//m_bTweenInProgress = false;
		}
		LeaveCS();


		/*
		 * the main thread will NEVER alter the variables unless ThreadIsBusy == false
		 *
		 * main thread sets busy = true
		 * work thread sets busy = false
		 *
		 * thread does not do memory allocation or freeing,
		 * main handles everything
		 */

		switch (task)
		{
		default:
		case WorkType::None:
			//DumpToFile("error.txt", "going to sleep", "\n");
			Suspend(); /* wait for work to show up */
			//DumpToFile("error.txt", "woke up", "\n");
			continue; /* go back to the beginning of the while loop */

		case WorkType::RenderOneFrame:
			/*
			 * calculate the final frame
			 */
			PrerenderFrames(m_dwNumPlainFramesThreadside,
				m_pPlainFramesThreadside,
				m_pObject1,
				NULL);
			break;

		case WorkType::RenderTwoAndInterpolateFrames:
			/*
			 * calculate the tween frames
			 */
			/*PrerenderFrames( m_dwNumTweenFramesThreadside,
							m_pTweenFramesThreadside,
							m_pObject1,
							m_pObject2 );
			PrerenderFrames( m_dwNumPlainFramesThreadside,
							m_pPlainFramesThreadside,
							m_pObject2,
							NULL );
			/*/
			//PrerenderFrames( m_dwNumPlainFramesThreadside,
			//					m_pPlainFramesThreadside,
			//					m_pObject2,
			//					NULL );
			ExperimentalPrerenderFrame(
				m_pPlainFramesThreadside[0],
				m_pObject2);
			ExperimentalPrerenderFrames(m_dwNumTweenFramesThreadside,
				m_pTweenFramesThreadside,
				m_pPlainFrames[0],
				m_pPlainFramesThreadside[0]);/**/
			break;

		} /* switch */


		/*
		 * BREAK jumps here
		 */

		EnterCS();
		m_pObject1 = NULL;
		m_pObject2 = NULL;
		LeaveCS();
	} /* while */

	return 0;
}

