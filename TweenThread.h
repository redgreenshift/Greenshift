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

#ifndef _TweenThread_H_
#define _TweenThread_H_

#include "ThreadedEntity.h"
#include "ContiguousAlignedMemoryAllocator.h"
//#include "HighResolutionTimer.h"


typedef struct tagTWEENDESCRIPTION
{
    DWORD    dwFrameSize;        /* number of bytes per frame */
    DWORD    dwMaximumFrames;    /* maximum number of frames */
    DWORD    dwDefaultFrames;    /* default number of frames */
    DWORD    dwTweenFrameRepeatValue;    /* number of times to repeat each tween frame */
    DWORD    dwPlainFrameRepeatValue;    /* number of times to repeat each normal frame */
    void    *pData;                /* extra data used by derived classes */
} TWEENDESCRIPTION;

/****************************************************************************
 *
 * PaletteTween
 *
 * answers a logical palette
 *
 ****************************************************************************/
class TweenThread : public ThreadedEntity
{
public:
                     TweenThread( const TWEENDESCRIPTION &tween_description );
    virtual         ~TweenThread();

    virtual int     ThreadProcedure( void *pData );


    error_t         Initialize( void *pObject1 );

    error_t         ScheduleTween( const DWORD dwFrames,
                                   void *pObject1,
                                   void *pObject2 );

    void            *GetFrame( void );



protected:
    /*
     * the function that does all the work,
     * derived classes only need to define this
     */
    virtual void    PrerenderFrames( const DWORD dwFrames,
                                     void **pFrames,
                                     void *pObject1,
                                     void *pObject2 ) = 0;
    virtual void    ExperimentalPrerenderFrames( const DWORD dwFrames,
                                     void **pFrames,
                                     void *pFirstFrame,
                                     void *pLastFrame ) = 0;
    virtual void    ExperimentalPrerenderFrame(
                                     void *pFrame,
                                     void *pObject1 ) = 0;


    inline void     SwapDWORD( DWORD *pDWORD1, DWORD *pDWORD2 )
    {
        DWORD   dwTemp;

        dwTemp   = *pDWORD1;
        *pDWORD1 = *pDWORD2;
        *pDWORD2 = dwTemp;
    };

    inline void     SwapPointers( void ***pPointer1, void ***pPointer2 )
    {
        void    **pTemp;

        pTemp      = *pPointer1;
        *pPointer1 = *pPointer2;
        *pPointer2 = pTemp;
    };

private:
    const DWORD     m_dwMaximumFrames;
    const DWORD     m_dwDefaultFrames;
    const DWORD     m_dwTweenFrameRepeatValue;
    const DWORD     m_dwPlainFrameRepeatValue;
    const DWORD     m_dwFrameSize;

    DWORD           m_dwState;
                /*
                 * 0 = loop on delta 1, ready to schedule tween
                 * 1 = loop on delta 1, tween when ready (also swap d1 and d2)
                 * 2 = tween once then return to state 0
                 *
                 */

    DWORD           m_dwCurrentFrame;
    DWORD           m_dwPlainRepetition;
    DWORD           m_dwTweenRepetition;

    DWORD           m_dwNumPlainFrames;
    DWORD           m_dwNumTweenFrames;
    DWORD           m_dwNumTweenFramesThreadside;
    DWORD           m_dwNumPlainFramesThreadside;

    void            **m_pPlainFrames;
    void            **m_pTweenFrames;
    void            **m_pTweenFramesThreadside;
    void            **m_pPlainFramesThreadside;

    void            *m_pObject1;    /* this is the object to tween from */
    void            *m_pObject2;    /* this is the object to tween to */


    void            *m_pUnalignedBuffer;    /* the pointer to the allocated chunk of memory */
};

#endif /* _TweenThread_H_ */

