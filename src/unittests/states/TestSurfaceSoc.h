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
* File Name : TestSurfaceSoc.h
* Creation Date : 30-06-2014
* Last Modified : Mo 07 Jul 2014 11:14:59 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSURFACESOC_
#define _TESTSURFACESOC_

#include <cxxtest/TestSuite.h>
class TestSurfaceSoc : public CxxTest::TestSuite
{
    public:
    void testCreation();
    void testCreation1DLookup();
};
#endif /* _TESTSURFACESOC_ */
