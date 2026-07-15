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
 * Interval - represents a finite arithmetic progression
 *
 ****************************************************************************/

#ifndef _Interval_H_
#define _Interval_H_


/****************************************************************************
 *
 * Interval - represents a finite arithmetic progression
 *
 ****************************************************************************/
class Interval
{
public:

/****************************************************************************
 *
 * Interval - default constructor
 *
 ****************************************************************************/
Interval()
{
    m_nStart = 0.0f;
    m_nStop  = 1.0f;
    m_nNormalizationFactor = 1.0f;
};

/****************************************************************************
 *
 * SetBounds - set bounds
 *
 ****************************************************************************/
void    SetBounds( const value_t nStart, const value_t nStop )
{
    m_nStart = nStart;
    m_nStop  = nStop;
    m_nNormalizationFactor = 1.0f / (m_nStop - m_nStart);
};


/****************************************************************************
 *
 * operator[] - return the value nReal percent between start and stop
 *
 ****************************************************************************/
inline value_t    operator[]( const value_t inValue )
{
    return m_nStart + (m_nStop - m_nStart) * inValue;
};


/****************************************************************************
 *
 * ToNormalized - return the value normalized to the interval 0..1
 *
 ****************************************************************************/
inline value_t    Normalize( const value_t inValue )
{
    return (inValue - m_nStart) * m_nNormalizationFactor;
};



protected:
    value_t        m_nNormalizationFactor;
    value_t        m_nStart;
    value_t        m_nStop;

};


#endif  /* _Interval_H_ */

