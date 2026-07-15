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
 * WindowDeviceDX
 *
 ****************************************************************************/

#include "WindowDeviceDX.h"

//#define NO_WAIT

/****************************************************************************
 *
 * WindowDeviceDX
 *
 ****************************************************************************/
WindowDeviceDX::WindowDeviceDX()
{
    InitializeCriticalSection(&m_csDX);

    m_pDD         = NULL;
    m_pDDSPrimary = NULL;
    m_pDDSBack    = NULL;
    m_pDDPalette  = NULL;
    *PGetMask()   = NULL;

    ZeroMemory( &m_hDDOverlayFX, sizeof(m_hDDOverlayFX) );
    m_hDDOverlayFX.dwSize = sizeof(m_hDDOverlayFX);

    /* source colorkey is color NOT overwritten */
    /* dest is color overwritten */
    PGetFX()->dckDestColorkey.dwColorSpaceHighValue = ColorKey();
    PGetFX()->dckDestColorkey.dwColorSpaceLowValue  = ColorKey();
}

/****************************************************************************
 *
 * ~WindowDeviceDX
 *
 ****************************************************************************/
WindowDeviceDX::~WindowDeviceDX()
{
    if( GetDD() != NULL )
    {
        ReleaseAllObjects();
        GetDD()->Release();
        *PGetDD() = NULL;
    }
    DeleteCriticalSection(&m_csDX);
}




/****************************************************************************
 *
 * Blit - 
 *
 ****************************************************************************/
HRESULT    WindowDeviceDX::Blit( void )
{
    HRESULT hRet  = DD_OK;
    HRESULT hRet2 = DD_OK;
    DDSURFACEDESC2  ddsd;
    HDC        hDC;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    if( GetBack()->IsLost() )
        hRet = GetBack()->Restore();  /* cannot restore the back buffer of a flipping chain */
    if( GetPrimary()->IsLost() )
        hRet = GetPrimary()->Restore();

    /* if surfaces were created in a different mode, recreate them */
    if( hRet == DDERR_WRONGMODE )
    {
        hRet2 = hRet;
        hRet = ResetDisplay();
        if( hRet == DD_OK )
            PrintPrivate(0,20, DXError(hRet2), PALETTERGB(255,0,0) );
    }

    if( hRet != DD_OK )
        return hRet;

#ifndef UNDEFINED
    hRet = GetBack()->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
    if( hRet != DD_OK )
        return hRet;

    GetBitCanvas()->CopyTo( ddsd.lpSurface,
        ddsd.dwWidth,
        ddsd.dwHeight,
        ddsd.lPitch,
        ddsd.ddpfPixelFormat.dwRGBBitCount / BITS_PER_BYTE - 1 );

    return GetBack()->Unlock(NULL);

#else
#ifdef USE_BITMAP
    if( (hRet = GetDC( &hDC )) == DD_OK )
    {
        GetBitCanvas()->CopyTo( Width(), Height(), hDC );
        return ReleaseDC(hDC);
    }
    else
        return hRet;
#else
    return 1;
#endif
#endif

}


/*
void flip()
{
    dd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
    primary->Flip(0, DDFLIP_WAIT);
}


/****************************************************************************
 *
 * OnFlip - 
 *
 ****************************************************************************/
HRESULT    WindowDeviceDX::OnFlip( void )
{
    RECT    srcRect;
    RECT    dstRect;
    HRESULT hRet;

        if( GetPrimary() == NULL || GetBack() == NULL )
            return FAILURE;
    srcRect.left   = 0;
    srcRect.top    = 0;
    srcRect.right  = Width();
    srcRect.bottom = Height();
    GetClientRect(GetWindow(), &dstRect );
    ClientToScreen(GetWindow(), (POINT*)&dstRect.left);
    ClientToScreen(GetWindow(), (POINT*)&dstRect.right);

//    DumpToFile("error.txt", GetBack() == NULL ? "NULL" : "not NULL", "\n");

//    do
    {

        if( GetPrimary()->IsLost() )
            GetPrimary()->Restore();
        if( GetBack()->IsLost() )
            GetBack()->Restore();

        if( ShouldFlip() )
//            hRet = GetPrimary()->Blt( NULL, GetBack(), NULL, DDBLT_WAIT, NULL);
            hRet = GetPrimary()->Flip( NULL, DDFLIP_WAIT );
        else/**/
        if( GetMode(WD_FULLSCREEN) )
#ifdef NO_WAIT
            hRet = GetPrimary()->Blt( NULL, GetBack(), &srcRect, DDBLT_DONOTWAIT , NULL);
#else
            hRet = GetPrimary()->Blt( NULL, GetBack(), &srcRect, DDBLT_WAIT, NULL);
#endif
        else
#ifdef NO_WAIT
            hRet = GetPrimary()->Blt( &dstRect, GetBack(), &srcRect, DDBLT_DONOTWAIT , NULL);
#else
            hRet = GetPrimary()->Blt( &dstRect, GetBack(), &srcRect, DDBLT_WAIT, NULL);
#endif
    }// while( hRet == DDERR_SURFACELOST );

//    DumpToFile( "arugh.txt", hRet, "\n" );

    return hRet;
}



