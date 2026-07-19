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
  * WindowDeviceOpenGL - OpenGL specific WindowDevice
  *
  ****************************************************************************/

#ifndef _WindowDeviceOpenGL_H_
#define _WindowDeviceOpenGL_H_

#include "WindowDevice.h"
#include <gl/gl.h>
  //#include <gl/glu.h>


  /****************************************************************************
   *
   * WindowDeviceOpenGL -
   *
   * this is what creates the OpenGL objects
   *
   ****************************************************************************/
class WindowDeviceOpenGL : public WindowDevice
{
public:
	WindowDeviceOpenGL();    /* constructor */
	virtual             ~WindowDeviceOpenGL();    /* destructor */

	void                Update(const long symbol);

protected:
	error_t             ClearScreen(void);    /* clears the back surface */
	virtual error_t     ClearScreenAll(void); /* clears all surfaces */
	/*    error_t                FillSurface( LPDIRECTDRAWSURFACE7 lpSurface,
										DWORD dwFillColor = 0 ); /* clear a surface
																  * using the
																  * specified color
																  */

																  /* how many places should call InitDisplay()? */
	virtual error_t     InitDisplay(void); /* create all the OpenGL
											  * surfaces
											  */
	virtual error_t     DisableDisplay(void); /* release all the OpenGL
												 */

	HRESULT             Blit(void);       /* tell BitCanvas to draw on me */
	virtual HRESULT     OnFlip(void);     /* swap the front and back      */
	virtual void        OnResize(void);   /* account for window changes   */
	virtual void        OnMove(void);     /* account for window changes   */

	virtual HRESULT     GetDC(HDC* pHDC);
	virtual HRESULT     ReleaseDC(HDC hdc);


	/*
	 * accessors, "protected" in case a subclass needs to access them
	 */
	HGLRC               GetRC(void);
	HGLRC* PGetRC(void);
	HRESULT             ReleaseRC(HGLRC hRC, HDC hDC);
	inline PIXELFORMATDESCRIPTOR    GetPFD(void);
	inline PIXELFORMATDESCRIPTOR* PGetPFD(void);
private:
	HDC                     m_hDC;
	HGLRC                   m_hGLRC;        /* the OpenGL render context */
	PIXELFORMATDESCRIPTOR   m_pfdPixelFormat;

	inline error_t      UpdateOverlayProperties(void);
	/* adjusts the overlay
	 * to make sure it's
	 * aligned properly
	 * according to the
	 * hardware capability
	 * and makes visibile
	 * if not already
	 */
};


#endif  /* _WindowDeviceOpenGL_H_ */

