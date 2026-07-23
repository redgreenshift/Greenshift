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
  * FilingClerk - handles all file access
  *
  ****************************************************************************/

#include "FilingClerk.h"
#include <ctype.h>

  /****************************************************************************
   *
   * FilingClerk - constructor
   *
   ****************************************************************************/
FilingClerk::FilingClerk()
{
	m_pFile = NULL;
	m_strFolder = NULL;
	m_strFile = NULL;
	m_strRoot = NULL;

	m_pTokenBuffer = NULL;
	m_nBufferLength = 0;
	m_nUsedLength = 0;

	m_hFindHandle = 0;
}


/****************************************************************************
 *
 * ~FilingClerk - destructor
 *
 ****************************************************************************/
FilingClerk::~FilingClerk()
{
	SAFE_FREE(m_strRoot);
	SAFE_FREE(m_strFile);
	SAFE_FREE(m_strFolder);
	SAFE_FREE(m_pTokenBuffer);
}




/*

  #include <stdlib.h>

  void _makepath( outPath, driveLetter, dirPath, filename, ext )

  _access( filename, mode )
 */



 /****************************************************************************
  *
  * SetRoot - root directory
  *
  ****************************************************************************/
error_t    FilingClerk::SetRoot(char* strRoot)
{
	SAFE_FREE(m_strRoot);
	m_strRoot = strdup(strRoot);

	return (m_strRoot != NULL) ? SUCCESS : ERR_MALLOC;
}

/****************************************************************************
 *
 * SetFolder - current filing folder to search
 *
 ****************************************************************************/
error_t    FilingClerk::SetFolder(char* strFolder)
{
	SAFE_FREE(m_strFolder);
	m_strFolder = strdup(strFolder);

	return (m_strFolder != NULL) ? SUCCESS : ERR_MALLOC;
}



/****************************************************************************
 *
 * OpenFile -
 *
 ****************************************************************************/
error_t FilingClerk::OpenFile(const char* strFileName, const char* strAccess)
{
	CloseFile(); /* make sure it's not already open */

	/*
	 * open the file for reading
	 */
	if ((m_pFile = fopen(strFileName, strAccess)) == NULL)
		return FAILURE;  //ERR_FILE

	return SUCCESS;
}


/****************************************************************************
 *
 * CloseFile -
 *
 ****************************************************************************/
error_t    FilingClerk::CloseFile(void)
{
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	return SUCCESS;
}


/****************************************************************************
 *
 * GetChar - returns the next character in the stream
 *
 ****************************************************************************/
int FilingClerk::GetChar(void)
{
	int c;
	char* lpTempString;

	if (m_nUsedLength >= m_nBufferLength - 2)
	{
		m_nBufferLength *= 2;

		/*
		 * if an error occurs while resizing the string,
		 * pretend we just found the end of the file...
		 * yeah, that's it... the end of the file...
		 */
		if ((lpTempString = (char*)realloc(m_pTokenBuffer,
			m_nBufferLength * sizeof(*m_pTokenBuffer))) == NULL)
			return EOF;

		m_pTokenBuffer = lpTempString;
	}

	if ((c = getc(m_pFile)) != EOF)
	{
		m_pTokenBuffer[m_nUsedLength++] = c;
		m_pTokenBuffer[m_nUsedLength] = '\0';
	}

	return c;
}



/****************************************************************************
 *
 * UnGetChar - puts a character back in the stream
 *
 ****************************************************************************/
int FilingClerk::UnGetChar(int c)
{
	if (m_nUsedLength > 0)
	{
		m_nUsedLength--;

		m_pTokenBuffer[m_nUsedLength] = '\0';
	}

	return ungetc(c, m_pFile);
}


/****************************************************************************
 *
 * GetChar - read in the next character from the file
 *
 ****************************************************************************/
int FilingClerk::PeekChar(void)
{
	int c;

	c = getc(m_pFile);

	ungetc(c, m_pFile);

	return c;
}



/****************************************************************************
 *
 * ClearBuffer - reset the token buffer so we can search for another token
 *
 ****************************************************************************/
