#pragma once
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

// This is the foundation header for the entire project.
// Compiler Configuration macros, Build Configuration macros, and Core Types
// They're part of the language of Greenshift itself.

/****************************************************************************
 *
 * Project Greenshift, the common include file.
 *
 * Historically this header was intended to be included by every Greenshift
 * source file (at the top) and served as a central location for project-wide
 * types, macros, configuration, and utility definitions.
 *
 * While the implementation details have evolved considerably, the core idea
 * remains intact.
 *
 * Project Greenshift.h continues to serve as the foundation header for the
 * project. It defines compiler configuration, build configuration, project
 * types, and other assumptions that are intended to be consistently visible
 * throughout the codebase.
 *
 * Early in development I encountered a particularly difficult bug caused by
 * conditional compilation and inconsistent compile-time assumptions between
 * translation units.
 *
 * The experience strongly influenced the original architecture of Greenshift
 * and motivated centralizing common project definitions in this header.
 *
 * The story below explains how that bug influenced the architecture of
 * Greenshift and motivated centralizing common project definitions in this
 * header.
 *
 * Historical Note (circa 2000-2001)
 * ---------------------------------
 * _Everything_ in Project Greenshift must include this file in the first line.
 *
 * Yes, it seems redundant and unnecessary, but this is for a couple reasons.
 *
 * The first is to identify the file as part of Project Greenshift.
 * This is more important if the code is reused by someone else in some other
 * program, so it's easy to tell which files are mine and which are not.
 * (ADDENDUM: not strictly true anymore, copyright notices serve that function)
 *
 * The second reason is to ensure that no matter what, certain assumptions
 * always apply to any code in Project Greenshift.
 * Namely the error_t and value_t data types, and NULL.
 *
 * Third, if something needs to be accessed from anywhere, this is the place.
 *
 *
 *
 *
 * I ran into problems between the compile and link stage when using macros for conditional compilation
 * those macros should have been defined at the lowest level.
 *
 * When compiling, the included files saw the defined macros,
 * thus making code for everything it was supposed to.
 *
 * but the linker (only looking at the header files) could not see the
 * macros defined in the C++ file, thus the linker calculated offset values
 * for the member functions as if half the code hadn't been compiled.
 *
 * The result was that the wrong functions were being called.
 * Example:
 *        printf( "%s\n", expression->PrintString() );
 *        was dereferencing a "NULL" pointer, which was especially puzzling since I wrote
 *        PrintString in such a manner that it should NEVER return NULL.
 *        (ok, checking my code, this is not true, but it probably wouldn't ever return NULL =)
 *
 *        It sometimes returns a string constant stating the error ("ERROR: malloc")
 *
 *        What should be done is use out parameters, and return an error_t
 *        (PrintString isn't a core function, and is mainly intended for
 *        debugging purposes, as differentiation isn't implemented yet,
 *        so it's not a priority at the moment)
 *
 *        Using the debugger, I realized Evaluate() was actually called, NOT PrintString.
 *        Evaluate happened to return 0 (zero) for the given values of the expression.
 *        The PrintString protocol was imposed (by the linker) on a call to Evaluate,
 *        thus the zero value was interpreted as a NULL pointer.
 *
 */


/****************************************************************************
*
* conditional compile #defines
* Compiler Configuration macros, Build Configuration macros
*
****************************************************************************/
//#define USE_FAST_BLIT 1
//#define USE_FLIP 1

 /* this is a toggle so I can put an insane amount of debugging code
  * and not have to toggle each individually using comments  */
#define EXTREME_DEBUGGING 0

// Reduce the amout of logging during startup (don't log the contents of every config)
#define HIDE_INIT_TRACE 1

//#define USE_FASTER_LINE_DRAW
#define USE_TIMER_TO_HIDE_MOUSE

/*
 * the floating point value type
 *
 * Configuration: Set to 1 for double, 0 for float
 */
#define USE_HIGH_PRECISION_FLOAT 0


// 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: _strdup. See online help for details.
// Extremely unlikely to cause a problem. It's more portable to leave it alone,
// and long term I'd like to move to std::string anyway. Let's just disable this warning.
#define _CRT_NONSTDC_NO_DEPRECATE

