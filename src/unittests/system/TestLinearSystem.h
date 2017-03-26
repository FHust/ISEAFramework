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
/*
 * TestLinearSystem.h
 *
 *  Created on: 04.12.2013
 *      Author: chris
 */

#ifndef TESTLINEARSYSTEM_H_
#define TESTLINEARSYSTEM_H_

#include <cxxtest/TestSuite.h>

class TestLinearSystem : public CxxTest::TestSuite
{
    public:
    void testParallelRCAlgElement();
    void testSingleAlgCellellement();

    private:
    std::vector< std::vector< double > > CopyToVector( const double data[7][4] );
};

#endif /* TESTLINEARSYSTEM_H_ */
