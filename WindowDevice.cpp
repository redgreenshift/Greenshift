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
 * WindowDevice
 *
 ****************************************************************************/

#include "WindowDevice.h"
#include "HighResolutionTimer.h"

/*
case WM_ACTIVATE:
        g_active = (LOWORD(w) != WA_INACTIVE) && !((BOOL)HIWORD(w));
        break;
    */

#define TIMER_STOPPED    -1
#define IDT_TIMER1 1

void MakeCursorVisible( void );
void MakeCursorInvisible( void );


/****************************************************************************
 *
 * WindowDevice
 *
 ****************************************************************************/
WindowDevice::WindowDevice()
{
    static const char strClassName[] = "GreenshiftWindowDevice";

    InitializeCriticalSection(&m_csSwitchingModes);

    m_dwWidth           = 640;
    m_dwHeight          = 480;
    m_dwBitDepth        = 8;
    m_dwFlags           = WD_DEFAULT;
    m_dwOverlayColorKey = 0x00000000;
    m_strWindowTitle[0] = '\0';
    m_pBitCanvas        = NULL;
    m_bShowFramerate    = false;
    m_bShowDebug        = false;
    m_bSwitchingModes   = false;

    m_hParentWindow     = NULL;

    m_dwWindowWidth      = 0;
    m_dwWindowHeight     = 0;
    m_dwFullscreenWidth  = 0;
    m_dwFullscreenHeight = 0;

    m_nWidth  = m_nWindowWidth  = m_nFullscreenWidth  = 640;
    m_nHeight = m_nWindowHeight = m_nFullscreenHeight = 480;
    m_nBitDepth = 8;

    /*
     * initialize the class description structure
     */
    ZeroMemory( &m_hWindowClass, sizeof(m_hWindowClass) );
    m_hWindowClass.cbSize        = sizeof(m_hWindowClass);
    m_hWindowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
    m_hWindowClass.cbClsExtra    = 0;
    m_hWindowClass.cbWndExtra    = sizeof(WindowDevice*);
    m_hWindowClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    m_hWindowClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    m_hWindowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    m_hWindowClass.lpszMenuName  = NULL;
    m_hWindowClass.lpszClassName = strClassName;
    m_hWindowClass.hIconSm       = NULL;

    m_hWindowClass.lpfnWndProc   = DefWindowProc;
    m_hWindowClass.hInstance     = NULL;
}


/****************************************************************************
 *
 * ~WindowDevice - defalut destructor
 *
 ****************************************************************************/
WindowDevice::~WindowDevice()
{
//    SetBitCanvas( NULL );

    UnregisterClass( m_hWindowClass.lpszClassName, m_hWindowClass.hInstance );

    DeleteCriticalSection(&m_csSwitchingModes);
}



/****************************************************************************
 *
 * Initialize - initializes
 *
 ****************************************************************************/
