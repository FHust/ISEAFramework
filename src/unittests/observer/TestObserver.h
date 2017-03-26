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
* File Name : ..TestObserver.cpp
* Creation Date : 18-11-2012
* Last Modified : Mi 13 Aug 2014 09:56:09 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include <cxxtest/TestSuite.h>
#include <vector>

class TestObserver : public CxxTest::TestSuite
{

    public:
    void testObserverOperationsSingleCell();

    private:
    std::vector< std::vector< double > > CopyToVector( const double data[7][4] );
};
