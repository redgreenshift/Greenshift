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
 * Expression - mathematical entity which can be manipulated symbolically
 *
 ****************************************************************************/

/*
 *    Written by Jared Ivey
 *
 */

#ifndef _Expression_H_
#define _Expression_H_

#include "MyDictionary.h"
//#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <malloc.h> /* for PrintString */
#include <stdio.h>  /* for sprintf() */
#include <ctype.h>

//#define REGULAR_EXPRESSION

/*
    - Class Hierarchy -

  - large inheritance hierarchy - :P

  Expression
  |
  +-ExpressionValue    (subclasses have equal precedence)
  | |
  | +-ExpressionConstant
  | |
  | +-ExpressionVariable
  |
  |
  +-ExpressionUnary    (subclasses have equal precedence)
  | |
  | +-ExpressionTrigonometric
  | | |
  | | +-ExpressionCos
  | | |
  | | +-ExpressionSin
  | | |
  | | +-ExpressionTan
  | | |
  | | +-ExpressionArcCos
  | | |
  | | +-ExpressionArcSin
  | | |
  | | +-ExpressionArcTan
  | |
  | +-ExpressionHyperbolic
  | | |
  | | +-ExpressionCosh
  | | |
  | | +-ExpressionSinh
  | | |
  | | +-ExpressionTanh
  | | |
  | | +-ExpressionArcCosh
  | | |
  | | +-ExpressionArcSinh
  | | |
  | | +-ExpressionArcTanh
  | |
  | +-ExpressionTrascendental
  |   |
  |   +-ExpressionExp
  |   |
  |   +-ExpressionLn
  |   |
  |   +-ExpressionLog
  |   |
  |   +-ExpressionSqr
  |   |
  |   +-ExpressionSqrt
  |
  +-ExpressionBinary
  | |
  | +-ExpressionExponential        (subclasses have equal precedence)
  | | |
  | | +-ExpressionPower
  | |
  | |
  | +-ExpressionMultiplicative    (subclasses have equal precedence)
  | | |
  | | +-ExpressionMult
  | | |
  | | +-ExpressionDiv
  | |
  | +-ExpressionAdditive        (subclasses have equal precedence)
  | | |
  | | +-ExpressionAdd
  | | |
  | | +-ExpressionSub
  | |
  | +-ExpressionList
  |   |
  |   +-ExpressionComma
  |
  +-ExpressionUserDefined
  |
  +-RegularExpression
    |
    +-RegularExpressionStar
    |
    +-RegularExpressionAnd
    |
    +-RegularExpressionOr

 */

/*

  Proper design using implementation inheritance requires a high level of
  expertise and discipline to prevent what is known as the fragile super
  class scenario. You should know whether a class will be extended by other
  sub classes. If you expect a class to be extended, it is as important to
  encapsulate implementation details from sub classes as it is to encapsulate
  them from clients.

  Object composition offers another way to achieve reuse without the tendency
  toward tight coupling. Object composition is based on black-box reuse, in
  which implementation details of a class are never revealed to the client.
  Clients know only about an available set of requests (the what). Objects
  never expose internal details of the response (the how).

  Black-box reuse is based on formal separation of interface and
  implementation. This means the interface becomes a first-class citizen. An
  interface is an independent data type that is defined on its own. This is an
  evolution of classic OOP, in which a public interface is defined within the
  context of a class definition.


  sinhx =  e^x - e^-x
        --------------
            2

 coshx =  e^x + e^-x
        ------------
            2

tanhx =    sinhx        e^x - e^-x 
        -----    =    ----------
        coshx        e^x + e^-x

 cothx = coshx        e^x + e^-x 
        ------    =    ----------
         sinhx        e^x - e^-x


sech   x =  1                2
            -----    =    ----------
            coshx        e^x + e^-x

 cschx =  1                2
        -----    =    ----------
        sinhx        e^x - e^-x

 */


enum
{
    ED_NULL = 0,        /* unused expression descriptions should be initialized to all zeros */
    ED_CONSTANT,        /* identifier is a named constant */
    ED_VARIABLE,        /* identifier is a variable */
//    ED_ARRAY,            /* array here, or as a separate field? */
                        /* though constant arrays do exist, it can be implemented as a variable array */
//    ED_ALIAS,            /* identifier that can be used in place of another */
    ED_OPERATOR,        /* function with zero or more arguments */
                        /* arguments are represented as expressions
                         * separated by the operator symbols
                         *
                         * operator symbols are stored as a doubly null terminated string
                         */
    ED_FUNCTION,        /* function with zero or more arguments */
                        /* arguments are represented as a parenthesis enclosed
                         * list of comma separated expressions.
                         */
    ED_FUNCTIONEX        /* function with an extra pointer
                         */