HRESULT    WindowDeviceDX::Flip( BitCanvas *pBitCanvas )
{
    HRESULT hRet  = DD_OK;
    HRESULT hRet2 = DD_OK;

    EnterCS();
    LockDX();

    if( pBitCanvas != NULL && (! IsIconic(GetWindow()) || GetMode(WD_OVERLAY) ) )
    {
        m_pBitCanvas = pBitCanvas;
        if( (hRet = Blit()) == DD_OK )
            hRet2 = WindowDevice::Flip();
    }

    UnlockDX();
    LeaveCS();

    return max( hRet, hRet2 );
}



/****************************************************************************
 *
 * OnResize - 
 *
 ****************************************************************************/
void    WindowDeviceDX::OnResize( void )
{
    UpdateOverlayProperties();
}


/****************************************************************************
 *
 * OnMove - 
 *
 ****************************************************************************/
void    WindowDeviceDX::OnMove( void )
{
    UpdateOverlayProperties();
}


/****************************************************************************
 *
 * GetDC - get a device context
 *
 ****************************************************************************/
HRESULT WindowDeviceDX::GetDC( HDC *pHDC )
{
    if( GetBack() != NULL )
        return GetBack()->GetDC(pHDC);
    else
        return DDERR_GENERIC;
}

/****************************************************************************
 *
 * ReleaseDC - release the device context
 *
 ****************************************************************************/
HRESULT WindowDeviceDX::ReleaseDC( HDC hdc )
{
    return GetBack()->ReleaseDC(hdc);
}



/****************************************************************************
 *
 * GetPrimaryDC - get a device context
 *
 ****************************************************************************/
HRESULT WindowDeviceDX::GetPrimaryDC( HDC *pHDC )
{
    if( GetBack() != NULL )
        return GetPrimary()->GetDC(pHDC);
    else
        return DDERR_GENERIC;
}

/****************************************************************************
 *
 * ReleasePrimaryDC - release the device context
 *
 ****************************************************************************/
HRESULT WindowDeviceDX::ReleasePrimaryDC( HDC hdc )
{
    return GetPrimary()->ReleaseDC(hdc);
}


/****************************************************************************
 *
 * ClearScreen - clears the back buffer
 *
 ****************************************************************************/
error_t    WindowDeviceDX::ClearScreen( void )
{
    return FillSurface( GetBack() );
}



/****************************************************************************
 *
 * ClearSurface - clears the passed surface
 *
 ****************************************************************************/
error_t    WindowDeviceDX::FillSurface( LPDIRECTDRAWSURFACE7 lpSurface, DWORD dwFillColor )
{
    HRESULT hRet = DD_OK;
    DDBLTFX fx;

    /* initialize the buffers to black */
    ZeroMemory(&fx, sizeof(fx));
    fx.dwSize = sizeof(fx);
    fx.dwFillColor = dwFillColor;

    EnterCS();

    if( lpSurface != NULL )
//#ifdef NO_WAIT
//         hRet = lpSurface->Blt(NULL, NULL, NULL, DDBLT_DONOTWAIT | DDBLT_COLORFILL, &fx);
//#else
         hRet = lpSurface->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
//#endif
    else
        hRet = FAILURE;

    LeaveCS();

    if( hRet != DD_OK )
        return FAILURE;

//DumpToFile("error.txt", 7, "\n");
    return SUCCESS;
}


/****************************************************************************
 *
 * ClearScreenAll
 *
 ****************************************************************************/
error_t    WindowDeviceDX::ClearScreenAll( void )
{
    HRESULT hRet = DD_OK;
    DDBLTFX fx;
    RECT    hRect;

    /* initialize the buffers to black */
    ZeroMemory(&fx, sizeof(fx));
    fx.dwSize = sizeof(fx);
    fx.dwFillColor = PALETTERGB(0,0,0);

    EnterCS();

    GetClientRect(GetWindow(), &hRect);
    ClientToScreen(GetWindow(), (POINT*)&hRect.left);
    ClientToScreen(GetWindow(), (POINT*)&hRect.right);


    if( hRet == DD_OK && GetBack() != NULL )
         hRet = GetBack()->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

    if( hRet == DD_OK && GetPrimary() != NULL )
        if( GetMode(WD_OVERLAY) )
            hRet = GetPrimary()->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);
        else
            hRet = GetPrimary()->Blt(&hRect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx);

    if( hRet == DD_OK )
        hRet = UpdateOverlayProperties();

    LeaveCS();

    if( hRet != DD_OK )
        return hRet;

    return SUCCESS;
}







/****************************************************************************
 *
 * accessors
 *
 ****************************************************************************/
