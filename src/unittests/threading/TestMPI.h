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
* File Name : TestMPI.h
* Creation Date : 13-11-2013
* Last Modified : Fr 21 Feb 2014 15:07:35 CET
* Created By : chris
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMPITHREADING_
#define _TESTMPITHREADING_

#include <cxxtest/TestSuite.h>

#ifdef BOOST_MPI
BOOST_SERIALIZATION_SHARED_PTR( TaskData )
BOOST_CLASS_EXPORT( TaskData )

BOOST_SERIALIZATION_SHARED_PTR( threading::daetask::DAEInputData< myMatrixType > )
BOOST_CLASS_EXPORT( threading::daetask::DAEInputData< myMatrixType > )

BOOST_SERIALIZATION_SHARED_PTR( threading::daetask::DAEOutputData< myMatrixType > )
BOOST_CLASS_EXPORT( threading::daetask::DAEOutputData< myMatrixType > )
#endif    // BOOST_MPI

class TestMPI : public CxxTest::TestSuite
{
    public:
    void testMPITestParallelResistors();
};


#endif /* _TESTMPITHREADING_ */
