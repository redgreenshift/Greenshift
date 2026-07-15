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
 * MetaDeltaField - 
 *
 ****************************************************************************/

#ifndef _MetaDeltaField_H_
#define _MetaDeltaField_H_

#include "MetaConfig.h"
#include "DeltaField.h"


class MetaDeltaField : public MetaConfig
{
protected:
    value_t                 m_nInterval;
//    value_t                m_nTweenTime;
    DWORD                   m_dwCurrent;
    DWORD                   m_dwNext;
    DeltaField              *m_pDeltaFields;
    Expression              *m_pIntervalTime;
    Expression              *m_pTweenDuration;
    HighResolutionTimer     m_hrInterval;

public:
                            MetaDeltaField();
    virtual                 ~MetaDeltaField();

    virtual error_t         InitializeDerived(
                            MyDictionary<char*> *inMainConfig,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals );

    virtual error_t         UpdateDerived( BitCanvas * pBitCanvas );
    virtual error_t         UpdateDerived( WindowDevice *pWindowDevice );


    void                    CheckInterval( void );

};



#endif  /* _MetaDeltaField_H_ */