LPDIRECTDRAW7           WindowDeviceDX::GetDD( void )
{
    return m_pDD;
}
LPDIRECTDRAWSURFACE7    WindowDeviceDX::GetPrimary( void )
{
    return m_pDDSPrimary;
}
LPDIRECTDRAWSURFACE7    WindowDeviceDX::GetBack( void )
{
    return m_pDDSBack;
}
LPDIRECTDRAWPALETTE     WindowDeviceDX::GetPalette( void )
{
    return m_pDDPalette;
}
LPDIRECTDRAW7           *WindowDeviceDX::PGetDD( void )
{
    return &m_pDD;
}
LPDIRECTDRAWSURFACE7    *WindowDeviceDX::PGetPrimary( void )
{
    return &m_pDDSPrimary;
}
LPDIRECTDRAWSURFACE7    *WindowDeviceDX::PGetBack( void )
{
    return &m_pDDSBack;
}
LPDIRECTDRAWPALETTE     *WindowDeviceDX::PGetPalette( void )
{
    return &m_pDDPalette;
}


/*
DDSCL_MULTITHREADED
DDSCL_ALLOWREBOOT   //with exclusive
DDSCL_NOWINDOWCHANGES   //don't change the window

  TestCooperativeLevel  WaitMessage(WM_DISPLAYCHANGE)
  IDD7->RestoreAllSurfaces()

Developers often use messages such as WM_ACTIVATEAPP and WM_DISPLAYCHANGE as notifications that their applications should restore or re-create the surfaces being used. In some cases, applications take action when they don't need to, or don't take action when they should.
  This method must be called by the same thread that created the application window.
 */

/****************************************************************************
 *
 * CoopLevel -
 *
 ****************************************************************************/
DWORD    WindowDeviceDX::CoopLevel( void )
{
    return CoopLevelAlways() | (GetMode(WD_FULLSCREEN) ? CoopLevelFullscreen(): CoopLevelWindow());
}

/****************************************************************************
 *
 * CoopLevelAlways -
 *
 ****************************************************************************/
DWORD    WindowDeviceDX::CoopLevelAlways( void )
{
    return DDSCL_MULTITHREADED;
}

/****************************************************************************
 *
 * CoopLevelWindow -
 *
 ****************************************************************************/
DWORD    WindowDeviceDX::CoopLevelWindow( void )
{
    return DDSCL_NORMAL;
}

/****************************************************************************
 *
 * CoopLevelFullscreen -
 *
 ****************************************************************************/
DWORD    WindowDeviceDX::CoopLevelFullscreen( void )
{
    return DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT; //DDSCL_NOWINDOWCHANGES 
}


/****************************************************************************
 *
 * DisableDisplay - perhaps I should release the surfaces here?
 *                    Maybe the overlay should be hidden in overlay mode?
 *
 ****************************************************************************/
error_t    WindowDeviceDX::DisableDisplay( void )
{
    return SUCCESS;
}


#ifdef UNDEFINED
/****************************************************************************
 *
 * DDEnumCallbackEx - Enumerate device callback
 *
 ****************************************************************************/
BOOL WINAPI DDEnumCallbackEx( GUID FAR *lpGUID,
                              LPSTR     lpDriverDescription,
                              LPSTR     lpDriverName,
                              LPVOID    lpContext,
                              HMONITOR  hm )
{
//    DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES;

    return ((WindowDeviceDX*)lpContext)->DDEnumCallbackEx( lpGUID, lpDriverDescription,
                              lpDriverName,
                              lpContext,
                              hm );
}

/****************************************************************************
 *
 * DDEnumCallbackEx - Enumerate device callback
 *
 ****************************************************************************/
BOOL    WindowDeviceDX::DDEnumCallbackEx( GUID FAR *lpGUID,
                              LPSTR     lpDriverDescription,
                              LPSTR     lpDriverName,
                              LPVOID    lpContext,
                              HMONITOR  hm )
{

    DumpToFile( "ddenum.txt", "driver description ", lpDriverDescription );
    DumpToFile( "ddenum.txt", "\n driver name ", lpDriverName );
    DumpToFile( "ddenum.txt", "\n\n" );

    return TRUE;
}
#endif

#ifdef UNDEFINED
/****************************************************************************
 *
 * EnumModesCallback - Enumerate display modes callback
 *
 ****************************************************************************/
HRESULT    WINAPI    EnumModesCallback( LPDDSURFACEDESC2 lpDDSurfaceDesc,
  LPVOID lpContext )
{
//    DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES;

    return ((WindowDeviceDX*)lpContext)->EnumModesCallback( lpDDSurfaceDesc,
                                                            lpContext );
}

/****************************************************************************
 *
 * EnumModesCallback - Enumerate device callback
 *
 ****************************************************************************/
HRESULT    WindowDeviceDX::EnumModesCallback( LPDDSURFACEDESC2 lpDDSurfaceDesc,
  LPVOID lpContext )
{

//    DumpToFile( "ddenum.txt", "driver description ", lpDriverDescription );
//    DumpToFile( "ddenum.txt", "\n driver name ", lpDriverName );
//    DumpToFile( "ddenum.txt", "\n\n" );

//    lpDDSurfaceDesc.dwWidth;
//    lpDDSurfaceDesc.dwHeight;

    return DDENUMRET_OK;
}