error_t    WindowDevice::Initialize( const char *strWindowTitle,
                                     const HINSTANCE hInstance,
                                     const WNDPROC hDefWindowProc,
                                     const HWND hParentWindow,
                                     MyDictionary<char*> *inConfig,
                                     MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals,
                                     MyDictionary<value_t*> *inValues)
{
    error_t err = SUCCESS;

    if( inConfig == NULL )
        return ERR_NULL;

    if( inValues != NULL )
    {
        inValues->SetValue("screen_depth",  &m_nBitDepth);
        inValues->SetValue("buffer_width",  &m_nWidth);
        inValues->SetValue("buffer_height", &m_nHeight);
        inValues->SetValue("window_width",  &m_nWindowWidth);
        inValues->SetValue("window_height", &m_nWindowHeight);
        inValues->SetValue("screen_width",  &m_nFullscreenWidth);
        inValues->SetValue("screen_height", &m_nFullscreenHeight);
    }

    m_hParentWindow     = hParentWindow;

    strcpy( m_strWindowTitle, strWindowTitle );

    m_dwBitDepth = (DWORD)Expression::Evaluate(inConfig->GetValue("screen_depth" ), m_nBitDepth, inValues, inGlobals );
    m_nBitDepth  = (value_t)m_dwBitDepth;
    m_dwWidth    = (DWORD)Expression::Evaluate(inConfig->GetValue("buffer_width" ), m_nWidth, inValues, inGlobals );
    m_nWidth     = (value_t)m_dwWidth;
    m_dwHeight   = (DWORD)Expression::Evaluate(inConfig->GetValue("buffer_height"), m_nHeight, inValues, inGlobals );
    m_nHeight    = (value_t)m_dwHeight;

    m_dwFlags    = WD_DIRECTX;  /* DirectX is default, need to allow OpenGL */
    m_dwFlags   |= (Expression::Evaluate(inConfig->GetValue("fullscreen"),
        0, NULL, inGlobals) != 0) ? WD_FULLSCREEN : WD_DEFAULT;
    m_dwFlags   |= (Expression::Evaluate(inConfig->GetValue("overlay_mode"),
        0, NULL, inGlobals) != 0) ? WD_OVERLAY : WD_DEFAULT;
    m_dwOverlayColorKey = (DWORD)Expression::Evaluate(
                   inConfig->GetValue( "overlay_color_key" ), 0, inValues, inGlobals );



    m_bShowFramerate = (
                        atol( inConfig->GetValue( "show_framerate", "0" ) 
                        ) != 0 ) ? true : false;
    m_bShowDebug     = (
                        atol( inConfig->GetValue( "show_debug_info", "0" ) 
                        ) != 0 ) ? true : false;

    m_dwWindowWidth  = (DWORD)Expression::Evaluate(inConfig->GetValue( "window_width"),  m_nWindowWidth, inValues, inGlobals );
    m_nWindowWidth   = (value_t)m_dwWindowWidth;
    m_dwWindowHeight = (DWORD)Expression::Evaluate(inConfig->GetValue( "window_height"), m_nWindowHeight, inValues, inGlobals );
    m_nWindowHeight  = (value_t)m_dwWindowHeight;
    m_dwFullscreenWidth  = (DWORD)Expression::Evaluate(inConfig->GetValue( "screen_width"),  m_nFullscreenWidth, inValues, inGlobals );
    m_nFullscreenWidth   = (value_t)m_dwFullscreenWidth;
    m_dwFullscreenHeight = (DWORD)Expression::Evaluate(inConfig->GetValue( "screen_height"), m_nFullscreenHeight, inValues, inGlobals );
    m_nFullscreenHeight  = (value_t)m_dwFullscreenHeight;


    if( m_dwFullscreenWidth == 0 )
        m_dwFullscreenWidth = m_dwWidth;

    if( m_dwFullscreenHeight == 0 )
        m_dwFullscreenHeight = m_dwHeight;

    if( m_dwWindowWidth == 0 )
        m_dwWindowWidth = m_dwWidth;

    if( m_dwWindowHeight == 0 )
        m_dwWindowHeight = m_dwHeight;



    //////
    if( hDefWindowProc != NULL )
        m_hDefWindowProc       = hDefWindowProc;/* whoo hoo!  it's working! */
    else
        m_hDefWindowProc       = DefWindowProc; /* whoo hoo!  it's working! */
    m_hWindowClass.lpfnWndProc = WindowDeviceWindowProcedure;
    m_hWindowClass.hInstance   = hInstance;


    if( ! RegisterClassEx( &m_hWindowClass ) )
        err = ERR_WINDOWDEVICE;


    ResumeAndWaitUntilThreadHasInitialized(); /* activate the thread */

    /*
     * initialized and not running means
     * an error occured in the thread when creating the window
     */
    if( ! IsActive() || ! IsBusy() )
        err = ERR_WINDOWDEVICE; /* GL gets this error returned */

    /*
    HBITMAP m_hBitmap;
    BITMAPINFO m_biBitmapInfo;
    void *m_pBits;

    ZeroMemory( &m_biBitmapInfo.bmiHeader, sizeof( m_biBitmapInfo.bmiHeader ) );
    m_biBitmapInfo.bmiHeader.biSize = sizeof( m_biBitmapInfo.bmiHeader );
    m_biBitmapInfo.bmiHeader.biWidth = 640;
    m_biBitmapInfo.bmiHeader.biHeight = 480;
    m_biBitmapInfo.bmiHeader.biPlanes = 1;
    m_biBitmapInfo.bmiHeader.biBitCount = 24;
    m_biBitmapInfo.bmiHeader.biCompression = BI_RGB;
    m_biBitmapInfo.bmiHeader.biSizeImage = 0;

    m_hBitmap = (HBITMAP)LoadImage(m_hWindowClass.hInstance, "Greenshift\\Greenshift.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

*/
    

//    m_hBitmap = CreateDIBSection( NULL, &m_biBitmapInfo, DIB_RGB_COLORS, &m_pBits, NULL, 0); //m_hBitmap, 0 );
//        DIB_PAL_COLORS 


    return err;
}


