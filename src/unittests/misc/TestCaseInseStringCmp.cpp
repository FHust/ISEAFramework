/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/
/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestCaseInseStringCmp.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:39:59 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestCaseInseStringCmp.h"

#include <cstring>
#include "../../misc/charArrayCmp.h"

void TestCaseInseStringCmp::TestStringCompare()
{
    const char* testString = "Das Ist EIn Brot";
    const char* testStringCorrect = "DAS IST EIN BROT";
    const char* testABig = "Ausw";
    const char* testASmall = "ausw";
    const char* testFBig = "Fusw";
    const char* testFSmall = "fusw";

    TS_ASSERT_EQUALS(misc::CaseInsensitiveStringCompare(testString, testStringCorrect), 0 );

    TS_ASSERT(misc::CaseInsensitiveStringCompare(testString, testABig) > 0 );
    TS_ASSERT(misc::CaseInsensitiveStringCompare(testString, testASmall) > 0);

    TS_ASSERT(misc::CaseInsensitiveStringCompare(testString, testFBig) < 0 );
    TS_ASSERT(misc::CaseInsensitiveStringCompare(testString, testFSmall) < 0);
}

