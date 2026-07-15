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
 * PhaseFunction - an N dimensional function that may have sets of subexpressions, or "phases", which may be evaluated in batches
 *
 ****************************************************************************/

#ifndef _PhaseFunction_H_
#define _PhaseFunction_H_

#include "Expression.h"
#include "MyDictionary.h"
//#include "ContiguousAlignedMemoryAllocator.h"

#ifdef EXTREME_DEBUGGING
#include <windows.h>  /* for MessageBox - REMOVE WHEN DONE DEBUGGING! */
#endif

#include <stdio.h>  /* for sprintf() */

/*
 * length = number of abstract entities
 * size   = number of bytes
 */
typedef struct tagPHASEDESCRIPTOR
{
    DWORD        dwPhaseLength;
    value_t        *pValues;
    Expression    **pFunctions;
} PHASEDESCRIPTOR;


/*
 * need to know the number of phases and number of expressions in each phase
 * before allocating the return values or compiling the expressions
 */


/****************************************************************************
 *
 * PhaseFunction - 
 *
 ****************************************************************************/
class PhaseFunction
{
public:
    PhaseFunction()
    {
        m_dwNumDimensions = 0;
        m_dwNumFunctions  = 0;
        m_dwNumPhases     = 0;

        m_pUnalignedMem  = NULL;
        m_pFunctions     = NULL;
        m_pPhases        = NULL;
    };
    ~PhaseFunction()
    {
        DWORD    i;
        DWORD    j;

        for( i = 0; i < NumPhases(); i++ )
        {
            for( j = 0; j < m_pPhases[i].dwPhaseLength; j++ )
            {
                SAFE_DELETE( m_pPhases[i].pFunctions[j] );
            }
        }

        for( i = 0; i < NumFunctions(); i++ )
        {
            for( j = 0; j < NumDimensions(); j++ )
            {
                SAFE_DELETE( m_pFunctions[i][j] );
            }
        }

        m_pFunctions     = NULL;
        m_pPhases        = NULL;
        SAFE_DELETE_ARRAY( m_pUnalignedMem );
    };


/****************************************************************************
 *
 * Initialize - 
 *
 ****************************************************************************/
    error_t    Initialize( const char *strPhaseID,// = "ABCD",
                        const char *strDimensionID,// = "XY"
                        MyDictionary<char*> *inConfig,
                        MyDictionary<value_t*> *inoutValues, //remind me again why this is in/out?
                        MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals )

