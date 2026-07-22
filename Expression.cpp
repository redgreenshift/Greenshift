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
  * Expression.cpp: implementation of the Expression class.
  *
  ****************************************************************************/

#include "Expression.h"

// TODO: JRDV: Consider updating to use std::wformat / std::wformat_string whenever updating to C++20
//#include <format> // The project currently defaults to C++14, and this header is only available in C++20
//std::wstring utf8_to_wstring(const std::string& s);
#include <string>
#include <codecvt>
std::wstring utf8_to_wstring(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(s);
}

#ifdef REGULAR_EXPRESSION
  /****************************************************************************
   *
   * Match - default, ignore it.
   *
   ****************************************************************************/
bool    Expression::Match(char* inString, int* outMatchedLength)
{
	//    *outMatchedLength = 0;
	return false;
}
#endif

/****************************************************************************
 ****************************************************************************
 *
 * Constructor / Destructors
 *
 ****************************************************************************
 ****************************************************************************/

 /*
  * default constructor
  */
Expression::Expression()
{
}
/*
 * default destructor
 */
Expression::~Expression()
{
}

/****************************************************************************
 *
 * ExpressionValue
 *
 ****************************************************************************/
ExpressionValue::ExpressionValue()
{
}
ExpressionValue::~ExpressionValue()
{
}


/****************************************************************************
 *
 * ExpressionConstant
 *
 ****************************************************************************/
ExpressionConstant::ExpressionConstant(const value_t inValue)
	: m_nConstantValue(inValue)
{
}
ExpressionConstant::~ExpressionConstant()
{
}


/****************************************************************************
 *
 * ExpressionUserDefined
 *
 ****************************************************************************/
ExpressionUserDefined::ExpressionUserDefined(EXPRESSIONDESCRIPTION* pExpDesc,
	Expression* inExpression1)
	: ExpressionUnary(inExpression1),
	m_edExpressionDescription(*pExpDesc)  /* SHOULD ACTUALLY COPY THE MEMBERS */
{
}
ExpressionUserDefined::~ExpressionUserDefined()
{
}


/****************************************************************************
 *
 * ExpressionVariable
 *
 ****************************************************************************/
ExpressionVariable::ExpressionVariable(char* inName, value_t* inValue)
	: m_strVariableName(inName),
	m_pVariableValue(inValue)
{
}
ExpressionVariable::~ExpressionVariable()
{
	free(VariableName());
}


/****************************************************************************
 *
 * ExpressionUnary
 *
 ****************************************************************************/
ExpressionUnary::ExpressionUnary(Expression* inExpression)
	: m_pUnaryTerm(inExpression)
{
}
ExpressionUnary::~ExpressionUnary()
{
	delete UnaryTerm();
}


/****************************************************************************
 *
 * ExpressionBinary
 *
 ****************************************************************************/
ExpressionBinary::ExpressionBinary(Expression* inExpression1,
	Expression* inExpression2)
	: m_pBinaryTerm1(inExpression1),
	m_pBinaryTerm2(inExpression2)
{
}

ExpressionBinary::~ExpressionBinary()
{
	delete SecondTerm();
	delete FirstTerm();
}


/****************************************************************************
 *
 * ExpressionTernary
 *
 ****************************************************************************/
ExpressionTernary::ExpressionTernary(Expression* inExpression1,
	Expression* inExpression2,
	Expression* inExpression3)
	: m_pTernaryTerm1(inExpression1),
	m_pTernaryTerm2(inExpression2),
	m_pTernaryTerm3(inExpression3)
{
}

ExpressionTernary::~ExpressionTernary()
{
	delete ThirdTerm();
	delete SecondTerm();
	delete FirstTerm();
}




/****************************************************************************
 *
 * ExpressionAdditive
 *
 ****************************************************************************/
ExpressionAdditive::ExpressionAdditive(Expression* inExpression1,
	Expression* inExpression2)
	: ExpressionBinary(inExpression1, inExpression2)
{
}
ExpressionAdditive::~ExpressionAdditive()
{
}


/****************************************************************************
 *
 * ExpressionMultiplicative
 *
 ****************************************************************************/
ExpressionMultiplicative::ExpressionMultiplicative(
	Expression* inExpression1,
	Expression* inExpression2)
	: ExpressionBinary(inExpression1, inExpression2)
{
}
ExpressionMultiplicative::~ExpressionMultiplicative()
{
}

/****************************************************************************
 *
 * ExpressionExponential
 *
 ****************************************************************************/
ExpressionExponential::ExpressionExponential(Expression* inExpression1,
	Expression* inExpression2)
	: ExpressionBinary(inExpression1, inExpression2)
{
}
ExpressionExponential::~ExpressionExponential()
{
}


/****************************************************************************
 *
 * ExpressionRelational
 *
 ****************************************************************************/
ExpressionRelational::ExpressionRelational(Expression* inExpression1,
	Expression* inExpression2)
	: ExpressionBinary(inExpression1, inExpression2)
{
}
ExpressionRelational::~ExpressionRelational()
{
}

/****************************************************************************
 *
 * ExpressionList
 *
 ****************************************************************************/
ExpressionList::ExpressionList(Expression* inExpression1,
	Expression* inExpression2)
	: ExpressionBinary(inExpression1, inExpression2)
{
}
ExpressionList::~ExpressionList()
{
}



#ifdef REGULAR_EXPRESSION
/****************************************************************************
 *
 * RegularExpression
 *
 ****************************************************************************/


 /****************************************************************************
  *
  * ExpressionSymbol
  *
  ****************************************************************************/
ExpressionSymbol::ExpressionSymbol(const long inValue)
	: m_nSymbolValue(inValue)
{
}
ExpressionSymbol::~ExpressionSymbol()
{
}


/****************************************************************************
 *
 * ExpressionStar
 *
 ****************************************************************************/
ExpressionStar::ExpressionStar(Expression* inExpression)
	: ExpressionUnary(inExpression)
{
}
ExpressionStar::~ExpressionStar()
{
}



/****************************************************************************
 *
 * ExpressionOr
 *
 ****************************************************************************/
ExpressionOr::ExpressionOr(Expression* inExpression1,
	Expression* inExpression2)
	: ExpressionBinary(inExpression1, inExpression2)
{
}
ExpressionOr::~ExpressionOr()
{
}


/****************************************************************************
 *
 * ExpressionAnd
 *
 ****************************************************************************/
ExpressionAnd::ExpressionAnd(Expression* inExpression1,
	Expression* inExpression2)
	: ExpressionBinary(inExpression1, inExpression2)
{
}
ExpressionAnd::~ExpressionAnd()
{
}
#endif

/*****************************************************************************
 *****************************************************************************
 *
 * Accessors... there are NO Modifiers!
 *
 *****************************************************************************
 *****************************************************************************/


bool    ExpressionConstant::IsConstantExpression(void) { return true; }
bool    ExpressionVariable::IsConstantExpression(void) { return false; }
bool    ExpressionUnary::IsConstantExpression(void) {
	return    UnaryTerm()->IsConstantExpression();
}
bool    ExpressionBinary::IsConstantExpression(void) {
	return    FirstTerm()->IsConstantExpression() &&
		SecondTerm()->IsConstantExpression();
}
bool    ExpressionTernary::IsConstantExpression(void) {
	return    FirstTerm()->IsConstantExpression() &&
		SecondTerm()->IsConstantExpression() &&
		ThirdTerm()->IsConstantExpression();
}

#ifdef REGULAR_EXPRESSION
bool    ExpressionSymbol::IsConstantExpression(void) { return true; }
bool    ExpressionSymbol::IsConstantExpression(value_t* inValue)
{
	return true;
};

#endif


bool    ExpressionConstant::IsConstantExpression(value_t* inValue)
{
	return true;
};

bool    ExpressionVariable::IsConstantExpression(value_t* inValue)
{
	return (inValue == PVariableValue());
}

bool    ExpressionUnary::IsConstantExpression(value_t* inValue)
{
	return UnaryTerm()->IsConstantExpression(inValue);
}

bool    ExpressionBinary::IsConstantExpression(value_t* inValue)
{
	return    FirstTerm()->IsConstantExpression(inValue) &&
		SecondTerm()->IsConstantExpression(inValue);
}

bool    ExpressionTernary::IsConstantExpression(value_t* inValue)
{
	return    FirstTerm()->IsConstantExpression(inValue) &&
		SecondTerm()->IsConstantExpression(inValue) &&
		ThirdTerm()->IsConstantExpression(inValue);
}



int    ExpressionValue::Size(void) { return 1; }
int    ExpressionUnary::Size(void) { return 1 + UnaryTerm()->Size(); }
int    ExpressionBinary::Size(void) {
	return 1 + FirstTerm()->Size() + SecondTerm()->Size();
}
int    ExpressionTernary::Size(void) {
	return 1 + FirstTerm()->Size() + SecondTerm()->Size() + ThirdTerm()->Size();
}

