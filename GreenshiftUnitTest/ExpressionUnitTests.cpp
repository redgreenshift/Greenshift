#include "pch.h"
#include "CppUnitTest.h"
#include "..\Expression.h"
#include "..\Expression.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(ExpressionUT)
	{
	public:
		TEST_METHOD(TestExpressionConstant)
		{
			error_t err;
			std::string const source = "3.14159";
			std::string const& strExpected = source;
			value_t fExpected = 3.14159;

			MyDictionary<value_t*> dictEmpty;
			Expression* myExpression;

			if ((err = Expression::Compile(source.c_str(), &myExpression, &dictEmpty, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = myExpression->PrintString();
				Assert::AreEqual(strExpected.c_str(), pszResult);

				value_t fResult = myExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult);
			}

			delete myExpression;
		}

		TEST_METHOD(TestExpressionAdd)
		{
			error_t err;
			value_t x = 0.2, y = 1.0;
			std::string const source = "x+y";
			std::string const expected = "(x+y)";
			value_t fExpected = 1.2;

			MyDictionary<value_t*> dict;
			Expression* myExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(source.c_str(), &myExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* result = myExpression->PrintString();
				Assert::AreEqual(expected.c_str(), result);

				const value_t fResult = myExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult);
			}

			delete myExpression;
		}

		TEST_METHOD(TestExpressionMult)
		{
			error_t err;
			value_t x = 6, y = 7;
			std::string const source = "x*y";
			std::string const expected = "(x*y)";
			value_t fExpected = 42;

			MyDictionary<value_t*> dict;
			Expression* myExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(source.c_str(), &myExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* result = myExpression->PrintString();
				Assert::AreEqual(expected.c_str(), result);

				const value_t fResult = myExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult);
			}

			delete myExpression;
		}
	};
}