/****************************************************************************
 *
 * ThreadProcedure - the window must belong to the calling thread
 *
 ****************************************************************************/
int        WindowDevice::ThreadProcedure( void *pData )
{
    BOOL    hRet;
    BOOL    bQuit = FALSE;
    MSG     msg;
    RECT    hRect;
    error_t err = SUCCESS;

    /* lock as early as I can so I can perform initialization */
    EnterCS();

    /****************************************
     * DO NOT RETURN WHILE MUTEX IS LOCKED! *
     ****************************************/

        hRect.left   = 0;
        hRect.top    = 0;
        hRect.right  = WindowWidth();
        hRect.bottom = WindowHeight();


        if( ! AdjustWindowRectEx( &hRect, Style(), false, StyleEx() ) )
            err = ERR_WINDOWDEVICE;
        else
            m_hWindow = CreateWindowEx(
                        StyleEx(),
                        m_hWindowClass.lpszClassName,
                        m_strWindowTitle,
                        Style(),
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        hRect.right - hRect.left + 1,
                        hRect.bottom - hRect.top + 1, 
                        NULL,//m_hParentWindow,    // ParentWindow
                        NULL, // Menu?
                        m_hWindowClass.hInstance, NULL);


        if( m_hWindow == NULL )
            err = ERR_WINDOWDEVICE;


        if( err == SUCCESS )
        {
            SetLastError(0);  /* clear the last error (if any) */

            if( ! SetWindowLong( m_hWindow, 0, (LONG)this ) 
                && GetLastError() )
                err = ERR_WINDOWDEVICE;
        }

        if( err == SUCCESS )
            err = UpdateWindowProperties();

        if( err == SUCCESS )
            err = InitDisplay();

        if( err == SUCCESS )
            UpdateCursor();

        if( err == SUCCESS )
            ThreadHasInitialized(true);
        else
            ThreadHasInitialized(false);

    LeaveCS(); /* release the mutex since initialization is complete */


    /*
     * if encountered an error during initialization, then quit
     */
    if( err != SUCCESS )
        return err;

#ifndef OPENGL_yadda_yadda_yadda

    while (hRet = GetMessage(&msg, NULL, 0, 0))
    {
        if( hRet == -1
            || msg.message == WM_QUIT )
        {
            break;
        }

        TranslateMessage (&msg);
        DispatchMessage (&msg);

    
    }
                
        /*
         * delta loop goes HERE!
         */

#else

    // Main loop
    while( !bQuit )
    {
        // Check for messages
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            // Handle or dispatch messages
            if( msg.message == WM_QUIT )
                    bQuit = TRUE;
            
            else
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            } 
        }

        else 
        {
            Update( 'Flop' );
//            Sleep(50);
            Sleep(10);
        }
        
    }//end of while

#endif


    EnterCS();    /* LOCK here */

        DisableDisplay();

//        SetBitCanvas( NULL );
//        PostQuitMessage(0);

        DestroyWindow( m_hWindow );
        m_hWindow = NULL;

    LeaveCS();    /* UNLOCKED here */

    return 0;
}



/****************************************************************************
 *
 * UpdateWindowProperties - 
 *
 ****************************************************************************/
