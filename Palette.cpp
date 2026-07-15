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
 * Palette - handles all file access
 *
 ****************************************************************************/

#include "Palette.h"

/****************************************************************************
 *
 * Palette - default constructor
 *
 ****************************************************************************/
Palette::Palette()
{
    m_nIntensity               = 0.0f;
    m_nTime                    = 0.0f;
    m_pExp1                    = NULL;
    m_pExp2                    = NULL;
    m_pExp3                    = NULL;
    m_nPaletteType             = PALETTE_STATIC_COLOR_MAP;
    GetPalette().palVersion    = 0;
    GetPalette().palNumEntries = PALETTE_SIZE;

    /*
     * testing to see if this is the reason for the black screen
     */
    DWORD i;
    for( i = 0; i < PALETTE_SIZE; i++ )
    {
        GetPalette().palPalEntry[i].peRed   = 255;
        GetPalette().palPalEntry[i].peGreen = 255;
        GetPalette().palPalEntry[i].peBlue  = 255;
        GetPalette().palPalEntry[i].peFlags = 0;
//        GetPalette().palPalEntry[i].peFlags = PC_NOCOLLAPSE;

    }

}


/****************************************************************************
 *
 * ~Palette - default destructor
 *
 ****************************************************************************/
Palette::~Palette()
{
    SAFE_DELETE( m_pExp1 );
    SAFE_DELETE( m_pExp2 );
    SAFE_DELETE( m_pExp3 );
}




/****************************************************************************
 *
 * SetPaletteEntry - 
 *
 ****************************************************************************/
void    Palette::SetPaletteEntry( const DWORD nIndex, const COLORREF crColor )
{
    GetPalette().palPalEntry[nIndex].peRed   = GetRValue( crColor );
    GetPalette().palPalEntry[nIndex].peGreen = GetGValue( crColor );
    GetPalette().palPalEntry[nIndex].peBlue  = GetBValue( crColor );
}


/****************************************************************************
 *
 * Clip - 
 *
 ****************************************************************************/
void    Palette::Clip( value_t *nValue )
{
    if( *nValue > 1.0f )
        *nValue = 1.0f;
    else
    if( *nValue < 0.0f )
        *nValue = 0.0f;
}



/****************************************************************************
 *
 * PercentToByte - 
 *
 ****************************************************************************/
BYTE    Palette::PercentToByte( const value_t nValue )
{
    return (BYTE)floor( nValue * 255.0f + 0.5f );
}



/****************************************************************************
 *
 * Initialize - 
 *
 ****************************************************************************/