                        /*
                         * inoutValues needs to be in/out, because you can only pass a single 
                         * value MyDictionary to Expression::Compile.  And you have no way of
                         * knowing what variables are needed for the Expression to compile
                         * (ie: I'm not assuming GForce/Greenshift vars are used)
                         *
                         * in other words I need to make a copy... no not necessarily
                         *
                         * that was just explaining the need for the "in" property
                         *
                         * as for the "out" property, it follows from the same
                         * "assume nothing" mentality.  The reason out is needed
                         * is that there might be OTHER variables in inConfig
                         * that rely on the values calculated by the Expressions
                         * I create.
                         *
                         * ok, now I get it, it's making sense now.
                         * the calling function should make a copy of inoutValues
                         * prior to passing it to PhaseFunction, if it doesn't want
                         * the values added to the MyDictionary it would have passed
                         * had it not made a copy
                         *
                         * This means MyDictionary needs to be able to copy itself
                         */
    {
        char    strID[8];
        error_t err;
        DWORD   dwPhase;
        DWORD   dwFunction;
        DWORD   dwDimension;
        DWORD   index;

        DWORD   dwPhaseFunctions;

        DWORD   dwAllocationSize;
        DWORD   dwPhaseTableSize;
        DWORD   dwPhaseValueTableSize;
        DWORD   dwPhaseFunctionTableSize;
        DWORD   dwParametricFunctionTableSize;
        DWORD   dwParametricFunctionPointerTableSize;



        /*
         * REMOVE THE TO LOWERS!  Let MyDictionary handle case
         */


        if( strPhaseID == NULL || strDimensionID == NULL || inConfig == NULL
            || inoutValues == NULL || inGlobals == NULL )
            return ERR_NULL;


        /*
         * in case an attempt is made to reinitialize a PhaseFunction
         * release the previously allocated memory
         */
        m_pFunctions     = NULL;
        m_pPhases        = NULL;
        SAFE_DELETE_ARRAY( m_pUnalignedMem );


        SetNumPhases( strlen(strPhaseID) ); /* maximum number of phases */
//        SetNumFunctions( 0 );

        /* OK for strPhaseID == "" ? NO! */
        if( NumPhases() == 0 )
//            return ERR_DIMENSIONLESS;
            return FAILURE;


        /*
         * count the number of phase functions (the sum of all the lengths)
         */
        dwPhaseFunctions = 0;
        for( dwPhase = 0;  strPhaseID[dwPhase] != '\0';  dwPhase++ )
        {
            /*
             * count the number of expressions in each phase
             */
            index = 0;
            do {    sprintf( strID,
                             "%c%d",
                             strPhaseID[dwPhase],
                             index++ );

            } while( inConfig->GetValue(strID) != NULL );

            /*
             * add to the tally of Phase Functions
             */
            dwPhaseFunctions += index - 1;
        }


        /*
         * count the number of dimensions that are actually defined
         */
        SetNumDimensions( strlen(strDimensionID) ); /* max num of dimensions,
                                                     * may change depending
                                                     * on the number used
                                                     */
        for( dwDimension = 0; dwDimension < NumDimensions(); dwDimension++ )
        {
            sprintf( strID, "%c0", strDimensionID[dwDimension] );

            /*
             * if one of the dimensions is not used
             * correct the number of dimensions and
             * break out of the for loop
             */
            if( inConfig->GetValue( strID ) == NULL )
            {
                SetNumDimensions( dwDimension );
                break; /* break out of the for loop */
            }
        }

//        SetNumDimensions( dwDimension );

    if( NumDimensions() != 0 )
    {
        /*
         * count number of functions that have all m_nNumDimensions dimensions
         */
        index = 0;
        dwDimension = NumDimensions();
//        while( strDimensionID[nDimension] == '\0' )
        while( dwDimension == NumDimensions() )
        {
//            for( nDimension = 0; strDimensionID[nDimension] != '\0'; nDimension++ )
            for( dwDimension = 0; dwDimension < NumDimensions(); dwDimension++ )
            {
                sprintf( strID,
                         "%c%d",
                         strDimensionID[dwDimension],
                         index++ );

                if( inConfig->GetValue(strID) == NULL )
                    break; /* break out of the for loop */
                           /* nDimension++ isn't evaluated before breaking out,
                            * so strDimensionID[nDimension] != '\0' and thus
                            * will exit the while loop
                            *
                            * index will be one greater than the
                            * number we need.  Simply subtract one.
                            */
            }
        }

        /*
         * set the number of Functions
         */
        SetNumFunctions( index - 1 );

    } /* if( m_nNumDimensions != 0 ) */


        /*
         * done counting, now allocate the memory based on the count
         */
        dwPhaseTableSize
            = NumPhases() * sizeof(*m_pPhases);

        dwPhaseValueTableSize
            = NumPhases() * dwPhaseFunctions * sizeof(*m_pPhases->pValues);

        dwPhaseFunctionTableSize
            = NumPhases() * dwPhaseFunctions * sizeof(*m_pPhases->pFunctions);

        dwParametricFunctionTableSize
            = NumFunctions() * NumDimensions() * sizeof(*m_pFunctions);

        dwParametricFunctionPointerTableSize
            = NumFunctions() * sizeof(m_pFunctions);

        dwAllocationSize = dwPhaseTableSize + dwPhaseValueTableSize
            + dwPhaseFunctionTableSize + dwParametricFunctionTableSize
            + dwParametricFunctionPointerTableSize;


#ifdef EXTREME_DEBUGGING
        dwAllocationSize += 10;  /**/
#endif

        /*
         * allocate space to store the phase stuff
         */
        m_pUnalignedMem = new BYTE[ dwAllocationSize ];
        if( m_pUnalignedMem == NULL )
            return ERR_MALLOC;

        /*
         * zero the memory
         */
        for( index = 0; index < dwAllocationSize; index++ )
            m_pUnalignedMem[index] = 0;

        /*
         * setup the pointers for use
         */
        m_pPhases = (PHASEDESCRIPTOR*)m_pUnalignedMem;



        /*
         * initialize the first entry in the phase table
         */
        m_pPhases[0].pValues
            = (value_t*)&m_pUnalignedMem[ dwPhaseTableSize ];
        m_pPhases[0].pFunctions = (Expression**)&m_pUnalignedMem[
            dwPhaseTableSize + dwPhaseValueTableSize ];

        /*
         * get all the phase lengths
         */
        dwPhaseFunctions = 0;
        for( dwPhase = 0;  strPhaseID[dwPhase] != '\0';  dwPhase++ )
        {

            /*
             * calculate the value and function pointers
             * relative to the first entry in the phase table
             */
            m_pPhases[dwPhase].pValues
                = &m_pPhases[0].pValues[ dwPhaseFunctions ];

            m_pPhases[dwPhase].pFunctions
                = &m_pPhases[0].pFunctions[ dwPhaseFunctions ];

            
            /*
             * loop and count the number of expressions in each phase
             */
            index = 0;
            sprintf( strID,
                "%c%d", strPhaseID[dwPhase], index++ );

            while( inConfig->GetValue(strID) != NULL )
            {
                /*
                 * add values to MyDictionary
                 */
                err = inoutValues->SetValue( strID,
                                    &m_pPhases[dwPhase].pValues[index-1] );

                if( err != SUCCESS )
                    return err;

                /*
                 * successfully added another phase function,
                 * increment the counter
                 */
                dwPhaseFunctions++;

                /*
                 * make the next ID
                 */
                sprintf( strID,
                    "%c%d",    strPhaseID[dwPhase], index++ );
            }

            /*
             * set the phase length (number of functions in the phase)
             */
            m_pPhases[dwPhase].dwPhaseLength = index - 1;
        }


#ifdef EXTREME_DEBUGGING
        if( NumDimensions() == 1 )
        DumpToFile( "error.txt", "getting ready to compile the Phase Functions", "\n" );
#endif

        /*
         * compile the Expressions
         */
        for( dwPhase = 0;  dwPhase < NumPhases();  dwPhase++ )
        {
            for( index = 0; index < m_pPhases[dwPhase].dwPhaseLength; index++ )
            {
                /* dis I forget to do tolower? */
                sprintf( strID, "%c%d", strPhaseID[dwPhase], index );

                err = Expression::Compile( inConfig->GetValue(strID),
                                    &(m_pPhases[dwPhase].pFunctions[index]),
                                    inoutValues,
                                    inGlobals ); /* the array variables */

                if( err != SUCCESS )
                {
#ifdef EXTREME_DEBUGGING
                    DumpToFile( "error.txt", ">>> Phase Function : " );
                    DumpToFile( "error.txt", inConfig->GetValue("NAME", "" ), " <<<\n" );
                    DumpToFile( "error.txt", ErrorString(err), " <<<\n" );
                    DumpToFile( "error.txt", strID, "<-- strExpressionID\n" );
                    DumpToFile( "error.txt", inConfig->GetValue(strID, ""), "<--\n" );
                    inoutValues->DebugDumpContents( "error.txt" );
#endif
                    return err;
                }

            }
        }


#ifdef EXTREME_DEBUGGING
        if( NumDimensions() == 1 )
        DumpToFile( "error.txt", "compiled the Phase Functions", "\n" );

#endif







        /*
         * check for the dimensions WITHOUT a number, just in case
         */


        if( NumDimensions() == 0 )
            return SUCCESS; /* doesn't have X or Y vars, so ignore the rest */



        /*
         * setup the pointers for use
         */
        m_pFunctions = (Expression***)&m_pUnalignedMem[ dwPhaseTableSize
                        + dwPhaseValueTableSize + dwPhaseFunctionTableSize ];



        /*
         * initialize the first entry in the function table
         */
        m_pFunctions[0]    = (Expression**)&m_pUnalignedMem[
            dwPhaseTableSize + dwPhaseValueTableSize
            + dwPhaseFunctionTableSize + dwParametricFunctionPointerTableSize ];


#ifdef EXTREME_DEBUGGING
        if( NumDimensions() == 1 )
        DumpToFile( "error.txt", "about to compile the other functions", "\n" );
#endif

        /*
         * compile the Expressions
         */
        for( dwFunction = 0;  dwFunction < NumFunctions();  dwFunction++ )
        {
            /*
             * set the function pointer table entry to the
             * first Expression pointer for that function
             */
            m_pFunctions[dwFunction] = &m_pFunctions[0][dwFunction * NumDimensions()]; /* is this right? */



            for( index = 0; index < NumDimensions(); index++ )
            {
                sprintf( strID, "%c%d", strDimensionID[index], dwFunction );

                /*
                 * this is causing an error for some WaveShapes
                 */
                err = Expression::Compile( inConfig->GetValue(strID),
                                    &(m_pFunctions[dwFunction][index]),
                                    inoutValues,
                                    inGlobals ); /* the array variables */

                if( err != SUCCESS )
                {
#ifdef EXTREME_DEBUGGING
                    DumpToFile( "error.txt", ">>> Phase Function : " );
                    DumpToFile( "error.txt", inConfig->GetValue("NAME", "" ), " <<<\n" );
                    DumpToFile( "error.txt", ErrorString(err), " <<<\n" );
                    DumpToFile( "error.txt", strID, "<--\n" );
                    DumpToFile( "error.txt", inConfig->GetValue(strID, ""), "<--\n" );
                    inoutValues->DebugDumpContents( "error.txt" );
#endif
                    return err;
                }

            }
        }

#ifdef EXTREME_DEBUGGING
        if( NumDimensions() == 1 )
        DumpToFile( "error.txt", "compiled the other functions", "\n" );

        DumpToFile( "error.txt", "returning SUCCESS", "\n" );
#endif

        return SUCCESS;
    };


#ifndef EXTREME_DEBUGGING
/****************************************************************************
 *
 * EvaluatePhase - 
 *
 ****************************************************************************/
    error_t    EvaluatePhase( const DWORD dwPhaseNumber )
    {
        DWORD i;

        if( dwPhaseNumber >= NumPhases() )
            return ERR_BOUNDS;

        for( i = 0;  i < m_pPhases[dwPhaseNumber].dwPhaseLength;  i++ )
        {
            m_pPhases[dwPhaseNumber].pValues[i] =
                m_pPhases[dwPhaseNumber].pFunctions[i]->Evaluate();
        }

        return SUCCESS;
    };

#else  /* EXTREME_DEBUGGING */
/****************************************************************************
 *
 * EvaluatePhase - EXTREME_DEBUGGING version
 *
 ****************************************************************************/
    error_t    EvaluatePhase( const DWORD dwPhaseNumber )
    {
        DWORD i;

        if( dwPhaseNumber >= NumPhases() )
        {
            DumpToFile( "error.txt", "there are only ", NumPhases(), " phases" );
            DumpToFile( "error.txt", " not ", dwPhaseNumber, " phases" );
            return FAILURE;
        }

        if( m_pPhases == NULL )
        {
            DumpToFile( "error.txt", "m_pPhases is NULL!", "\n" );
            MessageBox( NULL, "m_pPhases is NULL!", "ERROR!", MB_OK );
            return ERR_NULL;
        }

        /*  m_pPhases[nPhaseNumber].pFunctions
            is where the error is occuring

            actually, the place it occured is in SetNewWhatever because RANDOM
            was rounding the random number to an integer, which produced
            numbers that included the MAX integer passed, which then made the
            SetNewWhatever function pass a pointer to memory after the last
            Whatever.  The pointer didn't cause problems until the nonexistant
            PhaseFunction tried to access memory it was supposed to have
            allocated.  It was interpreting some memory location after the last
            Whatever as a pointer to the allocated memory.  Obviously, this
            "pointer" would point to some random location, which I didn't have
            access to mess with.  AARUUGGHH!!  at least this defect is solved.
        */

        if( m_pPhases[dwPhaseNumber].dwPhaseLength > 0 )  /* aarruugghh!! please work! */
        {
            if( m_pPhases[dwPhaseNumber].pFunctions == NULL )
            {
                DumpToFile( "error.txt", "m_pPhases[dwPhaseNumber].pFunctions is NULL!", "\n" );
                MessageBox( NULL, "m_pPhases[dwPhaseNumber].pFunctions is NULL!", "ERROR!", MB_OK );
                return ERR_NULL;
            }


            for( i = 0;  i < m_pPhases[dwPhaseNumber].dwPhaseLength;  i++ )
            {
                if( m_pPhases[dwPhaseNumber].pFunctions[i] == NULL )
                {
                    DumpToFile( "error.txt", "m_pPhases[dwPhaseNumber].pFunctions[i] is NULL!", "\n" );
                    DumpToFile( "error.txt", "dwPhaseNumber = ", dwPhaseNumber, "\n" );
                    DumpToFile( "error.txt", "i = ", i, "\n" );
                    DumpToFile( "error.txt", "m_pPhases[dwPhaseNumber].dwPhaseLength = ", m_pPhases[dwPhaseNumber].dwPhaseLength, "\n" );
                    MessageBox( NULL, "m_pPhases[dwPhaseNumber].pFunctions[i] is NULL!", "ERROR!", MB_OK );
                    return ERR_NULL;
                }

                m_pPhases[dwPhaseNumber].pValues[i] =
                    m_pPhases[dwPhaseNumber].pFunctions[i]->Evaluate();

    //            DumpToFile( "phasetest.txt", m_pPhases[nPhaseNumber].pValues[i], "\n" );
            }
        }


        return SUCCESS;
    };
#endif  /* EXTREME_DEBUGGING */



/****************************************************************************
 *
 * EvaluateFunction - 
 *
 ****************************************************************************/
    value_t    EvaluateFunction( const DWORD dwFunction, const DWORD dwDimension )
    {
#ifdef EXTREME_DEBUGGING
        Expression  ***pppE;
        Expression  **ppE;
        Expression  *pE;
        value_t     v;
#endif

        if( dwFunction < NumFunctions() && dwDimension < NumDimensions() )
        {
#ifdef EXTREME_DEBUGGING
            pppE = m_pFunctions;

            ppE = pppE[dwFunction];

            pE = ppE[dwDimension];

            v = pE->Evaluate();

            return v;

#else
            return m_pFunctions[dwFunction][dwDimension]->Evaluate();
#endif
        }
        else
        {
#ifdef EXTREME_DEBUGGING
            DumpToFile("error.txt", "invalid dimensions?  in PhaseFunction. Evaluate Function", "\n");
#endif
            return 0.0f;
        }
    };