error_t    WindowDevice::UpdateWindowProperties(void)
{
    RECT    hRect;

    SetWindowLong(m_hWindow, GWL_STYLE, WindowStyle() );
    SetWindowLong(m_hWindow, GWL_EXSTYLE, WindowStyleEx() );

    hRect.left   = 0;
    hRect.top    = 0;
    hRect.right  = WindowWidth();
    hRect.bottom = WindowHeight();


    /*
     * calculate the size of the window after borders and titlebar are wrapped around the display
     */
    if( AdjustWindowRectEx(&hRect, Style(), false, StyleEx()) )
    {
        ShowWindow( m_hWindow, SW_SHOW );
        UpdateWindow( m_hWindow );
        SetWindowPos( m_hWindow,
                      HWND_NOTOPMOST,    /* HWND_TOPMOST == always on top */
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      hRect.right - hRect.left + (GetMode(WD_FULLSCREEN) ? 1 : 0),
                      hRect.bottom - hRect.top + (GetMode(WD_FULLSCREEN) ? 1 : 0),
                      SWP_NOMOVE );// | SWP_NOSIZE );
        return SUCCESS;
    }
    else
        return FAILURE;
}
/*
#ifdef USE_FAST_BLIT
                      hRect.right - hRect.left + (GetMode(WD_FULLSCREEN) ? 1 : 0),
                      hRect.bottom - hRect.top + (GetMode(WD_FULLSCREEN) ? 1 : 0),
#elif USE_CLIPPER
                      hRect.right - hRect.left + (GetMode(WD_FULLSCREEN) ? 1 : 0),
                      hRect.bottom - hRect.top + (GetMode(WD_FULLSCREEN) ? 1 : 0),
#else
                      hRect.right - hRect.left,
                      hRect.bottom - hRect.top,
#endif
*/



/****************************************************************************
 *
 * UpdateCursor
 *
 ****************************************************************************/
void    WindowDevice::UpdateCursor( void )
{
//    CURSORINFO    hCursorInfo;
    RECT    hRect;

//    ZeroMemory( &hCursorInfo, sizeof(hCursorInfo) );
//    hCursorInfo.cbSize = sizeof(hCursorInfo);

    if( //GetCursorInfo( &hCursorInfo ) &&
        GetMode(WD_FULLSCREEN) )
    {
        GetClientRect(GetWindow(), &hRect);
        ClientToScreen(GetWindow(), (POINT*)&hRect.left);
        ClientToScreen(GetWindow(), (POINT*)&hRect.right);

        /*
         * restrict mouse movement to the inner part of the window
         */
        ClipCursor(&hRect);

        /*
         * hide the cursor
         */
        while( ShowCursor(false) >= 0 );
    }
    else
    {
        /*
         * allow the mouse cursor to roam free over the entire desktop
         */
        ClipCursor(NULL);

        /*
         * show the cursor
         */
        while( ShowCursor(true) < 0 );
    }
}



/****************************************************************************
 *
 * GetMode - if all the passed in flags are set, return those flags,
 *                otherwise return zero.
 *
 ****************************************************************************/
DWORD    WindowDevice::GetMode( const DWORD dwFlags )
{
    return ((GetFlags() & dwFlags) == dwFlags) ? dwFlags : 0;
}


//extern char *DXError( HRESULT error );

/****************************************************************************
 *
 * ResetDisplay - should parameterize the string
 *
 ****************************************************************************/
error_t    WindowDevice::ResetDisplay( void )
{
    error_t    err;

    EnterCS();
//        err = DisableDisplay();  /* should only do this if previously enabled */
        ToggleFlags( GetFlags() );    /* clear all flags, WD_DEFAULT */
        err = InitDisplay();
        PrintPrivate(0,0, "ERROR: changing display settings.  Reverting to defaults.", RGB(255,0,0) );
        UpdateCursor();
    LeaveCS();

    return err;
}


/****************************************************************************
 *
 * ToggleMode - 
 *
 ****************************************************************************/