/****************************************************************************
 *
 * CheckDeviceCapabilities - I need to clean up this code
 *
 ****************************************************************************/
error_t    WindowDeviceDX::CheckDeviceCapabilities( void )
{
    error_t    err = SUCCESS;
//        DDCAPS_OVERLAY

    ZeroMemory(&m_ddDriverCaps, sizeof(m_ddDriverCaps));
    m_ddDriverCaps.dwSize = sizeof(m_ddDriverCaps);
    ZeroMemory(&m_ddHELCaps, sizeof(m_ddHELCaps));
    m_ddHELCaps.dwSize = sizeof(m_ddHELCaps);

    if( GetDD()->GetCaps(&m_ddDriverCaps, &m_ddHELCaps) == DD_OK )
    {
/*
    DWORD    dwMaxVisibleOverlays;
    DWORD    dwCurrVisibleOverlays;
    DWORD    dwNumFourCCCodes;
    DWORD    dwAlignBoundarySrc;
    DWORD    dwAlignSizeSrc;
    DWORD    dwAlignBoundaryDest;
    DWORD    dwAlignSizeDest;
    DWORD    dwAlignStrideAlign;
    DWORD    dwMinOverlayStretch;
    DWORD    dwMaxOverlayStretch;
    */
#ifdef EXTREME_DEBUGGING
        DumpToFile( "overlaycaps.txt", "DDCAPS_OVERLAY ", m_ddDriverCaps.dwCaps & DDCAPS_OVERLAY, "\n");
        DumpToFile( "overlaycaps.txt", "DDCAPS_3D ", m_ddDriverCaps.dwCaps & DDCAPS_3D, "\n");
        DumpToFile( "overlaycaps.txt", "dwVidMemTotal ", m_ddDriverCaps.dwVidMemTotal, "\n");
        DumpToFile( "overlaycaps.txt", "dwVidMemFree ", m_ddDriverCaps.dwVidMemFree, "\n");
        DumpToFile( "overlaycaps.txt", "max visible overlays ", m_ddDriverCaps.dwMaxVisibleOverlays, "\n");
        DumpToFile( "overlaycaps.txt", "current visible overlays ", m_ddDriverCaps.dwCurrVisibleOverlays, "\n");
        DumpToFile( "overlaycaps.txt", "num FourCC codes ", m_ddDriverCaps.dwNumFourCCCodes, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignBoundarySrc ", m_ddDriverCaps.dwAlignBoundarySrc, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignSizeSrc ", m_ddDriverCaps.dwAlignSizeSrc, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignBoundaryDest ", m_ddDriverCaps.dwAlignBoundaryDest, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignSizeDest ", m_ddDriverCaps.dwAlignSizeDest, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignStrideAlign ", m_ddDriverCaps.dwAlignStrideAlign, "\n");
        DumpToFile( "overlaycaps.txt", "dwMinOverlayStretch ", m_ddDriverCaps.dwMinOverlayStretch / 1000.0, "\n");
        DumpToFile( "overlaycaps.txt", "dwMaxOverlayStretch ", m_ddDriverCaps.dwMaxOverlayStretch / 1000.0, "\n");
#endif
    
    
    }
    else
        err = FAILURE;


    return err;
}
#endif

/****************************************************************************
 *
 * InitDisplay - I need to clean up this code
 *
 ****************************************************************************/
error_t    WindowDeviceDX::InitDisplay( void )
{
    error_t             err  = SUCCESS;
    HRESULT             hRet = DD_OK;
    DDSURFACEDESC2      ddsd;
    DDSURFACEDESC2      ddsdMask;
    DWORD               dwDefaultRefreshRate = 0;
    DWORD               dwAdditionalOptions  = 0;
    LPDIRECTDRAWCLIPPER pClipper;

#ifdef EXTREME_DEBUGGING
        DumpToFile( "error.txt", "Beginning initialization of the display.", "\n" );
#endif


//    if( IsSwitchingModes() )
//        return 1;  /* I know this will cause problems!  Greenshift will halt on all display errors */

    SetSwitchingModes( true );  /* SEE IF THIS SOLVES THE CRASH WHILE SWITCHING MODES DEFECT */
    EnterCS();
    LockDX();

    ReleaseAllObjects();    /* kill any DD objects */

//    hRet = DirectDrawEnumerateEx( ::DDEnumCallbackEx, this, DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES );
//    if( hRet != DD_OK )
//        return hRet;

#ifdef EXTREME_DEBUGGING
    DumpToFile( "error.txt", "about to create the DirectDraw 7 object", "\n" );
#endif
    /*
     * Create the DirectDraw object if one hasn't already been created
     */
    if( GetDD() == NULL )
    {
        hRet = DirectDrawCreateEx(NULL, (void **)PGetDD(), IID_IDirectDraw7, NULL);
//        if( hRet != DD_OK )
//            return hRet;
#ifdef EXTREME_DEBUGGING
        if( hRet == DD_OK )
            DumpToFile( "error.txt", "created the DirectDraw 7 object", "\n" );
        else
        {
            DumpToFile( "error.txt", "failed to create the DirectDraw 7 object", "\n" );
            DumpToFile( "error.txt", DXError( hRet ), "\n" );
        }
#endif
    }


#ifdef UNDEFINED
    if( hRet == DD_OK )
    err = CheckDeviceCapabilities();
//    if( err != SUCCESS )
//        return err;

    if( hRet == DD_OK )
        hRet = GetDD()->EnumDisplayModes( 0, NULL, this, ::EnumModesCallback );
#endif

    /*
     * Set the cooperative level
     */
    if( hRet == DD_OK )
        hRet = GetDD()->SetCooperativeLevel( GetWindow(), CoopLevel() );
//    if( hRet != DD_OK )
//        return hRet;
#ifdef EXTREME_DEBUGGING
        if( hRet != DD_OK )
        {
            DumpToFile( "error.txt", "ERROR: ", "" );
            DumpToFile( "error.txt", DXError( hRet ), "\n" );
        }
#endif


    /*
     * set the display mode, if going fullscreen
     */
    if( hRet == DD_OK )
    if( GetMode(WD_FULLSCREEN) )
    {
        /*
         * set screen mode before creating the surfaces
         */
        hRet = GetDD()->SetDisplayMode( Width(), Height(), BitDepth(), dwDefaultRefreshRate, dwAdditionalOptions );
        if( hRet != DD_OK )
        {
            if( BitDepth() == 32 ) /* should actually check the capabilities of the display */
            {
                /*m_dwBitDepth = 24;  /* only tries 24 bit if 32 bit fails */
                hRet = GetDD()->SetDisplayMode( Width(), Height(), 24, dwDefaultRefreshRate, dwAdditionalOptions );
//                if( hRet != DD_OK )
//                    return hRet;
#ifdef EXTREME_DEBUGGING
        if( hRet != DD_OK )
        {
            DumpToFile( "error.txt", "ERROR: ", "" );
            DumpToFile( "error.txt", DXError( hRet ), "\n" );
        }
#endif
            }
//            else
//                return hRet;
        }
    } /* IsFullscreen() */



    /*
     * create mask surface
     */


        /* Create a clipper object since this is for a Windowed render */
    if( hRet == DD_OK )
    hRet = GetDD()->CreateClipper(0, &pClipper, NULL);
//    if (hRet != DD_OK)
//      return hRet;

#ifdef EXTREME_DEBUGGING
        if( hRet != DD_OK )
        {
            DumpToFile( "error.txt", "ERROR: ", "" );
            DumpToFile( "error.txt", DXError( hRet ), "\n" );
        }
#endif


    if( hRet == DD_OK )
    if( GetMode(WD_OVERLAY) )
    {
        ZeroMemory(&ddsdMask, sizeof(ddsdMask));
        ddsdMask.dwSize         = sizeof(ddsdMask);
        ddsdMask.dwFlags        = DDSD_CAPS;
    //    ddsdMask.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;
        ddsdMask.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;


        //create the primary surface
        hRet = GetDD()->CreateSurface(&ddsdMask, PGetMask(), NULL);
//        if(hRet != DD_OK)
//            return hRet;

#ifdef EXTREME_DEBUGGING
        if( hRet != DD_OK )
        {
            DumpToFile( "error.txt", "ERROR: ", "" );
            DumpToFile( "error.txt", DXError( hRet ), "\n" );
        }
#endif

            /* Associate the clipper with the window */
        if( hRet == DD_OK )
        {
            pClipper->SetHWnd(0, GetWindow());
            if( ! GetMode(WD_FULLSCREEN) )
            {
                GetMask()->SetClipper(pClipper);  /* use a clipper so the clear screen is confined to the window */
            }
            pClipper->Release();
            pClipper = NULL;
        }
    }








    /**
     *
     *create display surfaces
     */


    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
//    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;
    if( GetMode(WD_OVERLAY) )
        ddsd.ddsCaps.dwCaps    = DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;
    else
        ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;

    if( ShouldFlip() )
    {
        ddsd.dwFlags        |= DDSD_BACKBUFFERCOUNT;
        ddsd.dwBackBufferCount = 1;
        ddsd.ddsCaps.dwCaps    |= DDSCAPS_COMPLEX | DDSCAPS_FLIP;

    }

    if( hRet == DD_OK )
    if( GetMode(WD_OVERLAY) )
    {
//        ZeroMemory( &m_ddpfPixelFormat, sizeof(m_ddpfPixelFormat) );
//        m_ddpfPixelFormat.dwSize = sizeof(m_ddpfPixelFormat);

//        m_ddpfPixelFormat.dwRBitMask = 0xF800;
//        m_ddpfPixelFormat.dwGBitMask = 0x07E0;
//        m_ddpfPixelFormat.dwBBitMask = 0x001F;

//        m_ddpfPixelFormat.dwRGBBitCount = 16;
//        m_ddpfPixelFormat.dwFlags = DDPF_RGB;

//        m_ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
//        m_ddpfPixelFormat.dwRBitMask = 0x00FF0000;
//        m_ddpfPixelFormat.dwGBitMask = 0x0000FF00;
//        m_ddpfPixelFormat.dwBBitMask = 0x000000FF;

//        m_ddpfPixelFormat.dwRGBBitCount = 32;
//        m_ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPREMULT;

        GetMask()->GetSurfaceDesc(&ddsdMask);  /* get the pixel information */


        ddsd.dwFlags          |= DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.dwWidth           = Width();
        ddsd.dwHeight          = Height();

//        ddsd.ddckCKDestBlt.dwColorSpaceLowValue  = ColorKey();
//        ddsd.ddckCKDestBlt.dwColorSpaceHighValue = ColorKey();
//        ddsd.ddckCKDestOverlay.dwColorSpaceLowValue  = ColorKey();
//        ddsd.ddckCKDestOverlay.dwColorSpaceHighValue = ColorKey();
        ddsd.ddpfPixelFormat = ddsdMask.ddpfPixelFormat;
//        ddsd.ddpfPixelFormat = m_ddpfPixelFormat;

        // Could I SetSurfaceDesc() to get the pixel format to work?

        /*
         * the above thing does not work.
         *
         * the below structure is what does the magic
         */
    PGetFX()->dckDestColorkey.dwColorSpaceHighValue = ColorKey();
    PGetFX()->dckDestColorkey.dwColorSpaceLowValue  = ColorKey();

    }

#ifdef EXTREME_DEBUGGING
        DumpToFile( "error.txt", "about to create the display surface", "\n" );
#endif

    //create the primary surface
    if( hRet == DD_OK )
        hRet = GetDD()->CreateSurface(&ddsd, PGetPrimary(), NULL);
//    DumpToFile("error.txt", DXError(hRet), "\n");
//    if(hRet != DD_OK)
//        return hRet;

#ifdef EXTREME_DEBUGGING
        if( hRet != DD_OK )
        {
            DumpToFile( "error.txt", "ERROR: ", "" );
            DumpToFile( "error.txt", DXError( hRet ), "\n" );
        }
#endif


        // Create a clipper object since this is for a Windowed render
    if( hRet == DD_OK )
    hRet = GetDD()->CreateClipper(0, &pClipper, NULL);
//    if (hRet != DD_OK)
//        return hRet;

        // Associate the clipper with the window
    if( hRet == DD_OK )
    {
        if( ! GetMode(WD_FULLSCREEN) && ! GetMode(WD_OVERLAY) )
        {
            pClipper->SetHWnd(0, GetWindow());
            GetPrimary()->SetClipper(pClipper);  /* provide clipping so I don't overwrite windows over top of me */
        }
        pClipper->Release();
        pClipper = NULL;


        if( ShouldFlip() )
        {

            ddsd.dwFlags        |= DDSD_WIDTH | DDSD_HEIGHT;
            ddsd.dwWidth        = Width();
            ddsd.dwHeight       = Height();/**/
            /* this is returning an error */
            hRet = GetPrimary()->GetAttachedSurface( &ddsd.ddsCaps, PGetBack() );
        }
        else

        {

                /* Get the backbuffer. For fullscreen mode, the backbuffer was created
                   along with the primary, but windowed mode still needs to create one. */
            ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
            ddsd.dwWidth        = Width();
            ddsd.dwHeight       = Height();
            ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY ;
            hRet = GetDD()->CreateSurface(&ddsd, PGetBack(), NULL);  /* create back buffer */
//            if (hRet != DD_OK)
//                return FAILURE;
        }
    } /* if( hRet == DD_OK ) */


//    if (hRet != DD_OK)
//        return FAILURE;

/*********
 Init Palette
**********/

    if( hRet == DD_OK )
    if( BitDepth() == 8  && GetMode(WD_FULLSCREEN) )
    {
        int                     i;
        PALETTEENTRY            ape[256];

        for (i = 0; i < 256; i++)
        {
            ape[i].peRed   = (BYTE) (((i >> 5) & 0x07) * 255 / 7) >> 2;
            ape[i].peGreen = (BYTE) (((i >> 2) & 0x07) * 255 / 7);
            ape[i].peBlue  = (BYTE) (((i >> 0) & 0x03) * 255 / 3) >> 2;
            ape[i].peFlags = (BYTE) 0;
            ape[i].peRed   = (BYTE) 0;
    //        ape[i].peGreen = (BYTE) (((i >> 2) & 0x07) * 255 / 7);
            ape[i].peBlue  = (BYTE) 0;
            ape[i].peRed   = (BYTE) (i > 200)? i: 0;
            ape[i].peGreen = (BYTE) i;
            ape[i].peBlue  = (BYTE) (i > 200)? i: 0;
            ape[i].peFlags = (BYTE) 0;
        }

#ifdef EXTREME_DEBUGGING
        DumpToFile( "error.txt", "about to set the palette", "\n" );
#endif

    //    return (*pDDPalette)->SetEntries( 0, 0, 256, ape );
    //    DumpToFile("checkpalette.txt", DXError( (*pDDPalette)->SetEntries( 0, 0, 256, ape ) ), "\n" );

    //    MessageBox(NULL, "hello", "hello", MB_OK);
        /* ADDED PRIMARY SURFACE, REMOVE IF MESSED UP!  ugh!  still doesn't work... */
        EnterCS();
            if( GetBitCanvas() != NULL )
                hRet = GetDD()->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, GetBitCanvas()->PGetPalette()->palPalEntry, PGetPalette(), NULL);
            else
                hRet = GetDD()->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, ape, PGetPalette(), NULL);
        LeaveCS();
//        if( hRet != DD_OK )
//            return hRet;


        /*
         * set the palette for the front buffer
         */
        if( hRet == DD_OK )
            hRet = GetPrimary()->SetPalette( GetPalette() );
//        if( hRet != DD_OK )
//            return hRet;
#ifdef EXTREME_DEBUGGING
        if( hRet != DD_OK )
            DumpToFile( "error.txt", "successfully set the palette", "\n" );
        else
        {
            DumpToFile( "error.txt", "ERROR: ", "" );
            DumpToFile( "error.txt", DXError( hRet ), "\n" );
        }
#endif

    }