int    ExpressionValue::Depth(void) { return 1; }
int    ExpressionUnary::Depth(void) { return 1 + UnaryTerm()->Depth(); }
int    ExpressionBinary::Depth(void) {
	return 1 + max(FirstTerm()->Depth(), SecondTerm()->Depth());
}
int    ExpressionTernary::Depth(void) {
	int d1 = FirstTerm()->Depth();
	int d2 = SecondTerm()->Depth();
	int d3 = ThirdTerm()->Depth();
	return 1 + max3(d1, d2, d3);
}










/*****************************************************************************
 *****************************************************************************
 *
 * Evaluation
 *
 *
 * The code used for each type of expression is virtually identical, except
 * for the specific operation.  For most functions implemented, I exploited
 * this similarity using parameterized macros to define the code, and passed
 * the class name and operation.
 *
 * Building it this way allows me to make changes very easily, however,
 * writing the code like this will undoubtedly make it harder to follow for
 * those not familiar with macros in general, parameterized macros, or special
 * things that can only be used in macros like the token pasting operator ##.
 *
 * I am leaving most of the macro stuff in because it's very helpful to me.
 * However, for this function (the core of the expression evaluation) I will
 * fully write out the code without using the macros, but I will leave the
 * macro stuff commented out and explain how I used them so that (hopefully)
 * the other macros I used in my code can be understandable.
 *
 * Parameterized macro: takes parameters similar to the way functions do.
 *
 * Token pasting: ## allows things to be placed next to each other in macros
 *
 *   Example:
 *
 *     #define FOO( bar, baz ) foo##bar( baz )
 *
 *   Then the usage:
 *     FOO( d, eat )
 *     FOO( yadda, 7 )
 *
 *   Expands to:
 *     food( eat )
 *     fooyadda( 7 )
 *
 *
 *****************************************************************************
 *****************************************************************************/


value_t    ExpressionConstant::Evaluate(void)
{
	return ConstantValue();
}

#ifdef REGULAR_EXPRESSION
value_t    ExpressionSymbol::Evaluate(void)
{
	return (value_t)SymbolValue();
}
#endif

value_t    ExpressionVariable::Evaluate(void)
{
	return *PVariableValue();
}

/*
#define ADD(v1, v2) (v1) + (v2)
#define SUB(v1, v2) (v1) - (v2)
#define MUL(v1, v2) (v1) * (v2)
#define DIV(v1, v2) (v1) / (v2)
#define POW(v1, v2) exp( log((v1)) * (v2) )

#define EXPRESSION_UNARY__EVALUATE(OperationName, OperationSymbol)    \
value_t Expression##OperationName    ::Evaluate(void)\
{\
	return OperationSymbol( UnaryTerm()->Evaluate() );\
}

#define EXPRESSION_BINARY_EVALUATE(OperationName, OperationSymbol)    \
value_t Expression##OperationName    ::Evaluate(void)\
{\
	return OperationSymbol(FirstTerm()->Evaluate(), SecondTerm()->Evaluate());\
}


EXPRESSION_UNARY__EVALUATE(Sin, sin)
EXPRESSION_UNARY__EVALUATE(ArcSin, asin)
EXPRESSION_BINARY_EVALUATE(Add, ADD)
EXPRESSION_BINARY_EVALUATE(Sub, SUB)
EXPRESSION_BINARY_EVALUATE(Mult, MUL)
EXPRESSION_BINARY_EVALUATE(Div, DIV)
EXPRESSION_BINARY_EVALUATE(Power, POW)
*/

value_t ExpressionUserDefined::Evaluate(void)
{
	const value_t    nValue = UnaryTerm()->Evaluate();

	if (m_edExpressionDescription.edtType == ED_FUNCTION)
		return m_edExpressionDescription.pointer.unaryFunc(nValue);
	else
		if (m_edExpressionDescription.edtType == ED_FUNCTIONEX
			//        && m_edExpressionDescription.pExtraData != NULL
			)
			return m_edExpressionDescription.pointer.unaryFuncEx(
				nValue,
				m_edExpressionDescription.pExtraData);
		else
			return 0.0f;

}




value_t ExpressionSqrt::Evaluate(void)
{
	return (value_t)sqrt(UnaryTerm()->Evaluate());
}

value_t ExpressionSqr::Evaluate(void)
{
	const value_t    nTerm = UnaryTerm()->Evaluate();
	return nTerm * nTerm;
}

value_t ExpressionLog10::Evaluate(void)
{
	//    return (value_t)log( UnaryTerm()->Evaluate() ) / log(10);
	//    return (value_t)log( UnaryTerm()->Evaluate() ) * 0.434294481903f;
	return (value_t)log10(UnaryTerm()->Evaluate());
}

value_t ExpressionLn::Evaluate(void)
{
	return (value_t)log(UnaryTerm()->Evaluate());
}

value_t ExpressionExp::Evaluate(void)
{
	return (value_t)exp(UnaryTerm()->Evaluate());
}

value_t ExpressionFactorial::Evaluate(void)
{
	// double factorial(double x) {return std::tgamma(x+1);}
	return (value_t)std::tgamma(1 + UnaryTerm()->Evaluate());
}



value_t ExpressionCos::Evaluate(void)
{
	return (value_t)cos(UnaryTerm()->Evaluate());
}

value_t ExpressionSin::Evaluate(void)
{
	return (value_t)sin(UnaryTerm()->Evaluate());
}

value_t ExpressionTan::Evaluate(void)
{
	return (value_t)tan(UnaryTerm()->Evaluate());
}

value_t ExpressionArcCos::Evaluate(void)
{
	return (value_t)acos(UnaryTerm()->Evaluate());
}

value_t ExpressionArcSin::Evaluate(void)
{
	return (value_t)asin(UnaryTerm()->Evaluate());
}

value_t ExpressionArcTan::Evaluate(void)
{
	return (value_t)atan(UnaryTerm()->Evaluate());
}



/* Hyperbolic trig */

value_t ExpressionCosh::Evaluate(void)
{
	return (value_t)cosh(UnaryTerm()->Evaluate());
}

value_t ExpressionSinh::Evaluate(void)
{
	return (value_t)sinh(UnaryTerm()->Evaluate());
}

value_t ExpressionTanh::Evaluate(void)
{
	return (value_t)tanh(UnaryTerm()->Evaluate());
}

value_t ExpressionArcCosh::Evaluate(void)
{
	const value_t nTerm = UnaryTerm()->Evaluate();

	return (value_t)log(nTerm + sqrt(nTerm * nTerm - 1.0f));
}

value_t ExpressionArcSinh::Evaluate(void)
{
	const value_t nTerm = UnaryTerm()->Evaluate();

	return (value_t)log(nTerm + sqrt(nTerm * nTerm + 1.0f));
}

value_t ExpressionArcTanh::Evaluate(void)
{
	const value_t nTerm = UnaryTerm()->Evaluate();

	return 0.5f * (value_t)log((1.0f + nTerm) / (1.0f - nTerm));
}



value_t ExpressionAdd::Evaluate(void)
{
	return FirstTerm()->Evaluate() + SecondTerm()->Evaluate();
}

value_t ExpressionSub::Evaluate(void)
{
	return FirstTerm()->Evaluate() - SecondTerm()->Evaluate();
}

value_t ExpressionMult::Evaluate(void)
{
	return FirstTerm()->Evaluate() * SecondTerm()->Evaluate();
}

value_t ExpressionDiv::Evaluate(void)
{
	return FirstTerm()->Evaluate() / SecondTerm()->Evaluate();
}

value_t ExpressionMod::Evaluate(void)
{
	return (value_t)fmod(FirstTerm()->Evaluate(), SecondTerm()->Evaluate());
}

value_t ExpressionPower::Evaluate(void)
{
	//    return exp( log( FirstTerm()->Evaluate() ) * SecondTerm()->Evaluate() );
	return (value_t)pow(FirstTerm()->Evaluate(), SecondTerm()->Evaluate());
}

/* the comma operator evaluates both expressions, but only returns the second */
value_t ExpressionComma::Evaluate(void)
{
	FirstTerm()->Evaluate();
	return SecondTerm()->Evaluate();
}


value_t ExpressionConditional::Evaluate(void)
{
	return (FirstTerm()->Evaluate() != 0) ?
		SecondTerm()->Evaluate() : ThirdTerm()->Evaluate();
}


value_t ExpressionEquality::Evaluate(void)
{
	return FirstTerm()->Evaluate() == SecondTerm()->Evaluate();
}
value_t ExpressionNonEquality::Evaluate(void)
{
	return FirstTerm()->Evaluate() != SecondTerm()->Evaluate();
}
value_t ExpressionGreaterThan::Evaluate(void)
{
	return FirstTerm()->Evaluate() > SecondTerm()->Evaluate();
}
value_t ExpressionGreaterThanOrEqual::Evaluate(void)
{
	return FirstTerm()->Evaluate() >= SecondTerm()->Evaluate();
}
value_t ExpressionLessThan::Evaluate(void)
{
	return FirstTerm()->Evaluate() < SecondTerm()->Evaluate();
}
value_t ExpressionLessThanOrEqual::Evaluate(void)
{
	return FirstTerm()->Evaluate() <= SecondTerm()->Evaluate();
}