error_t    FilingClerk::ClearBuffer(void)
{
	if (m_pTokenBuffer == NULL)
	{
		m_nBufferLength = 32;

		if ((m_pTokenBuffer = (char*)malloc(
			m_nBufferLength * sizeof(*m_pTokenBuffer))) == NULL)
			return ERR_MALLOC;
	}

	m_nUsedLength = 0;
	m_pTokenBuffer[0] = '\0';

	return SUCCESS;
}



/****************************************************************************
 *
 * TokenBuffer - return the internal buffer used to collect characters
 *
 ****************************************************************************/
char* FilingClerk::TokenBuffer(void)
{
	return m_pTokenBuffer;
}




/****************************************************************************
 *
 * GetFirstFile -
 *
 ****************************************************************************/
char* FilingClerk::GetFirstFile(const char* strFileMask)
{
	//    char strFile[260];
	char strFile[_MAX_PATH];

	strcpy(strFile, m_strRoot);
	strcat(strFile, "\\");
	strcat(strFile, m_strFolder);
	strcat(strFile, "\\");
	strcat(strFile, strFileMask);

	/*
	 * close any previous unclosed searches
	 */
	if (m_hFindHandle != 0)
		EndSearch();

	/*
	 * find the first file
	 */
	 //    if( (m_hFindHandle = _findfirst( strFile, &m_wFindData )) != 0 )
	if ((m_hFindHandle = _findfirst(strFile, &m_wFindData)) != -1)
	{
		if (!(m_wFindData.attrib & _A_SUBDIR)
			&& !(m_wFindData.attrib & _A_HIDDEN))
			return m_wFindData.name;
		else
			return GetNextFile();
	}
	else
	{
#if EXTREME_DEBUGGING
		DumpToFile("error.txt",
			"ERROR: couldn't find any files matching the search string.\n\t");
		DumpToFile("error.txt", strFile, "\n");
#endif
		return EndSearch();
	}
}


/****************************************************************************
 *
 * GetNextFile -
 *
 ****************************************************************************/
char* FilingClerk::GetNextFile(void)
{
	/*
	 * if a search has been started, find the next one
	 */
	if (m_hFindHandle != 0)
	{
		/*
		 * if more files are found, return its name
		 */
		if (_findnext(m_hFindHandle, &m_wFindData) == 0)
		{
			/*
			 * if the found file is not a directory, return its name
			 */
			if (!(m_wFindData.attrib & _A_SUBDIR)
				&& !(m_wFindData.attrib & _A_HIDDEN))
				return m_wFindData.name;
			else
				return GetNextFile();  /* else return the next file */
		}
		else
			return EndSearch();
	}

	return EndSearch();
}


/****************************************************************************
 *
 * EndSearch - close the search handle early
 *      does not need to be called if GetFirstFile or GetNextFile return NULL,
 *      but it's ok to call it, if it makes you feel better.
 *
 ****************************************************************************/
char* FilingClerk::EndSearch(void)
{
	/*
	 * close the find handle
	 */
	if (m_hFindHandle != 0)
		_findclose(m_hFindHandle);

	/*
	 * mark the find handle as closed
	 */
	m_hFindHandle = 0;
	return NULL;
}



/****************************************************************************
 *
 * GetToken - return the type of the next token matched on the input stream
 *
 ****************************************************************************/