error_t        Palette::Initialize( MyDictionary<char*> *pConfig,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals )
{
    char        strIDRed[8];
    char        strIDGreen[8];
    char        strIDBlue[8];
    error_t     err = SUCCESS;
    WORD        i;
    char        *strHue         = NULL;
    char        *strSaturation  = NULL;
    char        *strValue       = NULL;
    char        *strLightness   = NULL;
    
    char        *strRed         = NULL;
    char        *strGreen       = NULL;
    char        *strBlue        = NULL;
    
    char        *strCyan        = NULL;
    char        *strMagenta     = NULL;
    char        *strYellow      = NULL;

    char        *strExp1        = NULL;
    char        *strExp2        = NULL;
    char        *strExp3        = NULL;


    /*
     * Why am I checking inGlobals?
     */
    if( pConfig == NULL || inGlobals == NULL )
        return FAILURE;


    m_nPaletteType = PALETTE_STATIC_COLOR_MAP;

    /*
     * palette entry flags are unused, so initialize to NULL
     */
    for( i = 0; i < PALETTE_SIZE; i++ )
        GetPalette().palPalEntry[i].peFlags = NULL;


    /*
     * determine palette type
     * Partial order, RGB, HSV, HLS, CMY
     */
    m_nPaletteType = PALETTE_UNKNOWN;
    if(  (strRed          = pConfig->GetValue("R")) != NULL
      && (strGreen        = pConfig->GetValue("G")) != NULL
      && (strBlue         = pConfig->GetValue("B")) != NULL )
    {
        m_nPaletteType = PALETTE_RGB;
    }

    if( m_nPaletteType == PALETTE_UNKNOWN
      && (strHue          = pConfig->GetValue("H")) != NULL
      && (strSaturation   = pConfig->GetValue("S")) != NULL )
    {

        if( (strValue     = pConfig->GetValue("V")) != NULL )
           m_nPaletteType = PALETTE_HSV;
        else
        if( (strLightness = pConfig->GetValue("L")) != NULL )
           m_nPaletteType = PALETTE_HLS;
    }

    if( m_nPaletteType == PALETTE_UNKNOWN
      && (strCyan         = pConfig->GetValue("C")) != NULL
      && (strMagenta      = pConfig->GetValue("M")) != NULL
      && (strYellow       = pConfig->GetValue("Y")) != NULL )
    {
        m_nPaletteType = PALETTE_CMY;

        // CMYK is probably unnecessary, considering it's for printing
//        if( (strBlacK = pConfig->GetValue("K")) != NULL )
//            m_nPaletteType = PALETTE_CMYK;
    }




    switch(m_nPaletteType)
    {
    case PALETTE_STATIC_COLOR_MAP:
        for( i = 0;  i < PALETTE_SIZE; i++ )
        {
            sprintf( strIDRed,   "r%d", i );
            sprintf( strIDGreen, "g%d", i );
            sprintf( strIDBlue,  "b%d", i );
            SetPaletteEntry( i,
                RGB(
                    atol( pConfig->GetValue( strIDRed,   "0" ) ),
                    atol( pConfig->GetValue( strIDGreen, "0") ),
                    atol( pConfig->GetValue( strIDBlue,  "0") )
                    )
                );
        }

        return SUCCESS;

    case PALETTE_RGB:
        strExp1 = strRed;
        strExp2 = strGreen;
        strExp3 = strBlue;
        break;

    case PALETTE_HSV:
        strExp1 = strHue;
        strExp2 = strSaturation;
        strExp3 = strValue;
        break;

    case PALETTE_HLS:
        strExp1 = strHue;
        strExp2 = strLightness;
        strExp3 = strSaturation;
        break;

    case PALETTE_CMY:
    case PALETTE_CMYK:
        strExp1 = strCyan;
        strExp2 = strMagenta;
        strExp3 = strYellow;
        break;

    default:
        return ERR_UNKNOWNPALETTETYPE;
    }


    if( (err = m_dValues.SetValue("i", &m_nIntensity)) != SUCCESS
     || (err = m_dValues.SetValue("t", &m_nTime)) != SUCCESS )
        return err;


    err = m_pfValues.Initialize( "ABCD", "", pConfig, &m_dValues, inGlobals );
    if( err != SUCCESS )
        return err;

    m_pfValues.EvaluatePhase(0);


    /*
     * delete any previously allocated expressions
     */
    SAFE_DELETE( m_pExp1 );
    SAFE_DELETE( m_pExp2 );
    SAFE_DELETE( m_pExp3 );

    /*
     * compile the three component expressions
     */
    err = Expression::Compile( strExp1, &m_pExp1, &m_dValues, inGlobals );
    if( err == SUCCESS )
        err = Expression::Compile( strExp2, &m_pExp2, &m_dValues, inGlobals );
    if( err == SUCCESS )
        err = Expression::Compile( strExp3, &m_pExp3, &m_dValues, inGlobals );
    if( err != SUCCESS )
        return err;

    return SUCCESS;
}



/****************************************************************************
 *
 * Initialize - copy palette contents
 *
 ****************************************************************************/
error_t    Palette::Initialize( Palette *palette )
{
    DWORD    i;
    LOGPALETTE    *lpLogPalette;

    if( palette == NULL )
        return ERR_PALETTE;

    /*
     * get WITH reevaluating
     */
    lpLogPalette = &palette->GetLogicalPalette();

    m_nIntensity               = 0.0f;
    m_nTime                    = 0.0f;
    m_pExp1                    = NULL;
    m_pExp2                    = NULL;
    m_pExp3                    = NULL;
    m_nPaletteType             = PALETTE_STATIC_COLOR_MAP;
    GetPalette().palVersion    = lpLogPalette->palVersion;
    GetPalette().palNumEntries = lpLogPalette->palNumEntries;

    for( i = 0; i < PALETTE_SIZE; i++ )
    {
        GetPalette().palPalEntry[i] = lpLogPalette->palPalEntry[i];
/*        GetPalette().palPalEntry[i].peRed = lpLogPalette->palPalEntry[i].peRed;
        GetPalette().palPalEntry[i].peGreen = lpLogPalette->palPalEntry[i].peGreen;
        GetPalette().palPalEntry[i].peBlue = lpLogPalette->palPalEntry[i].peBlue;
        GetPalette().palPalEntry[i].peFlags = lpLogPalette->palPalEntry[i].peFlags;/**/

    }/**/

    return SUCCESS;
}


