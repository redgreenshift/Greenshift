#pragma once
#include <algorithm>
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

#if EXTREME_DEBUGGING
#include <windows.h>  /* for MessageBox - REMOVE WHEN DONE DEBUGGING! */
#if defined(_MSC_VER)
#pragma warning(push)
// The warning is only in logging code, so deferring for now
#pragma warning(disable: 4244) // 'argument': conversion from 'const DWORD' to 'value_t', possible loss of data
#endif
#endif

#include <stdio.h>  /* for sprintf() */

/*
 * length = number of abstract entities
 * size   = number of bytes
 */
typedef struct tagPHASEDESCRIPTOR
{
	DWORD        dwPhaseLength;
	value_t* pValues;
	Expression** pFunctions;
} PHASEDESCRIPTOR;


/*
 * need to know the number of phases and number of expressions in each phase
 * before allocating the return values or compiling the expressions
 */


 /**
  * @brief A Phase Function is an N-dimensional function structure designed to
  * manage sets of mathematical subexpressions organized into "phases" which
  * can be evaluated in batches. It allows for the definition of complex,
  * multi-variable functions by uniquely indexing expressions using both
  * dimension/phase identifiers and their respective indices within those phases.
  */
class PhaseFunction
{
public:
	PhaseFunction()
	{
		m_dwNumDimensions = 0;
		m_dwNumFunctions = 0;
		m_dwNumPhases = 0;

		m_pUnalignedMem = NULL;
		m_pFunctions = NULL;
		m_pPhases = NULL;
	};
	~PhaseFunction()
	{
		DWORD    i;
		DWORD    j;

		if (m_pPhases != nullptr)
		{
			for (i = 0; i < NumPhases(); i++)
			{
				for (j = 0; j < m_pPhases[i].dwPhaseLength; j++)
				{
					SAFE_DELETE(m_pPhases[i].pFunctions[j]);
				}
			}
		}

		if (m_pFunctions != nullptr)
		{
			for (i = 0; i < NumFunctions(); i++)
			{
				for (j = 0; j < NumDimensions(); j++)
				{
					SAFE_DELETE(m_pFunctions[i][j]);
				}
			}
		}

		m_pFunctions = NULL;
		m_pPhases = NULL;
		SAFE_DELETE_ARRAY(m_pUnalignedMem);
	};


	/****************************************************************************
	 *
	 * Initialize -
	 *
	 ****************************************************************************/
	error_t    Initialize(const char* strPhaseID,// = "ABCD",
		const char* strDimensionID,// = "XY"
		MyDictionary<mychar_t*>* inConfig,
		MyDictionary<value_t*>* inoutValues, //remind me again why this is in/out?
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)

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


		if (strPhaseID == NULL || strDimensionID == NULL || inConfig == NULL
			|| inoutValues == NULL || inGlobals == NULL)
			return ERR_NULL;


		/*
		 * in case an attempt is made to reinitialize a PhaseFunction
		 * release the previously allocated memory
		 */
		m_pFunctions = NULL;
		m_pPhases = NULL;
		SAFE_DELETE_ARRAY(m_pUnalignedMem);


		SetNumPhases(strlen(strPhaseID)); /* maximum number of phases */
		//        SetNumFunctions( 0 );

				/* OK for strPhaseID == "" ? NO! */
		if (NumPhases() == 0)
			//            return ERR_DIMENSIONLESS;
			return FAILURE;


		/*
		 * count the number of phase functions (the sum of all the lengths)
		 */
		dwPhaseFunctions = 0;
		for (dwPhase = 0; strPhaseID[dwPhase] != '\0'; ++dwPhase)
		{
			/*
			 * count the number of expressions in each phase
			 * The functions are not always contiguous, and sometimes don't exist,
			 * so we need to treat missing values as "0"
			 *
			 * GForce configs don't always assign within a phase sequentially, and
			 * sometimes even _uses_ *omitted* values. I can only assume such values
			 * should default to zero. "A15" is the highest undefined-but-used value I
			 * have seen, so always create at least 15; if contiguous values are
			 * defined higher, keep reading them in as long as values exist.
			 */
			constexpr DWORD highestUndefinedButUsedValue = 15;
			index = 0;
			while (true)
			{
				// Build key: "<phaseChar><index>"
				int ret = snprintf(strID, _countof(strID),
									"%c%d", strPhaseID[dwPhase], index);
				if (ret < 0 || (size_t)ret >= _countof(strID))
					return FAILURE;

				if (inConfig->GetValue(strID) == nullptr)
				{
					if (index > highestUndefinedButUsedValue)
						break; // contiguous streak ended

					// Default missing values to ZERO; and then continue counting
					inConfig->SetValue(strID, strdup("0"));
				}
				++index;
			}

			/*
			 * add to the tally of Phase Functions
			 */
			dwPhaseFunctions += index;
		}