error_t    WindowDevice::ToggleMode( const DWORD dwFlags  )
{
    error_t    err;

    EnterCS();
        ToggleFlags( dwFlags );
        err = InitDisplay();

        if( err == SUCCESS )
            UpdateCursor();
        else
            err = ResetDisplay();
    LeaveCS();

//    Print(0,0, DXError(err) );
//    DumpToFile("error.txt", DXError(err),"\n");

    return err;
}


#ifdef UNDEFINED
/****************************************************************************
 *
 * SetBitCanvas - 
 *
 ****************************************************************************/
void    WindowDevice::SetBitCanvas( BitCanvas *pBitCanvas )
{
    EnterCS(); /* don't want to change the BitCanvas while drawing */

    if( m_pBitCanvas != NULL )
        m_pBitCanvas->RemoveDependent( this );

    m_pBitCanvas = pBitCanvas;

    if( m_pBitCanvas != NULL )
        m_pBitCanvas->AddDependent( this );

    LeaveCS();
}
#endif


/****************************************************************************
 *
 * Flip - 
 *
 ****************************************************************************/
error_t    WindowDevice::Flip( void )
{
    static char                 strFPS[128] = "0.0";
    static DWORD                dwFrames     = 0;
    static value_t              nTime;
    static HighResolutionTimer  hrtClock;
    error_t                     err         = SUCCESS;


    EnterCS();
        if( ++dwFrames > 50 )
        {
            nTime = hrtClock.Seconds();
//            ClearScreen();
            sprintf(strFPS, "%5.3f", (value_t)dwFrames / nTime );
            if( m_bShowFramerate )
                PrintPrivate( 0, 0, strFPS );
//                Print( 0, 0, strFPS, PALETTERGB(0,255,0) );
//            nFrames = 0;
//            hrtClock.Start();
        }
        
        err = OnFlip();  /* perform the flip, or blit */

        if( dwFrames > 50 )
        {
            if( m_bShowFramerate )
                PrintPrivate( 0, 0, strFPS );
//                Print( 0, 0, strFPS, PALETTERGB(0,255,0) );
            dwFrames = 0;
            hrtClock.Start();
        }
    LeaveCS();

    return err;
}


/****************************************************************************
 *
 * Resize - Upcall to derived class
 *
 ****************************************************************************/
void    WindowDevice::Resize( void )
{
    OnResize();
}


/****************************************************************************
 *
 * Move - Upcall to derived class
 *
 ****************************************************************************/
void    WindowDevice::Move( void )
{
    OnMove();
}




/****************************************************************************
 *
 * Print
 *
 ****************************************************************************/
error_t    WindowDevice::Print(const int x, const int y, const char *string, const COLORREF dwColor )
{
    if( m_bShowDebug )
        return     PrintPrivate(x, y, string, dwColor );
    else
        return SUCCESS;
}

/****************************************************************************
 *
 * Print
 *
 ****************************************************************************/
error_t    WindowDevice::PrintPrivate(const int x, const int y, const char *string, const COLORREF dwColor )
{
    HDC        hdc;
    int xValue;
    int yValue;


    if( x == -1 || x == -2 )
        xValue = Width() / 2;
    else
    if( x < 0 )
        xValue = Width() + x;
    else
        xValue = x;

    if( y == -1 )
        yValue = Height() / 2;
    else
    if( y < 0 )
        yValue = Height() + y;
    else
        yValue = y;

    EnterCS();

        if( this->GetDC(&hdc) == SUCCESS )
        {
//            SetBkColor(hdc, RGB(0, 0, 0));
            SetBkColor(hdc, PALETTERGB(0, 0, 0));
//            SetBkMode(hdc, TRANSPARENT);

            SetTextColor(hdc, dwColor );
    //        GetClientRect(m_hWindow, &rc);
    //        GetTextExtentPoint(hdc, string, lstrlen(string), &size);
    //        TextOut(hdc, (rc.right - size.cx) / 2, (rc.bottom - size.cy) / 2,
            if( x == -2 )
            {
                SetTextAlign( hdc, TA_CENTER );
                TextOut( hdc, xValue, yValue, string, strlen(string) );
                SetTextAlign( hdc, TA_LEFT );
            }
            else
            {
                TextOut( hdc, xValue, yValue, string, strlen(string) );
            }


            this->ReleaseDC(hdc);
        }

        if( GetMode(WD_OVERLAY) )
        if( this->GetPrimaryDC(&hdc) == SUCCESS )
        {
//            SetBkColor(hdc, RGB(0, 0, 0));
            SetBkColor(hdc, PALETTERGB(0, 0, 0));
//            SetBkMode(hdc, TRANSPARENT);

            SetTextColor(hdc, dwColor );
    //        GetClientRect(m_hWindow, &rc);
    //        GetTextExtentPoint(hdc, string, lstrlen(string), &size);
    //        TextOut(hdc, (rc.right - size.cx) / 2, (rc.bottom - size.cy) / 2,
            if( x == -2 )
            {
                SetTextAlign( hdc, TA_CENTER );
                TextOut( hdc, xValue, yValue, string, strlen(string) );
                SetTextAlign( hdc, TA_LEFT );
            }
            else
            {
                TextOut( hdc, xValue, yValue, string, strlen(string) );
            }


            this->ReleasePrimaryDC(hdc);
        }


    LeaveCS();


    return SUCCESS;
}


