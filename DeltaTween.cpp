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
 * DeltaTween
 *
 ****************************************************************************/

#include "DeltaTween.h"

/****************************************************************************
 *
 * DeltaTween - constructor
 *
 ****************************************************************************/
DeltaTween::DeltaTween( const TWEENDESCRIPTION &tween_description )
: TweenThread     ( tween_description ),
  m_lVisibleWidth ( ((PIXELMAPDESCRIPTION*)tween_description.pData)->dwWidth  ),
  m_lVisibleHeight( ((PIXELMAPDESCRIPTION*)tween_description.pData)->dwHeight )
{
    m_bFramerateTooLow = false;
    SetAspect(1.0f);
}


/****************************************************************************
 *
 * ~DeltaTween - destructor
 *
 ****************************************************************************/
DeltaTween::~DeltaTween()
{
}





/****************************************************************************
 *
 * SetAspect - set the aspect ratio
 *
 *
    If aspect/screen aspect < 1.0f
    width is -1,1
    height is -aspect/screen aspect, aspect/screen aspect

    width interval bounds are -0.5*width, 0.5*width
    height interval bounds are -0.5*height*screen aspect/aspect, 0.5*height*screen aspect/aspect
    simplified:  -0.5*width/aspect, 0.5*width/aspect



    If aspect/screen aspect > 1.0f
    width is -screen aspect/aspect, screen aspect/aspect
    height is -1,1

    width interval bounds are -0.5*width*aspect/screen aspect, 0.5*width*aspect/screen aspect
    simplified: width interval bounds are -0.5*height*aspect, 0.5*height*aspect
    height interval bounds are -0.5*height, 0.5*height

 *
 ****************************************************************************/
void    DeltaTween::SetAspect( value_t nAspect )
{
    const value_t        nWidth  = (value_t)Width();
    const value_t        nHeight = (value_t)Height();

    // nAspect is a term with units   width/height
    // so to compare screenwidth and screen height, you have to convert the units to match
    // screenwidth/aspect == screenheight


    if( nAspect == 0.0f )
        nAspect = nWidth / nHeight;


//    if( nAspect > nWidth / nHeight )   // Jared's way
    if( nAspect <= nWidth / nHeight )  // Andy's way
    {
        m_nWidthFactor  = 0.5f * nHeight * nAspect;
        m_nHeightFactor = 0.5f * nHeight;
    }
    else
    {
        m_nWidthFactor  = 0.5f * nWidth;
        m_nHeightFactor = 0.5f * nWidth / nAspect;
    }

    m_nWidthFactorInverse  = 1.0f / m_nWidthFactor;
    m_nHeightFactorInverse = 1.0f / m_nHeightFactor;
}


/****************************************************************************
 *
 * ConvertToScreen - maps a physical ordinate to a screen ordinate (should modify to allow aspect)
 *
 ****************************************************************************/
DWORD    DeltaTween::ConvertToScreen( const value_t nPhysicalValue,
                                     const long nHalfDimension,
                                     const long nDimension,
                                     const DWORD nBoundsType )
{
    long    nReturnValue;

    nReturnValue   =  (long)floor(nPhysicalValue); /* if I were to fmod and then add to make it positive, I would then floor it.  floor'ing now does the same thing */
    nReturnValue  += nHalfDimension;

    switch( nBoundsType )
    {
    default:
    case 0:    /* wrap */

        nReturnValue %= nDimension;
        while( nReturnValue < 0L )
            nReturnValue += nDimension;
        break;

    case 1:    /* clip */

        if( nReturnValue < 0L )
            nReturnValue = 0L;
        else
        if( nReturnValue >= nDimension )
            nReturnValue = nDimension - 1L;

        break;
    }

    return nReturnValue;
}



#define FRAME_INNER_LOOP
#ifdef FRAME_INNER_LOOP
/****************************************************************************
 *
 * PrerenderFrames - calculates the frames between fields
 *
 ****************************************************************************/