    /*
     * decided adding a new enum is a bad way to handle future expansion
     * an alternative is to have an additional field in EXPRESSIONDESCRIPTION
     * to store the number of parameters/arguments/operands
     */
//    ED_PARAMETERLESS,    /* ie:  function with zero arguments */
//    ED_UNARY,            /* function that takes one argument */
//    ED_BINARY,            /* function that takes two arguments */
//    ED_TERNARY,            /* function that takes two arguments */
//    ED_QUATERNARY        /* function that takes two arguments */
};
typedef int expression_desc_t;

typedef value_t(*pNoaryFunction_t)(void);
typedef value_t(*pNoaryFunctionEx_t)(void*);
typedef value_t(*pUnaryFunction_t)(value_t);
typedef value_t(*pUnaryFunctionEx_t)(value_t, void*);
typedef value_t(*pBinaryFunction_t)(value_t, value_t);
typedef value_t(*pBinaryFunctionEx_t)(value_t, value_t, void*);


typedef struct tagEXPRESSIONDESCRIPTION
{
    expression_desc_t       edtType;    /* identifies the type of expression */
    char *                  strName;    /* the name of the expression */
//    union {
        value_t             value;
//        DWORD               dwValue;
//    };

    union {
        void                *generic_data;
        value_t             *variable;
        pNoaryFunction_t    noaryFunc;
        pNoaryFunctionEx_t  noaryFuncEx;
        pUnaryFunction_t    unaryFunc;
        pUnaryFunctionEx_t  unaryFuncEx;
        pBinaryFunction_t   binaryFunc;
        pBinaryFunctionEx_t binaryFuncEx;
    } pointer;

    void *pExtraData;

} EXPRESSIONDESCRIPTION;



/****************************************************************************
 ****************************************************************************
 *
 * The Expression class - the base of all Expressions
 *
 * defines a mathematical expression which can be manipulated symbolically
 *
 ****************************************************************************
 ****************************************************************************/
class Expression
{
public:
            Expression();    /* constructor */
    virtual ~Expression();

    /* Unary */
    error_t Sqrt    (                           Expression **outExpression );
    error_t Sqr     (                           Expression **outExpression );
    error_t Log10   (                           Expression **outExpression );
    error_t Ln      (                           Expression **outExpression );
    error_t Exp     (                           Expression **outExpression );

    /* Trigonometric */
    error_t Cos     (                           Expression **outExpression );
    error_t Sin     (                           Expression **outExpression );
    error_t Tan     (                           Expression **outExpression );
    error_t ArcCos  (                           Expression **outExpression );
    error_t ArcSin  (                           Expression **outExpression );
    error_t ArcTan  (                           Expression **outExpression );

    /* Hyperbolic Trig */
    error_t Cosh    (                           Expression **outExpression );
    error_t Sinh    (                           Expression **outExpression );
    error_t Tanh    (                           Expression **outExpression );
    error_t ArcCosh (                           Expression **outExpression );
    error_t ArcSinh (                           Expression **outExpression );
    error_t ArcTanh (                           Expression **outExpression );

    /* Binary */
    error_t Add     ( Expression *inExpression, Expression **outExpression );
    error_t Sub     ( Expression *inExpression, Expression **outExpression );
    error_t Mult    ( Expression *inExpression, Expression **outExpression );
    error_t Div     ( Expression *inExpression, Expression **outExpression );
    error_t Mod     ( Expression *inExpression, Expression **outExpression );
    error_t Power   ( Expression *inExpression, Expression **outExpression );

    /* Relational */
    error_t  Equality           (Expression *op2, Expression**);
    error_t  NonEquality        (Expression *op2, Expression**);
    error_t  GreaterThan        (Expression *op2, Expression**);
    error_t  GreaterThanOrEqual (Expression *op2, Expression**);
    error_t  LessThan           (Expression *op2, Expression**);
    error_t  LessThanOrEqual    (Expression *op2, Expression**);