// Suppress MSVC CRT deprecation warnings.
//
// Greenshift intentionally uses standard C/C++ runtime APIs such as fopen().
// These APIs are not deprecated by the C or C++ standards; the warning is
// specific to Microsoft's "secure CRT" extensions. Migration to *_s APIs
// is not required and would reduce portability.
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#define STRICT 1

#include <cstdint>


#include "MathUtils.hpp"
#include "MemoryUtils.hpp"

 /****************************************************************************
  *
  * Standard Macros and types valid anywhere in Project Greenshift
  *
  ****************************************************************************/
#define BITS_PER_BYTE    8

 // ---------------------------------------------------------------------------
 // Fundamental integer types
 // ---------------------------------------------------------------------------
//using byte_t = std::uint8_t;
//using word_t = std::uint16_t;
//using dword_t = std::uint32_t;
typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t dword_t;

// ---------------------------------------------------------------------------
// Project-wide types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Numeric precision configuration
// ---------------------------------------------------------------------------

 /****************************************************************************
  *
  * Standard types valid anywhere in Project Greenshift
  *
  ****************************************************************************/

  // ---------------------------------------------------------------------------
  // Pixel storage
  // ---------------------------------------------------------------------------

using pixelmap_t = std::uint32_t;
using ppixelmap_t = pixelmap_t*;

 /**
  * @brief A PIXELMAP represents a single pixel's memory offset within a linear buffer, acting as the numerical tool used to map one position to another.
  *
  * pixel map is a value that maps a pixel to another pixel
  * its value is the offset into the buffer (x + y * width)
  */
typedef unsigned long   PIXELMAP;
typedef unsigned long* PPIXELMAP;


#if USE_HIGH_PRECISION_FLOAT
typedef double value_t;
#define VALUE_T_SIZE 8
#else
typedef float value_t;
#define VALUE_T_SIZE 4
#endif

// ---------------------------------------------------------------------------
// Legacy string types
// ---------------------------------------------------------------------------

typedef const char mychar_t;
typedef const char char_t;
typedef mychar_t* string_t;
//typedef std::string string_t;
// Future possibility:
// using string_t = std::string;

/*
 * everything in Project Greenshift should use
 * value_t as the floating point type.
 * This permits global changes between the
 * double and float types.
 *
 * I did this because I wanted to see the
 * performance difference between
 * 32bit and 64bit numbers
 */
typedef value_t* pvalue_t;


/*
 * I know I shouldn't have these typedef's and define's, but I don't want to
 * have everything include windows.h just for these things
 */
//typedef unsigned char BYTE;
//typedef unsigned short WORD;
//typedef unsigned long DWORD;



  /* extern void * memcpy_amd(void *dest, const void *src, size_t n); */



 /****************************************************************************
  *
  * Error Codes
  *
  ****************************************************************************/
