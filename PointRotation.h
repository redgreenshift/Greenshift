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
  * PointRotation - thing to simplify 3d rotation
  *
  ****************************************************************************/

#ifndef _PointRotation_H_
#define _PointRotation_H_


  /****************************************************************************
   *
   * PointRotation - currently implemented using matrix style rotation
   *
   ****************************************************************************/
class PointRotation
{
public:
	PointRotation() { ResetToIdentity(); };
	~PointRotation() {};

	void            RotatePoint(const value_t x,
		const value_t y,
		const value_t z,
		value_t* outX,
		value_t* outY,
		value_t* outZ)
	{
		*outX = (a * x) + (d * y) + (g * z);
		*outY = (b * x) + (e * y) + (h * z);
		*outZ = (c * x) + (f * y) + (i * z);
	};

	void            RotateX(const value_t nTilt)
	{

		const value_t a12 = b;
		const value_t a13 = c;

		const value_t a22 = e;
		const value_t a23 = f;

		const value_t a32 = h;
		const value_t a33 = i;

		const value_t b22 = (value_t)cos(nTilt);
		const value_t b23 = (value_t)-sin(nTilt);
		const value_t b32 = (value_t)sin(nTilt);
		const value_t b33 = (value_t)cos(nTilt);

		if (nTilt == 0.0f)
			return;

		b = (a12 * b22) + (a13 * b32);
		c = (a12 * b23) + (a13 * b33);

		e = (a22 * b22) + (a23 * b32);
		f = (a22 * b23) + (a23 * b33);

		h = (a32 * b22) + (a33 * b32);
		i = (a32 * b23) + (a33 * b33);
	};

	void            RotateY(const value_t nTwist)
	{
		const value_t a11 = a;

		const value_t a13 = c;
		const value_t a21 = d;

		const value_t a23 = f;
		const value_t a31 = g;

		const value_t a33 = i;

		const value_t b11 = (value_t)cos(nTwist);
		const value_t b13 = (value_t)sin(nTwist);
		const value_t b31 = (value_t)-sin(nTwist);
		const value_t b33 = (value_t)cos(nTwist);

		if (nTwist == 0.0f)
			return;

		a = (a11 * b11) + (a13 * b31);
		c = (a11 * b13) + (a13 * b33);

		d = (a21 * b11) + (a23 * b31);
		f = (a21 * b13) + (a23 * b33);

		g = (a31 * b11) + (a33 * b31);
		i = (a31 * b13) + (a33 * b33);
	};

	void            RotateZ(const value_t nTurn)
	{
		const value_t a11 = a;
		const value_t a12 = b;

		const value_t a21 = d;
		const value_t a22 = e;

		const value_t a31 = g;
		const value_t a32 = h;

		const value_t b11 = (value_t)cos(nTurn);
		const value_t b12 = (value_t)-sin(nTurn);
		const value_t b21 = (value_t)sin(nTurn);
		const value_t b22 = (value_t)cos(nTurn);

		if (nTurn == 0.0f)
			return;

		a = (a11 * b11) + (a12 * b21);
		b = (a11 * b12) + (a12 * b22);

		d = (a21 * b11) + (a22 * b21);
		e = (a21 * b12) + (a22 * b22);

		g = (a31 * b11) + (a32 * b21);
		h = (a31 * b12) + (a32 * b22);
	};

	void            ResetToIdentity(void)
	{
		a = 1.0f;  b = 0.0f;  c = 0.0f;
		d = 0.0f;  e = 1.0f;  f = 0.0f;
		g = 0.0f;  h = 0.0f;  i = 1.0f;
	};

protected:
	value_t a;
	value_t b;
	value_t c;

	value_t d;
	value_t e;
	value_t f;

	value_t g;
	value_t h;
	value_t i;
};

#endif  /* _PointRotation_H_ */