void DeltaTween::PrerenderFrames( const DWORD dwFrames,
                                  void    **pFrames,
                                  void    *pObject1,
                                  void    *pObject2 )
{
//    static const long       lWidth  = Width();
//    static const long       lHeight = Height();
    static const long       nHalfWidth  = Width() >> 1;
    static const long       nHalfHeight = Height() >> 1;
    static const value_t    fHalfWidth  = (value_t)nHalfWidth - 0.5f;
    static const value_t    fHalfHeight = (value_t)nHalfHeight - 0.5f;
    const value_t    nNumFramesInverse = 1.0f / (dwFrames + 1);
    const bool bTween = (pObject2 != NULL) ? true : false;
    PIXELMAP    **pPixelMap = (PIXELMAP**)pFrames;
    DeltaField  *pDF1       = (DeltaField*)pObject1;
    DeltaField  *pDF2       = (DeltaField*)pObject2;
    value_t     logicalX;
    value_t     logicalY;
//    value_t     deltaX;
//    value_t     deltaY;
    value_t     deltaX1;
    value_t     deltaY1;
    value_t     deltaX2;
    value_t     deltaY2;
    value_t     fPercent;
    long        destX;
    long        destY;
    long        displayX;
    long        displayY;

    DWORD       dwDestX;
    DWORD       dwDestY;
    DWORD       dwSourceOffset;
    DWORD       dwDestOffset = 0;
    DWORD       boundsType  = 0;

    DWORD       currentFrame;

    /*
     * yadda yadda yadda... always check for NULL
     */
    if( pPixelMap == NULL || pDF1 == NULL )//|| pDF2 == NULL )
        return;



/*  BLEH!  now I *need* to use the frame loop on the outside!
    if( bTween )
    {
        value_t nAspect1 = pDF1->GetAspect();
        value_t nAspect2 = pDF2->GetAspect();

        if( nAspect1 == 0.0f )
            nAspect1 = Width() / (value_t)Height();
        if( nAspect2 == 0.0f )
            nAspect2 = Width() / (value_t)Height();

        SetAspect( nAspect1 * (1.0f - nPercent) + nAspect2 * nPercent );

    }
    else/**/
        SetAspect( pDF1->GetAspect() );

    /*
     * logicalY loops across  1.0 to -1.0
     */
    for( destY = nHalfHeight,   dwDestY = Height();
                                dwDestY != 0;
         destY--,               dwDestY--
        )
    {
             /*
              * subtract 0.5 to calculate from center of pixel
              */
//        logicalY = (destY - 0.5f) / m_nHeightFactor;
        logicalY = (destY - 0.5f) * m_nHeightFactorInverse;

        for(    destX = -nHalfWidth,    dwDestX = Width();
                                        dwDestX != 0;
                destX++,                dwDestX--
            )
        {
//            logicalX = (destX + 0.5f) / m_nWidthFactor;
            logicalX = (destX + 0.5f) * m_nWidthFactorInverse;

            /*
             * logical delta
             */
//            deltaX1 = pDF1->GetDeltaX( logicalX, logicalY );
//            deltaY1 = pDF1->GetDeltaY( logicalX, logicalY );
            pDF1->GetDeltaXY( logicalX, logicalY, &deltaX1, &deltaY1 );

            if( bTween )
            {
//                deltaX2 = pDF2->GetDeltaX( logicalX, logicalY );
//                deltaY2 = pDF2->GetDeltaY( logicalX, logicalY );
                pDF2->GetDeltaXY( logicalX, logicalY, &deltaX2, &deltaY2 );
            }

            for( currentFrame = 0;  currentFrame < dwFrames;  currentFrame++ )
            {
                if( bTween )
                {
//                    fPercent = (currentFrame + 1.0f) / (dwFrames + 1.0f);
                    fPercent = (currentFrame + 1.0f) * nNumFramesInverse;

//                    deltaX = deltaX1 * (1.0f - fPercent) + deltaX2 * fPercent;
//                    deltaY = deltaY1 * (1.0f - fPercent) + deltaY2 * fPercent;
                    deltaX1 = deltaX1 + (deltaX2 - deltaX1) * fPercent;
                    deltaY1 = deltaY1 + (deltaY2 - deltaY1) * fPercent;
                }
/*                else
                {
                    deltaX = deltaX1;
                    deltaY = deltaY1;
                }/**/

                displayX = ConvertToScreen( destX + 0.5f + deltaX1 * m_nWidthFactor,
                                            nHalfWidth, Width(), boundsType );

                displayY = ConvertToScreen( 0.5f - destY - deltaY1 * m_nHeightFactor, /* invert y */
                                            nHalfHeight, Height(), boundsType );

                /*
                 * store the buffer offset
                 */
                dwSourceOffset            = displayY * Width() + displayX;

                pPixelMap[currentFrame][dwDestOffset] = dwSourceOffset;
            } /* for currentFrame */

            dwDestOffset++;

        }  /* logicalX loop */
Sleep(1);
                /* sleep for 1 ms every other time */
//        if( destY & 1L )
//        Sleep(1); /* let the main thread have a few more cycles if this thread is hogging too many */
    }  /* logicalY loop */

}