/****************************************************************************
 *
 * KeyPress - key handler
 *
 ****************************************************************************/
bool    WindowDevice::KeyPress( const DWORD dwKey )
{
    switch( dwKey )
    {
    case 'F':
        PrintPrivate(0,0,"           ");
        m_bShowFramerate = ! m_bShowFramerate;
        break;
    case 'D':
        ClearScreenAll();
//        Update( 'Pal' );
        m_bShowDebug = ! m_bShowDebug;
        break;
//    case 'C':
//        ClearScreenAll();
//        break;
//    case VK_F1:
    case '1':
        GetBitCanvas()->SetOptimizationLevel( BitCanvas::OL_CPP );
        ClearScreenAll();
        PrintPrivate(-1, 0, "default optimization" );
        break;

//    case VK_F2:
    case '2':
        GetBitCanvas()->SetOptimizationLevel( BitCanvas::OL_x86 );
        ClearScreenAll();
        PrintPrivate(-1, 0, "x86 assembly optimization" );
        break;

//    case VK_F3:
    case '3':
        GetBitCanvas()->SetOptimizationLevel( BitCanvas::OL_MMX );
        ClearScreenAll();
        PrintPrivate(-1, 0, "MMX assembly optimization" );
        break;

//    case VK_F4:
    case '4':
        GetBitCanvas()->SetOptimizationLevel( BitCanvas::OL_SSE );
        ClearScreenAll();
        PrintPrivate(-1, 0, "SSE assembly optimization" );
        break;


    default:    /* display character code */
/*//        DumpToFile( "error.txt", DXError(lpWindowDevice->ClearScreenAll()) , "\n");
        ClearScreenAll();
        char strBuffer[128];  //* static buffer for itoa
        itoa(dwKey, strBuffer, 10);
        Print(-1, 0, strBuffer );/**/
        return false;
    }

    return true;
}



/****************************************************************************
 *
 * MessageHandler - message handler
 *
 ****************************************************************************/