//    if (hRet != DD_OK)
//        return FAILURE;


    SetSwitchingModes( false );  /* should this go outside the locked section? */
    UnlockDX();
    LeaveCS();

#ifdef EXTREME_DEBUGGING
        DumpToFile( "error.txt", "Finished initializing the display.", "\n" );
#endif

    /*
     * clear all the surfaces
     */
    return ClearScreenAll();
}





/****************************************************************************
 *
 * SetPalette
 *
 ****************************************************************************/
HRESULT    WindowDeviceDX::SetPalette( LOGPALETTE *lpPalette )
{
    HRESULT    hRet;

    /*
     * ignore if I'm not fullscreen, or I'm not in paletted mode
     */
    if( ! GetMode(WD_FULLSCREEN) || BitDepth() != 8 )
        return DD_OK;


    if( lpPalette == NULL )
        return DD_OK;  /* what else could I return? */

//    hRet = GetDD()->CreatePalette(DDPCAPS_8BIT, GetBitCanvas()->PGetPalette()->palPalEntry, PGetPalette(), NULL);
//    hRet = GetPrimary()->SetPalette( GetPalette() );

    EnterCS();

//    hRet = PGetPalette()->SetEntries( 0, 0, lpPalette->palNumEntries, lpPalette->palPalEntry );
    hRet = m_pDDPalette->SetEntries( 0, 0, lpPalette->palNumEntries, lpPalette->palPalEntry );

    LeaveCS();

    return hRet;
}