#else



/*
  frame loop is on the outside... MUCH slower... this is for when (*IF*)
  time dependent are ever supported

  Check to see if it's time dependent, if not, don't re-evaluate
  Should be just as fast (or almost) as other method
 */

/****************************************************************************
 *
 * PrerenderFrames - calculates the frames between fields
 *
 ****************************************************************************/
void DeltaTween::PrerenderFrames( const DWORD dwFrames,
                                  void    **pFrames,
                                  void    *pObject1,
                                  void    *pObject2 )
{
    static const long        nHalfWidth  = Width() >> 1;
    static const long        nHalfHeight = Height() >> 1;
    static const value_t    fHalfWidth  = (value_t)nHalfWidth - 0.5f;
    static const value_t    fHalfHeight = (value_t)nHalfHeight - 0.5f;
    const value_t    nNumFramesInverse = 1.0f / (dwFrames + 1);

    PIXELMAP    **pPixelMap = (PIXELMAP**)pFrames;
    DeltaField  *pDF1       = (DeltaField*)pObject1;
    DeltaField  *pDF2       = (DeltaField*)pObject2;
    value_t     nAspect1;
    value_t     nAspect2;
    value_t     logicalX;
    value_t     logicalY;
    value_t     deltaX;
    value_t     deltaY;
    value_t     deltaX1;
    value_t     deltaY1;
    value_t     deltaX2;
    value_t     deltaY2;
    value_t     fPercent;
    long        destX;
    long        destY;
    long        displayX;
    long        displayY;

    DWORD       nDestX;
    DWORD       nDestY;
    DWORD       nSourceOffset;
    DWORD       nDestOffset = 0;
    DWORD       boundsType  = 0;

    DWORD       currentFrame;

    const bool  bTween = (pObject2 != NULL) ? true : false;

    /*
     * yadda yadda yadda... always check for NULL
     */
    if( pPixelMap == NULL || pDF1 == NULL )//|| pDF2 == NULL )
        return;

    nAspect1 = pDF1->GetAspect();
    if( bTween )
        nAspect2 = pDF2->GetAspect();
    else
        nAspect2 = 0.0f;

    if( nAspect1 == 0.0f )
        nAspect1 = Width() / (value_t)Height();
    if( nAspect2 == 0.0f )
        nAspect2 = Width() / (value_t)Height();


    SetAspect( nAspect1 );


    for( currentFrame = 0;  currentFrame < dwFrames;  currentFrame++ )
    {
        if( bTween )
        {
            fPercent = (currentFrame + 1.0f) * nNumFramesInverse;

            if( nAspect1 != nAspect2 )
            {
                SetAspect( nAspect1 * (1.0f - fPercent) + nAspect2 * fPercent );
            }

//            fPercent = (currentFrame + 1.0f) / (dwFrames + 1.0f);

        }

        nDestOffset = 0;

        /*
         * logicalY loops across  1.0 to -1.0
         */
        for( destY = nHalfHeight,    nDestY = Height();
                                    nDestY != 0;
             destY--,                nDestY--
            )
        {
                 /*
                  * subtract 0.5 to calculate from center of pixel
                  */
    //        logicalY = (destY - 0.5f) / m_nHeightFactor;
            logicalY = (destY - 0.5f) * m_nHeightFactorInverse;

            for(    destX = -nHalfWidth,    nDestX = Width();
                                            nDestX != 0;
                    destX++,                nDestX--
                )
            {
    //            logicalX = (destX + 0.5f) / m_nWidthFactor;
                logicalX = (destX + 0.5f) * m_nWidthFactorInverse;

                /*
                 * logical delta
                 */
                deltaX1 = pDF1->GetDeltaX( logicalX, logicalY );
                deltaY1 = pDF1->GetDeltaY( logicalX, logicalY );

                if( bTween )
                {
                    deltaX2 = pDF2->GetDeltaX( logicalX, logicalY );
                    deltaY2 = pDF2->GetDeltaY( logicalX, logicalY );
                }

                if( bTween )
                {
                    deltaX = deltaX1 * (1.0f - fPercent) + deltaX2 * fPercent;
                    deltaY = deltaY1 * (1.0f - fPercent) + deltaY2 * fPercent;
                }
                else
                {
                    deltaX = deltaX1;
                    deltaY = deltaY1;
                }

                displayX = ConvertToScreen( destX + 0.5f + deltaX * m_nWidthFactor,
                                            nHalfWidth, Width(), boundsType );

                displayY = ConvertToScreen( 0.5f - destY - deltaY * m_nHeightFactor,
                                            nHalfHeight, Height(), boundsType );

                /*
                 * store the buffer offset
                 */
                nSourceOffset            = displayY * Width() + displayX;

                pPixelMap[currentFrame][nDestOffset] = nSourceOffset;

                nDestOffset++;

            }  /* logicalX loop */
        }  /* logicalY loop */

     } /* frame loop */

}