#ifdef UNDEFINED
/****************************************************************************
 *
 * Initialize - tween initialize
 *
 ****************************************************************************/
error_t Palette::Initialize( Palette *p1, Palette *p2, const value_t nPercent )
{
    WORD                i;
    WORD                nEntries;
    LOGPALETTE          *pal1;
    LOGPALETTE          *pal2;
    static PALETTEENTRY *dst = GetPalette().palPalEntry;
    PALETTEENTRY        *src1;
    PALETTEENTRY        *src2;

    if( p1 == NULL || p2 == NULL )
        return ERR_PALETTE;

    pal1 = &p1->GetLogicalPalette(); /* get with reevaluating */
    pal2 = &p2->GetLogicalPalette(); /* get with reevaluating */
    src1 = pal1->palPalEntry;
    src2 = pal2->palPalEntry;

    nEntries = min(pal1->palNumEntries, pal2->palNumEntries);


    m_nIntensity               = 0.0f;
    m_nTime                    = 0.0f;
    m_pExp1                    = NULL;
    m_pExp2                    = NULL;
    m_pExp3                    = NULL;
    m_nPaletteType             = PALETTE_STATIC_COLOR_MAP;
    GetPalette().palVersion    = pal2->palVersion;
    GetPalette().palNumEntries = nEntries;


    for( i = 0; i < nEntries; i++ )
    {
        GetPalette().palPalEntry[i].peRed = (BYTE)(
            src1[i].peRed * (1.0f - nPercent) +
            src2[i].peRed * (nPercent));
        
        GetPalette().palPalEntry[i].peGreen = (BYTE)(
            src1[i].peGreen * (1.0f - nPercent) +
            src2[i].peGreen * (nPercent));

        GetPalette().palPalEntry[i].peBlue = (BYTE)(
            src1[i].peBlue * (1.0f - nPercent) +
            src2[i].peBlue * (nPercent));

        GetPalette().palPalEntry[i].peFlags = pal2->palPalEntry[i].peFlags;
    }


    return SUCCESS;
}

#else
/****************************************************************************
 *
 * Initialize - tween initialize
 *
 ****************************************************************************/
error_t Palette::Initialize( Palette *p1, Palette *p2, const value_t nPercent )
{
	DWORD	    i;
	WORD	    nEntries;
	LOGPALETTE	*pal1;
	LOGPALETTE	*pal2;

	if( p1 == NULL || p2 == NULL )
		return ERR_PALETTE;

	pal1 = &p1->GetLogicalPalette(); /* get with reevaluating */
	pal2 = &p2->GetLogicalPalette(); /* get with reevaluating */
	nEntries = min(pal1->palNumEntries, pal2->palNumEntries);


	m_nIntensity               = 0.0f;
	m_nTime                    = 0.0f;
	m_pExp1                    = NULL;
	m_pExp2                    = NULL;
	m_pExp3                    = NULL;
	m_nPaletteType             = PALETTE_STATIC_COLOR_MAP;
	GetPalette().palVersion    = pal2->palVersion;
	GetPalette().palNumEntries = nEntries;


	for( i = 0; i < nEntries; i++ )
	{
#ifdef UNDEFINED
		GetPalette().palPalEntry[i].peRed = (BYTE)(
			pal1->palPalEntry[i].peRed * (1.0f - nPercent) +
			pal2->palPalEntry[i].peRed * (nPercent));
		
		GetPalette().palPalEntry[i].peGreen = (BYTE)(
			pal1->palPalEntry[i].peGreen * (1.0f - nPercent) +
			pal2->palPalEntry[i].peGreen * (nPercent));

		GetPalette().palPalEntry[i].peBlue = (BYTE)(
			pal1->palPalEntry[i].peBlue * (1.0f - nPercent) +
			pal2->palPalEntry[i].peBlue * (nPercent));

		GetPalette().palPalEntry[i].peFlags = pal2->palPalEntry[i].peFlags;
#else
        BYTE    tmp1;
        BYTE    tmp2;
        PALETTEENTRY tmpPE;

        tmp1 = pal1->palPalEntry[i].peRed;
        tmp2 = pal2->palPalEntry[i].peRed;
		tmpPE.peRed = (BYTE)( tmp1 + (tmp2 - tmp1) * nPercent );
		
        tmp1 = pal1->palPalEntry[i].peGreen;
        tmp2 = pal2->palPalEntry[i].peGreen;
		tmpPE.peGreen = (BYTE)( tmp1 + (tmp2 - tmp1) * nPercent );

        tmp1 = pal1->palPalEntry[i].peBlue;
        tmp2 = pal2->palPalEntry[i].peBlue;
		tmpPE.peBlue = (BYTE)( tmp1 + (tmp2 - tmp1) * nPercent );

		tmpPE.peFlags = pal2->palPalEntry[i].peFlags;

        GetPalette().palPalEntry[i] = tmpPE;
#endif
	}

	return SUCCESS;
}
#endif



