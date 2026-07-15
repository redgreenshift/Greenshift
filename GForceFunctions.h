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
 * GForceFunctions - in order to maintain compatibility with G-Force configs
 *
 ****************************************************************************/

#ifndef _GForceFunctions_H_
#define _GForceFunctions_H_

#include "Vis.h"
#include <stdlib.h> /* rand() & srand() */


value_t GForce_mag ( value_t nValue, void *pData );
value_t GForce_fft ( value_t nValue, void *pData );
value_t GForce_pos ( value_t nValue );
value_t GForce_abs ( value_t nValue );
value_t GForce_sqr ( value_t nValue );
value_t GForce_sgn ( value_t nValue );
value_t GForce_rnd ( value_t nValue );
value_t GForce_clip( value_t nValue );
value_t GForce_trnc( value_t nValue );
value_t GForce_flor( value_t nValue );
value_t GForce_wrap( value_t nValue );
value_t GForce_sqwv( value_t nValue );
value_t GForce_trwv( value_t nValue );
value_t GForce_seed( value_t nValue );
value_t GForce_tan ( value_t nValue );
value_t GForce_atan( value_t nValue );
value_t GForce_log ( value_t nValue );
value_t GForce_exp ( value_t nValue );
value_t GForce_sqrt( value_t nValue );


#endif /* _GForceFunctions_H_ */