token_t    FilingClerk::GetToken(void)
{
	int    c;
	int delimiter;
	enum {
		inDefault,
		inMultiCharacter,
		inSTRING,
		inLineCOMMENT,
		inCOMMENT,
		inDELIMITED
	} state = inDefault;


	/*
	 * start a new token buffer
	 */
	ClearBuffer();

	/*
	 *
	 */
	while ((c = GetChar()) != EOF)
	{
		switch (state)
		{
		default:
		case inDefault: /*****************
						 * BEGIN inDefault
						 *****************/

			switch (c)
			{
				//            case ' ':
				//            case 1:
				//            case '\n':
				//            case '\r':
				//            case '\t':
				//                m_nUsedLength--; /* ignore whitespace */
				//                break;

			case '=':
				return '=';

			case ',':
				return ',';

			case '[':
				state = inDELIMITED;
				delimiter = ']';
				break;


			case '\"':
				state = inSTRING;
				break;

			case '/':
				if (PeekChar() == '*')
					state = inCOMMENT;
				else
					if (PeekChar() == '/')
						state = inLineCOMMENT;
					else
						state = inMultiCharacter;
				break;

			case ';':
				state = inLineCOMMENT;
				break;

			default:
				//                if( strchr(",=", c) != NULL )
				//                    return c;
				//                else
								/*
								 * Space is after all control characters, and before all
								 * other typed characters.
								 */
				if (c != ' ' && !iscntrl(c) && c != 0x7f)//'\127' )
					state = inMultiCharacter;
				else
					m_nUsedLength--; /* ignore whitespace */

				break;
			}

			break;      /******************
						 * END of inDefault
						 ******************/


		case inCOMMENT: /*****************
						 * BEGIN inCOMMENT
						 *****************/

			switch (c)
			{
			case '*':
				if (PeekChar() == '/')
				{
					GetChar();

					return tokCOMMENT;
				}

				break;
			default:
				break;
			}

			break;      /******************
						 * END of inCOMMENT
						 ******************/

		case inLineCOMMENT:
			/*********************
			 * BEGIN inLineCOMMENT
			 *********************/

			switch (c)
			{
			case '\n': /* linefeed */
			case '\r': /* carriage return */
				return tokCOMMENT;

			default:
				break;
			}

			break;      /**********************
						 * END of inLineCOMMENT
						 **********************/


		case inSTRING:  /****************
						 * BEGIN inSTRING
						 ****************/

			switch (c)
			{
			case '\"':
				return tokSTRING;
				break;

			case '\\':
				m_nUsedLength--; /* backup one */

				//                switch( c = GetChar() )
				//                {
				//                case '\"':
				//                    break;
				//                }

				break;
			default:
				break;
			} /* inSTRING switch ( c ) */


			break;      /*****************
						 * END of inString
						 *****************/


		case inDELIMITED:/*******************
						  * BEGIN inDELIMITED
						  *******************/

			if (c == delimiter)
				return tokHEADER;

			break;      /********************
						 * END of inDELIMITED
						 ********************/


		case inMultiCharacter:
			/************************
			 * BEGIN inMultiCharacter
			 ************************/

			switch (c)
			{
			case ' ':
			case '\t':
			case '\n': /* linefeed */
			case '\r': /* carriage return */
				m_pTokenBuffer[--m_nUsedLength] = '\0';
				return tokID;
				//                return tokSTRING;

			case '=':
			case ',':
			case '\"':
			case '/':

				UnGetChar(c);        /* unget... yadda */

				return tokID;

			} /* inMultiCharacter switch ( c ) */


			break;      /*************************
						 * END of inMultiCharacter
						 *************************/

		} /* switch ( state ) */

		/********************************
		 *
		 * the "END of inXXXX"
		 * BREAK statements jump to here
		 *
		 ********************************/

	} /* while ( GetChar != EOF ) */

	/*
	 * handle the case where a token ends right at EOF
	 */
	if (strlen(TokenBuffer()) > 1)
	{
		UnGetChar(c);

		switch (state)
		{
		case inSTRING:
			return tokSTRING;
		case inCOMMENT:
		case inLineCOMMENT:
			return tokCOMMENT;
		default:
			return tokID;
		}
	}

	return tokEND;

}  /* END - GetToken() */



/****************************************************************************
 *
 * LoadConfig - read in a directory of files, as a set of associations,
 *        allocates a MyDictionary for each successfully parsed file, and
 *        returns the number of successfully parsed files.
 *
 *        Each MyDictionary has an extra "NAME" value added so you can tell which
 *        file it was from, this is so it can be used to display the name of a
 *        particle or whatnot
 *
 ****************************************************************************/