#ifdef REGULAR_EXPRESSION
value_t ExpressionStar::Evaluate(void)
{
	return ERR_SHOULD_NOT_IMPLEMENT;
}

value_t ExpressionOr::Evaluate(void)
{
	union {
		value_t    float1;
		__int64 int1;
	};

	union {
		value_t float2;
		__int64 int2;
	};

	union {
		value_t float3;
		__int64 int3;
	};

	float1 = FirstTerm()->Evaluate();
	float2 = SecondTerm()->Evaluate();
	int3 = int1 | int2;

	return float3;
}

value_t ExpressionAnd::Evaluate(void)
{
	union {
		value_t    float1;
		__int64 int1;
	};

	union {
		value_t float2;
		__int64 int2;
	};

	union {
		value_t float3;
		__int64 int3;
	};

	float1 = FirstTerm()->Evaluate();
	float2 = SecondTerm()->Evaluate();
	int3 = int1 & int2;

	return float3;
}

#endif







/****************************************************************************
 ****************************************************************************
 *
 * Mathematical Operations
 *
 * NOTE: should make copies
 *
 * so far, pretty much useless.  I don't use it.
 *
 ****************************************************************************
 ****************************************************************************/


 /* pre:        ---
  * post:    outExpression is the sin of this
  */
#define EXPRESSION_UNARY__MATH_OPERATION(OperationName)    \
error_t    Expression    ::OperationName    (Expression **outExpression)\
{\
    return New##OperationName    (this, NULL, outExpression);\
}

  /* pre:     inExpression is valid
   * post:    outExpression is the sum of this and inExpression
   * post:    inExpression is invalid (should not be used in other expressions,
   *            copies are ok)
   */
#define EXPRESSION_BINARY_MATH_OPERATION(OperationName)    \
error_t    Expression    ::OperationName    (Expression *inExpression,\
                                     Expression **outExpression)\
{\
    return New##OperationName    (this, inExpression, outExpression);\
}

   /* pre:     inExpression2, inExpression3 are valid
	* post:    outExpression is the sum of this and inExpression
	* post:    inExpression is invalid (should not be used in other expressions,
	*            copies are ok)
	*/
#define EXPRESSION_TERNARY_MATH_OPERATION(OperationName)    \
error_t    Expression    ::OperationName    (Expression *inExpression2, Expression *inExpression3,\
                                     Expression **outExpression)\
{\
    return New##OperationName    (this, inExpression2, inExpression3, outExpression);\
}



EXPRESSION_UNARY__MATH_OPERATION(Sqrt)
EXPRESSION_UNARY__MATH_OPERATION(Sqr)
EXPRESSION_UNARY__MATH_OPERATION(Log10)
EXPRESSION_UNARY__MATH_OPERATION(Ln)
EXPRESSION_UNARY__MATH_OPERATION(Exp)
EXPRESSION_UNARY__MATH_OPERATION(Factorial)

EXPRESSION_UNARY__MATH_OPERATION(Cos)
EXPRESSION_UNARY__MATH_OPERATION(Sin)
EXPRESSION_UNARY__MATH_OPERATION(Tan)
EXPRESSION_UNARY__MATH_OPERATION(ArcCos)
EXPRESSION_UNARY__MATH_OPERATION(ArcSin)
EXPRESSION_UNARY__MATH_OPERATION(ArcTan)

EXPRESSION_UNARY__MATH_OPERATION(Cosh)
EXPRESSION_UNARY__MATH_OPERATION(Sinh)
EXPRESSION_UNARY__MATH_OPERATION(Tanh)
EXPRESSION_UNARY__MATH_OPERATION(ArcCosh)
EXPRESSION_UNARY__MATH_OPERATION(ArcSinh)
EXPRESSION_UNARY__MATH_OPERATION(ArcTanh)

EXPRESSION_BINARY_MATH_OPERATION(Add)
EXPRESSION_BINARY_MATH_OPERATION(Sub)
EXPRESSION_BINARY_MATH_OPERATION(Mult)
EXPRESSION_BINARY_MATH_OPERATION(Div)
EXPRESSION_BINARY_MATH_OPERATION(Mod)
EXPRESSION_BINARY_MATH_OPERATION(Power)

EXPRESSION_BINARY_MATH_OPERATION(Equality)
EXPRESSION_BINARY_MATH_OPERATION(NonEquality)
EXPRESSION_BINARY_MATH_OPERATION(GreaterThan)
EXPRESSION_BINARY_MATH_OPERATION(GreaterThanOrEqual)
EXPRESSION_BINARY_MATH_OPERATION(LessThan)
EXPRESSION_BINARY_MATH_OPERATION(LessThanOrEqual)

EXPRESSION_TERNARY_MATH_OPERATION(Conditional)

#ifdef REGULAR_EXPRESSION
EXPRESSION_UNARY__MATH_OPERATION(Star)
EXPRESSION_BINARY_MATH_OPERATION(Or)
EXPRESSION_BINARY_MATH_OPERATION(And)
#endif

/****************************************************************************
 *
 * Copy
 *
 * pretty much useless, since it's never used yet
 *
 ****************************************************************************/

#define EXPRESSION_UNARY__COPY(OperationName)    \
error_t    Expression##OperationName    ::Copy(Expression **outExpression)\
{\
    Expression *tmpExpression = nullptr;\
    error_t err = SUCCESS;\
\
    if( (err = UnaryTerm()->Copy(&tmpExpression)) == SUCCESS )\
        return New##OperationName ( tmpExpression, NULL, outExpression );\
    else\
        return err;\
}

#define EXPRESSION_BINARY_COPY(OperationName)    \
error_t    Expression##OperationName    ::Copy(Expression **outExpression)\
{\
    Expression *term1 = nullptr;\
    Expression *term2 = nullptr;\
    error_t err1 = SUCCESS;\
    error_t err2 = SUCCESS;\
\
    if( (err1 = FirstTerm()->Copy(&term1)) == SUCCESS &&\
        (err2 = SecondTerm()->Copy(&term2)) == SUCCESS )\
        return New##OperationName ( term1, term2, outExpression );\
    else\
        return max(err1, err2);\
}

#define EXPRESSION_TERNARY_COPY(OperationName)    \
error_t    Expression##OperationName    ::Copy(Expression **outExpression)\
{\
    Expression *term1 = nullptr;\
    Expression *term2 = nullptr;\
    Expression *term3 = nullptr;\
    error_t err1 = SUCCESS;\
    error_t err2 = SUCCESS;\
    error_t err3 = SUCCESS;\
\
    if( (err1 = FirstTerm()->Copy(&term1)) == SUCCESS &&\
        (err2 = SecondTerm()->Copy(&term2)) == SUCCESS &&\
        (err3 = ThirdTerm()->Copy(&term3)) == SUCCESS)\
        return New##OperationName ( term1, term2, term3, outExpression );\
    else\
        return max3(err1, err2, err3);\
}


	error_t    ExpressionConstant::Copy(Expression** outExpression)
{
	return NewConstant(ConstantValue(), outExpression);
}

#ifdef REGULAR_EXPRESSION
error_t    ExpressionSymbol::Copy(Expression** outExpression)
{
	return NewSymbol(SymbolValue(), outExpression);
}
#endif

error_t    ExpressionUserDefined::Copy(Expression** outExpression)
{
	Expression* term1;
	error_t        err = SUCCESS;

	if ((err = UnaryTerm()->Copy(&term1)) == SUCCESS)
		return NewUserDefined(&m_edExpressionDescription,
			term1,
			outExpression);
	else
		return err;
}

error_t    ExpressionVariable::Copy(Expression** outExpression)
{
	return NewVariable(VariableName(), PVariableValue(), outExpression);
}


EXPRESSION_UNARY__COPY(Sqrt)
EXPRESSION_UNARY__COPY(Sqr)
EXPRESSION_UNARY__COPY(Log10)
EXPRESSION_UNARY__COPY(Ln)
EXPRESSION_UNARY__COPY(Exp)
EXPRESSION_UNARY__COPY(Factorial)

EXPRESSION_UNARY__COPY(Sin)
EXPRESSION_UNARY__COPY(Cos)
EXPRESSION_UNARY__COPY(Tan)
EXPRESSION_UNARY__COPY(ArcCos)
EXPRESSION_UNARY__COPY(ArcSin)
EXPRESSION_UNARY__COPY(ArcTan)

EXPRESSION_UNARY__COPY(Cosh)
EXPRESSION_UNARY__COPY(Sinh)
EXPRESSION_UNARY__COPY(Tanh)
EXPRESSION_UNARY__COPY(ArcCosh)
EXPRESSION_UNARY__COPY(ArcSinh)
EXPRESSION_UNARY__COPY(ArcTanh)

EXPRESSION_BINARY_COPY(Add)
EXPRESSION_BINARY_COPY(Sub)
EXPRESSION_BINARY_COPY(Mult)
EXPRESSION_BINARY_COPY(Div)
EXPRESSION_BINARY_COPY(Mod)
EXPRESSION_BINARY_COPY(Power)