BOOL    WindowDevice::MessageHandler( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam )
{
    static int        idTimer1 = TIMER_STOPPED;

    switch (message)
    {
        case WM_CLOSE:
             PostQuitMessage(0);
            return 0;
        case WM_CREATE:

                SetTimer(hWindow,     /* handle to main window */
                idTimer1 = IDT_TIMER1,/* timer identifier      */
                2500,                 /* 2.5-second interval   */
                (TIMERPROC) NULL);    /* no timer callback     */

            return 0;
        case WM_ERASEBKGND: return 0;
        case WM_DESTROY:

             KillTimer(hWindow, IDT_TIMER1 );
             idTimer1 = TIMER_STOPPED;

             PostQuitMessage(0);
            return 0;

#ifdef USE_TIMER_TO_HIDE_MOUSE

        case WM_MOUSEMOVE:
            MakeCursorVisible();
             break;

        case WM_TIMER: 
 
            switch (wParam) 
            { 
                case IDT_TIMER1: 
                    // Process the 2.5-second timer. 
 
                    MakeCursorInvisible();
                     return 0; 
 
//                case IDT_TIMER2: 
                    // Process the 5-minute timer. 

//                    return 0; 
                default:
                    break;
            } 
            break;

            return 0;
#endif

        /*
        case WM_ACTIVATE:
        case WM_ACTIVATE:
        g_active = (LOWORD(w) != WA_INACTIVE) && !((BOOL)HIWORD(w));
        break;
    
            */


        case WM_MOVE:
            this->Move();
            return 0;

        case WM_SIZE:

            switch(wParam)
            {
            case SIZE_MINIMIZED:
                KillTimer(hWindow, IDT_TIMER1);
                idTimer1 = TIMER_STOPPED;
                break;
            case SIZE_RESTORED:
            case SIZE_MAXIMIZED:

                if( idTimer1 == TIMER_STOPPED )
                    SetTimer(hWindow, idTimer1 = IDT_TIMER1, 2500, (TIMERPROC)NULL );
                break;
            default:
                break;
            }

            this->Resize();
            return 0;


//        case WM_KILLFOCUS:
//            return 0;

        case WM_RBUTTONDBLCLK:
            this->ToggleMode(WD_OVERLAY);
            return 0;

        case WM_LBUTTONDBLCLK:
//        case WM_MBUTTONDBLCLK:
//        case WM_MBUTTONDOWN:
            this->ToggleMode(WD_FULLSCREEN);
            return 0;

        case WM_SYSKEYUP: /*  alt is pressed */
            switch(wParam)
            {
                case VK_RETURN: /* Alt + Enter */
                    this->ToggleMode(WD_FULLSCREEN);
                    return 0;

                default:
                    /* let the Default Window Procedure handle it */
                    break;
            }
            break;

        /***********************
         *
         * KeyUp and KeyDown must be separate so that the same toggle isn't toggled twice
         *
         ***********************/
        case WM_KEYUP:
//            switch(wParam)
            {
//                default:
                this->KeyPress(wParam);
//                    break;
            }
//            return 0;

            /* FALLTHROUGH! */

        case WM_KEYDOWN:

            switch(wParam)
            {
                case VK_ESCAPE:    /* exit the program */
                    PostQuitMessage(0);
                    break;

                default:
                    if( m_hParentWindow != NULL )
                        PostMessage(m_hParentWindow,message,wParam,lParam);
                    /* let the Parent Window handle it */
                    break;
            }
            return 0;
    }

    /*
     * Break jumps here
     */
//    return DefWindowProc(hWindow,message,wParam,lParam);
    return m_hDefWindowProc(hWindow,message,wParam,lParam);
}


/****************************************************************************
 *
 * Style
 *
 ****************************************************************************/
DWORD    WindowDevice::Style( void )
{
    if( GetMode(WD_FULLSCREEN) )
        return FullscreenStyle();
    else
        return WindowStyle();
}


/****************************************************************************
 *
 * StyleEx
 *
 ****************************************************************************/
DWORD    WindowDevice::StyleEx( void )
{
    if( GetMode(WD_FULLSCREEN) )
        return FullscreenStyleEx();
    else
        return WindowStyleEx();
}



/****************************************************************************
 *
 * WindowStyle
 *
 ****************************************************************************/
DWORD    WindowDevice::WindowStyle( void )
{
    return WS_SYSMENU | WS_CAPTION | WS_THICKFRAME
            | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
}


/****************************************************************************
 *
 * FullscreenStyle
 *
 ****************************************************************************/
DWORD    WindowDevice::FullscreenStyle( void )
{
    return WS_POPUP;
}


/****************************************************************************
 *
 * WindowStyleEx
 *
 ****************************************************************************/
DWORD    WindowDevice::WindowStyleEx( void )
{
    return WS_EX_OVERLAPPEDWINDOW;
}


/****************************************************************************
 *
 * FullscreenStyleEx
 *
 ****************************************************************************/
DWORD    WindowDevice::FullscreenStyleEx( void )
{
    return 0;
}



