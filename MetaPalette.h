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
 * MetaPalette - 
 *
 ****************************************************************************/

#ifndef _MetaPalette_H_
#define _MetaPalette_H_

#include "MetaConfig.h"
#include "Palette.h"




class MetaPalette : public MetaConfig
{
protected:
    DWORD                   m_dwCurrent;
    DWORD                   m_dwNext;
    value_t                 m_nPercent;
    value_t                 m_nInterval;
    value_t                 m_nTween;
    Palette                 *m_pPalettes;
    Expression              *m_pInterval;
    Expression              *m_pTweenTime;
    HighResolutionTimer     m_hrTweenTimer;
    HighResolutionTimer     m_hrIntervalTimer;
    bool                    m_bTweening;


public:
                        MetaPalette();
    virtual             ~MetaPalette();

    virtual error_t     InitializeDerived(
                            MyDictionary<char*> *inMainConfig,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals );

    virtual error_t     UpdateDerived( BitCanvas * pBitCanvas );
    virtual error_t     UpdateDerived( WindowDevice *pWindowDevice );

    void                IntervalCheck( void );

};


#endif  /* _MetaPalette_H_ */