/****************************************************************************
 *
 * ReleaseAllObjects
 *
 ****************************************************************************/
void WindowDeviceDX::ReleaseAllObjects( void )
{
    if( GetDD() != NULL )
        GetDD()->SetCooperativeLevel( GetWindow(), DDSCL_NORMAL );
    UpdateWindowProperties();

    ReleaseAllSurfaces();
}




/****************************************************************************
 *
 * ReleaseAllObjects
 *
 ****************************************************************************/
void WindowDeviceDX::ReleaseAllSurfaces( void )
{
    // shouldn't release the back buffer if a flipping chain is used
    if( GetBack() != NULL )
    {
        GetBack()->Release();
        *PGetBack() = NULL;
    }
    if( GetPrimary() != NULL )
    {
        GetPrimary()->Release();
        *PGetPrimary() = NULL;
    }
    if( GetPalette() != NULL )
    {
        GetPalette()->Release();
        *PGetPalette() = NULL;
    }
    if( GetMask() != NULL )
    {
        GetMask()->Release();
        *PGetMask() = NULL;
    }
}











    /*****/
#ifdef UNDEFINED

    DDCAPS    driverCaps;
    DDCAPS    helCaps;
    ZeroMemory(&driverCaps, sizeof(driverCaps));
    driverCaps.dwSize = sizeof(driverCaps);
    ZeroMemory(&helCaps, sizeof(helCaps));
    helCaps.dwSize    = sizeof(helCaps);

    if( GetDD()->GetCaps(&driverCaps, &helCaps) == DD_OK )
    {
/*
    DWORD    dwMaxVisibleOverlays;
    DWORD    dwCurrVisibleOverlays;
    DWORD    dwNumFourCCCodes;
    DWORD    dwAlignBoundarySrc;
    DWORD    dwAlignSizeSrc;
    DWORD    dwAlignBoundaryDest;
    DWORD    dwAlignSizeDest;
    DWORD    dwAlignStrideAlign;
    DWORD    dwMinOverlayStretch;
    DWORD    dwMaxOverlayStretch;
    */

        DumpToFile( "overlaycaps.txt", "DDCAPS_OVERLAY ", driverCaps.dwCaps & DDCAPS_OVERLAY, "\n");
        DumpToFile( "overlaycaps.txt", "DDCAPS_3D ", driverCaps.dwCaps & DDCAPS_3D, "\n");
        DumpToFile( "overlaycaps.txt", "dwVidMemTotal ", driverCaps.dwVidMemTotal, "\n");
        DumpToFile( "overlaycaps.txt", "dwVidMemFree ", driverCaps.dwVidMemFree, "\n");
        DumpToFile( "overlaycaps.txt", "max visible overlays ", driverCaps.dwMaxVisibleOverlays, "\n");
        DumpToFile( "overlaycaps.txt", "current visible overlays ", driverCaps.dwCurrVisibleOverlays, "\n");
        DumpToFile( "overlaycaps.txt", "num FourCC codes ", driverCaps.dwNumFourCCCodes, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignBoundarySrc ", driverCaps.dwAlignBoundarySrc, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignSizeSrc ", driverCaps.dwAlignSizeSrc, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignBoundaryDest ", driverCaps.dwAlignBoundaryDest, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignSizeDest ", driverCaps.dwAlignSizeDest, "\n");
        DumpToFile( "overlaycaps.txt", "dwAlignStrideAlign ", driverCaps.dwAlignStrideAlign, "\n");
        DumpToFile( "overlaycaps.txt", "dwMinOverlayStretch ", driverCaps.dwMinOverlayStretch / 1000.0, "\n");
        DumpToFile( "overlaycaps.txt", "dwMaxOverlayStretch ", driverCaps.dwMaxOverlayStretch / 1000.0, "\n");

    
    
    }

