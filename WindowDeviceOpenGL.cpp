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

#if 0 // Disabling OpenGL because I think it doesn't work since I really need a pointer to video memory

#include "WindowDeviceOpenGL.h"



// Enable OpenGL

VOID EnableOpenGL( HWND hWnd, HDC * hDC, HGLRC * hRC );

// Disable OpenGL

VOID DisableOpenGL( HWND hWnd, HDC hDC, HGLRC hRC );

/*
 * Note! Recently it was pointed out to me that under some OpenGL ICDs
 * (ATI Rage Pro, RivaTNT, etc.), the window style of CS_OWNDC must be
 * included in the window class. Otherwise OpenGL will not function.
 * This information was provided by Ryan Haksi (thanx Ryan!). I have
 * corrected this in the GLSAMPLE program given at the end.
 */

/****************************************************************************
 *
 * WindowDeviceOpenGL - constructor
 *
 ****************************************************************************/
WindowDeviceOpenGL::WindowDeviceOpenGL()
{
    m_hDC   = NULL;
    m_hGLRC = NULL;
}


/****************************************************************************
 *
 * ~WindowDeviceOpenGL - destructor
 *
 ****************************************************************************/
WindowDeviceOpenGL::~WindowDeviceOpenGL()
{
    ::DisableOpenGL(GetWindow(), m_hDC, m_hGLRC );
}


/*
 * NEED to distinguish between the functions called from the thread and those not called from thread.

  are public and protected already split like that?  Not necessarily, since public
  functions may call protected ones.
 */


/****************************************************************************
 *
 * EnableDisplay - called from the ThreadProcedure
 *
 ****************************************************************************/
error_t    WindowDeviceOpenGL::InitDisplay( void )
{
    error_t    err = SUCCESS;

    ::EnableOpenGL( GetWindow(), &m_hDC, &m_hGLRC );

    return err;
}


/****************************************************************************
 *
 * DisableDisplay - called from the ThreadProcedure
 *
 ****************************************************************************/
error_t    WindowDeviceOpenGL::DisableDisplay( void )
{
    error_t    err = SUCCESS;

    ::DisableOpenGL( GetWindow(), m_hDC, m_hGLRC );

    return err;
}


/****************************************************************************
 *
 * GetPFD
 *
 ****************************************************************************/
PIXELFORMATDESCRIPTOR    WindowDeviceOpenGL::GetPFD( void )
{
    return m_pfdPixelFormat;
}

/****************************************************************************
 *
 * PGetPFD
 *
 ****************************************************************************/
PIXELFORMATDESCRIPTOR    *WindowDeviceOpenGL::PGetPFD( void )
{
    return &m_pfdPixelFormat;
}



/****************************************************************************
 *
 * GetRC
 *
 ****************************************************************************/
HGLRC    WindowDeviceOpenGL::GetRC( void )
{
    return m_hGLRC;
}

/****************************************************************************
 *
 * PGetRC
 *
 ****************************************************************************/
HGLRC    *WindowDeviceOpenGL::PGetRC( void )
{
    return &m_hGLRC;
}

/****************************************************************************
 *
 * ReleaseRC
 *
 ****************************************************************************/
HRESULT    WindowDeviceOpenGL::ReleaseRC( HGLRC hRC, HDC hDC )
{
  wglMakeCurrent( NULL, NULL );
  wglDeleteContext( hRC );
  return ReleaseDC( hDC );
}


/****************************************************************************
 *
 * GetDC
 *
 ****************************************************************************/
HRESULT    WindowDeviceOpenGL::GetDC( HDC *pHDC )
{
//    DumpToFile("error.txt", glGetError(), "\n" );
//    *pHDC = ::GetDC( GetWindow() );
    *pHDC = m_hDC;
    return SUCCESS;
}


/****************************************************************************
 *
 * ReleaseDC
 *
 ****************************************************************************/
HRESULT    WindowDeviceOpenGL::ReleaseDC( HDC hdc )
{
//    return (::ReleaseDC( GetWindow(), hdc ) == 1) ? SUCCESS : FAILURE;
    return SUCCESS;
}


/****************************************************************************
 *
 * OnResize
 *
 ****************************************************************************/
void    WindowDeviceOpenGL::OnResize( void )
{
    /* UpdateOverlayProperties(); */
}

/****************************************************************************
 *
 * OnMove
 *
 ****************************************************************************/
void    WindowDeviceOpenGL::OnMove( void )
{
    /* UpdateOverlayProperties(); */
//    ClearScreen();
}


/****************************************************************************
 *
 * ClearScreen
 *
 ****************************************************************************/
error_t    WindowDeviceOpenGL::ClearScreen( void )
{
    error_t err = SUCCESS;
//    void *pPixels;
//    DWORD    nWidth;
//    DWORD    nHeight;
//    DWORD    nPixelFormat;


//    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
//    glClear( GL_COLOR_BUFFER_BIT );
//    SwapBuffers( m_hDC );


#ifdef UNDEFINED
    EnterCS();

    if( GetBitCanvas() != NULL )
    {

        pPixels = GetBitCanvas()->GetPixelMemory(&nWidth, &nHeight, &nPixelFormat);
        if( pPixels == NULL )
            err = FAILURE;

        if( err == SUCCESS )
        {
 // DumpToFile("error.txt", "Finished Blit!\n" );
//    DumpToFile("error.txt", glGetError(), "\n" );

//    glReadBuffer( GL_BACK );
//    glDrawBuffer( GL_FRONT );
    glDrawBuffer( GL_BACK );

//    glRasterPos2i( 0, 0 );
    glDrawPixels( nWidth, nHeight, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);
//    glDrawPixels( nWidth, nHeight, GL_RGBA, GL_UNSIGNED_INT, pPixels);
//    glBitmap( nWidth, nHeight, 0.0f, 0.0f, 0.0f, 0.0f, pPixels );
//    glReadPixels( (640 - nWidth) >> 1, (480 - nHeight)>> 1, nWidth, nHeight, GL_COLOR );
//    glCopyPixels( (640 - nWidth) >> 1, ((480 - nHeight)>> 1) + nHeight, nWidth, nHeight, GL_COLOR );

    
            SwapBuffers( m_hDC );

        }

    }

    LeaveCS();
#endif

    return err;
}



