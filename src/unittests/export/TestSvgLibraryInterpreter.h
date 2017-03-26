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
* File Name : TestSvgLibraryInterpreter.h
* Creation Date : 15-04-2015
* Last Modified : Mo 20 Apr 2015 20:21:49 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSVGLIBRARYINTERPRETER_
#define _TESTSVGLIBRARYINTERPRETER_
#include <cxxtest/TestSuite.h>

class TestSvgLibraryInterpreter : public CxxTest::TestSuite
{
    public:
    void testCreation();
    void testFindAttribute();
    void testUnfindableTagId();
    void testUngetableTagId();
    void testGetValidTagIdSimple();
    void testGetValidTagWithPlace();
};
#endif /* _TESTSVGLIBRARYINTERPRETER_ */
