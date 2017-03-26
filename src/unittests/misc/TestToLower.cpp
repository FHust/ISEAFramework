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
* File Name : TestToLower.cpp
* Creation Date : 21-02-2014
* Last Modified : Do 27 Feb 2014 15:11:36 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestToLower.h"
#include "../../misc/toLower.h"
#include <cstring>

void TestToLower::TestToLow()
{
    const char *testCharArray = "DAS_IST_EIN_TEST";
    TS_ASSERT( !strcmp( misc::ToLower(testCharArray).get(), "das_ist_ein_test" ) );
}