/****************************************************************************
 *
 * GetLogicalPalette - 
 *
 ****************************************************************************/
LOGPALETTE    &Palette::GetLogicalPalette( void )
{
    DWORD       i;
    value_t     c1, c2, c3;
    COLORREF    crColor;


    m_nTime      = m_hrTimer.Seconds();  /* new evaluation, new time */

    if( m_nPaletteType == PALETTE_STATIC_COLOR_MAP )
        return GetPalette();

    m_pfValues.EvaluatePhase(1);

    /*
     * I know having the loop outside is slower but
     * it significantly reduces the duplicated code
     */

    for( i = 0; i < PALETTE_SIZE; i++ )
    {
        m_nIntensity = i / (value_t)(PALETTE_SIZE - 1);

        m_pfValues.EvaluatePhase(2);

        m_pfValues.EvaluatePhase(3);
        c1 = m_pExp1->Evaluate();

        m_pfValues.EvaluatePhase(3);
        c2 = m_pExp2->Evaluate();

        m_pfValues.EvaluatePhase(3);
        c3 = m_pExp3->Evaluate();


        switch( m_nPaletteType )
        {
        case PALETTE_RGB:
            Clip(&c1);
            Clip(&c2);
            Clip(&c3);
            crColor = RGB(  PercentToByte(c1),
                            PercentToByte(c2),
                            PercentToByte(c3) );
            break;

        case PALETTE_HSV:
            crColor = HSVToRGB(c1, c2, c3);
            break;

        case PALETTE_HLS:
            crColor = HLSToRGB(c1, c2, c3);
            break;

        case PALETTE_CMY:
        case PALETTE_CMYK: /* bleh, for now ignore K */
            c1 = 1.0f - c1;
            c2 = 1.0f - c2;
            c3 = 1.0f - c3;
            Clip(&c1);
            Clip(&c2);
            Clip(&c3);
            crColor = RGB(  PercentToByte(c1),
                            PercentToByte(c2),
                            PercentToByte(c3) );
            break;

        default:
            crColor = RGB(i, i, i);  /* If I don't know what it is,
                                      * it will be a grayscale palette! */
            break;
        }

        SetPaletteEntry( i, crColor );
    }

    return GetPalette();
}


#include "GForceFunctions.h"

/****************************************************************************
 *
 * HSVToRGB - Hue Saturation Value (a.k.a. Brightness)
 *
 ****************************************************************************/
COLORREF    Palette::HSVToRGB( const value_t nnHue,
                               const value_t nnSaturation,
                               const value_t nnValue )
{
    value_t nRed;
    value_t nGreen;
    value_t nBlue;
    value_t nHue = GForce_wrap(nnHue);
    value_t nSaturation = GForce_clip(nnSaturation);
//    value_t nSaturation = nnSaturation;
    value_t nValue = GForce_clip(nnValue);
    value_t h, x, f, p, q, t;

//    Clip(&nHue);
//    Clip(&nSaturation);
//    Clip(&nValue);

    if( nSaturation <= 0.0f )
    {
        nRed   = nValue;
        nGreen = nValue;
        nBlue  = nValue;
    }
    else
    {
        h = nHue*6.0f;
//        if( h >= 6.0f )
//            h = 5.999f;
        x = (value_t)floor(h);
        f = h-x;
        p = nValue*(1.0f-nSaturation);
        q = nValue*(1.0f-nSaturation*f);
        t = nValue*(1.0f-nSaturation*(1.0f-f));
        switch( (int)(x) )
        {
            case 6:
            case 0:
                nRed   = nValue;
                nGreen = t;
                nBlue  = p;
                break;
            case 1:
                nRed   = q;
                nGreen = nValue;
                nBlue  = p;
                break;
            case 2:
                nRed   = p;
                nGreen = nValue;
                nBlue  = t;
                break;
            case 3:
                nRed   = p;
                nGreen = q;
                nBlue  = nValue;
                break;
            case 4:
                nRed   = t;
                nGreen = p;
                nBlue  = nValue;
                break;
            case 5:
                nRed   = nValue;
                nGreen = p;
                nBlue  = q;
                break;
            default:
                return(0);
                break;
        }
    }
    Clip(&nRed);
    Clip(&nGreen);
    Clip(&nBlue);

    return RGB( PercentToByte(nRed),
                PercentToByte(nGreen),
                PercentToByte(nBlue));
}