    /* Ternary Operations */
    error_t  Conditional(Expression* op2, Expression* op3, Expression**outExpression);

#ifdef REGULAR_EXPRESSION
    error_t Star    (                           Expression **outExpression );
    error_t Or      ( Expression *inExpression, Expression **outExpression );
    error_t And     ( Expression *inExpression, Expression **outExpression );
#endif

/*    error_t    Evaluate(value_t *outValue);    /* outValue is set to the
                                             * evaluated value of the
                                             * constant expression
                                             */
    char    *PrintString(void);    /* builds a string representation
                                         * of the expression it defines
                                         */




    /* why am I declaring these virtual again?
     * is it just to ensure they get redefined in subclasses?
     * NO!  it's so function calls in Expression
     * will reference the redefined functions
     */



    /***********************************************
     * Should be overridden by the Abstract classes
     * and Value classes
     ***********************************************/
    virtual char *  PrintString( char * inStr, int &nLength ) = 0;    /* builds a string representation
                                                         * of the expression it defines
                                                         */


    virtual bool    IsConstantExpression(void) = 0;
    virtual bool    IsConstantExpression(value_t *inValue) = 0;


    virtual error_t Copy(Expression **outExpression) = 0;       /* creates a copy of the expression tree
                                                                 */



    virtual int     Size( void ) = 0;   /* answers the number of
                                         * nodes in the tree
                                         */
    virtual int     Depth( void ) = 0;  /* answers the length of
                                         * the longest branch
                                         */



    /************************************************
     * Should be overridden by the Concrete classes
     ************************************************/
#ifdef REGULAR_EXPRESSION
    virtual bool    Match( char *inString, int *outMatchedLength ); /* just the RegularExpression classes */
#endif

    // JRDV(REVIEW): return should be const
    virtual char    *Operator(void) = 0;    /* returns the string
                                             * representation for the operation
                                             * it performs, NULL otherwise
                                             */

    virtual value_t Evaluate(void) = 0;        /* evaluate the expression
                                             * at all the variables
                                             * current values
                                             */

    virtual error_t PartialSimplification(
                        value_t *inValue,
                        Expression **outExpression) = 0;
    









protected:

    /************************************************************************
     *
     * Instance Creation
     *
     * No one outside the expression class has any buisness using these
     *
     ************************************************************************/
        /********************************************************************
         * all of the "NewOperation" functions must have the same prototype
         * so that pointers to functions can be used
         ********************************************************************/
    /* Scalar */
    static error_t  NewConstant (const value_t,              Expression **);
    static error_t  NewUserDefined(EXPRESSIONDESCRIPTION*,   Expression*, Expression **);
    static error_t  NewVariable (char *,       value_t *,    Expression **);

    /* Unary */
    static error_t  NewSqrt     (Expression *, Expression *, Expression **);
    static error_t  NewSqr      (Expression *, Expression *, Expression **);
    static error_t  NewLog10    (Expression *, Expression *, Expression **);
    static error_t  NewLn       (Expression *, Expression *, Expression **);
    static error_t  NewExp      (Expression *, Expression *, Expression **);

    /* Trigonometric */
    static error_t  NewCos      (Expression *, Expression *, Expression **);
    static error_t  NewSin      (Expression *, Expression *, Expression **);
    static error_t  NewTan      (Expression *, Expression *, Expression **);
    static error_t  NewArcCos   (Expression *, Expression *, Expression **);
    static error_t  NewArcSin   (Expression *, Expression *, Expression **);
    static error_t  NewArcTan   (Expression *, Expression *, Expression **);

    /* Hyperbolic Trig */
    static error_t  NewCosh     (Expression *, Expression *, Expression **);
    static error_t  NewSinh     (Expression *, Expression *, Expression **);
    static error_t  NewTanh     (Expression *, Expression *, Expression **);
    static error_t  NewArcCosh  (Expression *, Expression *, Expression **);
    static error_t  NewArcSinh  (Expression *, Expression *, Expression **);
    static error_t  NewArcTanh  (Expression *, Expression *, Expression **);

    /* Binary */
    static error_t  NewAdd      (Expression *, Expression *, Expression **);
    static error_t  NewSub      (Expression *, Expression *, Expression **);
    static error_t  NewMult     (Expression *, Expression *, Expression **);
    static error_t  NewDiv      (Expression *, Expression *, Expression **);
    static error_t  NewMod      (Expression *, Expression *, Expression **);
    static error_t  NewPower    (Expression *, Expression *, Expression **);