		/*
		 * count the number of dimensions that are actually defined
		 */
		SetNumDimensions(strlen(strDimensionID)); /* max num of dimensions,
													 * may change depending
													 * on the number used
													 */
		for (dwDimension = 0; dwDimension < NumDimensions(); ++dwDimension)
		{
			int ret = snprintf(strID, _countof(strID), "%c0", strDimensionID[dwDimension]);
			if (ret < 0 || (size_t)ret >= _countof(strID))
				return FAILURE;

			/*
			 * if one of the dimensions is not used
			 * correct the number of dimensions and
			 * break out of the for loop
			 */
			if (inConfig->GetValue(strID) == NULL)
			{
				SetNumDimensions(dwDimension);
				break; /* break out of the for loop */
			}
		}

		if (NumDimensions() != 0)
		{
			/*
			 * count number of functions that have all m_nNumDimensions dimensions
			 * EXAMPLE: X0,Y0,X1,Y1 would be 2 functions.
			 * EXAMPLE: X0,Y0,Z0,X1,Y1,Z1 would be 2 functions.
			 * EXAMPLE: X0,Y0,X1,Y1,X2,Y2 would be 3 functions.
			 * EXAMPLE: X0,Y0,Z0,X1,Y1 would be 1 function (no Z1).
			 */
			dwFunction = 0;
			for (bool done = false; !done; ++dwFunction)
			{
				for (dwDimension = 0; dwDimension < NumDimensions(); ++dwDimension)
				{
					// Build key: "<dimensionChar><fnNumber>"
					int ret = snprintf(strID, _countof(strID),
										"%c%d", strDimensionID[dwDimension], dwFunction);
					if (ret < 0 || (size_t)ret >= _countof(strID))
						return FAILURE;

					if (inConfig->GetValue(strID) == nullptr)
					{
						if (dwFunction > 0)
							--dwFunction;
						done = true;
						break; // did not match all expected dimensions, so we must have hit the end; stop counting, do not count this iteration.
					}
				}
			}

			/*
			 * set the number of Functions
			 */
			SetNumFunctions(dwFunction);

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


#if EXTREME_DEBUGGING
		dwAllocationSize += 10;  /**/
#endif

		/*
		 * allocate space to store the phase stuff
		 */
		m_pUnalignedMem = new BYTE[dwAllocationSize];
		if (m_pUnalignedMem == NULL)
			return ERR_MALLOC;

		/*
		 * zero the memory
		 */
		for (index = 0; index < dwAllocationSize; index++)
			m_pUnalignedMem[index] = 0;

		// For efficiency, remove from inConfig any unnecessary phase functions
		// that are "0" and never referenced by any other functions.
		// Otherwise the workaround above to "Default missing values to ZERO"
		// adds unnecessary cost to EVERY config!
		PruneDanglingZeroPhaseFunctions(inConfig, std::string{ strPhaseID });

		/*
		 * setup the pointers for use
		 */
		m_pPhases = (PHASEDESCRIPTOR*)m_pUnalignedMem;



		/*
		 * initialize the first entry in the phase table
		 */
		m_pPhases[0].pValues
			= (value_t*)&m_pUnalignedMem[dwPhaseTableSize];
		m_pPhases[0].pFunctions = (Expression**)&m_pUnalignedMem[
			dwPhaseTableSize + dwPhaseValueTableSize];

		/*
		 * get all the phase lengths
		 */
		dwPhaseFunctions = 0;
		for (dwPhase = 0; strPhaseID[dwPhase] != '\0'; dwPhase++)
		{

			/*
			 * calculate the value and function pointers
			 * relative to the first entry in the phase table
			 */
			m_pPhases[dwPhase].pValues
				= &m_pPhases[0].pValues[dwPhaseFunctions];

			m_pPhases[dwPhase].pFunctions
				= &m_pPhases[0].pFunctions[dwPhaseFunctions];


			/*
			 * loop and count the number of expressions in each phase
			 */
			index = 0;
			int ret = snprintf(strID, _countof(strID),
								"%c%d", strPhaseID[dwPhase], index++);
			if (ret < 0 || (size_t)ret >= _countof(strID))
				return FAILURE;

			while (inConfig->GetValue(strID) != NULL)
			{
				/*
				 * add values to MyDictionary
				 */
				err = inoutValues->SetValue(strID,
					&m_pPhases[dwPhase].pValues[index - 1]);

				if (err != SUCCESS)
					return err;

				/*
				 * successfully added another phase function,
				 * increment the counter
				 */
				dwPhaseFunctions++;

				/*
				 * make the next ID
				 */
				int ret = snprintf(strID, _countof(strID), "%c%d", strPhaseID[dwPhase], index++);
				if (ret < 0 || (size_t)ret >= _countof(strID))
					return FAILURE;
			}

			/*
			 * set the phase length (number of functions in the phase)
			 */
			m_pPhases[dwPhase].dwPhaseLength = index - 1;
		}


#if EXTREME_DEBUGGING
#ifndef HIDE_INIT_TRACE
		if (NumDimensions() == 1)
			DumpToFile("error.txt", "getting ready to compile the Phase Functions", "\n");
#endif
#endif

		/*
		 * compile the Expressions
		 */
		for (dwPhase = 0; dwPhase < NumPhases() && dwPhase < (dwAllocationSize / sizeof(*m_pPhases)); ++dwPhase)
		{
			// NOTE: The above check should be unnecessary: "&& dwPhase < (dwAllocationSize/sizeof(*m_pPhases))"
			// but I get a warning on the following line
			// WARNING C6385: Reading invalid data from 'm_pPhases':  the readable size is 'dwAllocationSize*1' bytes, but '24' bytes may be read.
			// The initial part of the check "dwPhase < NumPhases()" should already be sufficient, but the static analyzer can't figure it out.
			//
			// Adding an unnecessary check to address the issue without suppressing the warning.
			for (index = 0; index < m_pPhases[dwPhase].dwPhaseLength; ++index)
			{
				/* did I forget to do tolower? */
				int ret = snprintf(strID, _countof(strID), "%c%d", strPhaseID[dwPhase], index);
				if (ret < 0 || (size_t)ret >= _countof(strID))
					return FAILURE;

				err = Expression::Compile(inConfig->GetValue(strID),
					&(m_pPhases[dwPhase].pFunctions[index]),
					inoutValues,
					inGlobals); /* the array variables */

				if (err != SUCCESS)
				{
#if EXTREME_DEBUGGING
					DumpToFile("error.txt", ">>> Phase Function : ");
					DumpToFile("error.txt", inConfig->GetValue("NAME", ""), " <<<\n");
					DumpToFile("error.txt", ErrorString(err), " <<<\n");
					DumpToFile("error.txt", strID, "<-- strExpressionID\n");
					DumpToFile("error.txt", inConfig->GetValue(strID, ""), "<--\n");
					inoutValues->DebugDumpContents("error.txt");
#endif
					return err;
				}

			}
		}


#if EXTREME_DEBUGGING
#ifndef HIDE_INIT_TRACE
		if (NumDimensions() == 1)
			DumpToFile("error.txt", "compiled the Phase Functions", "\n");
#endif
#endif







		/*
		 * check for the dimensions WITHOUT a number, just in case
		 */
		if (NumDimensions() == 0)
			return SUCCESS; /* doesn't have X or Y vars, so ignore the rest */



		/*
		 * setup the pointers for use
		 */
		m_pFunctions = (Expression***)&m_pUnalignedMem[dwPhaseTableSize
			+ dwPhaseValueTableSize + dwPhaseFunctionTableSize];



		/*
		 * initialize the first entry in the function table
		 */
		m_pFunctions[0] = (Expression**)&m_pUnalignedMem[
			dwPhaseTableSize + dwPhaseValueTableSize
				+ dwPhaseFunctionTableSize + dwParametricFunctionPointerTableSize];


#if EXTREME_DEBUGGING
#ifndef HIDE_INIT_TRACE
		if (NumDimensions() == 1)
			DumpToFile("error.txt", "about to compile the other functions", "\n");
#endif
#endif

		/*
		 * compile the Expressions
		 */
		for (dwFunction = 0; dwFunction < NumFunctions(); dwFunction++)
		{
			/*
			 * set the function pointer table entry to the
			 * first Expression pointer for that function
			 */
			m_pFunctions[dwFunction] = &m_pFunctions[0][dwFunction * NumDimensions()]; /* is this right? */



			for (index = 0; index < NumDimensions(); index++)
			{
				int ret = snprintf(strID, _countof(strID), "%c%d", strDimensionID[index], dwFunction);
				if (ret < 0 || (size_t)ret >= _countof(strID))
					return FAILURE;

				/*
				 * this is causing an error for some WaveShapes
				 */
				err = Expression::Compile(inConfig->GetValue(strID),
					&(m_pFunctions[dwFunction][index]),
					inoutValues,
					inGlobals); /* the array variables */

				if (err != SUCCESS)
				{
#if EXTREME_DEBUGGING
					DumpToFile("error.txt", ">>> Phase Function : ");
					DumpToFile("error.txt", inConfig->GetValue("NAME", ""), " <<<\n");
					DumpToFile("error.txt", ErrorString(err), " <<<\n");
					DumpToFile("error.txt", strID, "<--\n");
					DumpToFile("error.txt", inConfig->GetValue(strID, ""), "<--\n");
					inoutValues->DebugDumpContents("error.txt");
#endif
					return err;
				}

			}
		}

#if EXTREME_DEBUGGING
#ifndef HIDE_INIT_TRACE
		if (NumDimensions() == 1)
			DumpToFile("error.txt", "compiled the other functions", "\n");

		DumpToFile("error.txt", "returning SUCCESS", "\n");
#endif
#endif

		return SUCCESS;
	};