error_t FilingClerk::LoadConfig(MyDictionary<char*>** ppConfig,
	DWORD* inoutNumAllocated)
{
	error_t				err = SUCCESS;
	char*				strFile = NULL;
	DWORD				nConfig = 0;
	DWORD               nNumConfig = 0;
	MyDictionary<char*>* pTmpConfig = NULL;


	if (ppConfig == NULL || inoutNumAllocated == NULL)
		return ERR_NULL;

	nNumConfig = 0;
	strFile = GetFirstFile("*.*");

	/*
	 * count the number of files
	 */
	while (strFile != NULL)
	{
		if (!EndsIn(strFile, ".zip"))
			nNumConfig++;
		strFile = GetNextFile();
	}

	if (nNumConfig > 0)
	{
		nNumConfig = nNumConfig + *inoutNumAllocated;
		/*
		 * allocate space for nConfig Dictionaries
		 */
		pTmpConfig = new MyDictionary<char*>[nNumConfig];
		if (pTmpConfig == NULL)
			return ERR_MALLOC;


		/*
		 * now find all the files over again
		 */

		strFile = GetFirstFile("*.*");

		nConfig = *inoutNumAllocated;

		/*
		 * loop through the files again, but in case the number of files
		 * changed (for whatever reason), use the extra check
		 */
		while ((strFile != NULL) && (nConfig < nNumConfig))
		{
			if (!EndsIn(strFile, ".zip"))
			{
				err = GetData(strFile, &pTmpConfig[nConfig]);

				/*
				#if EXTREME_DEBUGGING
				#define DICT_DUMP_FILE "configdump.txt"
								DumpToFile(DICT_DUMP_FILE, "\n-----------------------\nNAME: ");
								DumpToFile(DICT_DUMP_FILE, strFile, "\n");
								pTmpConfig[nConfig].DebugDumpContents(DICT_DUMP_FILE);
				#endif
				*/

				if (err == SUCCESS)
					err = pTmpConfig[nConfig].SetValue(
						"NAME", strdup(strFile));

				if (err == SUCCESS)
					nConfig++;
				else
					pTmpConfig[nConfig].WipeContents();
			}


			strFile = GetNextFile();
		}
#ifdef UNDEFINED
	}

	nNumConfig = nConfig;

	for (nConfig = 0; nConfig < *inoutNumAllocated; nConfig++)
	{
		err = pTmpConfig[nConfig].Import((*ppConfig)[nConfig]);
		if (err != SUCCESS)
			break;
	}


	if (err == SUCCESS)
	{
		if (*inoutNumAllocated > 0)
			delete[] * ppConfig;

		*inoutNumAllocated = nNumConfig;
		*ppConfig = pTmpConfig;

		return SUCCESS;
	}
	else
	{
		delete[] pTmpConfig;
		return err;
	}
#else
		nNumConfig = nConfig;

		for (nConfig = 0; nConfig < *inoutNumAllocated; nConfig++)
		{
			err = pTmpConfig[nConfig].Import((*ppConfig)[nConfig]);
			if (err != SUCCESS)
				break;
		}


		if (err == SUCCESS)
		{
			if (*inoutNumAllocated > 0)
				delete[] * ppConfig;

			*inoutNumAllocated = nNumConfig;
			*ppConfig = pTmpConfig;

			return SUCCESS;
		}
		else
		{
			delete[] pTmpConfig;
			return err;
		}
	}
#endif
	return err;
}


/****************************************************************************
 *
 * LoadColorMaps - similar to LoadConfig, except handles the special parsing
 *        requirements for palettes, color maps, or whatever you call them.
 *
 ****************************************************************************/