/*
    I found some pascal source code that does HLS to RGB
    so I implemented it in C++, not worrying about optimizing for now.

{ RGB - a function to convert a set of HLS color values into
        its RGB counterpart.

        Enter the hue (0 to 360 degrees), light (0 to 100),
        and saturation (0 to 100), separated by spaces.  The
        Red, Green, and Blue intensities (0 to 100) are returned.

        Some HLS values are not valid.
        A value of -1 is returned for R,G, and B if any of H,L, or S
        is out of its range

   HOW IT'S DONE:
   If R,G,B,L,S range from 0 to 1 then

   L = (max +min)/2

          L<=.5                               L >.5
      ------------------               --------------------
   S = (max -min)/(max +min)      S = (max -min)/(2 -max -min)
   S = (max -min)/( 2L )          S = (max -min)/(2(1-L))

   LS = (max -min)/2              (1-L)S = (max -min)/2

   max = L +LS                    max = L +(1-L)S
   min = L -LS                    min = L -(1-L)S

   Then the min and max can be assigned to R,G, or B using Hue:

   0                   120                  240                  360
   |      G = min       |      B = min       |      R = min       |
   ----------------------------------------------------------------
   B = max   |       R = max      |       G = max      |    B = max

   The missing color is then determined from

   Y = X *tan(H), where X and Y are the vector sum of R,G, and B vectors.

        X = -1/2    *R  -1/2     *G  +B
        Y = root3/2 *R  -root3/2 *G

   and solving for the missing color, with special attention to conditions
   that lead to division by zero in the solution, though these should be
   taken care of by checking for S=0.
}

function rgb(color_in: color_type): color_type;

   const
      Pi = 3.14159265359;
      root3 = 3**(1/2);

   var
      R, G, B: real;
      H, L, S: real;
      minval, maxval: real;

   function chop(a, tolerance: real): real;
      begin
      if abs(a) < tolerance then
         chop := 0
      else
         chop := a
      end;

   function tan(t: real): real;
      begin
      tan := sin(t)/cos(t)
      end;

   begin
   H := color_in.c1;
   L := color_in.c2/100;
   S := color_in.c3/100;

   if (H>360) or (H<0) or (L<0) or (L>1.0) or (S>1.0) or (S<0) then
      begin
      writeln('  *** component ranges are (0..360,0..100,0..100) ***');
      rgb := colorize(-1,-1,-1)
      end
   else
      begin
      if chop(S,1e-3) = 0 then
         begin
         R := L;
         G := L;
         B := L;
         end
      else
         begin
         if L <=0.5 then
            begin
            maxval := L +S*L;
            minval := L -S*L
            end
         else
            begin
            maxval := S -S*L +L;
            minval := -S +S*L +L
            end;

         R := -1;
         G := -1;
         B := -1;

         if (H >= 300) or (H < 60) then
            B := maxval;
         if (H >= 60) and (H < 180) then
            R := maxval;
         if (H >= 180) and (H < 300) then
            G := maxval;

         if (H >= 0) and (H < 120) then
            G := minval;
         if (H >= 120) and (H < 240) then
            B := minval;
         if (H >= 240) and (H <= 360) then
            R := minval;

         H := H *Pi/180.0;

         if R < 0 then
            if chop(tan(H)+root3,1e-3) = 0 then
               R := B		{ red, green and blue are equal }
            else
               R := (tan(H)*(B -G/2) +root3/2*G)/(root3/2 +tan(H)/2)
         else if G < 0 then
            if chop(tan(H) -root3,1e-3) = 0 then
               G := B		{ green, red and blue are equal }
            else
               G := (root3/2*R +tan(H)*(-B +R/2)) / (-tan(H)/2 +root3/2)
         else if B < 0 then
            if chop(tan(H),1e-3) = 0 then
               B := R		{ blue, red and green are equal }
            else
               B := (root3/2*R -root3/2*G +tan(H)*(R/2 +G/2))/tan(H)
         end;  {else not gray}
      rgb := colorize(100*chop(R,1e-4), 100*chop(G,1e-4), 100*chop(B,1e-4))
      end   {else valid values}
   end;  {rgb proc}

{-----------------------------------------------------------------------------}

*/

