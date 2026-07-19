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
  * WindowDeviceDX - DirectX specific WindowDevice
  *
  ****************************************************************************/

#ifndef _WindowDeviceDX_H_
#define _WindowDeviceDX_H_

#include "WindowDevice.h"
#include <ddraw.h>
#include "DXError.h"

  /****************************************************************************
   *
   * WindowDeviceDX -
   *
   * this is what creates the DX objects
   *
   ****************************************************************************/
class WindowDeviceDX : public WindowDevice
{
public:
	void* GetPixelMemory(void)
	{
		static DDSURFACEDESC2 ddsd;
		if (GetBack()->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL)
			!= DD_OK)
			return NULL;

		return &ddsd;
	};
	void ReleasePixelMemory(void)
	{
		GetBack()->Unlock(NULL);
	};
	void DumpCaps(void);
	WindowDeviceDX();    /* constructor */
	virtual             ~WindowDeviceDX();    /* destructor */

#ifdef UNDEFINED
	BOOL                DDEnumCallbackEx(GUID FAR* lpGUID,
		LPSTR     lpDriverDescription,
		LPSTR     lpDriverName,
		LPVOID    lpContext,
		HMONITOR  hm);
#endif

#ifdef UNDEFINED
	HRESULT WINAPI      EnumModesCallback(
		LPDDSURFACEDESC2 lpDDSurfaceDesc,
		LPVOID lpContext);
#endif

protected:
	/* check capabilities of hardware */
//    error_t             CheckDeviceCapabilities( void );
	error_t             ClearScreen(void);    /* clears the back surface */
	virtual error_t     ClearScreenAll(void); /* clears all surfaces */
	error_t             FillSurface(LPDIRECTDRAWSURFACE7 lpSurface,
		DWORD dwFillColor = PALETTERGB(0, 0, 0));
	/* clear a surface
	 * using the
	 * specified color
	 */

	 /* how many places should call InitDisplay()? */
	virtual error_t     InitDisplay(void); /* create all the DD
											  * surfaces
											  */
	virtual error_t     DisableDisplay(void); /* disable all the DD surfaces
												 */
public:
	virtual HRESULT     SetPalette(LOGPALETTE* lpPalette);
	virtual HRESULT     Flip(BitCanvas* pBitCanvas);
	//    HRESULT             Blit( void );       /* tell BitCanvas to draw on me */
	//    virtual HRESULT     OnFlip( void );     /* swap the front and back      */

protected:
	HRESULT             Blit(void);       /* tell BitCanvas to draw on me */
	virtual HRESULT     OnFlip(void);     /* swap the front and back      */
	virtual void        OnResize(void);   /* account for window changes   */
	virtual void        OnMove(void);     /* account for window changes   */

	virtual HRESULT     GetDC(HDC* pHDC);
	virtual HRESULT     ReleaseDC(HDC hdc);
	virtual HRESULT     GetPrimaryDC(HDC* pHDC);
	virtual HRESULT     ReleasePrimaryDC(HDC hdc);

	DWORD               CoopLevel(void);    /* DD cooperative level flags */
	virtual DWORD       CoopLevelAlways(void); /* flags for all modes */
	virtual DWORD       CoopLevelWindow(void); /* flags only for windowed */
	virtual DWORD       CoopLevelFullscreen(void);/* flags for fullscreen */


	virtual void        ReleaseAllObjects(void);    /* release all surfaces and release exclusive mode */
	virtual void        ReleaseAllSurfaces(void); /* release all surfaces and palette */


	/*
	 * accessors, "protected" in case a subclass needs to access them
	 */
	inline LPDIRECTDRAW7            GetDD(void);
	inline LPDIRECTDRAWSURFACE7     GetPrimary(void);
	inline LPDIRECTDRAWSURFACE7     GetBack(void);
	inline LPDIRECTDRAWPALETTE      GetPalette(void);
	inline LPDIRECTDRAWSURFACE7     GetMask(void) { return m_pDDSOverlayMask; };
	inline LPDIRECTDRAW7* PGetDD(void);
	inline LPDIRECTDRAWSURFACE7* PGetPrimary(void);
	inline LPDIRECTDRAWSURFACE7* PGetBack(void);
	inline LPDIRECTDRAWPALETTE* PGetPalette(void);
	inline LPDIRECTDRAWSURFACE7* PGetMask(void) { return &m_pDDSOverlayMask; };
	inline DDOVERLAYFX* PGetFX(void) { return &m_hDDOverlayFX; };
	inline void LockDX(void) { EnterCriticalSection(&m_csDX); };
	inline void UnlockDX(void) { LeaveCriticalSection(&m_csDX); };
private:
	CRITICAL_SECTION        m_csDX;
	LPDIRECTDRAW7           m_pDD;            /* The DirectDraw7 Object     */
	LPDIRECTDRAWSURFACE7    m_pDDSPrimary;    /* DirectDraw primary surface */
	LPDIRECTDRAWSURFACE7    m_pDDSBack;       /* DirectDraw back surface    */
	LPDIRECTDRAWSURFACE7    m_pDDSOverlayMask;/* DirectDraw mask surface    */
	DDOVERLAYFX             m_hDDOverlayFX;   /* effects to use for overlay */
	LPDIRECTDRAWPALETTE     m_pDDPalette;     /* The primary surface palette*/

	DDCAPS                  m_ddDriverCaps;/* the device driver capabilities*/
	DDCAPS                  m_ddHELCaps;/* the hardware emulation layer caps*/

	//    DDPIXELFORMAT           m_ddpfPixelFormat;

	inline error_t          UpdateOverlayProperties(void);
	/* adjusts the overlay
	 * to make sure it's
	 * aligned properly
	 * according to the
	 * hardware capability
	 * and makes visibile
	 * if not already
	 */
};


#endif  /* _WindowDeviceDX_H_ */