EXPRESSION_BINARY_COPY(Equality)
EXPRESSION_BINARY_COPY(NonEquality)
EXPRESSION_BINARY_COPY(GreaterThan)
EXPRESSION_BINARY_COPY(GreaterThanOrEqual)
EXPRESSION_BINARY_COPY(LessThan)
EXPRESSION_BINARY_COPY(LessThanOrEqual)

EXPRESSION_TERNARY_COPY(Conditional)

#ifdef REGULAR_EXPRESSION
EXPRESSION_UNARY__COPY(Star)
EXPRESSION_BINARY_COPY(Or)
EXPRESSION_BINARY_COPY(And)
#endif

EXPRESSION_BINARY_COPY(Comma)






/****************************************************************************
 *
 * Partial Simplification
 *
 * pretty much useless, seems to slow the code down
 *
 ****************************************************************************/

	error_t    ExpressionConstant::PartialSimplification(value_t* inValue,
		Expression** outExpression)
{
	return Copy(outExpression);
}

#ifdef REGULAR_EXPRESSION
error_t    ExpressionSymbol::PartialSimplification(value_t* inValue,
	Expression** outExpression)
{
	return Copy(outExpression);
}
#endif

error_t    ExpressionVariable::PartialSimplification(value_t* inValue,
	Expression** outExpression)
{
	if (inValue == PVariableValue())
		return NewConstant(*PVariableValue(), outExpression);
	else
		return Copy(outExpression);
}



error_t ExpressionUserDefined::PartialSimplification
(value_t* inValue, Expression** outExpression)
{
	Expression* tmpExpression;
	error_t err;

	if (this->IsConstantExpression(inValue))
		return NewConstant(this->Evaluate(), outExpression);
	else if ((err =
		UnaryTerm()->PartialSimplification(inValue, &tmpExpression)
		) == SUCCESS)
		return NewUserDefined(&m_edExpressionDescription,
			tmpExpression,
			outExpression);
	else
		return err;
}



#define EXPRESSION_UNARY__PARTIAL(OperationName) \
error_t Expression##OperationName    ::PartialSimplification\
    (value_t *inValue, Expression **outExpression)\
{\
    Expression *tmpExpression = nullptr;\
    error_t err = SUCCESS;\
\
    if( this->IsConstantExpression(inValue) )\
        return NewConstant( this->Evaluate(), outExpression );\
    else if( ( err =    \
                 UnaryTerm()->PartialSimplification(inValue, &tmpExpression)\
             ) == SUCCESS )\
        return New##OperationName    (tmpExpression, NULL, outExpression);\
    else\
        return err;\
}

#define EXPRESSION_BINARY_PARTIAL(OperationName) \
error_t Expression##OperationName    ::PartialSimplification\
    (value_t *inValue, Expression **outExpression)\
{\
    Expression *term1 = nullptr, *term2 = nullptr;\
    error_t err1 = SUCCESS;\
    error_t err2 = SUCCESS;\
\
    if( this->IsConstantExpression(inValue) )\
        return NewConstant( this->Evaluate(), outExpression );\
    else if( ( err1 = FirstTerm()->PartialSimplification(inValue, &term1)\
             ) == SUCCESS\
        &&     ( err2 = SecondTerm()->PartialSimplification(inValue, &term2)\
             ) == SUCCESS )\
        return New##OperationName(term1, term2, outExpression);\
    else\
        return max( err1, err2 );\
}

#define EXPRESSION_TERNARY_PARTIAL(OperationName) \
error_t Expression##OperationName    ::PartialSimplification\
    (value_t *inValue, Expression **outExpression)\
{\
    Expression *term1 = nullptr, *term2 = nullptr, *term3 = nullptr;\
    error_t err1 = SUCCESS;\
    error_t err2 = SUCCESS;\
    error_t err3 = SUCCESS;\
\
    if( this->IsConstantExpression(inValue) )\
        return NewConstant( this->Evaluate(), outExpression );\
    else if( ( err1 = FirstTerm()->PartialSimplification(inValue, &term1)\
             ) == SUCCESS\
        &&     ( err2 = SecondTerm()->PartialSimplification(inValue, &term2)\
             ) == SUCCESS\
        &&     ( err3 = ThirdTerm()->PartialSimplification(inValue, &term3)\
             ) == SUCCESS )\
        return New##OperationName(term1, term2, term3, outExpression);\
    else\
        return max3( err1, err2, err3 );\
}

EXPRESSION_UNARY__PARTIAL(Sqrt)
EXPRESSION_UNARY__PARTIAL(Sqr)
EXPRESSION_UNARY__PARTIAL(Log10)
EXPRESSION_UNARY__PARTIAL(Ln)
EXPRESSION_UNARY__PARTIAL(Exp)
EXPRESSION_UNARY__PARTIAL(Factorial)

EXPRESSION_UNARY__PARTIAL(Sin)
EXPRESSION_UNARY__PARTIAL(Cos)
EXPRESSION_UNARY__PARTIAL(Tan)
EXPRESSION_UNARY__PARTIAL(ArcCos)
EXPRESSION_UNARY__PARTIAL(ArcSin)
EXPRESSION_UNARY__PARTIAL(ArcTan)

EXPRESSION_UNARY__PARTIAL(Cosh)
EXPRESSION_UNARY__PARTIAL(Sinh)
EXPRESSION_UNARY__PARTIAL(Tanh)
EXPRESSION_UNARY__PARTIAL(ArcCosh)
EXPRESSION_UNARY__PARTIAL(ArcSinh)
EXPRESSION_UNARY__PARTIAL(ArcTanh)

EXPRESSION_BINARY_PARTIAL(Add)
EXPRESSION_BINARY_PARTIAL(Sub)
EXPRESSION_BINARY_PARTIAL(Mult)
EXPRESSION_BINARY_PARTIAL(Div)
EXPRESSION_BINARY_PARTIAL(Mod)
EXPRESSION_BINARY_PARTIAL(Power)
EXPRESSION_BINARY_PARTIAL(Comma)

EXPRESSION_BINARY_PARTIAL(Equality)
EXPRESSION_BINARY_PARTIAL(NonEquality)
EXPRESSION_BINARY_PARTIAL(GreaterThan)
EXPRESSION_BINARY_PARTIAL(GreaterThanOrEqual)
EXPRESSION_BINARY_PARTIAL(LessThan)
EXPRESSION_BINARY_PARTIAL(LessThanOrEqual)

EXPRESSION_TERNARY_PARTIAL(Conditional)

#ifdef REGULAR_EXPRESSION
EXPRESSION_UNARY__PARTIAL(Star)
EXPRESSION_BINARY_PARTIAL(Or)
EXPRESSION_BINARY_PARTIAL(And)
#endif







/****************************************************************************
 ****************************************************************************
 *
 * Operators
 *
 ****************************************************************************
 ****************************************************************************/

 /* default has no operator -- Scalar */
	char* ExpressionValue::Operator(void) { return NULL; };

/* Unary */
char* ExpressionSqrt::Operator(void) { return "sqrt"; };
char* ExpressionSqr::Operator(void) { return "sqr"; };
char* ExpressionLog10::Operator(void) { return "log10"; };
char* ExpressionLn::Operator(void) { return "log"; };
char* ExpressionExp::Operator(void) { return "exp"; };
char* ExpressionFactorial::Operator(void) { return "fact"; };

/* Trigonometric */
char* ExpressionCos::Operator(void) { return "cos"; };
char* ExpressionSin::Operator(void) { return "sin"; };
char* ExpressionTan::Operator(void) { return "tan"; };
char* ExpressionArcCos::Operator(void) { return "acos"; };
char* ExpressionArcSin::Operator(void) { return "asin"; };
char* ExpressionArcTan::Operator(void) { return "atan"; };

/* Hyperbolic Trig */
char* ExpressionCosh::Operator(void) { return "cosh"; };
char* ExpressionSinh::Operator(void) { return "sinh"; };
char* ExpressionTanh::Operator(void) { return "tanh"; };
char* ExpressionArcCosh::Operator(void) { return "acosh"; };
char* ExpressionArcSinh::Operator(void) { return "asinh"; };
char* ExpressionArcTanh::Operator(void) { return "atanh"; };

/* Binary */
char* ExpressionAdd::Operator(void) { return "+"; };
char* ExpressionSub::Operator(void) { return "-"; };
char* ExpressionMult::Operator(void) { return "*"; };
char* ExpressionDiv::Operator(void) { return "/"; };
char* ExpressionMod::Operator(void) { return "%"; };
char* ExpressionPower::Operator(void) { return "^"; };
char* ExpressionComma::Operator(void) { return ","; };

/* Relational */
char* ExpressionEquality::Operator(void) { return "=="; };
char* ExpressionNonEquality::Operator(void) { return "!="; };
char* ExpressionGreaterThan::Operator(void) { return ">"; };
char* ExpressionGreaterThanOrEqual::Operator(void) { return ">="; };
char* ExpressionLessThan::Operator(void) { return "<"; };
char* ExpressionLessThanOrEqual::Operator(void) { return "<="; };