#endif
    /*****/

/*
 Programming Notes
=================
  For details on how to setup a windowed mode DirectDraw app, see the WindowedMode
  sample. 
  
  To use overlays in general do the following steps in addition to those needed to 
  author a windowed mode DirectDraw app:

    1. Check to see if hardware supports overlays - check IDirectDraw::GetCaps 
       for DDCAPS_OVERLAY.

    2. Size the window to meet the hardware overlay size restrictions.

    3. Create an overlay surface (create it with 1 backbuffer if needed), and 
       set its pixel format to a desired format that is supported by the device.  

    4. Set the dest color key on the overlay to the background color of the window. 
       Be sure to choose a color for the background of the window that Windows typically
       does not use otherwise, the overlay will be drawn on top of overlapping windows.

    5. Call UpdateOverlay to display or hide the overlay on the desktop.  WM_PAINT is 
       a good place for this.

    6. When WM_SIZE or WM_MOVE is sent, then update the src and dest rects and 
       check to make sure that they are within the hardware limits.
       
  To animate the overlay, instead of drawing to the off-screen plain back buffer as 
  in windowed mode case, just draw to the the overlay's backbuffer then flip it. 
*/

/****************************************************************************
 *
 * UpdateOverlayProperties
 *
 ****************************************************************************/