#endif





/****************************************************************************
 *
 * ExperimentalPrerenderFrame - calculates the frames between fields
 *
 ****************************************************************************/
void DeltaTween::ExperimentalPrerenderFrame( 
                                  void    *pFrame,
                                  void    *pObject1 )
{
//    static const long       lWidth  = Width();
//    static const long       lHeight = Height();
    static const long       nHalfWidth  = Width() >> 1;
    static const long       nHalfHeight = Height() >> 1;
    PIXELMAP    *pPixelMap = (PIXELMAP*)pFrame;
    DeltaField  *pDF1      = (DeltaField*)pObject1;
    value_t     logicalX;
    value_t     logicalY;
//    value_t     deltaX;
//    value_t     deltaY;
    value_t     deltaX1;
    value_t     deltaY1;
    long        destX;
    long        destY;
    long        displayX;
    long        displayY;

    DWORD       dwDestX;
    DWORD       dwDestY;
    DWORD       dwSourceOffset;
    DWORD       dwDestOffset = 0;
    DWORD       boundsType  = 0;

    /*
     * yadda yadda yadda... always check for NULL
     */
    if( pPixelMap == NULL || pDF1 == NULL )//|| pDF2 == NULL )
        return;

    boundsType = pDF1->GetEdgeWrap() ? 0 : 1;

    m_bFramerateTooLow = false;

    SetAspect( pDF1->GetAspect() );

    /*
     * logicalY loops across  1.0 to -1.0
     */
    for( destY = nHalfHeight,   dwDestY = Height();
                                dwDestY != 0;
         destY--,               dwDestY--
        )
    {
             /*
              * subtract 0.5 to calculate from center of pixel
              */
//        logicalY = (destY - 0.5f) / m_nHeightFactor;
        logicalY = (destY - 0.5f) * m_nHeightFactorInverse;

        for(    destX = -nHalfWidth,    dwDestX = Width();
                                        dwDestX != 0;
                destX++,                dwDestX--
            )
        {
//            logicalX = (destX + 0.5f) / m_nWidthFactor;
            logicalX = (destX + 0.5f) * m_nWidthFactorInverse;

            /*
             * logical delta
             */
            pDF1->GetDeltaXY( logicalX, logicalY, &deltaX1, &deltaY1 );

//            deltaX1 *= 0.5f;
//            deltaY1 *= 0.5f;

            displayX = ConvertToScreen( destX + 0.5f + deltaX1 * m_nWidthFactor,
                                        nHalfWidth, Width(), boundsType );

            displayY = ConvertToScreen( 0.5f - destY - deltaY1 * m_nHeightFactor, /* invert y */
                                        nHalfHeight, Height(), boundsType );

            /*
             * store the buffer offset
             */
            dwSourceOffset          = displayY * Width() + displayX;

            pPixelMap[dwDestOffset] = dwSourceOffset;

            dwDestOffset++;

        }  /* logicalX loop */
        if( m_bFramerateTooLow )
            Sleep(3);
                /* sleep for 1 ms every other time */
//        if( destY & 1L )
//        Sleep(1); /* let the main thread have a few more cycles if this thread is hogging too many */
    }  /* logicalY loop */

}