#ifdef USE_TIMER_TO_HIDE_MOUSE



void MakeCursorVisible( void )
{
    CURSORINFO    hCursorInfo;

    ZeroMemory( &hCursorInfo, sizeof(hCursorInfo) );
    hCursorInfo.cbSize = sizeof(hCursorInfo);

    if( GetCursorInfo( &hCursorInfo ) && ! (hCursorInfo.flags & CURSOR_SHOWING) )
        ShowCursor(true);
}

void MakeCursorInvisible( void )
{
    CURSORINFO    hCursorInfo;

    ZeroMemory( &hCursorInfo, sizeof(hCursorInfo) );
    hCursorInfo.cbSize = sizeof(hCursorInfo);

    if( GetCursorInfo( &hCursorInfo ) && (hCursorInfo.flags & CURSOR_SHOWING) )
        ShowCursor(false);
}

#endif

/****************************************************************************
 *
 * WindowDeviceWindowProcedure
 *
 ****************************************************************************/
LRESULT CALLBACK WindowDeviceWindowProcedure(HWND hWindow,
                                             UINT message,
                                             WPARAM wParam,
                                             LPARAM lParam)
{
//    char strBuffer[128];
    WindowDevice    *lpWindowDevice;
//    static int        idTimer1 = TIMER_STOPPED;

    /*
     * get a handle to the WindowDevice
     */
    lpWindowDevice = (WindowDevice*)GetWindowLong(hWindow, 0);

    if( lpWindowDevice != NULL )
        return lpWindowDevice->MessageHandler(hWindow,message,wParam,lParam);
    else
        return DefWindowProc(hWindow,message,wParam,lParam);
}




#ifdef UNDEFINED


    //        else if(mod == &mod1)
    //        {
    //            PostMessage(mod->hwndParent,message,wParam,lParam);
    //        }

            /* causes problems when GetWindowLong returns something bad... when I didn't use long window (or something)
            if(mod == &mod1)
              {    // get this_mod from our window's user data
                winampVisModule *this_mod = (winampVisModule *) GetWindowLong(hwnd,GWL_USERDATA);
                PostMessage(this_mod->hwndParent,message,wParam,lParam);
            }*/

//        break;

        case WM_MOVE:
            {    // get config_x and config_y for configuration
                RECT r;
                GetWindowRect(hWindow,&r);
    //            GetWindowRect(hMainWnd,&r);
    //            config_x = r.left;
    //            config_y = r.top;
            }
        return 0;
    }
    return DefWindowProc(hWindow,message,wParam,lParam);

}

#endif


/* The New function needs to see the subclasses so it can create them */
#include "WindowDeviceDX.h"
//#include "WindowDeviceDX5.h"
//#include "WindowDeviceOpenGL.h"


/****************************************************************************
 *
 * New
 *
 ****************************************************************************/
error_t     WindowDevice::New( const HINSTANCE hInstance,
                               WindowDevice** outWindowDevice,
                               const char *strWindowTitle,
                               MyDictionary<char*> *inConfig,
                               MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals,
                               MyDictionary<value_t*> *inValues,

                               const HWND hParentWindow,
                               const WNDPROC hWindowMsgHandler)
{
    DWORD    dwFlags;

    if( outWindowDevice == NULL
        || strWindowTitle == NULL
        || inConfig == NULL )
        return ERR_NULL;


    dwFlags    = WD_DIRECTX;  /* DirectX is default, need to allow OpenGL */

//    if( dwFlags & WD_OPENGL )
//        (*outWindowDevice) = new WindowDeviceOpenGL;
//    else
    if( dwFlags & WD_DIRECTX )
        (*outWindowDevice) = new WindowDeviceDX;
    else
        return ERR_WINDOWDEVICE;


    if( *outWindowDevice == NULL )
        return ERR_WINDOWDEVICE;


    return (*outWindowDevice)->Initialize(strWindowTitle,
                                            hInstance,
                                            hWindowMsgHandler,
                                            hParentWindow,
                                            inConfig,
                                            inGlobals,
                                            inValues );
}