    /* Relational */
    static error_t  NewEquality(Expression*, Expression*, Expression**);
    static error_t  NewNonEquality(Expression*, Expression*, Expression**);
    static error_t  NewGreaterThan(Expression*, Expression*, Expression**);
    static error_t  NewGreaterThanOrEqual(Expression*, Expression*, Expression**);
    static error_t  NewLessThan(Expression*, Expression*, Expression**);
    static error_t  NewLessThanOrEqual(Expression*, Expression*, Expression**);

    /* Ternary */
    static error_t  NewConditional(Expression*, Expression*, Expression*, Expression**);

#ifdef REGULAR_EXPRESSION
    static error_t  NewSymbol   (const long,                 Expression **);
    static error_t  NewOr       (Expression *, Expression *, Expression **);
    static error_t  NewAnd      (Expression *, Expression *, Expression **);
    static error_t  NewStar     (Expression *, Expression *, Expression **);
#endif
    static error_t  NewComma    (Expression *, Expression *, Expression **);



    /************************************************************************
     *
     * Expression Compiler Stuff
     *
     ************************************************************************/
public:
    /*
     * Both Compile and Parse should be thread safe
     */
    /*
     * return NaN on error.  This function is used to replace the atol( dict.get()) stuff
     */
    static value_t  Evaluate(const char *inString,
                            value_t nErrorValue,
                            MyDictionary<value_t*> *inValues,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals );
    static error_t  Compile(const char *inString,
                            Expression **outExpression,
                            MyDictionary<value_t*> *inValues,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals );

private:
    typedef error_t (*creation_function_t)(    Expression *,
                                            Expression *,
                                            Expression **);
    typedef struct opStruct {
        char                *opString;
        creation_function_t    newExpression;
        int                    opType;
    } parsingLogic_t;


    static parsingLogic_t *ParsingLogic(void);

        /*
         * Returns true if inString matches a number [-+]{0,1}([1-9][0-9]*([.][0-9]+){0,1})
         */
    static bool     MatchNumber       (char *inString, int inLength);
    static bool     MatchIdentifier   (char *inString, int inLength);
    static bool     MatchOperator     (char *inString, char *inOpString, int inLength);
    static bool     AtBeginningOfToken(char *inString, char *position);


    static error_t  Compile(char *inString,
                            const int inLength,
                            Expression **outExpression,
                            MyDictionary<value_t*> *inValues,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals );

};    /* class Expression */






/****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 *
 *  Abstract Classes
 *
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************/


/****************************************************************************
 * I am the superclass to all sigular value expressions
 *
 * typically these only include constants and variables,
 * but I am here to facilitate further expansion if
 * expressions ever need to handle a new datatype.
 ****************************************************************************/
class ExpressionValue : public Expression
{
public:
                ExpressionValue(); /* default constructor */
    virtual     ~ExpressionValue();

    char        *Operator( void );
    int         Size( void );
    int         Depth( void );

};


/****************************************************************************
 * I am the superclass to all operations involving only one operand
 *
 * my children may access their operand through the function UnaryTerm()
 ****************************************************************************/
class ExpressionUnary : public Expression
{
private:
    Expression *    m_pUnaryTerm;
protected:
    inline Expression *    UnaryTerm(void)  { return m_pUnaryTerm; };
public:
                 ExpressionUnary(Expression *inExpression1);
    virtual     ~ExpressionUnary();

    char        *PrintString( char * inStr, int &nLength );
    bool        IsConstantExpression( void );
    bool        IsConstantExpression( value_t *inValue );
    int         Size( void );
    int         Depth( void );
};

/* Do I really want to add the extra level? */
#ifdef UNDEFINED
class ExpressionTrigonometric : public ExpressionUnary
{
public:
                 ExpressionTrigonometric() {};
    virtual     ~ExpressionTrigonometric() {};

};

class ExpressionHyperbolic : public ExpressionUnary
{
public:
                 ExpressionHyperbolic() {};
    virtual     ~ExpressionHyperbolic() {};

};

#endif

/****************************************************************************
 * I am the superclass to all operations involving two operands
 *
 * my children may access their operands through FirstTerm() and SecondTerm()
 ****************************************************************************/