/* Ternary */
char* ExpressionConditional::Operator(void) { return "?:"; }; /* JRDV: WRONG! */

#ifdef REGULAR_EXPRESSION
char* ExpressionStar::Operator(void) { return "#"; };
char* ExpressionOr::Operator(void) { return "|"; };
char* ExpressionAnd::Operator(void) { return "&"; };
#endif
char* ExpressionUserDefined::Operator(void)
{
	return m_edExpressionDescription.strName;
};



/****************************************************************************
 ****************************************************************************
 *
 * PrintString    something tells me this is a memory leak... a big one.
 * also more error checking might be nice
 *
 ****************************************************************************
 ****************************************************************************/

 //DEBUG!  Move to top of file if kept
 //#include <strstrea.h>

 //    ostrstream str;

 /*
  * hmmm, output string stream sounds like a good idea
  * on second thought, no it doesn't.  I don't want to
  * have a million of includes just to use one little
  * thing in each of the libraries.
  *
  * why not just try it and see how I like it.
  * have both versions, and conditionally compile on of them
  */

  /*
   *pre:    this is a valid Expression (shouldn't be possible to create an invalid)
   *post:   returns a string representation of the expression that could be used
   *        to create a new identical expression
   *    or    a string stating an error occured (prolly should change this)
   *
   *note:    should NEVER return NULL
   *note:    should implement a way to remove unnecessary parens
   */
char* Expression::PrintString(void)
{
	char* tmpStr;
	char* retString;
	int     nCount = 1; /* for the null character */

	/*
	 * allocate space for the null character
	 */
	if ((tmpStr = (char*)malloc(nCount)) != NULL)
	{
		*tmpStr = '\0'; /* write the null character */

		/*
		 * build the string representation
		 */
		if ((retString = this->PrintString(tmpStr, nCount)) != NULL)
			return retString;
	}

	/* else, something went wrong */
	return "PrintString: ERROR creating string";
}



/*
 * Pre:        nLength contains the number of chars that are already allocated
 * Note:    inStr *MUST* be realloc'd prior to use
 *
 * Pre:        inStr is null terminated, and is where stuff should be appended
 *
 * Post:    nLength contains the new allocated size
 * Post:    inStr is no longer valid
 *
 * Post:    if NULL is returned, the allocated memory has been freed
 */

char* ExpressionVariable::PrintString(char* inStr, int& nLength)
{
	char* tmpStr;
	int     nCount;

	nCount = strlen(VariableName());

	nLength += nCount;    /* adjust the new length */

	/*
	 * make some space
	 */
	if ((tmpStr = (char*)realloc(inStr, nLength)) != NULL)
	{
		/*
		 * add yer stuff
		 */
		return strcat(tmpStr, VariableName());
	}
	else
	{    /* realloc failure, free the memory and get out */
		free(inStr);
		return NULL;
	}
}


char* ExpressionConstant::PrintString(char* inStr, int& nLength)
{
	/* max double is:        1.7976931348623158e+308
	 * min pos double is:    2.2250738585072014e-308
	 * min epsilon            2.2204460492503131e-016
	 * 23 chars + 1 for null character, and 1 for optional leading sign means
	 * at most 25 characters are necessary to always have enough spece to
	 * create the string representation of a double
	 */
	char    buffer[32]; /* bigger than would ever be necessary */
	char* tmpStr;
	int     nCount;

	/*
	 * convert num to string, and count the number of characters
	 */
	nCount = snprintf(buffer, _countof(buffer), "%g", ConstantValue());

	nLength += nCount;

	/*
	 * resize inStr so the num will fit
	 */
	if ((tmpStr = (char*)realloc(inStr, nLength)) != NULL)
	{
		/* add the number */
		return strcat(tmpStr, buffer);
	}
	else
	{    /* realloc failure, free the memory and get out */
		free(inStr);
		return NULL;
	}
}

#ifdef REGULAR_EXPRESSION
char* ExpressionSymbol::PrintString(char* inStr, int& nLength)
{
	/* max double is:        1.7976931348623158e+308
	 * min pos double is:    2.2250738585072014e-308
	 * min epsilon            2.2204460492503131e-016
	 * 23 chars + 1 for null character, and 1 for optional leading sign means
	 * at most 25 characters are necessary to always have enough spece to
	 * create the string representation of a double
	 */
	char    buffer[30]; /* bigger than would ever be necessary */
	char* tmpStr;
	int     nCount;

	/*
	 * convert num to string, and count the number of characters
	 */
	nCount = snprintf(buffer, _countof(buffer), "%c", SymbolValue());

	nLength += nCount;

	/*
	 * resize inStr so the num will fit
	 */
	if ((tmpStr = (char*)realloc(inStr, nLength)) != NULL)
	{
		/* add the number */
		return strcat(tmpStr, buffer);
	}
	else
	{    /* realloc failure, free the memory and get out */
		free(inStr);
		return NULL;
	}
}
#endif

char* ExpressionUnary::PrintString(char* inStr, int& nLength)
{
	char* tmpStr;
	char* tmpTmpStr;
	int     nCount; /* additional space required just for
					 * the current operation *not* including
					 * the additional space needed for children
					 */

					 /* left & right parens + length of operator */
	nCount = 2 + strlen(Operator());

	nLength += nCount;


	/*
	 * resize inStr so the current operation may be added
	 */
	if ((tmpStr = (char*)realloc(inStr, nLength)) != NULL)
	{
		strcat(tmpStr, Operator());    /* add the operator */
		strcat(tmpStr, "(");            /* and the opening paren */
		if ((tmpTmpStr = UnaryTerm()->PrintString(tmpStr, nLength)) != NULL)
		{
			//            tmpStr = tmpTmpStr;
			return strcat(tmpTmpStr, ")"); /* and now the closing paren */
		}
	}
	else
		/* realloc failure, free the memory and get out */
		free(inStr);

	/* else, something went wrong */
	return NULL;
}

char* ExpressionBinary::PrintString(char* inStr, int& nLength)
{
	char* strTerm1, * strTerm2;
	char* tmpStr;
	int nCount; /* additional space required just for
				 * the current operation *not* including
				 * the additional space needed for children
				 */

				 /* left & right parens + length of operator */
	nCount = 2 + strlen(Operator());

	nLength += nCount;

	/* make space for current operation and parens */
	if ((tmpStr = (char*)realloc(inStr, nLength)) != NULL)
	{
		strTerm1 = strcat(tmpStr, "(");

		if ((tmpStr = FirstTerm()->PrintString(strTerm1, nLength)) != NULL)
		{
			strTerm2 = strcat(tmpStr, Operator());

			if ((tmpStr = SecondTerm()->PrintString(strTerm2, nLength)) != NULL)
			{
				return strcat(tmpStr, ")");
			}
		}
	}
	else
		/* realloc failure, free the memory and get out */
		free(inStr);

	/* else, something bad happened */
	return NULL;
}

char* ExpressionTernary::PrintString(char* inStr, int& nLength)
{
	char* strTerm1, * strTerm2, * strTerm3;
	char* tmpStr;
	int nCount; /* additional space required just for
				 * the current operation *not* including
				 * the additional space needed for children
				 */

				 /* left & right parens + length of operator */
	nCount = 2 + 2; // strlen(Operator());

	nLength += nCount;

	/* make space for current operation and parens */
	if ((tmpStr = (char*)realloc(inStr, nLength)) != NULL)
	{
		strTerm1 = strcat(tmpStr, "(");

		if ((tmpStr = FirstTerm()->PrintString(strTerm1, nLength)) != NULL)
		{
			strTerm2 = strcat(tmpStr, "?"); // TODO: need to refactor Operator(), or use a format string.

			if ((tmpStr = SecondTerm()->PrintString(strTerm2, nLength)) != NULL)
			{
				strTerm3 = strcat(tmpStr, ":"); // TODO: need to refactor Operator(), or use a format string.

				if ((tmpStr = ThirdTerm()->PrintString(strTerm3, nLength)) != NULL)
				{
					return strcat(tmpStr, ")");
				}
			}
		}
	}
	else
		/* realloc failure, free the memory and get out */
		free(inStr);

	/* else, something bad happened */
	return NULL;
}








/*****************************************************************************
 *****************************************************************************
 *                                                                           *
 * Instance Creation -- the ONLY direct reference to any Expression subclass *
 *                                                                           *
 *****************************************************************************
 *****************************************************************************/

 /*
  * these are the ONLY full references to the class names ANYWHERE in the code
  */

  /* Scalar */
#define NEW_CONSTANT(value)         (new ExpressionConstant(value))
#define NEW_USERDEFINED(desc, exp1) (new ExpressionUserDefined(desc, exp1))
#define NEW_VARIABLE(name, value)   (new ExpressionVariable(name, value))

