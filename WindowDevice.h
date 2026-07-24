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
  * WindowDevice - base class for windows
  *
  ****************************************************************************/

#ifndef _WindowDevice_H_
#define _WindowDevice_H_

#include "ThreadedEntity.h"
#include "BitCanvas.h"

  /* define strict before including windows */
#ifndef STRICT
#define STRICT 1
#endif
#include <windows.h>
//#include <ddraw.h>


#define GS_WindowStyle   ( WS_SYSMENU     \
                         | WS_CAPTION     \
                         | WS_THICKFRAME  \
                         | WS_MAXIMIZEBOX \
                         | WS_MINIMIZEBOX )
#define GS_FullscreenStyle      WS_POPUP
#define GS_WindowStyleEx        WS_EX_OVERLAPPEDWINDOW
#define GS_FullscreenStyleEx    0

#define GS_Style    (IsFullscreen() ? GS_FullscreenStyle : GS_WindowStyle)
#define GS_StyleEx  (IsFullscreen() ? GS_FullscreenStyleEx : GS_WindowStyleEx)


#define WD_DEFAULT              0x00
#define WD_FULLSCREEN           0x01
#define WD_OVERLAY              0x02
#define WD_FSFLIP               0x04
#define WD_DIRECTX              0x08
#define WD_OPENGL               0x10

/*
 * the window procedure
 */
LRESULT CALLBACK WindowDeviceWindowProcedure(HWND hWindow,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);


/****************************************************************************
 *
 * WindowDevice - base class for windows
 *
 * this is what creates the window
 *
 ****************************************************************************/
class WindowDevice : public ThreadedEntity
{
public:
	virtual void* GetPixelMemory(void) = 0; /* hack for graphics class */
	virtual void ReleasePixelMemory(void) = 0; /* hack for graphics class */
private:
	value_t     m_nWidth, m_nHeight, m_nBitDepth,
		m_nWindowWidth, m_nWindowHeight,
		m_nFullscreenWidth, m_nFullscreenHeight;

public:
	WindowDevice();
	virtual             ~WindowDevice();

	inline bool         IsSwitchingModes(void)
	{
		bool    bIsSwitchingModes;

		EnterCriticalSection(&m_csSwitchingModes);
		bIsSwitchingModes = m_bSwitchingModes;
		LeaveCriticalSection(&m_csSwitchingModes);

		return bIsSwitchingModes;
	};
protected:
	inline void         SetSwitchingModes(const bool bIsSwitchingModes)
	{
		EnterCriticalSection(&m_csSwitchingModes);
		m_bSwitchingModes = bIsSwitchingModes;
		LeaveCriticalSection(&m_csSwitchingModes);
	};/**/
public:

	static error_t  New(const HINSTANCE hInstance,
		WindowDevice** outWindowDevice,
		const char* strWindowTitle,
		MyDictionary<mychar_t*>* inConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals,
		MyDictionary<value_t*>* inValues,
		const HWND hParentWindow,
		const WNDPROC hDefWindowProc = DefWindowProc);

	error_t Initialize(const char* strWindowTitle,
		const HINSTANCE hInstance,
		const WNDPROC hWindowMsgHandler,
		const HWND hParentWindow,
		MyDictionary<mychar_t*>* inConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals,
		MyDictionary<value_t*>* inValues);

	void                Resize(void);  /* called upon WM_SIZE */
	void                Move(void);    /* called upon WM_MOVE */
	bool                KeyPress(const DWORD dwKey);/* key handler */
	BOOL MessageHandler(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam);

	/*
	 * toggles the flags passed in and reinitializes the display
	 */
	error_t             ToggleMode(const DWORD dwFlags);
	DWORD               GetMode(const DWORD dwFlags);
	error_t             Print(const int x,
		const int y,
		const char* string,
		const COLORREF dwColor = PALETTERGB(255, 255, 255));

	virtual int         ThreadProcedure(void* pData);/* thread entry point */

