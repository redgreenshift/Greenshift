#include "pch.h"
#include "..\Project Greenshift.h"
#include "CppUnitTest.h"
#include "..\Expression.h"
#include "..\Expression.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(ExpressionUT)
	{
		value_t const defaultTolerance = 0.0001f; // common default float tolerance; accounts for typical rounding errors, unless greater precision is required

	public:

		/********************************************************************
		* ExpressionValue
		*/
		TEST_METHOD(TestExpressionConstant)
		{
			error_t err;
			std::string const original = "3.14159";
			std::string const& expected = original;
			value_t const fExpected = 3.14159f;

			MyDictionary<value_t*> dictEmpty;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dictEmpty, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionVariable)
		{
			error_t err;
			value_t x = 5.4f;
			std::string const original = "x";
			std::string const& expected = original;
			value_t const fExpected = 5.4f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}
			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		/********************************************************************
		* ExpressionAdditive
		*/
		TEST_METHOD(TestExpressionAdd)
		{
			error_t err;
			value_t x = 0.2f, y = 1.0f;
			std::string const original = "x+y";
			std::string const expected = "(x+y)";
			value_t const fExpected = 1.2f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionSub)
		{
			error_t err;
			value_t x = 0.2f, y = 1.0f;
			std::string const original = "x-y";
			std::string const expected = "(x-y)";
			value_t const fExpected = -0.8f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		/********************************************************************
		* ExpressionMultiplicative
		*/
		TEST_METHOD(TestExpressionMult)
		{
			error_t err;
			value_t x = 6, y = 7;
			std::string const original = "x*y";
			std::string const expected = "(x*y)";
			value_t const fExpected = 42;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionDiv)
		{
			error_t err;
			value_t x = 6, y = 7;
			std::string const original = "x/y";
			std::string const expected = "(x/y)";
			value_t const fExpected = 0.8571428571f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		/********************************************************************
		* ExpressionExponential
		*/
		TEST_METHOD(TestExpressionPower)
		{
			error_t err;
			value_t x = 6, y = 7;
			std::string const original = "x^y";
			std::string const expected = "(x^y)";
			value_t const fExpected = 279936;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		/********************************************************************
		* ExpressionTrascendental
		*/
		TEST_METHOD(TestExpressionSquare)
		{
			error_t err;
			value_t x = 6;
			std::string const original = "sqr(x)";
			std::string const expected = original;
			value_t const fExpected = 36;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionSqrt)
		{
			error_t err;
			value_t x = 42;
			std::string const original = "sqrt(x)";
			std::string const expected = original;
			value_t const fExpected = 6.480740698407860230965967436088f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionLog10)
		{
			error_t err;
			value_t x = 1006;
			std::string const original = "log10(x)";
			std::string const expected = original;
			value_t const fExpected = 3.002597980719908592311962985004f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionNaturalLogarithm)
		{
			error_t err;
			const value_t e = 2.71828182845904523536f;
			value_t x = 6001;
			std::string const original = "log(x)";
			std::string const expected = original;
			value_t const fExpected = 8.6996814009895128476452547350611f; // ln(6001)

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionExp)
		{
			error_t err;
			const value_t e = 2.71828182845904523536f;
			value_t x = 4.2f;
			std::string const original = "exp(x)";
			std::string const expected = original;
			value_t const fExpected = 66.686331040925141644992114575504f; // e ^ 4.2

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		/********************************************************************
		* ExpressionTrigonometric
		*/
		TEST_METHOD(TestExpressionSin)
		{
			error_t err;
			value_t x = 4.2f;
			std::string const original = "sin(x)";
			std::string const expected = original;
			value_t const fExpected = -0.871576f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionCos)
		{
			error_t err;
			value_t x = 4.2f;
			std::string const original = "cos(x)";
			std::string const expected = original;
			value_t const fExpected = -0.490261f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionTan)
		{
			error_t err;
			value_t x = 4.2f;
			std::string const original = "tan(x)";
			std::string const expected = original;
			value_t const fExpected = 1.77778f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionArcSin)
		{
			error_t err;
			value_t x = 0.2f;
			std::string const original = "asin(x)";
			std::string const expected = original;
			value_t const fExpected = 0.201358f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionArcCos)
		{
			error_t err;
			value_t x = 0.2f;
			std::string const original = "acos(x)";
			std::string const expected = original;
			value_t const fExpected = 1.36944f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionArcTan)
		{
			error_t err;
			value_t x = 4.2f;
			std::string const original = "atan(x)";
			std::string const expected = original;
			value_t const fExpected = 1.33705f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		/********************************************************************
		* ExpressionHyperbolic
		*/

		TEST_METHOD(TestExpressionSinh)
		{
			error_t err;
			value_t x = 4.2f;
			std::string const original = "sinh(x)";
			std::string const expected = original;
			value_t const fExpected = 33.3357f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionCosh)
		{
			error_t err;
			value_t x = 4.2f;
			std::string const original = "cosh(x)";
			std::string const expected = original;
			value_t const fExpected = 33.3507f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionTanh)
		{
			error_t err;
			value_t x = 4.2f;
			std::string const original = "tanh(x)";
			std::string const expected = original;
			value_t const fExpected = 0.99955f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionArcSinh)
		{
			error_t err;
			value_t x = 0.2f;
			std::string const original = "asinh(x)";
			std::string const expected = original;
			value_t const fExpected = 0.19869f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionArcCosh)
		{
			error_t err;
			value_t x = 3.762196f;
			std::string const original = "acosh(x)";
			std::string const expected = original;
			value_t const fExpected = 2;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionArcTanh)
		{
			error_t err;
			value_t x = 0.761594f;
			std::string const original = "atanh(x)";
			std::string const expected = original;
			value_t const fExpected = 1.0f;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}


		/********************************************************************
		* ExpressionRelational
		*/

		TEST_METHOD(TestExpressionEqualityTrue)
		{
			error_t err;
			std::string const original = "1 == 1";
			std::string const expected = "(1==1)";
			value_t const fExpected = 1;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionEqualityFalse)
		{
			error_t err;
			std::string const original = "1 == 2";
			std::string const expected = "(1==2)";
			value_t const fExpected = 0;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionNonEqualityTrue)
		{
			error_t err;
			std::string const original = "1 != 2";
			std::string const expected = "(1!=2)";
			value_t const fExpected = 1;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionNonEqualityFalse)
		{
			error_t err;
			std::string const original = "1 != 1";
			std::string const expected = "(1!=1)";
			value_t const fExpected = 0;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionGreaterThanTrue)
		{
			error_t err;
			std::string const original = "2 > 1";
			std::string const expected = "(2>1)";
			value_t const fExpected = 1;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionGreaterThanFalse)
		{
			error_t err;
			std::string const original = "1 > 2";
			std::string const expected = "(1>2)";
			value_t const fExpected = 0;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionGreaterThanOrEqualTrue1)
		{
			error_t err;
			std::string const original = "2 >= 1";
			std::string const expected = "(2>=1)";
			value_t const fExpected = 1;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionGreaterThanOrEqualTrue2)
		{
			error_t err;
			std::string const original = "2 >= 2";
			std::string const expected = "(2>=2)";
			value_t const fExpected = 1;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionGreaterThanOrEqualFalse)
		{
			error_t err;
			std::string const original = "1 >= 2";
			std::string const expected = "(1>=2)";
			value_t const fExpected = 0;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionLessThanTrue)
		{
			error_t err;
			std::string const original = "1 < 2";
			std::string const expected = "(1<2)";
			value_t const fExpected = 1;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionLessThanFalse)
		{
			error_t err;
			std::string const original = "2 < 1";
			std::string const expected = "(2<1)";
			value_t const fExpected = 0;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionLessThanOrEqualTrue1)
		{
			error_t err;
			std::string const original = "1 <= 2";
			std::string const expected = "(1<=2)";
			value_t const fExpected = 1;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionLessThanOrEqualTrue2)
		{
			error_t err;
			std::string const original = "2 <= 2";
			std::string const expected = "(2<=2)";
			value_t const fExpected = 1;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionLessThanOrEqualFalse)
		{
			error_t err;
			std::string const original = "2 <= 1";
			std::string const expected = "(2<=1)";
			value_t const fExpected = 0;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, defaultTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

	};
}
