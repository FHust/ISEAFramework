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
* File Name : TestView.cpp
* Creation Date : 09-04-2015
* Last Modified : Mi 14 Okt 2015 12:45:43 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestView.h"
#include "../../misc/view.h"
#include <boost/scoped_array.hpp>
#include <cstring>

template < typename T >
void TestVariousNumericTypes()
{
    const size_t sizeOfArray = 10;
    std::vector< std::vector< T > > denseArray( sizeOfArray, std::vector< T >() );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        denseArray.at( i ).resize( sizeOfArray );
        for ( size_t j = 0; j < sizeOfArray; ++j )
        {
            denseArray[i].at( j ) = j;
        }
    }

    View< T > view( denseArray, 0, 0, 5, 5 );
    for ( size_t i = 0; i < 5; ++i )
    {
        for ( size_t j = 0; j < 5; ++j )
        {
            TS_ASSERT_EQUALS( view[i][j], j );
        }
    }

    SliceView< T > slice( view[0] );

    for ( size_t i = 0; i < 5; ++i )
    {
        TS_ASSERT_EQUALS( slice[i], i );
    }
}

void TestView::TestViewCreation()
{
    TestVariousNumericTypes< double >();
    TestVariousNumericTypes< float >();
    TestVariousNumericTypes< int >();
    TestVariousNumericTypes< size_t >();
    TestVariousNumericTypes< char >();
}

void TestView::TestViewCreationFromArray()
{
    boost::scoped_array< double > arr( new double[8] );
    for ( size_t i = 0; i < 8; ++i )
    {
        arr[i] = static_cast< double >( i );
    }

    // double *arrayPointer = &arr[0];
    View< double > view( arr.get(), 2, 4, 0, 0, 3, 5 );
    size_t counter = 0;
    for ( size_t y = 0; y < 4; ++y )
    {
        for ( size_t x = 0; x < 2; ++x )
        {
            TS_ASSERT_DELTA( view[x][y], static_cast< double >( counter ), 0.0001 );
            ++counter;
        }
    }
}