	// Yes this is inefficient, but it's portable and should work
	// TODO: Consider more efficient approaches, and move to TextUtils.cpp
	std::string toLower(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char ch) { return std::tolower(ch); });
		return s;
	}
	bool MyStrStrIA(const char* pHaystack, const char* pNeedle)
	{
		std::string haystack = { pHaystack };
		std::string needle = { pNeedle };

		haystack = toLower(haystack);
		needle = toLower(needle);

		return haystack.find(needle) != std::string::npos;
	}

	/// <summary>
	/// Prunes unreferenced zero-valued phase function entries in the config to limit
	/// unnecessary work and maintain contiguous index values within each phase.
	///
	/// <para>---</para>
	///
	/// This process identifies zero-valued entries that are not referenced as substrings in any
	/// other key/value pair and culls the tail of unused functions at the end of each phase.
	///
	/// <para>---</para>
	///
	/// NOTE: Some unreferenced functions may remain in the middle of a range to maintain
	/// a contiguous index structure required by existing logic.
	/// </summary>
	/// <param name="config">The dictionary to be sanitized in place.</param>
	/// <param name="phases">A string containing the valid phase characters (e.g., "ABCD").</param>
	/// <returns>An error code indicating success or failure.</returns>
	error_t PruneDanglingZeroPhaseFunctions(MyDictionary<mychar_t*>* config, const std::string& phases)
	{
		error_t err = SUCCESS;
		std::map<char, size_t> maxNonzeroInPhase;

		// Pseudocode:
		// for each value in inConfig->AsArray(&outArray, &dwNumElements)
		// if (isnumeric(id[1])
		//		&& id[0] is in strPhaseID
		//		&& its VALUE == "0")
		//	{
		//		Loop through the entire array and look for id in *any* of the values
		//		if (no matches)
		//		{
		//			build a vector of values to cleanse
		//		}
		//	}
		//
		// for each unused ID that is above the highest used ID, RemoveValue
		Association<mychar_t*>** configArray = nullptr;
		DWORD dwNumElements = 0;
		std::vector<std::string> toDelete;
		err = config->AsArray(&configArray, &dwNumElements);
		if (err != SUCCESS)
			return err;

		for (DWORD i = 0; i < dwNumElements; ++i)
		{
			mychar_t* keyID = configArray[i]->GetKey();
			mychar_t* val = configArray[i]->GetValue();

			if (val[0] == '0' && val[1] == '\0') // Value is exactly "0"
			{
				// Determine if Key is a Phase Function ID
				if (std::isdigit(keyID[1])
					&& (phases.find(toupper(keyID[0])) != std::string::npos
						|| phases.find(tolower(keyID[0])) != std::string::npos))
				{
					bool found = false;
					// Iterate through the list and check values,
					// ALL values including PHASE FUNCTIONS and REGULAR FUNCTIONS like X0,Y1,Z2
					for (DWORD inner = 0; inner < dwNumElements; ++inner)
					{
						mychar_t* valInner = configArray[inner]->GetValue();

						// Determine if the phase function ID is referenced in this value.
						if (MyStrStrIA(valInner, keyID))
						{
							// Yes, this gets some false positives, like key="A2" will match "A22",
							// but this doesn't matter too much since we would need to preserve "A2"
							// in this case anyway.
							//
							// The intention is to not have to add 40 "dummy" functions to EVERY config.
							// If there are a couple extra ever now and then, I'm not worried.
							// We MUST NOT cull used values; unused values are mostly harmless.
							found = true;

							const char currentPhase = keyID[0];
							const int currentIndex = atoi(&keyID[1]);
							auto itFound = maxNonzeroInPhase.find(currentPhase);
							if (itFound != maxNonzeroInPhase.cend())
							{
								if (itFound->second < static_cast<size_t>(currentIndex))
								{
									maxNonzeroInPhase.erase(itFound);
									maxNonzeroInPhase.emplace(currentPhase, currentIndex);
								}
							}
							else
								maxNonzeroInPhase.emplace(currentPhase, currentIndex);

							break;
						}
					}

					// Add the key to delete
					if (!found)
						toDelete.emplace_back(std::string{ keyID });
				}
			}
			else
			{
				// Value is non-zero: keep track of the highest value per-phase
				auto pos = phases.find(toupper(keyID[0]));
				if (pos != std::string::npos)
				{
					const char currentPhase = keyID[0];
					const int currentIndex = atoi(&keyID[1]);
					auto itFound = maxNonzeroInPhase.find(currentPhase);
					if (itFound != maxNonzeroInPhase.cend())
					{
						if (itFound->second < (size_t)(currentIndex))
						{
							maxNonzeroInPhase.erase(itFound);
							maxNonzeroInPhase.emplace(currentPhase, currentIndex);
						}
					}
					else
						maxNonzeroInPhase.emplace(currentPhase, currentIndex);
				}
			}
		}

		// Iterate through the Phase functions again, and find the highest value in each phase that is non-zero;
		// Only delete above that point.
		// It's hacky, but gets the job done. For the common case, it eliminates all the excess.
		// For the Configs that have stray variables, just those configs will have extra functions to evaluate.
		//
		// Since all the function names are now contiguous, we don't have to modify the code at the end of
		// PhaseFunction::Initialize to account for non-contiguous ranges where each phase could have a
		// different bound, because we don't easily know when to stop. Now the stop condition has been restored.
		for (const std::string& keyID : toDelete)
		{
			auto pos = phases.find(toupper(keyID[0]));
			if (pos != std::string::npos)
			{
				const char currentPhase = keyID[0];
				const int currentIndex = atoi(&keyID[1]);
				auto itFound = maxNonzeroInPhase.find(currentPhase);
				if (itFound != maxNonzeroInPhase.cend())
				{
					if (size_t(currentIndex) > itFound->second)
					{
						config->RemoveValue(keyID.c_str());
					}
				}
				else
				{
					config->RemoveValue(keyID.c_str());
				}
			}
		}

		return err;
	}


