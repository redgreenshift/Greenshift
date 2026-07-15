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

#ifndef _FilingClerk_H_
#define _FilingClerk_H_

#include "MyDictionary.h"
#include <malloc.h>
//#include <fstream.h>
#include <io.h>
#include <stdlib.h> /* for tolower and _MAX_PATH */
//#include <ctype.h>  /* for tolower */
#include <stdio.h>  /* for FILE access */

#define WINDOWS_PATH_DELIMITER  "\\"
#define UNIX_PATH_DELIMITER     "/"
#define MAC_PATH_DELIMITER      ":"

#ifdef _WIN32

#define PATH_DELIMITER  WINDOWS_PATH_DELIMITER

#else  /* ndef _WIN32 */

#ifndef _MAC

#define PATH_DELIMITER  UNIX_PATH_DELIMITER

#else  /* def _MAC */

#define PATH_DELIMITER  MAC_PATH_DELIMITER

#endif /* _MAC */

#endif /* _WIN32 */


/****************************************************************************
 *
 * FilingClerk
 *
 * a file can be thought of as a set of associations
 *
 * the filing clerk merely returns a MyDictionary of such a file
 *
 * Should I use a stream object rather than a FILE pointer?
 * I'd like to be able to use a stream object so the parser could use a
 * string stream, but think it's accessing a file
 *
 * Need to separate the File access stuff from the parsing stuff
 *
 * the Parser has a FilingClerk and a Tokenizer.
 *
 * Parser is told where to look, Parser asks FilingClerk for a stream.
 *
 * The stream location is translated to a folder, or file.
 * But could just as esily be a string stream.
 *
 * Only the Parser talks to the FilingClerk.
 *
 * the stream is given to Tokenizer who tokenizes the input.
 *
 * The stream is returned to FilingClerk when done?  so he can close it?
 *
 * Parser knows how to parse, and create Dictionaries.
 *
 * Protocol is an iostream (fstream, strstream, stdiostream)
 *
 * Each has a streambuf (filebuf, strstreambuf, stdiobuf)

  #include <iostream.h>

  ios::skipws
  ios::left
  ios::right (default)

  ios.eof() nonzero if eof is reached, same as setting eofbit error flag

  istream::read(char*, int nCount)  useful for binary stream input

  ifstream  myFile;

  myFile.open( "filename.ext", iosmode );

  int _fileno( FILE* );   gets a file handle


  fstream, strstream are iostreams
 ****************************************************************************/

typedef int token_t;


class FilingClerk
{
public:
                FilingClerk();
                ~FilingClerk();

    error_t     SetRoot( char *strFolder ); /* base directory */
    error_t     SetFolder( char *strFolder ); /* specific section */
    
    /*
     * load config files
     */
    error_t     LoadConfig( MyDictionary<char*> **ppConfig, DWORD *inoutNumAllocated );
    error_t     LoadColorMaps( MyDictionary<char*> **ppConfig, DWORD *inoutNumAllocated );
    error_t     GetData( char *id, MyDictionary<char*> *outMyDictionary, MyDictionary<MyDictionary<char*>*> *outExtra = NULL );
    error_t     GetColorMap( char *id, MyDictionary<char*> *outMyDictionary );

protected:
    char    *m_strRoot;      /* main folder currently working in */
    char    *m_strFolder;    /* subfolder currently working in */
    char    *m_strFile;      /* full path to file currently working with */
    FILE    *m_pFile;        /* file pointer */

    char    *m_pTokenBuffer; /* storage for recent matched token of input */
    int     m_nBufferLength; /* length of space allocated for token buffer */
    int     m_nUsedLength;   /* length of used space in token buffer */

    _finddata_t     m_wFindData;
    long            m_hFindHandle;

    /**** Tokenization ****/
    enum {    tokEND = EOF, /* single character tokens are their ASCII value */
            tokSTRING = 256, tokID, tokNUMBER, tokCOMMENT, tokHEADER};

    token_t     GetToken( void );//char **tokenString );
    int         GetChar( void );
    int         UnGetChar( int c );
    int         PeekChar( void );

    error_t     ClearBuffer( void );
    char        *TokenBuffer( void );
    /******/


    error_t     OpenFile( const char *strFileName, const char *strAccess = "rb" );
    int         CloseFile( void );


    bool        EndsIn(const char *inString, const char *inMatchString)
    {
        bool bMatched;
        int  iStr, iMat;

        /*
         * NULL check
         */
        if( inString == NULL || inMatchString == NULL )
            return false;

        /*
         * determine both string lengths
         */
        iStr = strlen( inString );
        iMat = strlen( inMatchString );

        /*
         * a atring cannot end in something larger than itself
         */
        if( iStr < iMat )
            return false;

        /*
         * since inString is longer (or equal) in length,
         * we shall loop over the last iMat chars in both strings
         */
        iStr = iStr - iMat;
        iMat = iMat - iMat;

        bMatched = true;

        while( bMatched && inString[iStr] != '\0' )
        {
            if( tolower(inString[iStr++]) == tolower(inMatchString[iMat++]) )
                bMatched = true;
            else
                bMatched = false;
        }

        return bMatched;
    };




    /*
     * File Searching
     */
    char        *GetFirstFile( const char *strFileMask );
    char        *GetNextFile( void );
    char        *EndSearch( void );
};


#endif    /* _FilingClerk_H_ */