error_t FilingClerk::LoadColorMaps(MyDictionary<char*>** ppConfig,
	DWORD* inoutNumAllocated)
{
	error_t             err = SUCCESS;
	char* strFile = NULL;
	DWORD               nConfig = 0;
	DWORD               nNumConfig = 0;
	MyDictionary<char*>* pTmpConfig = NULL;
	//    DWORD    i;

	if (ppConfig == NULL || inoutNumAllocated == NULL)
		return ERR_NULL;


	nNumConfig = 0;
	strFile = GetFirstFile("*.*");

	/*
	 * count the number of files
	 */
	while (strFile != NULL)
	{
		if (!EndsIn(strFile, ".zip"))
			nNumConfig++;
		strFile = GetNextFile();
	}

	if (nNumConfig > 0)
	{
		nNumConfig = nNumConfig + *inoutNumAllocated;
		/*
		 * allocate space for nConfig Dictionaries
		 */
		pTmpConfig = new MyDictionary<char*>[nNumConfig];
		if (pTmpConfig == NULL)
			return ERR_MALLOC;

		/*
		 * now find all the files over again
		 */

		strFile = GetFirstFile("*.*");

		nConfig = *inoutNumAllocated;

		/*
		 * loop through the files again, but in case the number of files
		 * changed (for whatever reason), use the extra check
		 */
		while ((strFile != NULL) && (nConfig < nNumConfig))
		{
			if (!EndsIn(strFile, ".zip"))
			{
				if (EndsIn(strFile, ".map"))
				{
					err = GetColorMap(strFile, &pTmpConfig[nConfig]);
				}
				else
					err = GetData(strFile, &pTmpConfig[nConfig]);


				if (err == SUCCESS)
					err = pTmpConfig[nConfig].SetValue(
						"NAME", strdup(strFile));

				if (err == SUCCESS)
					nConfig++;
				else
					pTmpConfig[nConfig].WipeContents();
			}

			strFile = GetNextFile();
		}
#ifdef UNDEFINED
	}

	nNumConfig = nConfig;



	for (nConfig = 0; nConfig < *inoutNumAllocated; nConfig++)
	{
		err = pTmpConfig[nConfig].Import((*ppConfig)[nConfig]);
		if (err != SUCCESS)
			break;
	}


	if (err == SUCCESS)
	{
		if (*inoutNumAllocated > 0)
			delete[] * ppConfig;

		*inoutNumAllocated = nNumConfig;
		*ppConfig = pTmpConfig;

		return SUCCESS;
	}
	else
	{
		delete[] pTmpConfig;
		return err;
	}
#else
		nNumConfig = nConfig;



		for (nConfig = 0; nConfig < *inoutNumAllocated; nConfig++)
		{
			err = pTmpConfig[nConfig].Import((*ppConfig)[nConfig]);
			if (err != SUCCESS)
				break;
		}


		if (err == SUCCESS)
		{
			if (*inoutNumAllocated > 0)
				delete[] * ppConfig;

			*inoutNumAllocated = nNumConfig;
			*ppConfig = pTmpConfig;

			return SUCCESS;
		}
		else
		{
			delete[] pTmpConfig;
			return err;
		}
	}
#endif
	return err;
}


/****************************************************************************
 *
 * GetColorMap - returns file as a MyDictionary, just like GetData, except the
 *            file is just a list of numbers, sets of three, for the RGB parts
 *
 ****************************************************************************/
