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
 * DeltaTween - abstract base class for tweening objects
 *
 ****************************************************************************/

#ifndef _DeltaTween_H_
#define _DeltaTween_H_

#include "TweenThread.h"
#include "DeltaField.h"

typedef struct tagPIXELMAPDESCRIPTION
{
    DWORD   dwWidth;
    DWORD   dwHeight;
} PIXELMAPDESCRIPTION;


/****************************************************************************
 *
 * DeltaTween
 *
 * answers a pixel map
 *
 ****************************************************************************/
class DeltaTween : public TweenThread
{
public:
    bool            m_bFramerateTooLow;
protected:
    const long      m_lVisibleWidth;
    const long      m_lVisibleHeight;
//    value_t         m_nAspect;
    value_t         m_nWidthFactor;
    value_t         m_nHeightFactor;
    value_t         m_nWidthFactorInverse;
    value_t         m_nHeightFactorInverse;

    inline long     Width( void ) { return m_lVisibleWidth; };
    inline long     Height( void ) { return m_lVisibleHeight; };

public:
                    DeltaTween( const TWEENDESCRIPTION &tween_description );
    virtual         ~DeltaTween();

    inline PIXELMAP *GetPixelMap( void )    { return (PIXELMAP*)GetFrame(); };

protected:
    void            SetAspect( value_t nAspect, const bool bZoom );
    virtual void    PrerenderFrames( const DWORD dwFrames,
                                     void **pFrames,
                                     void *pObject1,
                                     void *pObject2 );
    virtual void    ExperimentalPrerenderFrames( const DWORD dwFrames,
                                     void **pFrames,
                                     void *pFirstFrame,
                                     void *pLastFrame );
    virtual void    ExperimentalPrerenderFrame(
                                     void *pFrame,
                                     void *pObject1 );

    inline long    ConvertToScreen( const value_t nPhysicalValue,
                             const long nHalfDimension,
                             const long nDimension,
                             const DWORD nBoundsType );

};

#endif /* _DeltaTween_H_ */