void TestView::TestFill()
{
    const size_t sizeOfArray = 10;
    std::vector< std::vector< double > > denseArray( sizeOfArray, std::vector< double >() );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        denseArray.at( i ).resize( sizeOfArray );
        for ( size_t j = 0; j < sizeOfArray; ++j )
        {
            denseArray[i].at( j ) = j;
        }
    }

    TS_ASSERT_EQUALS( denseArray.size(), sizeOfArray );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        TS_ASSERT_EQUALS( denseArray.at( i ).size(), sizeOfArray );
    }

    View< double > view( denseArray, 0, 0, 5, 5 );
    const double FILLELEMENT = 10;
    // Change top left corner to 10s
    view.FillAll( FILLELEMENT );

    /// 10 10 10 10 10 5 6 7 8 9
    /// 10 10 10 10 10 5 6 7 8 9
    /// 10 10 10 10 10 5 6 7 8 9
    /// 10 10 10 10 10 5 6 7 8 9
    /// 10 10 10 10 10 5 6 7 8 9
    //  0  1  2  3  4  5 6 7 8 9
    //  0  1  2  3  4  5 6 7 8 9
    //  0  1  2  3  4  5 6 7 8 9
    //  0  1  2  3  4  5 6 7 8 9
    //  0  1  2  3  4  5 6 7 8 9
    //
    // Check if top left corner of the dense matrix is filled with 10s
    for ( size_t i = 0; i < 5; ++i )
    {
        for ( size_t j = 0; j < 5; ++j )
        {
            TS_ASSERT_EQUALS( denseArray[i][j], FILLELEMENT );
        }
    }

    // Check if top right
    for ( size_t i = 0; i < 10; ++i )
    {
        for ( size_t j = 5; j < 10; ++j )
        {
            TS_ASSERT_EQUALS( denseArray[i][j], j );
        }
    }

    for ( size_t i = 5; i < 10; ++i )
    {
        for ( size_t j = 0; j < 5; ++j )
        {
            TS_ASSERT_EQUALS( denseArray[i][j], j );
        }
    }

    View< double > viewB( denseArray, 5, 5, 10, 10 );
    viewB.FillAll( -1 );

    /// 10 10 10 10 10 5  6  7  8  9
    /// 10 10 10 10 10 5  6  7  8  9
    /// 10 10 10 10 10 5  6  7  8  9
    /// 10 10 10 10 10 5  6  7  8  9
    /// 10 10 10 10 10 5  6  7  8  9
    //  0  1  2  3  4 -1 -1 -1 -1 -1
    //  0  1  2  3  4 -1 -1 -1 -1 -1
    //  0  1  2  3  4 -1 -1 -1 -1 -1
    //  0  1  2  3  4 -1 -1 -1 -1 -1
    //  0  1  2  3  4 -1 -1 -1 -1 -1

    for ( size_t i = 5; i < 10; ++i )
    {
        for ( size_t j = 5; j < 10; ++j )
        {
            TS_ASSERT_EQUALS( denseArray[i][j], -1 );
        }
    }
}
void TestView::TestViewView()
{
    const size_t sizeOfArray = 10;
    std::vector< std::vector< double > > denseArray( sizeOfArray, std::vector< double >() );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        denseArray.at( i ).resize( sizeOfArray );
        for ( size_t j = 0; j < sizeOfArray; ++j )
        {
            denseArray[i].at( j ) = j;
        }
    }

    TS_ASSERT_EQUALS( denseArray.size(), sizeOfArray );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        TS_ASSERT_EQUALS( denseArray.at( i ).size(), sizeOfArray );
    }

    View< double > view( denseArray, 0, 0, 5, 5 );
    View< double > viewsSubview( view, 1, 1, 3, 3 );

    const double FILLELEMENT = 10;
    // Change top left corner to 10s
    viewsSubview.FillAll( FILLELEMENT );
    for ( size_t i = 1; i < 3; ++i )
    {
        for ( size_t j = 1; j < 3; ++j )
        {
            TS_ASSERT_EQUALS( denseArray[i][j], FILLELEMENT );
        }
    }

    //  0  1  2  3  4  5  6  7  8  9
    //  0  10 10 10 4  5  6  7  8  9
    //  0  10 10 10 4  5  6  7  8  9
    //  0  10 10 10 4  5  6  7  8  9
    //  0  1  2  3  4  5  6  7  8  9
    //  0  1  2  3  4  5  6  7  8  9
    //  0  1  2  3  4  5  6  7  8  9
    //  0  1  2  3  4  5  6  7  8  9
    //  0  1  2  3  4  5  6  7  8  9
    //  0  1  2  3  4  5  6  7  8  9

    for ( size_t i = 0; i < 1; ++i )
    {
        for ( size_t j = 0; j < 10; ++j )
        {
            TS_ASSERT_EQUALS( denseArray[i][j], j );
        }
    }

    for ( size_t i = 0; i < 4; ++i )
    {
        for ( size_t j = 4; j < 10; ++j )
        {
            TS_ASSERT_EQUALS( denseArray[i][j], j );
        }
    }

    for ( size_t i = 3; i < 10; ++i )
    {
        for ( size_t j = 0; j < 10; ++j )
        {
            TS_ASSERT_EQUALS( denseArray[i][j], j );
        }
    }
}

void TestView::TestCorruptView()
{
    const size_t sizeOfArray = 10;
    std::vector< std::vector< double > > denseArray( sizeOfArray, std::vector< double >() );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        denseArray.at( i ).resize( sizeOfArray );
        for ( size_t j = 0; j < sizeOfArray; ++j )
        {
            denseArray[i].at( j ) = j;
        }
    }

    TS_ASSERT_THROWS_ASSERT( View< double > view( denseArray, 6, 0, 5, 5 ), const std::out_of_range &e,
                             TS_ASSERT_DIFFERS( strcmp( e.what(), "x_end < x_start" ), 1 ) );
    TS_ASSERT_THROWS_ASSERT( View< double > view( denseArray, 0, 6, 5, 5 ), const std::out_of_range &e,
                             TS_ASSERT_DIFFERS( strcmp( e.what(), "y_end < y_start" ), 1 ) );


    TS_ASSERT_THROWS( View< double > view( denseArray, 9, 0, 11, 5 ), const std::out_of_range &e );
    TS_ASSERT_THROWS( View< double > view( denseArray, 0, 0, 5, 11 ), const std::out_of_range &e );
}