/****************************************************************************
 *
 * HLSToRGB - Hue Lightness Saturation - not implemented yet (copied HSV)
 *
 ****************************************************************************/
COLORREF    Palette::HLSToRGB( const value_t nnHue,
                               const value_t nnLightness,
                               const value_t nnSaturation )
{
    value_t nRed;
    value_t nGreen;
    value_t nBlue;
    value_t nHue = GForce_wrap(nnHue);
    value_t nLightness = GForce_clip(nnLightness);
    value_t nSaturation = GForce_clip(nnSaturation);
//    value_t h, x, f, p, q, t;
    value_t nMinVal, nMaxVal;

    if( nSaturation <= 0.0f )
    {
        nRed = nLightness;
        nGreen = nLightness;
        nBlue = nLightness;
    }
    else
    {
        if( nLightness <= 0.5f )
        {
            nMaxVal = nLightness + nSaturation * nLightness;
            nMinVal = nLightness - nSaturation * nLightness;
        }
        else
        {
            nMaxVal = nSaturation - nSaturation * nLightness + nLightness;
            nMinVal = -nSaturation + nSaturation * nLightness + nLightness;
        }

        nRed = -1;
        nGreen = -1;
        nBlue = -1;

        if( nHue >= 300.0f/360.0f || nHue < 60.0f/360.0f )
            nBlue = nMaxVal;

        if( nHue >= 60.0f/360.0f && nHue < 180.0f/360.0f )
            nRed = nMaxVal;

        if( nHue >= 180.0f/360.0f && nHue < 300.0f/360.0f )
            nGreen = nMaxVal;


        if( nHue >= 0.0f/360.0f && nHue < 120.0f/360.0f )
            nGreen = nMinVal;

        if( nHue >= 120.0f/360.0f && nHue < 240.0f/360.0f )
            nBlue = nMinVal;

        if( nHue >= 240.0f/360.0f && nHue <= 360/360 )
            nRed = nMinVal;


        nHue = nHue * 2.0f * 3.141592653589f;

        if( nRed < 0.0f )
        {
            if( fabs( tan(nHue) + 1.73205080757f ) <= 1e-3 ) /* RGB are equal */
                nRed = nBlue;
            else
                nRed = (value_t)((tan(nHue) * (nBlue - nGreen/2.0f) + 1.73205080757f/2.0f*nGreen) / (1.73205080757f/2.0f + tan(nHue)/2.0f));
        }
        else
        if( nGreen < 0.0f )  /* here below is wrong */
        {
            if( fabs( tan(nHue) + 1.73205080757f ) <= 1e-3 ) /* RGB are equal */
                nGreen = nBlue;
            else
                nRed = (value_t)((1.73205080757f/2.0f*nRed + tan(nHue) * (-nBlue + nRed/2.0f)) / (-tan(nHue)/2.0f + 1.73205080757f/2.0f));
        }
        else
        if( nBlue < 0.0f )
        {
            if( fabs( tan(nHue) + 1.73205080757f ) <= 1e-3 ) /* RGB are equal */
                nBlue = nRed;
            else
                nRed = (value_t)((1.73205080757f/2.0f*nRed - 1.73205080757f/2.0f*nGreen + tan(nHue) * (nRed/2.0f + nGreen/2.0f)) / (tan(nHue)));
        }
    }
    Clip(&nRed);
    Clip(&nGreen);
    Clip(&nBlue);

    return RGB( PercentToByte(nRed),
                PercentToByte(nGreen),
                PercentToByte(nBlue));
}