class ExpressionBinary : public Expression
{
private:
    Expression *    m_pBinaryTerm1;
    Expression *    m_pBinaryTerm2;
protected:
    inline Expression *    FirstTerm( void )    { return m_pBinaryTerm1; };
    inline Expression *    SecondTerm( void )   { return m_pBinaryTerm2; };
public:
            ExpressionBinary(   Expression *inExpression1,
                                Expression *inExpression2 );
    virtual ~ExpressionBinary();

    char    *PrintString( char * inStr, int &nLength );
    bool    IsConstantExpression( void );
    bool    IsConstantExpression( value_t *inValue );
    int     Size( void );
    int     Depth( void );
};


class ExpressionAdditive : public ExpressionBinary
{
public:
            ExpressionAdditive( Expression *inExpression1,
                                Expression *inExpression2 );
    virtual ~ExpressionAdditive();
};

class ExpressionMultiplicative : public ExpressionBinary
{
public:
            ExpressionMultiplicative(   Expression *inExpression1,
                                        Expression *inExpression2 );
    virtual ~ExpressionMultiplicative();
};

class ExpressionExponential : public ExpressionBinary
{
public:
            ExpressionExponential(  Expression *inExpression1,
                                    Expression *inExpression2 );
    virtual ~ExpressionExponential();
};

class ExpressionRelational : public ExpressionBinary
{
public:
            ExpressionRelational(  Expression *inExpression1,
                                    Expression *inExpression2 );
    virtual ~ExpressionRelational();
};

class ExpressionList : public ExpressionBinary
{
public:
            ExpressionList( Expression *inExpression1,
                            Expression *inExpression2 );
    virtual ~ExpressionList();
};







/****************************************************************************
 * I am the superclass to all operations involving three operands
 *
 * my children may access their operands
 * through FirstTerm() SecondTerm() and ThirdTerm()
 ****************************************************************************/
class ExpressionTernary : public Expression
{
private:
    Expression *    m_pTernaryTerm1;
    Expression *    m_pTernaryTerm2;
    Expression *    m_pTernaryTerm3;
protected:
    inline Expression *    FirstTerm( void )    { return m_pTernaryTerm1; };
    inline Expression *    SecondTerm( void )   { return m_pTernaryTerm2; };
    inline Expression *    ThirdTerm( void )    { return m_pTernaryTerm3; };
public:
                ExpressionTernary(  Expression *inExpression1,
                                    Expression *inExpression2,
                                    Expression *inExpression3);
    virtual     ~ExpressionTernary();

    char        *PrintString( char * inStr, int &nLength );
    bool        IsConstantExpression( void );
    bool        IsConstantExpression( value_t *inValue );
    int         Size( void );
    int         Depth( void );
};




/****************************************************************************
 * I am the superclass to all operations involving an arbitrary number of operands
 *
 * my children may access their operands through Term(index)
 ****************************************************************************/
class ExpressionNAry : public Expression
{
private:
    Expression *    m_pTernaryTerm1;
    Expression *    m_pTernaryTerm2;
    Expression *    m_pTernaryTerm3;
protected:
    inline Expression *    FirstTerm( void )    { return m_pTernaryTerm1; };
    inline Expression *    SecondTerm( void )   { return m_pTernaryTerm2; };
    inline Expression *    ThirdTerm( void )    { return m_pTernaryTerm3; };
public:
            ExpressionNAry( Expression *inExpression1,
                            Expression *inExpression2 );
    virtual ~ExpressionNAry();

    char    *PrintString( char * inStr, int &nLength );
    bool    IsConstantExpression( void );
    bool    IsConstantExpression( value_t *inValue );
    int     Size( void );
    int     Depth( void );
};






/*
 * These are what all expressions must implement
 */


/****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 *
 *  Concrete Classes
 *
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************/

class ExpressionConstant : public ExpressionValue
{
private:
    const value_t    m_nConstantValue;
protected:    /* returns the constant value */
    inline const value_t    ConstantValue( void ) { return m_nConstantValue; };
public:
                 ExpressionConstant(const value_t inValue);
    virtual     ~ExpressionConstant();

    char        *PrintString( char * inStr, int &nLength );
    bool        IsConstantExpression(void);
    bool        IsConstantExpression(value_t *inValue);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};



class ExpressionVariable : public ExpressionValue
{
private:
    value_t *m_pVariableValue;
    char    *m_strVariableName;
protected:
    inline value_t  *PVariableValue( void ) { return m_pVariableValue; };
    inline char     *VariableName( void )   { return m_strVariableName; };
public:
                 ExpressionVariable(char *inName, value_t *inValue);
    virtual     ~ExpressionVariable();

