#pragma once
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
  * TweenThread - abstract base class for tweening objects
  *
  ****************************************************************************/

#include "ThreadedEntity.h"


typedef struct tagTWEENDESCRIPTION
{
	dword_t    dwFrameSize;        /* number of bytes per frame */
	dword_t    dwMaximumFrames;    /* maximum number of frames */
	dword_t    dwDefaultFrames;    /* default number of frames */
	dword_t    dwTweenFrameRepeatValue;    /* number of times to repeat each tween frame */
	dword_t    dwPlainFrameRepeatValue;    /* number of times to repeat each normal frame */
	void* pData;                /* extra data used by derived classes */
} TWEENDESCRIPTION;

/****************************************************************************
 *
 * PaletteTween
 *
 * answers a logical palette
 *
 ****************************************************************************/

/**
* @brief A TweenThread is an abstract class that inherits from ThreadedEntity,
* designed to precalculate interpolation frames between two data states
* (represented by m_pObject1 and m_pObject2). It manages a background thread
* that transitions between "plain" frames of the current state and interpolated
* "tween" frames, providing a seamless visual transition when requested via
* ScheduleTween.
*/
class TweenThread : public ThreadedEntity
{
public:
	TweenThread(const TWEENDESCRIPTION& tween_description);
	virtual         ~TweenThread() override;

	virtual int     ThreadProcedure(void* pData) override;


	error_t         Initialize(void* pObject1);

	error_t         ScheduleTween(const dword_t dwFrames,
		void* pObject1,
		void* pObject2);

	void* GetFrame(void);



protected:
	/*
	 * the function that does all the work,
	 * derived classes only need to define this
	 */
	virtual void    PrerenderFrames(const dword_t dwFrames,
		void** pFrames,
		void* pObject1,
		void* pObject2) = 0;
	virtual void    ExperimentalPrerenderFrames(const dword_t dwFrames,
		void** pFrames,
		void* pFirstFrame,
		void* pLastFrame) = 0;
	virtual void    ExperimentalPrerenderFrame(
		void* pFrame,
		void* pObject1) = 0;


	inline void     SwapDWORD(dword_t* pDWORD1, dword_t* pDWORD2)
	{
		dword_t   dwTemp;

		dwTemp = *pDWORD1;
		*pDWORD1 = *pDWORD2;
		*pDWORD2 = dwTemp;
	};

	inline void     SwapPointers(void*** pPointer1, void*** pPointer2)
	{
		void** pTemp;

		pTemp = *pPointer1;
		*pPointer1 = *pPointer2;
		*pPointer2 = pTemp;
	};

private:
	// What is the difference between m_dwMaximumFrames and m_dwDefaultFrames?
	// m_dwMaximumFrames is used when allocating the frames.
	// TweenThread::ScheduleTween(const DWORD dwFrames, void* pObject1, void* pObject2)
	//		-- specifies the number of frames. If dwFrames==0, we use m_dwDefaultFrames
	//		-- THIS is the **ONLY** usage of m_dwDefaultFrames.
	//		-- The dwFrames argument is **ALWAYS** 0 right now.
	// Typically, m_dwMaximumFrames and m_dwDefaultFrames will be the same.
	// This allows for the possibility to have a variable number of frames.
	const dword_t     m_dwMaximumFrames;
	const dword_t     m_dwDefaultFrames;
	const dword_t     m_dwTweenFrameRepeatValue;
	const dword_t     m_dwPlainFrameRepeatValue;
	const dword_t     m_dwFrameSize;

	/*
	 * 0 = loop on delta 1, ready to schedule tween
	 * 1 = loop on delta 1, tween when ready (also swap d1 and d2)
	 * 2 = tween once then return to state 0
	 *
	 */
	enum class TweenState
	{
		Serving_NoPendingWork = 0,		// Steady state for the current field (ready to schedule tween)
		Serving_AwaitingTweenData = 1,	// Still serving field 1 (The "holding pattern" serving F1 while waiting for F2 data)
		Tweening = 2,					// Using the new interpolation data (tween once then return to state 0)
	} m_state = TweenState::Serving_NoPendingWork;

	dword_t           m_dwCurrentFrame;
	dword_t           m_dwPlainRepetition;
	dword_t           m_dwTweenRepetition;

	dword_t           m_dwNumPlainFrames;
	dword_t           m_dwNumTweenFrames;
	dword_t           m_dwNumTweenFramesThreadside;
	dword_t           m_dwNumPlainFramesThreadside;

	void** m_pPlainFrames;
	void** m_pTweenFrames;
	void** m_pTweenFramesThreadside;
	void** m_pPlainFramesThreadside;

	void* m_pObject1;    /* this is the object to tween from */
	void* m_pObject2;    /* this is the object to tween to */


	void* m_pUnalignedBuffer;    /* the pointer to the allocated chunk of memory */
};

