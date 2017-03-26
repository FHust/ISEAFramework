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
* File Name : TestParser.h
* Creation Date : 20-01-2015
* Last Modified : Do 26 Mär 2015 11:55:35 CET
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTPARSER_
#define _TESTPARSER_
#include <cxxtest/TestSuite.h>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "../../xmlparser/xmlparameter.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

/// Test the Parser implementation of GetElement....
class TestParser : public CxxTest::TestSuite
{
    public:
    void TestGetElementUnsignedIntVecValue();
    void TestGetElementDoubleVecValue();
    void TestGetElementDoubleVecVecValue();
};
#endif /* _TESTPARSER_ */