/* Unary */
#define NEW_SQRT(exp1)              (new ExpressionSqrt(exp1))
#define NEW_SQR(exp1)               (new ExpressionSqr(exp1))
#define NEW_LOG10(exp1)             (new ExpressionLog10(exp1))
#define NEW_LN(exp1)                (new ExpressionLn(exp1))
#define NEW_EXP(exp1)               (new ExpressionExp(exp1))
#define NEW_FACTORIAL(exp1)			(new ExpressionFactorial(exp1))

/* Trigonometric */
#define NEW_SIN(exp1)               (new ExpressionSin(exp1))
#define NEW_COS(exp1)               (new ExpressionCos(exp1))
#define NEW_TAN(exp1)               (new ExpressionTan(exp1))
#define NEW_ARCCOS(exp1)            (new ExpressionArcCos(exp1))
#define NEW_ARCSIN(exp1)            (new ExpressionArcSin(exp1))
#define NEW_ARCTAN(exp1)            (new ExpressionArcTan(exp1))

/* Hyperbolic Trig */
#define NEW_COSH(exp1)              (new ExpressionCosh(exp1))
#define NEW_SINH(exp1)              (new ExpressionSinh(exp1))
#define NEW_TANH(exp1)              (new ExpressionTanh(exp1))
#define NEW_ARCCOSH(exp1)           (new ExpressionArcCosh(exp1))
#define NEW_ARCSINH(exp1)           (new ExpressionArcSinh(exp1))
#define NEW_ARCTANH(exp1)           (new ExpressionArcTanh(exp1))

/* Binary */
#define NEW_ADD(exp1, exp2)         (new ExpressionAdd(exp1, exp2))
#define NEW_SUB(exp1, exp2)         (new ExpressionSub(exp1, exp2))
#define NEW_MULT(exp1, exp2)        (new ExpressionMult(exp1, exp2))
#define NEW_DIV(exp1, exp2)         (new ExpressionDiv(exp1, exp2))
#define NEW_MOD(exp1, exp2)         (new ExpressionMod(exp1, exp2))
#define NEW_POWER(exp1, exp2)       (new ExpressionPower(exp1, exp2))
#define NEW_COMMA(exp1, exp2)       (new ExpressionComma(exp1, exp2))

/* Relational */
#define NEW_EQUALITY(exp1, exp2)    (new ExpressionEquality(exp1, exp2))
#define NEW_NONEQUALITY(exp1, exp2) (new ExpressionNonEquality(exp1, exp2))
#define NEW_GREATERTHAN(exp1, exp2) (new ExpressionGreaterThan(exp1, exp2))
#define NEW_GTOREQUAL(exp1, exp2)   (new ExpressionGreaterThanOrEqual(exp1, exp2))
#define NEW_LESSTHAN(exp1, exp2)    (new ExpressionLessThan(exp1, exp2))
#define NEW_LTOREQUAL(exp1, exp2)   (new ExpressionLessThanOrEqual(exp1, exp2))

/* Ternary */
#define NEW_CONDITIONAL(exp1, exp2, exp3) (new ExpressionConditional(exp1, exp2, exp3))

/* Regular Expression */
#ifdef REGULAR_EXPRESSION
#define NEW_SYMBOL(value)           (new ExpressionSymbol(value))
#define NEW_STAR(exp1)              (new ExpressionStar(exp1))
#define NEW_OR(exp1, exp2)          (new ExpressionOr(exp1, exp2))
#define NEW_AND(exp1, exp2)         (new ExpressionAnd(exp1, exp2))
#endif


/*
 * these are the ONLY references to the indirect referencing macros
 */
error_t Expression::NewConstant(value_t inValue, Expression** outExpression)
{
	if ((*outExpression = NEW_CONSTANT(inValue)) != NULL)
		return SUCCESS;
	else
		return ERR_CONSTANT;
}

#ifdef REGULAR_EXPRESSION
error_t    Expression::NewSymbol(long inValue, Expression** outExpression)
{
	if ((*outExpression = NEW_SYMBOL(inValue)) != NULL)
		return SUCCESS;
	else
		return ERR_SYMBOL;
}
#endif