error_t FilingClerk::GetColorMap(char* id, MyDictionary<char*>* outMyDictionary)
{
	char    strID[8];
	error_t    err;
	int        token = 0;
	char* strTOKEN = NULL;
	char* strToken = NULL;
	char* strExp = NULL;
	char* strTmp = NULL;
	enum { look_for_red, look_for_green, look_for_blue } state = look_for_red;
	DWORD    nEntryNumber = 0;


	//    return FAILURE;  /* hack to get it to work */

	if (id == NULL || outMyDictionary == NULL)
		return FAILURE;

	/*****/


	if (m_strFolder == NULL || m_strRoot == NULL)
	{
		SAFE_FREE(m_strFile);
		m_strFile = strdup(id);
		if (m_strFile == NULL)
			return ERR_MALLOC;
	}
	else
	{
		strTmp = (char*)realloc(m_strFile, strlen(id)
			+ strlen(m_strFolder)
			+ strlen(m_strRoot) + 3);
		if (strTmp == NULL)
			return ERR_REALLOC;


		m_strFile = strTmp;
		strcpy(m_strFile, m_strRoot);
		if (m_strRoot[0] != '\0')
			strcat(m_strFile, "\\");
		strcat(m_strFile, m_strFolder);
		if (m_strFolder[0] != '\0')
			strcat(m_strFile, "\\");
		strcat(m_strFile, id);

		//        sprintf( m_strFile, "%s\\%s", m_strFolder, id );
	}


	/******/



		/*
		 * open the file for reading
		 */
	if ((err = OpenFile(m_strFile)) != SUCCESS)
		return err;



	do
	{
		token = GetToken();
		/*
		 * strTOKEN is freed after each iteration of the loop
		 * thus it should not be modified.  However, strToken
		 * may be modified inside the switch statement since
		 * it may be useful to remove the quotes on a string
		 */
		SAFE_FREE(strTOKEN);
		strTOKEN = strToken = strdup(TokenBuffer());
		if (strTOKEN == NULL)
			return ERR_MALLOC;


		//    DumpToFile( "error2.txt", token, "\n" );

		switch (token)
		{
		default:
		case tokCOMMENT: /* ignore comments */
			break;

			//        case tokSTRING:
		case tokID:
		case tokNUMBER: /* number parsing doesn't work yet */

			/*
			 * should allow higher numbers...
			 */
			if (nEntryNumber > 255)
			{
				token = tokEND;  /* exit! */
				break;
			}

			{
				int blah = atoi(strToken);
				if (blah == 0 && strToken[0] != '0')
					break;  /* ignore non numbers */
			}



			switch (state)
			{
			case look_for_red:
				if (snprintf(strID, _countof(strID), "r%d", nEntryNumber) == 0)
					break;
				outMyDictionary->SetValue(strID, (strToken));
				strTOKEN = NULL;
				state = look_for_green;
				break;

			case look_for_green:
				if (snprintf(strID, _countof(strID), "g%d", nEntryNumber) == 0)
					break;
				outMyDictionary->SetValue(strID, (strToken));
				strTOKEN = NULL;
				state = look_for_blue;
				break;

			case look_for_blue:
				if (snprintf(strID, _countof(strID), "b%d", nEntryNumber) == 0)
					break;
				nEntryNumber++;
				outMyDictionary->SetValue(strID, (strToken));
				strTOKEN = NULL;
				state = look_for_red;
				break;

			default:
				break;
			}
			break;

		} /* switch ( token ) */


		/******************
		 * BREAK jumps here
		 ******************/
		SAFE_FREE(strTOKEN);
		strToken = NULL;

	} while (token != tokEND);


	//    outMyDictionary->DebugDumpContents( "argh.txt" );

	return CloseFile();
}



/****************************************************************************
 *
 * GetData - parses the file, and returns file as a MyDictionary
 *
 ****************************************************************************/
