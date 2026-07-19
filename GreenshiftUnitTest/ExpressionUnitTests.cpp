#include "pch.h"
#include "CppUnitTest.h"
#include "..\Expression.h"
#include "..\Expression.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(ExpressionUT)
	{
		value_t const floatTolerance = 0.00001f;

	public:
		TEST_METHOD(TestExpressionConstant)
		{
			error_t err;
			std::string const original = "3.14159";
			std::string const& expected = original;
			value_t const fExpected = 3.14159;

			MyDictionary<value_t*> dictEmpty;
			std::shared_ptr<Expression> spExpression;

			if ((err = Expression::Compile(original.c_str(), spExpression, &dictEmpty, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, floatTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionVariable)
		{
			error_t err;
			value_t x = 5.4;
			std::string const original = "x";
			std::string const& expected = original;
			value_t const fExpected = 5.4;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, floatTolerance, L"Evaluate failed");
			}
			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionAdd)
		{
			error_t err;
			value_t x = 0.2, y = 1.0;
			std::string const original = "x+y";
			std::string const expected = "(x+y)";
			value_t const fExpected = 1.2;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, floatTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

		TEST_METHOD(TestExpressionSub)
		{
			error_t err;
			value_t x = 0.2, y = 1.0;
			std::string const original = "x-y";
			std::string const expected = "(x-y)";
			value_t const fExpected = -0.8;

			MyDictionary<value_t*> dict;
			std::shared_ptr<Expression> spExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(original.c_str(), spExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{
				const char* pszResult = spExpression->PrintString();
				Assert::AreEqual(expected.c_str(), pszResult, L"PrintString failed");

				const value_t fResult = spExpression->Evaluate();
				Assert::AreEqual(fExpected, fResult, floatTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

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
				Assert::AreEqual(fExpected, fResult, floatTolerance, L"Evaluate failed");
			}

			Assert::IsTrue(err == SUCCESS, L"Compile failed");
		}

	};
}