    friend error_t ImportPhase( PhaseFunction &DestPF, PhaseFunction &SourcePF, const DWORD dwPhase )
    {
        DWORD   i;
        DWORD   dwPhaseLength;

        if( SourcePF.m_dwNumPhases <= dwPhase
            || DestPF.m_dwNumPhases <= dwPhase )
            return ERR_BOUNDS;

        dwPhaseLength = min( DestPF.m_pPhases[dwPhase].dwPhaseLength,
                        SourcePF.m_pPhases[dwPhase].dwPhaseLength );

        for( i = 0; i < dwPhaseLength; i++ )
        {
            DestPF.m_pPhases[dwPhase].pValues[i] =
                SourcePF.m_pPhases[dwPhase].pValues[i];
        }

    };




    /*
     * to reduce the chances of accidental modification
     */
    inline DWORD    NumDimensions( void )   { return m_dwNumDimensions; };
    inline DWORD    NumFunctions( void )    { return m_dwNumFunctions; };
    inline DWORD    NumPhases( void )       { return m_dwNumPhases; };

protected:
    inline void     SetNumDimensions( const DWORD dwNumDimensions )
                                    { m_dwNumDimensions = dwNumDimensions; };
    inline void     SetNumFunctions( const DWORD dwNumFunctions )
                                    { m_dwNumFunctions = dwNumFunctions; };
    inline void     SetNumPhases( const DWORD dwNumPhases )
                                    { m_dwNumPhases = dwNumPhases; };


protected:
    DWORD           m_dwNumDimensions;
    DWORD           m_dwNumFunctions;
    DWORD           m_dwNumPhases;
    PHASEDESCRIPTOR *m_pPhases;
    Expression      ***m_pFunctions;
    BYTE            *m_pUnalignedMem;
};


#endif  /* _PhaseFunction_H_ */