error_t Expression::NewUserDefined(EXPRESSIONDESCRIPTION* pExpDescription,
	Expression* inExpression1,
	Expression** outExpression)
{
	if ((*outExpression = NEW_USERDEFINED(pExpDescription, inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_USERDEFINED;
}

error_t Expression::NewVariable(char* inName,
	value_t* inValue,
	Expression** outExpression)
{
	char* tmpName = nullptr;

	if ((tmpName = strdup(inName)) != NULL &&
		(*outExpression = NEW_VARIABLE(tmpName, inValue)) != NULL)
		return SUCCESS;
	else
		return ERR_VARIABLE;
}






error_t Expression::NewSqrt(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_SQRT(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_SQRT;
}

error_t Expression::NewSqr(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_SQR(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_SQR;
}

error_t Expression::NewLog10(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_LOG10(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_LOG10;
}

error_t Expression::NewLn(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_LN(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_LN;
}

error_t    Expression::NewExp(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_EXP(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_EXP;
}

error_t Expression::NewFactorial(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_FACTORIAL(inExpression1)) != NULL)
		return SUCCESS;
	else
		return FAILURE;
}

error_t Expression::NewCos(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_COS(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_COS;
}

error_t Expression::NewSin(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_SIN(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_SIN;
}

error_t Expression::NewTan(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_TAN(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_TAN;
}

error_t Expression::NewArcCos(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_ARCCOS(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_ARCCOS;
}

error_t Expression::NewArcSin(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_ARCSIN(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_ARCSIN;
}

error_t Expression::NewArcTan(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_ARCTAN(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_ARCTAN;
}



error_t Expression::NewCosh(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_COSH(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_COSH;
}

error_t Expression::NewSinh(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_SINH(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_SINH;
}

error_t Expression::NewTanh(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_TANH(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_TANH;
}

error_t Expression::NewArcCosh(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_ARCCOSH(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_ARCCOSH;
}

error_t Expression::NewArcSinh(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_ARCSINH(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_ARCSINH;
}

error_t Expression::NewArcTanh(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_ARCTANH(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_ARCTANH;
}





error_t     Expression::NewAdd(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_ADD(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_ADD;
}

error_t     Expression::NewSub(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_SUB(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_SUB;
}

error_t     Expression::NewMult(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_MULT(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_MULT;
}

error_t     Expression::NewDiv(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_DIV(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_DIV;
}

error_t     Expression::NewMod(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_MOD(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_MOD;
}

error_t     Expression::NewPower(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_POWER(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_POW;
}


error_t     Expression::NewComma(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_COMMA(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_COMMA;
}


error_t     Expression::NewEquality(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_EQUALITY(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_RELATIONAL;
}

error_t     Expression::NewNonEquality(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_NONEQUALITY(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_RELATIONAL;
}

error_t     Expression::NewGreaterThan(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_GREATERTHAN(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_RELATIONAL;
}

error_t     Expression::NewGreaterThanOrEqual(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_GTOREQUAL(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_RELATIONAL;
}

error_t     Expression::NewLessThan(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_LESSTHAN(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_RELATIONAL;
}

error_t     Expression::NewLessThanOrEqual(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_LTOREQUAL(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_RELATIONAL;
}


error_t     Expression::NewConditional(Expression* inExpression1,
	Expression* inExpression2,
	Expression* inExpression3,
	Expression** outExpression)
{
	if ((*outExpression = NEW_CONDITIONAL(inExpression1, inExpression2, inExpression3)) != NULL)
		return SUCCESS;
	else
		return ERR_RELATIONAL;
}



#ifdef REGULAR_EXPRESSION
error_t Expression::NewStar(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_STAR(inExpression1)) != NULL)
		return SUCCESS;
	else
		return ERR_STAR;
}


error_t     Expression::NewOr(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_OR(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_OR;
}

error_t     Expression::NewAnd(Expression* inExpression1,
	Expression* inExpression2,
	Expression** outExpression)
{
	if ((*outExpression = NEW_AND(inExpression1, inExpression2)) != NULL)
		return SUCCESS;
	else
		return ERR_AND;
}
#endif









/****************************************************************************
 ****************************************************************************
 *                                                                          *
 * Compiler / Parser                                                        *
 *                                                                          *
 ****************************************************************************
 ****************************************************************************/



 /****************************************************************************
  * Expression::Evaluate
  *
  * a helper function to quickly evaluate a string
  *
  * nErrorValue isn't necessarily bad, it's so you can specify a default value
  * in case Expression cannot parse the string
  ****************************************************************************/
value_t Expression::Evaluate(const char* inString,
	value_t inErrorValue,
	MyDictionary<value_t*>* inValues,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{
	error_t     err;
	Expression* tmpExpression = NULL;
	value_t     nReturnValue = inErrorValue;

	err = Expression::Compile(inString, &tmpExpression, inValues, inGlobals);
	if (err == SUCCESS)
		nReturnValue = tmpExpression->Evaluate();

	SAFE_DELETE(tmpExpression);

	return nReturnValue;
}




/****************************************************************************
 * Expression::Compile
 *
 * strip out the whitespace before compiling
 ****************************************************************************/
error_t Expression::Compile(const char* inString,
	std::shared_ptr<Expression>& outExpression,
	MyDictionary<value_t*>* inValues,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{
	Expression* myExpression;

	error_t err = Expression::Compile(inString,
		&myExpression,
		inValues,
		inGlobals);

	if (err == SUCCESS)
		outExpression.reset(myExpression);

	return err;
}

error_t Expression::Compile(const char* inString,
	Expression** outExpression,
	MyDictionary<value_t*>* inValues,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{
	error_t err = SUCCESS;
	char* strWithoutWhitespace = NULL;    /* holder for the string without whitespace */
	char* strSource;        /* keeps the current place in the source string (inString) */
	char* strDestination;    /* keeps the current place in the destination string (strCompactString) */
	MyDictionary<value_t*>                emptyValuesDict, * pDictValues;
	MyDictionary<EXPRESSIONDESCRIPTION*>    emptyMyDictionary, * pDictGlobals;

	/*
	 * the private Compile function REQUIRES all parameters to be validated
	 * prior to its invocation... translation:
	 *        check the parameters in the public Compile function so
	 *        they don't have to be checked by the private function
	 */
	if (inString == NULL || outExpression == NULL)
		return ERR_NULL;

	/*
	 * allocate space for the compacted string
	 */
	strWithoutWhitespace = new char[1 + strlen(inString)];
	if (strWithoutWhitespace == NULL)
		return ERR_MALLOC;


	/*
	 * strip whitespace
	 */
	strSource = (char*)inString;    /* have to cast because compiler complains a (const char *) isn't a (char *) */
	strDestination = strWithoutWhitespace;
	while ((*strDestination = *strSource++) != '\0')
	{
		switch (*strDestination)
		{
		case '\t':
		case '\n':
		case '\r':
		case ' ':
		case '\127':
			break;
		default:
			if (*strDestination > ' ') /* some configs somehow get weird control characters in them */
				strDestination++;
			break;
		}
	}


	/*
	 * aarruugghh!  G-Force allows empty strings?!  ok, fine, I'll just swap it for zero
	 */
	if (strWithoutWhitespace[0] == '\0')
	{
		delete[] strWithoutWhitespace;
		if ((strWithoutWhitespace = new char[2]) == NULL)
			return ERR_MALLOC;

		strcpy(strWithoutWhitespace, "0");
	}



	/*
	 * it's ok for the global MyDictionary to be NULL,
	 * we'll just pass an empty MyDictionary to Compile
	 */
	if (inGlobals == NULL)
		pDictGlobals = &emptyMyDictionary;
	else
		pDictGlobals = inGlobals;

	if (inValues == NULL)
		pDictValues = &emptyValuesDict;
	else
		pDictValues = inValues;


	/*
	 * compile the expression
	 */
	err = Expression::Compile(strWithoutWhitespace,
		strlen(strWithoutWhitespace),
		outExpression,
		pDictValues,
		pDictGlobals);

	/*
	 * not necessary to do so, but set the expression to NULL
	 */
	if (err != SUCCESS)
		*outExpression = NULL;

	SAFE_DELETE_ARRAY(strWithoutWhitespace);    /* reclaim memory */

	return err;
}





bool Expression::MatchNumber(char* inString, int inLength)
{
	int i;

	if (isdigit(inString[0])
		|| inString[0] == '-'
		|| inString[0] == '+'
		|| inString[0] == '.')
	{
		for (i = 1; i < inLength; i++)
		{
			if (!isdigit(inString[i]) && inString[i] != '.')
				return false;
		}

		return true;
	}
	else
		return false;
}

bool Expression::MatchIdentifier(char* inString, int inLength)
{
	int i;

	if (isalpha(inString[0]) || inString[0] == '_')
	{
		for (i = 1; i < inLength; i++)
			if (!isalnum(inString[i]) && inString[i] != '_')
				return false;

		return true;
	}
	else
		return false;
}

/*bool Expression::MatchIdentifier(char *inString, int inLength)
{
	int i;

	if( isalpha(inString[0]) )
	{
		for( i = 1; i < inLength; i++ )
			if( !isalnum(inString[i]) )
				return false;

		return true;
	}
	else
		return false;
}/**/

bool Expression::MatchOperator(char* inString, char* inOpString, int inLength)
{
	/*
	 * case insensitive
	 */
	return (0 == _strnicmp(inString, inOpString, inLength));
}


/*
 * Multi-character operations may not contain symbols (just alphanumerics)
 *    [I really should allow these things]
 *
 * Single-character operations may not consist of alphanumerics
 */
bool Expression::AtBeginningOfToken(char* inString, char* position)
{
	if (
		position <= inString    /* if we're at the beginning
								 * of the string, this had
								 * BETTER be the beginning
								 * of the operation
								 */
		||
		position[-1] == ')'     /* an RPAREN cannot be part
								 * of another token
								 */
		||

		/*! isalnum(position[0])/* if not alpha it's a
								 * single character operation
								 */
		(
			!isalnum(position[0])    /* special symbol */
			&&
			(isalnum(position[-1])   /* not special symbol */
				|| position[-1] == '\''  /* symbol */
				)

			)
		||
		(
			isalnum(position[0])/* we're in the middle of an
								 * identifier
								 */
			&&
			!isalnum(position[-1])/* and the preceeding char
									* is not part of the
									* identifier
									*/
			)
		)/**/
		return true;
	else
		return false;
}




#define UNARY_OP 1
#define BINARY_OP 2
#define TERNARY_OP 3

#define UNARY(str, func)    {    (str), (New##func), UNARY_OP   }
#define BINARY(str, func)   {    (str), (New##func), BINARY_OP  }
#define TERNARY(str, func)  {    (str), (New##func), TERNARY_OP }

/****************************************************************************
 * ParsingLogic:
 *
 * pre:        none
 *
 * post:    should never return NULL, since the array is built at compile time
 * post:    returns a double NULL terminated array describing the parsing logic
 *            (you're at the end when you hit NULL twice in a row)
 *
 ****************************************************************************/
Expression::parsingLogic_t* Expression::ParsingLogic(void)
{
	/*
	 * The table that defines the order of operations
	 * Higher precedence is lower on the table
	 */
	static parsingLogic_t ops[] = {

		// https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
		/* Comma has the lowest precedence of all */
		BINARY(",", Comma),
		{NULL, },
//        TERNARY("?\0:", Conditional),                   {NULL, },
#ifdef REGULAR_EXPRESSION
		BINARY("|", Or),                                {NULL, },
		BINARY("&", And),                               {NULL, },
		UNARY("#", Star),                               {NULL, },
		{NULL, },
#endif

		// Relational (Equality)
		BINARY("==", Equality),
		BINARY("!=", NonEquality),
		{NULL, },

		// Relational (GT/LT/GTOE/LTOE are higher precedence than Equality)
		BINARY(">=", GreaterThanOrEqual), // Matching is greedy, therefore the longer match must appear first in the list (i.e. ">=" before ">")
		BINARY(">", GreaterThan),
		BINARY("<=", LessThanOrEqual),
		BINARY("<", LessThan),
		{NULL, },

		// Additive
		BINARY("+", Add),
		BINARY("-", Sub),
		{NULL, },

		// Multiplicative
		BINARY("*", Mult),
		BINARY("/", Div),
		BINARY("%", Mod),
		{NULL, },

		// Exponentiation
		BINARY("^", Power),
		{NULL, },

		/* the parser now checks to make sure the beginning of the token is found,
		 * so the order of tan vs. arctan doesn't matter...
		 * HOWEVER the parser matches whatever it finds first, so if sqr is placed
		 * before sqrt, then sqrt will never be used */
		UNARY("sqrt",  Sqrt),
		UNARY("sqr",   Sqr),
		UNARY("log10", Log10),
		UNARY("log",   Ln),
		UNARY("exp",   Exp),
		UNARY("fact",  Factorial),
		UNARY("cosh",  Cosh),
		UNARY("sinh",  Sinh),
		UNARY("tanh",  Tanh),
		UNARY("acosh", ArcCosh),
		UNARY("asinh", ArcSinh),
		UNARY("atanh", ArcTanh),
		UNARY("cos",   Cos),
		UNARY("sin",   Sin),
		UNARY("tan",   Tan),
		UNARY("acos",  ArcCos),
		UNARY("asin",  ArcSin),
		UNARY("atan",  ArcTan),
		{NULL, },

		/* NULL terminator */

		{NULL, },
		/*
		 * just doing a NULL, instead of {NULL, } causes:
		 * error C2676: binary '==' : 'struct Expression::opStruct'
		 * does not define this operator or a conversion to a type
		 * acceptable to the predefined operator
		 */
	};

	return ops;
}


/****************************************************************************
 * Compile:
 *
 *    The core of the expression parser... the thing that makes it go...
 ****************************************************************************/


 /*
  *pre:    inString is NOT NULL,
  *pre:    inString is NOT a constant
  *pre:    inString is nul terminated
  *pre:    inString has whitespace removed
  *pre:    inMyDictionary is not NULL
  *pre:    inLength contains the length of inString
  *pre:    you don't care what happens to characters inString[0..inLength]
  *
  *post:    if SUCCESS is returned, *outExpression points to a valid Expression
  *post:    inString is destroyed    (should be freed, if you allocated it)
  */
error_t Expression::Compile(char* inString,
	const int inLength,
	Expression** outExpression,
	MyDictionary<value_t*>* inValues,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{
	static parsingLogic_t* orderOfOperations = ParsingLogic();
	/*
	 * holds the parsing logic
	 */
	parsingLogic_t* currentOperation;   /* points to the current place in
										 * the parsing logic, so it doesn't
										 * have to keep being dereferenced,
										 * and mainly to improve readability
										 */
	int operationLength;                /* length of current operation string
										 */



	char* position;         /* current position while parsing inString
							 */
	int nPrecedence;        /* index of first operation in current precedence
							 */
	int nOperation;         /* index of current operator in current precedence
							 */
	int nParenLevel;        /* number of nested parens at current position
							 */



	Expression* term1 = NULL;   /* temporary holder for the first operand
								 */
	error_t err1 = SUCCESS;     /* error value associated with creating *term1
								 */
	char* strTerm1 = nullptr;   /* points to the first character in the string
								 * to parse for the first operand subexpression
								 */
	Expression* term2 = NULL;   /* temporary holder for the second operand
								 */
	error_t err2 = SUCCESS;     /* error value associated with creating *term2
								 */
	char* strTerm2 = nullptr;   /* points to the first character in the string
								 * to parse for the second operand subexpression
								 */




								 /*
								  * hmmm, this is returning for some reason
								  */
								  /*
								   * obligatory NULL check
								   */
	if (inString == NULL || inLength <= 0)
		return FAILURE;



	/*
	 * See if entire string is a number
	 */
	if (MatchNumber(inString, inLength))
	{
		return NewConstant((value_t)atof(inString), outExpression);

	}
	else
		/*
		 * see if it's an identifier
		 */
		if (MatchIdentifier(inString, inLength))
		{
			char* strID = inString;
			EXPRESSIONDESCRIPTION* pED;
			value_t* pValue;    /* pointer to the variable associated with var
								 */

			while (true)
			{
				if ((pValue = inValues->GetValue(strID)) != NULL)
					return NewVariable(inString, pValue, outExpression);

				if ((pED = inGlobals->GetValue(strID)) != NULL)
					switch (pED->edtType)
					{
					default:
					case ED_NULL:
						return ERR_NOTFOUND;

					case ED_CONSTANT:
						return NewConstant(pED->value, outExpression);

					case ED_VARIABLE:
						return NewVariable(pED->strName, pED->pointer.variable, outExpression);

					}

				/*
				 * break jumps here
				 */

				break; /* this break leaves the while loop */
			}

			return ERR_NOTFOUND;
		}


	/*
	 * operator & expression cannot fit into a single character
	 */
	if (inLength == 1)
		return ERR_COMPILE;


	/*
	 * to get here means inString is neither a number nor a variable
	 * therefore it's necessary to attempt to break it down into an expression
	 */




	 /*    while( orderOfOperations[nPrecedence = ++nOperation].opString != NULL )
	  */

	  /*
	   * Precedence loop
	   */
	for (nParenLevel = 0,
		nOperation = 0,
		nPrecedence = 0;

		orderOfOperations[nPrecedence].opString != NULL;

		nPrecedence = ++nOperation
		)
	{

		/*
		 * Reverse traversal loop
		 */
		for (position = &inString[inLength - 1];
			position >= inString;
			position--)
		{
			switch (*position)
			{
			case ')':
				nParenLevel++;
				break;
			case '(':
				nParenLevel--;
				break;
			default:
				if (nParenLevel == 0 && AtBeginningOfToken(inString, position))
				{
					/*
					 * Operation loop
					 */
					for (nOperation = nPrecedence,
						currentOperation = &orderOfOperations[nOperation];

						currentOperation->opString != NULL;

						currentOperation = &orderOfOperations[++nOperation]
						)
					{
						/*
						 * opString isn't NULL, so it's safe
						 */
						operationLength = strlen(currentOperation->opString);


						if (MatchOperator(position, currentOperation->opString, operationLength))
						{
							switch (currentOperation->opType)
							{
							case UNARY_OP:
								/* pointer to the character after the unary operation */
								strTerm1 = position + operationLength;
								if ((err1 = Compile(strTerm1, strlen(strTerm1), &term1, inValues, inGlobals)) == SUCCESS)
									return currentOperation->newExpression(term1, NULL, outExpression);
								else
									return err1;

							case BINARY_OP: /* bad things happen when the unary - is matched
											 * except the unary - isn't currently in the expression...
											 */
								*position = '\0';   /* split the string in
													 * to two substrings.
													 * one for each of the
													 * subexpressions to be
													 * compiled
													 */

								strTerm1 = inString;
								strTerm2 = position + operationLength;

								/*
								 * The change here is to add unary
								 * negation, without hardcoding the
								 * unary negation string into the code
								 */
								 /* if( (err1 = Compile( strTerm1, strlen(strTerm1), inMyDictionary, &term1 )) == SUCCESS &&
								  *    (err2 = Compile( strTerm2, strlen(strTerm2), inMyDictionary, &term2 )) == SUCCESS )
								  */
								if (((inString == position && (err1 = NewConstant(0.0f, &term1)) == SUCCESS) ||
									(err1 = Compile(strTerm1, strlen(strTerm1), &term1, inValues, inGlobals)) == SUCCESS) &&
									(err2 = Compile(strTerm2, strlen(strTerm2), &term2, inValues, inGlobals)) == SUCCESS)
								{
									return currentOperation->newExpression(term1, term2, outExpression);
								}
								else
									return max(err1, err2);

								return currentOperation->newExpression(term1, term2, outExpression);

							default:
								return ERR_COMPILE;
							}    /* switch */
						}    /* if */
					}    /* for Operation loop */
				}    /* if */
				break;
			} /* switch */
		}    /* for Reverse traversal loop */
	}    /* for Precedence loop */




#ifdef UNDEFINED
	if (MatchIdentifier(inString, inLength))
	{
		WORD                  nSanityCheck = 0;
		char* strID = inString;
		EXPRESSIONDESCRIPTION* pED;
		value_t* pValue;    /* pointer to the variable associated with var
							 */

		while (true)
		{
			if ((pValue = inValues->GetValue(strID)) != NULL)
				return NewVariable(inString, pValue, outExpression);

			if ((pED = inGlobals->GetValue(strID)) != NULL)
				switch (pED->edtType)
				{
				default:
				case ED_NULL:
					return ERR_NOTFOUND;

				case ED_CONSTANT:
					return NewConstant(pED->value, outExpression);

				case ED_VARIABLE:
					return NewVariable(pED->strName, pED->pointer.variable, outExpression);

				case ED_ALIAS:
					if (nSanityCheck++ > 1000)
						break;

					strID = pED->strName;
					continue; /* loop back up to the while */
				}

			/*
			 * break jumps here
			 */

			break; /* this break leaves the while loop */
		}

		return ERR_NOTFOUND;
	}
#endif


#ifndef UNDEFINED
	{
		EXPRESSIONDESCRIPTION* pExpDesc;
		if ((strTerm1 = strchr(inString, '(')) != NULL)
		{
			*strTerm1 = '\0';

			if ((pExpDesc = inGlobals->GetValue(inString)) != NULL)
			{
				*strTerm1 = '(';
				if ((err1 = Compile(strTerm1, strlen(strTerm1), &term1, inValues, inGlobals)) == SUCCESS)
				{
					*strTerm1 = '\0';
					/* should free term1 on error */
					return NewUserDefined(pExpDesc, term1, outExpression);

				}
				//                else
				//                    return err;  /* no, don't return err, let the below code try */


			}

			*strTerm1 = '(';
		}
	}
#endif


	/*
	 * if nothing was matched,    (ie: we got here)
	 * has parens around it,    (check the parens)
	 * and it's safe to remove those parens,    (test safeToRemoveParens)
	 * remove em and try again
	 */
	if (inString[0] == '(' && inString[inLength - 1] == ')')
	{
		/*
		 * erase the closing paren & pretend opening paren isn't there
		 */
		inString[inLength - 1] = '\0';
		strTerm1 = &inString[1];

		return Compile(strTerm1, inLength - 2, outExpression, inValues, inGlobals);
	};

#ifdef REGULAR_EXPRESSION
	if (inString[0] == '\'' && inString[inLength - 1] == '\'')
	{
		/*
		 * erase the closing paren & pretend opening paren isn't there
		 */
		 //        DumpToFile("testing something.txt", inString, "\n");
		 //        inString[ inLength - 1 ] = '\0';
		 //        strTerm1 = &inString[1];
		return NewSymbol(inString[1], outExpression);
	};
#endif




	return ERR_COMPILE;    /* couldn't parse it */

}