#ifdef UNDEFINED

/****************************************************************************
 *
 * ExperimentalPrerenderFrames - calculates the frames between fields
 * ONLY called with no NULL pointers
 *
 ****************************************************************************/
void DeltaTween::ExperimentalPrerenderFrames( const DWORD dwFrames,
                                  void    **pFrames,
                                  void    *pFirstFrame,
                                  void    *pLastFrame )
{
//    const long       lWidth  = Width();
//    const long       lHeight = Height();
    const DWORD       dwLength = (DWORD)(Width() * Height());
    const value_t    nNumFramesInverse = 1.0f / (dwFrames + 1);
    PIXELMAP    **ppPixelMap = (PIXELMAP**)pFrames;
    PIXELMAP    *pPixelMap1 = (PIXELMAP*)pFirstFrame;
    PIXELMAP    *pPixelMap2 = (PIXELMAP*)pLastFrame;

    DWORD       dwSourceOffset;
    DWORD       dwDestOffset;

    DWORD       dwCurrentFrame;

    /*
     * yadda yadda yadda... always check for NULL
     */
    if( ppPixelMap == NULL || pPixelMap1 == NULL || pPixelMap2 == NULL )
        return;



    /*
     * logicalY loops across  1.0 to -1.0
     */
    for( dwDestOffset = 0;  dwDestOffset < dwLength; dwDestOffset++ )
    {
        const long lSource1 = (long)pPixelMap1[dwDestOffset];
        const long lSource2 = (long)pPixelMap2[dwDestOffset];
        const long lDeltaX1  = lSource1 % Width();
        const long lDeltaY1  = lSource1 - lDeltaX1;
        const long lDeltaX2  = lSource2 % Width();
        const long lDeltaY2  = lSource2 - lDeltaX2;/**/
/*        const long lDeltaY1  = lSource1 / Width();
        const long lDeltaX1  = lSource1 - lDeltaY1;
        const long lDeltaY2  = lSource2 / Width();
        const long lDeltaX2  = lSource2 - lDeltaY2;/**/
        const value_t deltaX1  = (value_t)lDeltaX1;
        const value_t deltaY1  = (value_t)lDeltaY1;
        const value_t deltaX2  = (value_t)lDeltaX2;
        const value_t deltaY2  = (value_t)lDeltaY2;


        for( dwCurrentFrame = 0;  dwCurrentFrame < dwFrames;  dwCurrentFrame++ )
        {
            const value_t fPercent = (dwCurrentFrame + 1.0f) * nNumFramesInverse;
            const value_t deltaX = deltaX1 + (deltaX2 - deltaX1) * fPercent;
            const value_t deltaY = deltaY1 + (deltaY2 - deltaY1) * fPercent;

            const long displayX = (long)deltaX;
//            const long displayY = deltaY - deltaY % Width();
            const long displayY = ((long)deltaY) / Width();


            /*
             * store the buffer offset
             */
            dwSourceOffset            = displayY * Width() + displayX;

            ppPixelMap[dwCurrentFrame][dwDestOffset] = dwSourceOffset;
        } /* for currentFrame */
    }

}