    char        *PrintString( char * inStr, int &nLength );
    bool        IsConstantExpression(void);
    bool        IsConstantExpression(value_t *inValue);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};


class ExpressionUserDefined : public ExpressionUnary
{
protected:
    char                    *m_strName;
    EXPRESSIONDESCRIPTION   m_edExpressionDescription;
public:
                ExpressionUserDefined( EXPRESSIONDESCRIPTION *pExpDescription,
                                       Expression *inExpression1 );
    virtual     ~ExpressionUserDefined();

    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};


/****************************************************************************
 *
 * Unary
 *
 ****************************************************************************/

class ExpressionSqrt : public ExpressionUnary
{
public:
    ExpressionSqrt( Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionSqr : public ExpressionUnary
{
public:
    ExpressionSqr(  Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionLog10 : public ExpressionUnary
{
public:
    ExpressionLog10(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};


class ExpressionLn : public ExpressionUnary
{
public:
    ExpressionLn(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionExp : public ExpressionUnary
{
public:
    ExpressionExp(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};


/****************************************************************************
 *
 * Trigonometric
 *
 ****************************************************************************/

class ExpressionCos : public ExpressionUnary
{
public:
    ExpressionCos(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionSin : public ExpressionUnary
{
public:
    ExpressionSin(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionTan : public ExpressionUnary
{
public:
    ExpressionTan(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionArcCos : public ExpressionUnary
{
public:
    ExpressionArcCos(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionArcSin : public ExpressionUnary
{
public:
    ExpressionArcSin(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionArcTan : public ExpressionUnary
{
public:
    ExpressionArcTan(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};


/****************************************************************************
 *
 * Hyperbolic
 *
 ****************************************************************************/
class ExpressionCosh : public ExpressionUnary
{
public:
    ExpressionCosh(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionSinh : public ExpressionUnary
{
public:
    ExpressionSinh(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionTanh : public ExpressionUnary
{
public:
    ExpressionTanh(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionArcCosh : public ExpressionUnary
{
public:
    ExpressionArcCosh(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionArcSinh : public ExpressionUnary
{
public:
    ExpressionArcSinh(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionArcTanh : public ExpressionUnary
{
public:
    ExpressionArcTanh(Expression *inExpression1)
        : ExpressionUnary(inExpression1) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};




/****************************************************************************
 *
 * Binary
 *
 ****************************************************************************/
class ExpressionAdd : public ExpressionAdditive
{
public:
    ExpressionAdd (Expression *inExpression1, Expression *inExpression2)
        : ExpressionAdditive (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionSub : public ExpressionAdditive
{
public:
    ExpressionSub (Expression *inExpression1, Expression *inExpression2)
        : ExpressionAdditive (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionMult : public ExpressionMultiplicative
{
public:
    ExpressionMult (Expression *inExpression1, Expression *inExpression2)
        : ExpressionMultiplicative (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionDiv : public ExpressionMultiplicative
{
public:
    ExpressionDiv (Expression *inExpression1, Expression *inExpression2)
        : ExpressionMultiplicative (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionMod : public ExpressionMultiplicative
{
public:
    ExpressionMod (Expression *inExpression1, Expression *inExpression2)
        : ExpressionMultiplicative (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionPower : public ExpressionExponential
{
public:
    ExpressionPower (Expression *inExpression1, Expression *inExpression2)
        : ExpressionExponential (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};





class ExpressionComma : public ExpressionList
{
public:
    ExpressionComma (Expression *inExpression1, Expression *inExpression2)
        : ExpressionList (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};





class ExpressionEquality : public ExpressionRelational
{
public:
    ExpressionEquality (Expression *inExpression1, Expression *inExpression2)
        : ExpressionRelational (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionNonEquality : public ExpressionRelational
{
public:
    ExpressionNonEquality (Expression *inExpression1, Expression *inExpression2)
        : ExpressionRelational (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};


class ExpressionGreaterThan : public ExpressionRelational
{
public:
    ExpressionGreaterThan (Expression *inExpression1, Expression *inExpression2)
        : ExpressionRelational (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};


class ExpressionGreaterThanOrEqual : public ExpressionRelational
{
public:
    ExpressionGreaterThanOrEqual (Expression *inExpression1, Expression *inExpression2)
        : ExpressionRelational (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};

class ExpressionLessThan : public ExpressionRelational
{
public:
    ExpressionLessThan (Expression *inExpression1, Expression *inExpression2)
        : ExpressionRelational (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};


class ExpressionLessThanOrEqual : public ExpressionRelational
{
public:
    ExpressionLessThanOrEqual (Expression *inExpression1, Expression *inExpression2)
        : ExpressionRelational (inExpression1, inExpression2) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};




/****************************************************************************
 *
 * ExpressionConditional
 *
 ****************************************************************************/
class ExpressionConditional : public ExpressionTernary
{
private:
    Expression      *m_pCondition; /* WHAT is this for again? */
public:
    ExpressionConditional (Expression *inExpression1, Expression *inExpression2, Expression *inExpression3)
        : ExpressionTernary (inExpression1, inExpression2, inExpression3) {};
    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);
};




/****************************************************************************
 *
 * RegularExpression
 *
 ****************************************************************************/



/*

class RegularExpression //: private Expression
{
public:
    RegularExpression() {};
    ~RegularExpression() {};
//    virtual char*    PrintString( char*inString, int &nLength) = 0;
    virtual bool    Match( char *inString, int *outMatchedLength ) = 0;
};
*/

#ifdef REGULAR_EXPRESSION

class ExpressionSymbol : public ExpressionValue//, public RegularExpression
{
private:
    const long    m_nSymbolValue;
protected:
    inline long    SymbolValue( void ) { return m_nSymbolValue; };
public:
                 ExpressionSymbol(const long inValue);
    virtual     ~ExpressionSymbol();

    char        *PrintString( char * inStr, int &nLength );
    bool        IsConstantExpression(void);
    bool        IsConstantExpression(value_t *inValue);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);

    bool    Match( char *inString, int *outMatchedLength )
    {
        if( *inString == SymbolValue() )
        {
            *outMatchedLength = 1; /* this is the only place a non zero value is returned! */
            return true;
        }
        else
        {
            *outMatchedLength = 0; /* do I need this? */
            return false;
        }
    };
};


class ExpressionStar : public ExpressionUnary//, public RegularExpression
{
public:
                 ExpressionStar(Expression *inExpression1);
    virtual     ~ExpressionStar();

    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(value_t *inValue, Expression **outExpression);



    bool        Match( char *inString, int *outMatchedLength )
    {
        int matchedLength;
        int termLength;

        /*
         * ICK!  I don't like this code!
         * ok, better now that "RegularExpression" is gone
         */
        matchedLength = 0;

        while( UnaryTerm()->Match( inString+matchedLength, &termLength ) )
        {
            matchedLength += termLength;
        }

        *outMatchedLength = matchedLength;
        return true;
    };

};


class ExpressionOr : public ExpressionBinary//, public RegularExpression
{
public:
                 ExpressionOr ( Expression *inExpression1,
                                Expression *inExpression2);
    virtual     ~ExpressionOr();

    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);



    bool        Match( char *inString, int *outMatchedLength )
    {
        int term1Length;
        int term2Length;

        /*
         * actually, this should *not* use the logical or ||
         * it should use the bitwise or | and use the one that
         * matched the longest string
         */
        if( FirstTerm()->Match(inString, &term1Length) |
            SecondTerm()->Match(inString, &term2Length) )
        {
            *outMatchedLength = max(term1Length,term2Length);
            return true;
        }
        else
        {
            *outMatchedLength = 0;
            return false;
        }

    };
};

class ExpressionAnd : public ExpressionBinary//, public RegularExpression
{
public:
                 ExpressionAnd (Expression *inExpression1,
                                Expression *inExpression2);
    virtual     ~ExpressionAnd();

    char        *Operator(void);
    value_t     Evaluate(void);
    error_t     Copy(Expression **outExpression);
    error_t     PartialSimplification(  value_t *inValue,
                                        Expression **outExpression);



    bool        Match( char *inString, int *outMatchedLength )
    {
        int term1Length;
        int term2Length;

        if( FirstTerm()->Match(inString, &term1Length) &&
            SecondTerm()->Match(inString+term1Length, &term2Length) )
        {
            *outMatchedLength = term1Length + term2Length;
            return true;
        }
        else
        {
            *outMatchedLength = 0;
            return false;
        }

    };
};
#endif



#endif /* _Expression_H_ */

