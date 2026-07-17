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
#include "Project Greenshift.h"
#include "Expression.h"
#include <float.h>
#define printError(num)        printf("%s\n", ErrorString(num)) // TODO: JRDV: I don't understand where errorStrings[num] came from... ErrorString(num) is what I expected
#include "MyDictionary.h"


int mainHack(void)
{
    return 0;
}

#if 1 // JRDV: I think this is old code.... but I'm unsure.  It *has* been almost 20 years!  Not old code.  this is how I tested the code before building the DLL for WinAmp

static union {
    int integer;
    float decimal;
};

int main(void)
{
    error_t err;
    value_t x = 0.2, y = 1.0, somevalue;
    MyDictionary<value_t*> dict; // ("x", &x, "y", &y);  // JRDV: I have NO IDEA how this could have possibly compiled earlier...
    Expression *myExpression;
//    Expression *myOtherExpression;

    dict.SetValue("x", &x);
    dict.SetValue("y", &y);

//    "x*(0-y)*arcSin(0-((1^2*(3-(4+5)))/6))"

    printf("JRDV\n");

//    if ((err = Expression::Compile("x+y", &myExpression, &dict, nullptr/*globals*/)) == SUCCESS)
    if ((err = Expression::Compile("x*-y*arcsin(1^2*(3-(4+5))/6", &myExpression, &dict, nullptr/*globals*/)) == SUCCESS)
//    if( (err = Expression::Compile("x*-y*arcsin(1^2*(3-(4+5))/6)", &dict, &myExpression)) == SUCCESS)
//    if( (err = Expression::Compile("x*-y*arcsin(-1^2*3-(4+5)/6)", &dict, &myExpression)) == SUCCESS)
//    if( (err = Expression::Parse("x*y*arcsin(1^2*(3-(4+5))/6)", &dict, &myExpression)) == SUCCESS)
    {    
        
        printf("%s\n", myExpression->PrintString());

//        exit(0);

        for(x=0.0; x<10.0; x+=1.0)
        {
            y = x;
            printf("%f\n", myExpression->Evaluate());

            printf("size: %d\tdepth: %d\n", myExpression->Size(), myExpression->Depth() );

//            if( myExpression->PartialSimplification(&x, &myOtherExpression) == SUCCESS )
//                printf("\t\t%s\n", myOtherExpression->PrintString() );

//            printf("%s\n", _isnan(myExpression->Evaluate()) ? "is NaN" : "is a number" );

//            printf("%d\n", (int)myExpression->Evaluate() );
        }

        printf("%s\n", myExpression->PrintString());
        printf("%s\n", myExpression->PrintStream());

        delete myExpression;
    }
//    else
 //       printError(err);


    integer = 0;

    printf("%f\n", decimal);


    printf("formatted number: %g\n", 123.0);


    return 0;
}

#ifdef UNDEFINED
int main(void)
{
    error_t err;
    value_t x = 0.0, y = 0.0, somevalue;
    Dictionary dict("x", &x, "y", &y);
    Expression *myExpression;
//    Expression *myOtherExpression;

//    "x*(0-y)*arcSin(0-((1^2*(3-(4+5)))/6))"

    if( (err = Expression::Compile("x*-y*arcsin(1^2*(3-(4+5))/6)", &dict, &myExpression)) == SUCCESS)
//    if( (err = Expression::Compile("x*-y*arcsin(-1^2*3-(4+5)/6)", &dict, &myExpression)) == SUCCESS)
//    if( (err = Expression::Parse("x*y*arcsin(1^2*(3-(4+5))/6)", &dict, &myExpression)) == SUCCESS)
    {    
        
        printf("%s\n", myExpression->PrintString());

//        exit(0);

        for(x=0.0; x<10.0; x+=1.0)
        {
            y = x;
            printf("%f\n", myExpression->Evaluate());

            printf("size: %d\tdepth: %d\n", myExpression->Size(), myExpression->Depth() );

//            if( myExpression->PartialSimplification(&x, &myOtherExpression) == SUCCESS )
//                printf("\t\t%s\n", myOtherExpression->PrintString() );

//            printf("%s\n", _isnan(myExpression->Evaluate()) ? "is NaN" : "is a number" );

//            printf("%d\n", (int)myExpression->Evaluate() );
        }

        printf("%s\n", myExpression->PrintString());

        delete myExpression;
    }
    else
        printError(err);


    integer = 0;

    printf("%f\n", decimal);


    printf("formatted number: %g\n", 123.0);


    return 0;
}

#endif