#else

/****************************************************************************
 *
 * ExperimentalPrerenderFrames - calculates the frames between fields
 * ONLY called with no NULL pointers
 *
 ****************************************************************************/
void DeltaTween::ExperimentalPrerenderFrames( const DWORD dwFrames,
                                  void    **pFrames,
                                  void    *pFirstFrame,
                                  void    *pLastFrame )
{
//    const long       lWidth  = Width();
//    const long       lHeight = Height();
    const DWORD       dwLength = (DWORD)(Width() * Height());
    const value_t    nNumFramesInverse = 1.0f / (dwFrames + 1);
    PIXELMAP    **ppPixelMap = (PIXELMAP**)pFrames;
    PIXELMAP    *pPixelMap1 = (PIXELMAP*)pFirstFrame;
    PIXELMAP    *pPixelMap2 = (PIXELMAP*)pLastFrame;

    DWORD       dwSourceOffset;
    DWORD       dwDestOffset;

    DWORD       dwCurrentFrame;

    /*
     * yadda yadda yadda... always check for NULL
     */
    if( ppPixelMap == NULL || pPixelMap1 == NULL || pPixelMap2 == NULL )
        return;



    /*
     * logicalY loops across  1.0 to -1.0
     */
    for( dwCurrentFrame = 0;  dwCurrentFrame < dwFrames;  dwCurrentFrame++ )
    {
        const value_t fPercent = (dwCurrentFrame + 1.0f) * nNumFramesInverse;

        for( dwDestOffset = 0;  dwDestOffset < dwLength; dwDestOffset++ )
        {
            const long lSource1 = (long)pPixelMap1[dwDestOffset];
            const long lSource2 = (long)pPixelMap2[dwDestOffset];
            const long lDeltaX1  = lSource1 % Width();
            const long lDeltaY1  = lSource1 - lDeltaX1;
            const long lDeltaX2  = lSource2 % Width();
            const long lDeltaY2  = lSource2 - lDeltaX2;
//            const value_t deltaX1  = (value_t)lDeltaX1;
//            const value_t deltaY1  = (value_t)lDeltaY1;
//            const value_t deltaX2  = (value_t)lDeltaX2;
//            const value_t deltaY2  = (value_t)lDeltaY2;

//            const value_t deltaX = deltaX1 + (deltaX2 - deltaX1) * fPercent;
//            const value_t deltaY = deltaY1 + (deltaY2 - deltaY1) * fPercent;
            const value_t deltaX = (value_t)lDeltaX1 + (lDeltaX2 - lDeltaX1) * fPercent;
            const value_t deltaY = (value_t)lDeltaY1 + (lDeltaY2 - lDeltaY1) * fPercent;

            const long displayX = (long)deltaX;
    //            const long displayY = deltaY - deltaY % Width();
            const long displayY = ((long)deltaY) / Width();



            /*
             * store the buffer offset
             */
            dwSourceOffset            = displayY * Width() + displayX;

            ppPixelMap[dwCurrentFrame][dwDestOffset] = dwSourceOffset;
        } /* for currentFrame */
    }

}




#endif