	virtual HRESULT     SetPalette(LOGPALETTE* lpPalette) = 0;
	virtual HRESULT     Flip(BitCanvas* pBitCanvas) = 0;

protected:
	error_t             PrintPrivate(const int x,
		const int y,
		const char* string,
		const COLORREF dwColor = PALETTERGB(255, 255, 255));
	/*
	 * accessors
	 */
	inline DWORD        Width(void) { return m_dwWidth; };
	inline DWORD        Height(void) { return m_dwHeight; };
	inline DWORD        WindowWidth(void) { return m_dwWindowWidth; };
	inline DWORD        WindowHeight(void) { return m_dwWindowHeight; };
	inline DWORD        FullscreenWidth(void) { return m_dwFullscreenWidth; };
	inline DWORD        FullscreenHeight(void) { return m_dwFullscreenHeight; };
	inline DWORD        BitDepth(void) { return m_dwBitDepth; };
	inline DWORD        ColorKey(void) { return m_dwOverlayColorKey; };
	inline BitCanvas* GetBitCanvas(void) { return m_pBitCanvas; };
	inline HWND         GetWindow(void) { return m_hWindow; };
	inline DWORD        ShouldFlip(void)
	{
		return  (GetMode(WD_FULLSCREEN | WD_FSFLIP)) || GetMode(WD_OVERLAY);
	};

	/*
	 * modifiers
	 */
	error_t             Flip(void);                   /* swap the front and
														 * back buffers
														 */
	void                UpdateCursor(void);
	error_t             UpdateWindowProperties(void); /* adjusts the window
														 * appearance between
														 * the fullscreen and
														 * windowed versions.
														 * make it visible if
														 * not already set */

	error_t             ResetDisplay(void);   /* create all the DD
												 * surfaces
												 */


												 /*
												  * hook into subclass to let it recreate its display objects
												  */
	virtual error_t     InitDisplay(void) = 0;
	virtual error_t     DisableDisplay(void) = 0;
	virtual error_t     ClearScreen(void) = 0;
	virtual error_t     ClearScreenAll(void) = 0;
	virtual void        OnResize(void) = 0;
	virtual void        OnMove(void) = 0;
	virtual HRESULT     OnFlip(void) = 0;
	virtual HRESULT     GetDC(HDC* pHDC) = 0;
	virtual HRESULT     ReleaseDC(HDC hdc) = 0;
	virtual HRESULT     GetPrimaryDC(HDC* pHDC) = 0;
	virtual HRESULT     ReleasePrimaryDC(HDC hdc) = 0;



private:
	char                m_strWindowTitle[256];
	WNDCLASSEX          m_hWindowClass;
	HWND                m_hWindow;
	HWND                m_hParentWindow;
	WNDPROC             m_hDefWindowProc;    /* default window procedure  */

	DWORD               m_dwWidth;      /* the width  of the screen       */
	DWORD               m_dwHeight;     /* the height of the screen       */
	DWORD               m_dwBitDepth;   /* current display bit depth      */
	DWORD               m_dwFlags;      /* storage for various attributes */
	DWORD               m_dwOverlayColorKey;

	DWORD               m_dwFullscreenWidth;
	DWORD               m_dwFullscreenHeight;
	DWORD               m_dwWindowWidth;
	DWORD               m_dwWindowHeight;

protected:
	BitCanvas* m_pBitCanvas;    /* the entity that draws on me  */

private:
	CRITICAL_SECTION    m_csSwitchingModes;
	bool                m_bShowFramerate;
	bool                m_bShowDebug;
	bool                m_bSwitchingModes;

	inline DWORD        GetFlags(void) { return m_dwFlags; };
	inline void         ToggleFlags(const DWORD dwFlags)
	{
		m_dwFlags ^= dwFlags;
	};

	/*
	 * style flags
	 */
	DWORD               Style(void);
	DWORD               StyleEx(void);

	DWORD               WindowStyle(void);
	DWORD               FullscreenStyle(void);

	DWORD               WindowStyleEx(void);
	DWORD               FullscreenStyleEx(void);
};


#endif  /* _WindowDevice_H_ */