/****************************************************************************
 *
 * ClearScreenAll
 *
 ****************************************************************************/
error_t    WindowDeviceOpenGL::ClearScreenAll( void )
{
//    glDrawBuffer( GL_FRONT_AND_BACK );
    return ClearScreen();
}






/****************************************************************************
 *
 * Blit - 
 *
 ****************************************************************************/
HRESULT    WindowDeviceOpenGL::Blit( void )
{
    HRESULT hRet = SUCCESS;
    error_t err  = SUCCESS;
//    HDC        hDC;
//    HGLRC    hRC;
    void *pPixels;
    DWORD    nWidth;
    DWORD    nHeight;
    DWORD    nPixelFormat;

//    hRet = GetRC( &hRC, &hDC );
//    if( hRet != SUCCESS )
//        return hRet;

    if( GetBitCanvas() != NULL )
    {

        pPixels = GetBitCanvas()->GetPixelMemory(&nWidth, &nHeight, &nPixelFormat);
        if( pPixels == NULL )
            err = FAILURE;

        if( err == SUCCESS )
        {
 // DumpToFile("error.txt", "Finished Blit!\n" );
//    DumpToFile("error.txt", glGetError(), "\n" );

//    glDrawBuffer( GL_FRONT );
//    glDrawBuffer( GL_BACK );

//    glRasterPos2i( 0, 0 );
//    glRasterPos2d( 0, (((480 - nHeight) >> 1) + nHeight) / -480.0f );
    glRasterPos2d( -(value_t)nWidth / (value_t)Width(), -(value_t)nHeight / (value_t)Height() );
    glDrawPixels( nWidth, nHeight, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);
//    glDrawPixels( nWidth, nHeight, GL_RGBA, GL_UNSIGNED_INT, pPixels);

//    SwapBuffers( m_hDC );
        }

    }



    return hRet;
}


/****************************************************************************
 *
 * OnFlip - 
 *
 ****************************************************************************/
HRESULT    WindowDeviceOpenGL::OnFlip( void )
{
    HRESULT        hRet = SUCCESS;
/*    HDC            hDC;

    hRet = GetDC( &hDC );

    if( hRet != SUCCESS )
        return hRet;

//    hRet = SwapBuffers( hDC );

    return ReleaseDC( hDC );
    /**/
//  DumpToFile("error.txt", "Entering Flip!\n" );
//  wglMakeCurrent( m_hDC, GetRC() );

    SwapBuffers( m_hDC );

//    DumpToFile("error.txt", (DWORD)m_hDC, "\n" );
//  DumpToFile("error.txt", "Finished Flip!\n" );


    return hRet;/**/
}
/*
    srcRect.left   = 0;
    srcRect.top    = 0;
    srcRect.right  = Width();
    srcRect.bottom = Height();
    GetClientRect(GetWindow(), &dstRect );
    ClientToScreen(GetWindow(), (POINT*)&dstRect.left);
    ClientToScreen(GetWindow(), (POINT*)&dstRect.right);
*/

#if EXTREME_DEBUGGING
/****************************************************************************
 *
 * Update - 
 *
 ****************************************************************************/
void    WindowDeviceOpenGL::Update( const long symbol )
{
    HRESULT hRet = SUCCESS;
    HRESULT hRet2 = SUCCESS;

    EnterCS();

    //make a "blit" function!
    if( symbol == 'Flop' && GetBitCanvas() != NULL )
    {
        if( (hRet = Blit()) != SUCCESS 
            || (hRet2 = Flip()) != SUCCESS 
            )
        {
            if( hRet2 == SUCCESS )
            DumpToFile( "error.txt", ErrorString(hRet), " 'Update:Blit'\n");//, ErrorString(hRet) );
            else
            DumpToFile( "error.txt", ErrorString(hRet2), " 'Update:OnFlip'\n");//, ErrorString(hRet) );
        }

    }

    LeaveCS();
}
#endif



// Enable OpenGL

VOID EnableOpenGL( HWND hWnd, HDC * hDC, HGLRC * hRC )
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    // Get the device context (DC)
    *hDC = GetDC( hWnd );

    // Set the pixel format for the DC
    ZeroMemory( & pfd, sizeof( pfd ) );
    pfd.nSize        = sizeof( pfd );
    pfd.nVersion    = 1;
    pfd.dwFlags        = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits    = 24;
    pfd.cDepthBits    = 16;
    pfd.iLayerType    = PFD_MAIN_PLANE;
    iFormat            = ChoosePixelFormat( *hDC, &pfd );
    SetPixelFormat( *hDC, iFormat, &pfd );

    // Create and enable the render context (RC)
    *hRC            = wglCreateContext( * hDC);
    wglMakeCurrent( *hDC, *hRC);
}

// Disable OpenGL

VOID DisableOpenGL( HWND hWnd, HDC hDC, HGLRC hRC )
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );
}

#endif