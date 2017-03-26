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
* File Name : TestCmp_str.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:52:44 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "../../misc/cmp_str.h"
#include "TestCmp_str.h"

void TestCmp_Str::TestCmpStr()
{
    misc::cmp_str CmpStr;
    TS_ASSERT_EQUALS(CmpStr("EinString", "EinString"), false);
    TS_ASSERT(CmpStr("EinString", "EinSxring"));
    TS_ASSERT_EQUALS(CmpStr("EinString", "EinStrang"), false);

    misc::equal_str EqStr;
    TS_ASSERT(EqStr("EinString", "EinString"));
    TS_ASSERT(EqStr("EinString", "eINsTRING"));
    TS_ASSERT_EQUALS(EqStr("EinString", "INsTRIN"), false);
}


