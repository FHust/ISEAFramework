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
* File Name : TestSlice.cpp
* Creation Date : 08-04-2015
* Last Modified : Mi 15 Apr 2015 11:33:58 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestSlice.h"
#include "../../misc/slice.h"

template < typename T >
void TestVariousNumericTypes()
{
    const size_t sizeOfArray = 10;
    T x[sizeOfArray];
    std::vector< T * > vec;
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        x[i] = i;
        vec.push_back( &x[i] );
    }
    SliceView< T > xSlice( vec );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        TS_ASSERT_EQUALS( xSlice[i], x[i] );
    }
}


void TestSlice::TestSliceCreation()
{
    TestVariousNumericTypes< double >();
    TestVariousNumericTypes< float >();
    TestVariousNumericTypes< int >();
    TestVariousNumericTypes< size_t >();
    TestVariousNumericTypes< char >();
}

void TestSlice::TestSliceEditing()
{
    const size_t sizeOfArray = 10;
    double x[sizeOfArray];
    std::vector< double * > vec;
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        x[i] = i;
        vec.push_back( &x[i] );
    }
    SliceView< double > slice( vec );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        slice[sizeOfArray - i - 1] = i;
    }

    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        TS_ASSERT_EQUALS( x[i], slice[i] );
        TS_ASSERT_EQUALS( slice[i], sizeOfArray - i - 1 );
    }
}