typedef enum {
	/* Error Name */        /* thing that returns the specific error */

	SUCCESS = 0,           /* anything - means no error */

	ERR_FAKE = 1,           /* placeholder to catch things not using an
							 * enumerated error code and doing something
							 * like "return 1"
							 */

	FAILURE,                /* anything - general failure only used until
							 * a custom error code and message are added
							 */

	ERR_NULL,               /* anything - NULL passed as a parameter.
							 * if NULL is a valid value for the function,
							 * you don't have to return this.
							 */

	ERR_MALLOC,             /* anything */
	ERR_REALLOC,            /* anything */
	ERR_BOUNDS,             /* anything - array access out of bounds */

	ERR_ALREADYDEPENDENT,   /* Model */

	ERR_NOTFOUND,           /* MyDictionary */
	ERR_WINDOWDEVICE,       /* WindowDevice */
	ERR_BITCANVAS,          /* BitCanvas */
	ERR_OVERLAY,            /* BitCanvas */
	ERR_THREADBUSY,         /* ThreadedEntity */
	ERR_PALETTE,            /* Palette */
	ERR_UNKNOWNPALETTETYPE, /* Palette */
	ERR_NOTDELTAFIELD,      /* DeltaField */
	//    ERR_SUBCLASS_RESPONSIBILITY,    /* Expression */
	ERR_SHOULD_NOT_IMPLEMENT,/* Expression */
	ERR_COMPILE,            /* Expression */
	//    ERR_UNARY_NEGATION,     /* Expression */
	//    ERR_OPERATION_NOT_DEFINED,    /* Expression */
	ERR_DIV_BY_ZERO,        /* Expression */
	ERR_CONSTANT,           /* Expression */
	ERR_SYMBOL,             /* Expression */
	ERR_VARIABLE,           /* Expression */
	ERR_USERDEFINED,        /* Expression */
	ERR_SQR,                /* Expression */
	ERR_SQRT,               /* Expression */
	ERR_EXP,                /* Expression */
	ERR_LN,                 /* Expression */
	ERR_LOG10,              /* Expression */
	ERR_COS,                /* Expression */
	ERR_SIN,                /* Expression */
	ERR_TAN,                /* Expression */
	ERR_ARCCOS,             /* Expression */
	ERR_ARCSIN,             /* Expression */
	ERR_ARCTAN,             /* Expression */
	ERR_COSH,               /* Expression */
	ERR_SINH,               /* Expression */
	ERR_TANH,               /* Expression */
	ERR_ARCCOSH,            /* Expression */
	ERR_ARCSINH,            /* Expression */
	ERR_ARCTANH,            /* Expression */
	ERR_ADD,                /* Expression */
	ERR_SUB,                /* Expression */
	ERR_MULT,               /* Expression */
	ERR_DIV,                /* Expression */
	ERR_MOD,                /* Expression */
	ERR_POW,                /* Expression */
	ERR_COMMA,              /* Expression */
	ERR_RELATIONAL,         /* Expression JRDV: Either extend or rewrite */
	ERR_OR,                 /* Expression */
	ERR_AND,                /* Expression */
	ERR_STAR,               /* Expression */
	ERR_DD_FAILURE,
	//    ERR_UNKNOWNOPCODE,      /* Expression - when using the VM approach */

	ERR_UNDEFINED,          /* the error code right after the last error,
							 * so can use a check like:
							 *
							 * if( err > SUCCESS && err < ERR_UNDEFINED )
							 *        return err;
							 * else
							 *        return ERR_INVALIDERRORCODE;
							 */

	ERR_INVALIDERRORCODE    /* same thing as undefined error -
							 * I just couldn't decide between the two
							 */
} error_t; // the error type (error codes are listed above)



//class ProjectGreenshift
//{
//public:
	/************************************************************************
	 *
	 * ErrorString - maps error code to a human readable message
	 *
	 ************************************************************************/
	 //    friend