error_t    FilingClerk::GetData(char* id, MyDictionary<char*>* outMyDictionary,
	MyDictionary<MyDictionary<char*>*>* outExtra)
{
	error_t    err = ERR_FAKE;
	int        token = 0;
	char* strTOKEN = NULL;
	char* strToken = NULL;
	char* strID = NULL;
	char* strExp = NULL;
	char* strTmp = nullptr;
	enum { look_for_id, look_for_expression } state = look_for_id;
	enum { d_default, d_extra } DictionaryRenamedToBuild = d_default;
	MyDictionary<char*>* pIndirectMyDictionary = outMyDictionary;

	/*
	 * outExtra contains extra dictionaries for storing section specific data
	 *
	 * the key is of the form "[section-name]"
	 *
	 * when a section is encountered in a config file, if the corresponding
	 * section name is defined in outExtra, then all config data following
	 * that section heading is placed in the corresponding extra MyDictionary.
	 *
	 * if the section name is not defined, or outExtra is NULL, then the
	 * everything for that section is added to the default outMyDictionary
	 */


	if (id == NULL || outMyDictionary == NULL)
		return ERR_NULL;


	if (m_strFolder == NULL || m_strRoot == NULL)
	{
		SAFE_FREE(m_strFile);
		m_strFile = strdup(id);
		if (m_strFile == NULL)
			return ERR_MALLOC;
	}
	else
	{
		strTmp = (char*)realloc(m_strFile, strlen(id)
			+ strlen(m_strFolder)
			+ strlen(m_strRoot) + 3);
		if (strTmp == NULL)
			return ERR_REALLOC;


		m_strFile = strTmp;
		strcpy(m_strFile, m_strRoot);
		if (m_strRoot[0] != '\0')
			strcat(m_strFile, "\\");
		strcat(m_strFile, m_strFolder);
		if (m_strFolder[0] != '\0')
			strcat(m_strFile, "\\");
		strcat(m_strFile, id);

		//        sprintf( m_strFile, "%s\\%s", m_strFolder, id );
	}

	/*
	 * open the file for reading
	 */
	if ((err = OpenFile(m_strFile)) != SUCCESS)
	{
#if EXTREME_DEBUGGING
		if (err != SUCCESS)
		{
			DumpToFile("error.txt",
				"ERROR: FilingClerk::GetData() -> OpenFile()", "\n");
			DumpToFile("error.txt", ErrorString(err), "\n");
		}
#endif
		return err;
	}



	do
	{
		token = GetToken();
		/*
		 * strTOKEN is freed after each iteration of the loop
		 * thus it should not be modified.  However, strToken
		 * may be modified inside the switch statement since
		 * it may be useful to remove the quotes on a string
		 */
		SAFE_FREE(strTOKEN);
		strTOKEN = strToken = strdup(TokenBuffer());
		if (strTOKEN == NULL)
			return ERR_MALLOC;

		/*
		 * when a header is found, and the header is defined,
		 * start using the extra MyDictionary, else, default to outMyDictionary
		 */
		if (token == tokHEADER)
		{
			if (outExtra == NULL ||
				(pIndirectMyDictionary = outExtra->GetValue(strToken)) == NULL
				)
			{
				pIndirectMyDictionary = outMyDictionary;
			}

			state = look_for_id;
			continue;
		}


		switch (state)
		{
		default:
		case look_for_id:
			/*******************
			 * BEGIN look for ID
			 *******************/

			switch (token)
			{
			case tokCOMMENT: /* ignore comments */
				break;

			case '=':
				state = look_for_expression;
				break;

			case tokSTRING:
			case tokID:
			default:
				/*
				 * if this is the first time through here, strID == NULL
				 * so just duplicate the token string
				 */
				if (strID == NULL)
				{
					strID = strdup(strToken);
					if (strID == NULL)
						return ERR_MALLOC;
				}
				else
				{
					strTmp = (char*)realloc(strID, strlen(strID)
						+ strlen(strToken)
						+ 1);
					if (strTmp == NULL)
						return ERR_REALLOC;

					/* append the token string and assign it to strID */
					strID = strcat(strTmp, strToken);
				}

				break;

			} /* switch ( token ) */

			break;        /********************
						 * END of look for ID
						 ********************/


		case look_for_expression:
			/***************************
			 * BEGIN look for expression
			 ***************************/

			switch (token)
			{
			case tokCOMMENT:
				break;
			case tokEND:
			case ',':    /* found the expression part,
						 * continue searching for an
						 * ID when done here
						 */
				state = look_for_id;

				/*
				 * should never be NULL, but just in case...
				 * keep MyDictionary from dereferencing NULL
				 */
				if (strID == NULL)    strID = "";
				if (strExp == NULL)    strExp = "";

				//                err = outMyDictionary->SetValue( strID, strExp );
				err = pIndirectMyDictionary->SetValue(strID, strExp);
				if (err != SUCCESS)
					return err;
				/*
				 * ensure we don't free the data passed to MyDictionary, as
				 * MyDictionary does not make a copy of the data passed to it
				 */
				strID = NULL;
				strExp = NULL;

				break;

			case tokSTRING:
				/*
				 * remove quotes on the string
				 */
				strToken[strlen(strToken) - 1] = '\0';
				strToken++;
			case tokID:
			default:

				/*
				 * if this is the first time through here, strExp == NULL
				 * so just duplicate the token string
				 */
				if (strExp == NULL)
				{
					strExp = strdup(strToken);
					if (strExp == NULL)
						return ERR_MALLOC;
				}
				else /* append token string to the expression found so far */
				{
					strTmp = (char*)realloc(strExp, strlen(strExp)
						+ strlen(strToken)
						+ 1);
					if (strTmp == NULL)
						return ERR_REALLOC;

					/* append the token string and assign it to strExp */
					strExp = strcat(strTmp, strToken);
				}

				break;

			} /* switch ( token ) */


			break;        /****************************
						 * END of look for expression
						 ****************************/

		}


		/******************
		 * BREAK jumps here
		 ******************/
		SAFE_FREE(strTOKEN);
		strToken = NULL;

	} while (token != tokEND);



	return CloseFile();

}  /* END - GetData() */