#if !EXTREME_DEBUGGING
	/****************************************************************************
	 *
	 * EvaluatePhase -
	 *
	 ****************************************************************************/
	error_t    EvaluatePhase(const DWORD dwPhaseNumber)
	{
		DWORD i;

		if (dwPhaseNumber >= NumPhases())
			return ERR_BOUNDS;

		for (i = 0; i < m_pPhases[dwPhaseNumber].dwPhaseLength; i++)
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
	error_t    EvaluatePhase(const DWORD dwPhaseNumber)
	{
		DWORD i;

		if (dwPhaseNumber >= NumPhases())
		{
			DumpToFile("error.txt", "there are only ", NumPhases(), " phases");
			DumpToFile("error.txt", " not ", dwPhaseNumber, " phases\n");
			return FAILURE;
		}

		if (m_pPhases == NULL)
		{
			DumpToFile("error.txt", "m_pPhases is NULL!", "\n");
			MessageBoxA(NULL, "m_pPhases is NULL!", "ERROR!", MB_OK);
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

		if (m_pPhases[dwPhaseNumber].dwPhaseLength > 0)  /* aarruugghh!! please work! */
		{
			if (m_pPhases[dwPhaseNumber].pFunctions == NULL)
			{
				DumpToFile("error.txt", "m_pPhases[dwPhaseNumber].pFunctions is NULL!", "\n");
				MessageBoxA(NULL, "m_pPhases[dwPhaseNumber].pFunctions is NULL!", "ERROR!", MB_OK);
				return ERR_NULL;
			}


			for (i = 0; i < m_pPhases[dwPhaseNumber].dwPhaseLength; i++)
			{
				if (m_pPhases[dwPhaseNumber].pFunctions[i] == NULL)
				{
					DumpToFile("error.txt", "m_pPhases[dwPhaseNumber].pFunctions[i] is NULL!", "\n");
					DumpToFile("error.txt", "dwPhaseNumber = ", dwPhaseNumber, "\n");
					DumpToFile("error.txt", "i = ", i, "\n");
					DumpToFile("error.txt", "m_pPhases[dwPhaseNumber].dwPhaseLength = ", m_pPhases[dwPhaseNumber].dwPhaseLength, "\n");
					MessageBoxA(NULL, "m_pPhases[dwPhaseNumber].pFunctions[i] is NULL!", "ERROR!", MB_OK);
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
	value_t    EvaluateFunction(const DWORD dwFunction, const DWORD dwDimension)
	{
#if EXTREME_DEBUGGING
		Expression*** pppE;
		Expression** ppE;
		Expression* pE;
		value_t     v;
#endif

		if (dwFunction < NumFunctions() && dwDimension < NumDimensions())
		{
#if EXTREME_DEBUGGING
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
#if EXTREME_DEBUGGING
			DumpToFile("error.txt", "invalid dimensions?  in PhaseFunction. Evaluate Function", "\n");
#endif
			return 0.0f;
		}
	};


	friend error_t ImportPhase(PhaseFunction& DestPF, PhaseFunction& SourcePF, const DWORD dwPhase)
	{
		DWORD   i;
		DWORD   dwPhaseLength;

		if (SourcePF.m_dwNumPhases <= dwPhase
			|| DestPF.m_dwNumPhases <= dwPhase)
			return ERR_BOUNDS;

		dwPhaseLength = min(DestPF.m_pPhases[dwPhase].dwPhaseLength,
			SourcePF.m_pPhases[dwPhase].dwPhaseLength);

		for (i = 0; i < dwPhaseLength; i++)
		{
			DestPF.m_pPhases[dwPhase].pValues[i] =
				SourcePF.m_pPhases[dwPhase].pValues[i];
		}

		return SUCCESS;
	};



	/*
	 * to reduce the chances of accidental modification
	 */
	inline DWORD    NumDimensions(void) { return m_dwNumDimensions; };
	inline DWORD    NumFunctions(void) { return m_dwNumFunctions; };
	inline DWORD    NumPhases(void) { return m_dwNumPhases; };

protected:
	inline void     SetNumDimensions(const DWORD dwNumDimensions)
	{
		m_dwNumDimensions = dwNumDimensions;
	};
	inline void     SetNumFunctions(const DWORD dwNumFunctions)
	{
		m_dwNumFunctions = dwNumFunctions;
	};
	inline void     SetNumPhases(const DWORD dwNumPhases)
	{
		m_dwNumPhases = dwNumPhases;
	};


protected:
	DWORD           m_dwNumDimensions;
	DWORD           m_dwNumFunctions;
	DWORD           m_dwNumPhases;
	PHASEDESCRIPTOR* m_pPhases;
	Expression*** m_pFunctions;
	BYTE* m_pUnalignedMem;
};

#if EXTREME_DEBUGGING
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#endif

#endif  /* _PhaseFunction_H_ */

