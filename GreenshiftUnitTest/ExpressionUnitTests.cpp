#include "pch.h"
#include "CppUnitTest.h"
#include "..\Expression.h"
#include "..\Expression.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(ExpressionUnitTest)
	{
	public:

		TEST_METHOD(TestExpressionAdd1)
		{
			error_t err;
			value_t x = 0.2, y = 1.0;
			std::string const expression = "x+y";
			std::string const expected = "(x+y)";
			value_t nExpected = 1.2;

			MyDictionary<value_t*> dict;
			Expression* myExpression;

			dict.SetValue("x", &x);
			dict.SetValue("y", &y);

			if ((err = Expression::Compile(expression.c_str(), &myExpression, &dict, nullptr/*globals*/)) == SUCCESS)
			{

				char* result = myExpression->PrintString();
				Assert::AreEqual(expected.c_str(), result);

				value_t     nReturnValue = myExpression->Evaluate();
				Assert::AreEqual(nExpected, nReturnValue);
			}

			delete myExpression;
		}
	};
}