error_t    WindowDeviceDX::UpdateOverlayProperties( void  )
{
    HRESULT        hRet = DD_OK;
    RECT        srcRect;
    RECT        dstRect;

    if( ! GetMode(WD_OVERLAY) )
        return SUCCESS;

    GetClientRect(GetWindow(), &dstRect);
    ClientToScreen(GetWindow(), (POINT*)&dstRect.left);
    ClientToScreen(GetWindow(), (POINT*)&dstRect.right);

    srcRect.left   = 0;
    srcRect.top    = 0;
    srcRect.right  = Width();
    srcRect.bottom = Height();


    
    if( GetPrimary() != NULL && GetMask() != NULL )
    {
#ifdef UNDEFINED
        if( GetMode(WD_FULLSCREEN) )
        hRet = GetPrimary()->UpdateOverlay( &srcRect, GetMask(), &srcRect, DDOVER_SHOW, NULL );
        else
        hRet = GetPrimary()->UpdateOverlay( &srcRect, GetMask(), &dstRect, DDOVER_SHOW, NULL );
#else
        if( GetMode(WD_FULLSCREEN) )
        hRet = GetPrimary()->UpdateOverlay( &srcRect, GetMask(), &srcRect, DDOVER_SHOW | DDOVER_KEYDESTOVERRIDE, PGetFX() );
        else
        hRet = GetPrimary()->UpdateOverlay( &srcRect, GetMask(), &dstRect, DDOVER_SHOW | DDOVER_KEYDESTOVERRIDE, PGetFX() );
#endif

    }
    if( hRet != DD_OK )
        return FAILURE;

    return FillSurface( GetMask(), ColorKey() );
}