#ifdef UNDEFINED
//#ifdef _DLL
inline char* ErrorString(error_t errCode)
{
	//        switch( errCode )
	//        {
	//        case SUCCESS:
	//            return "success";
	//        default:
	return "the descriptive error messages are removed from the DLL to save space";
	//      }
};
#else
inline const char* ErrorString(error_t errCode)
{
	switch (errCode)
	{
	case SUCCESS:
		return "success";
	case ERR_FAKE:
		return "ERROR: something is returning 1 instead of FAILURE";
	case FAILURE:
		return "failure";
	case ERR_NULL:
		return "silly programmer, NULL is for coredumps!";
	case ERR_MALLOC:
		return "malloc: error allocating memory";
	case ERR_REALLOC:
		return "realloc: error reallocating memory";
	case ERR_BOUNDS:
		return "array access out of bounds";
	case ERR_ALREADYDEPENDENT:
		return "already a dependent of the model";
	case ERR_NOTFOUND:
		return "var not found in dictionary";
	case ERR_WINDOWDEVICE:
		return "error creating the window device";
	case ERR_BITCANVAS:
		return "invalid bit depth";
	case ERR_OVERLAY:
		return "overlay error";
	case ERR_THREADBUSY:
		return "thread is busy... not accepting work at the moment";
	case ERR_PALETTE:
		return "error creating a palette";
	case ERR_UNKNOWNPALETTETYPE:
		return "unable to determine the type of palette";
	case ERR_NOTDELTAFIELD:
		return "config file is not a delta field";
		//        case ERR_SUBCLASS_RESPONSIBILITY:
		//            return "subclass responsibility";
	case ERR_SHOULD_NOT_IMPLEMENT:
		return "should not implement";
	case ERR_COMPILE:
		return "the entered expression could not be parsed, since it's not a valid expression";
		//        case ERR_UNARY_NEGATION:
		//            return "we are sorry, unary negation has not been implemented at this time";
		//        case ERR_OPERATION_NOT_DEFINED:
		//            return "matched operator has been declared in orderOfOperations, but the instance creation code has not been implemented yet";
	case ERR_DIV_BY_ZERO:
		return "division by zero";
	case ERR_CONSTANT:
		return "ExpressionConstant creation failed";
	case ERR_SYMBOL:
		return "ExpressionSymbol creation failed";
	case ERR_VARIABLE:
		return "ExpressionVariable creation failed";
	case ERR_USERDEFINED:
		return "ExpressionUserDefined creation failed";
	case ERR_SIN:
		return "ExpressionSin creation failed";
	case ERR_COS:
		return "ExpressionCos creation failed";
	case ERR_ARCSIN:
		return "ExpressionArcSin creation failed";
	case ERR_ADD:
		return "ExpressionAdd creation failed";
	case ERR_SUB:
		return "ExpressionSub creation failed";
	case ERR_MULT:
		return "ExpressionMult creation failed";
	case ERR_DIV:
		return "ExpressionDiv creation failed";
	case ERR_MOD:
		return "ExpressionMod creation failed";
	case ERR_POW:
		return "ExpressionPower creation failed";
	case ERR_COMMA:
		return "ExpressionComma creation failed";
	case ERR_OR:
		return "ExpressionOr creation failed";
	case ERR_AND:
		return "ExpressionAnd creation failed";
	case ERR_STAR:
		return "ExpressionStar creation failed";
	case ERR_DD_FAILURE:
		return "Direct Draw failure of some kind";
		//        case ERR_UNKNOWNOPCODE:
		//            return "unknown opcode";
	case ERR_UNDEFINED:
	case ERR_INVALIDERRORCODE:
	default:
#ifndef EXTENDED_DEFAULT_ERROR_MESSAGE
		return "<< invalid error code >>";
#else
		return "<< invalid error code >>"
			//                " : no message is defined for this error code.\n"
			"\n\n\tThis means one of two things:\n"
			" 1) no message is defined for this error code, or\n"
			" 2) something is returning an error code that does not exist.\n"
			//                " 2) the error message isn't defined in \"Project Greenshift.h\""
			;
#endif

	}
};

//};

#endif

#ifdef UNDEFINED_MACRO_SO_THE_FOLLOWING_TEXT_IS_HIDDEN_FROM_THE_COMPILER

to explain the below comment scheme, I like to be able to toggle between
two bits of code using comments... but the catch is I want to be able to
toggle by changing only one character.

The first /* denotes the beginning of a comment (obviously)
and the following /*/ is interpreted as the end of this comment.

This is because the first slash of /*/ is part of the comment,
thus ignored, and the */ part is seen as the end of the comment.

Since that's the end of the comment, the lines following that are
uncommented and the /**/ comment doesn't really do anything... yet...


NOW, placing a forward - slash in front of the opening "/*"
turns the multiline comment into a single line comment
//* this in turn stops commenting anything past that line,
and the previous end of comment /*/ is now interpreted as a begin comment.

Finally, the /**/ comment that previously didn't seem to do anything
now functions as the closing to the new comment.


There is another catch... this does not work if multiline comments are used,
so this scheme is mainly for use when debugging small sections of code.

For larger sections of code, #ifdef conditional compiles are much better

#endif /* UNDEFINED_MACRO_SO_THE_FOLLOWING_TEXT_IS_HIDDEN_FROM_THE_COMPILER */




#ifdef UNDEFINED_SO_YOU_CAN_SEE_THE_COMMENT_SCHEME_IN_SYNTAX_HIGHLIGHT_EDITOR

/*
this line is a comment
/*/
this line is not a comment
/**/


//*
this line is not a comment
/*/
this line is a comment
/**/

#endif /* UNDEFINED_SO_YOU_CAN_SEE_THE_COMMENT_SCHEME */

