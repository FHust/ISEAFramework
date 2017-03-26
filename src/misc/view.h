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
* File Name : view.h
* Creation Date : 08-04-2015
* Last Modified : Mi 14 Okt 2015 12:33:22 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _VIEW_
#define _VIEW_

// ETC
#include "slice.h"
#include "../exceptions/error_proto.h"

class TestSlice;

/// View is a proxy class for vector<vector <T> > which allows sub vector<vector <T *> > to be created like a normal
/// one.
/// The View directly manipulates the provide vector mView
/// WARNINIG this class has the same caveats as pointers do. E.G. if the vector<vector <T> > goes out of scope or is
/// moved,
/// then this proxy class will crash
template < typename T >
class View
{
    public:
    friend class ::TestSlice;
    typedef T type_t;

    /// Construct a view of the array from the elements x_start until including x_end
    View( T *vec, size_t max_size_x, size_t max_size_y, size_t x_start, size_t y_start, size_t x_end, size_t y_end );

    /// Construct a view of the vector from the elements x_start until including x_end
    View( std::vector< std::vector< T > > &vec, size_t x_start, size_t y_start, size_t x_end, size_t y_end );
    /// Construct a view of a view
    View( const View< T > &view, size_t x_start, size_t y_start, size_t x_end, size_t y_end );

    /// returns a Subview which can be accessed with the operator[]
    SliceView< T > operator[]( size_t x );

    /// This function fills all elements of the view with the element value
    void FillAll( T value );

    const size_t mXDim;
    const size_t mYDim;

    private:
    void CheckDimensionSanity( size_t x_start, size_t y_start, size_t x_end, size_t y_end );
    std::vector< std::vector< T * > > mView;
};


template < typename T >
void View< T >::CheckDimensionSanity( size_t x_start, size_t y_start, size_t x_end, size_t y_end )
{
    if ( x_start > x_end )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", x_start, x_end );

    if ( y_start > y_end )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", y_start, y_end );
}


template < typename T >
View< T >::View( std::vector< std::vector< T > > &vec, size_t x_start, size_t y_start, size_t x_end, size_t y_end )
    : mXDim( x_end - x_start )
    , mYDim( y_end - y_start )
{
    CheckDimensionSanity( x_start, y_start, x_end, y_end );

    mView.resize( x_end - x_start, std::vector< T * >() );

    size_t x = 0, y = 0;
    for ( size_t i = x_start; i < x_end; ++i )
    {
        y = 0;
        mView.at( x ).resize( y_end - y_start );
        for ( size_t j = y_start; j < y_end; ++j )
        {
            mView.at( x ).at( y ) = &vec.at( i ).at( j );
            ++y;
        }
        ++x;
    }
}

template < typename T >
View< T >::View( T *vec, size_t max_size_x, size_t max_size_y, size_t x_start, size_t y_start, size_t x_end, size_t y_end )
    : mXDim( x_end - x_start )
    , mYDim( y_end - y_start )
{
    CheckDimensionSanity( x_start, y_start, x_end, y_end );
    mView.resize( x_end - x_start, std::vector< T * >() );

    if ( max_size_y + 1 < y_end )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", max_size_y + 1, y_end );

    if ( max_size_x + 1 < x_end )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", max_size_x + 1, x_end );

    size_t x = 0, y = 0;
    for ( size_t i = x_start; i < x_end; ++i )
    {
        y = 0;
        mView.at( x ).resize( y_end - y_start );
        for ( size_t j = y_start; j < y_end; ++j )
        {
            mView.at( x ).at( y ) = &vec[j * max_size_x + i];
            ++y;
        }
        ++x;
    }
}


template < typename T >
View< T >::View( const View< T > &view, size_t x_start, size_t y_start, size_t x_end, size_t y_end )
    : mXDim( x_end - x_start )
    , mYDim( y_end - y_start )
{
    CheckDimensionSanity( x_start, y_start, x_end, y_end );
    mView.resize( x_end - x_start, std::vector< T * >() );

    size_t x = 0, y = 0;
    for ( size_t i = x_start; i < x_end; ++i )
    {
        y = 0;
        mView.at( x ).resize( y_end - y_start );
        for ( size_t j = y_start; j < y_end; ++j )
        {
            mView.at( x ).at( y ) = view.mView.at( i ).at( j );
            ++y;
        }
        ++x;
    }
}

template < typename T >
SliceView< T > View< T >::operator[]( size_t x )
{
    if ( x < mView.size() )
        return SliceView< T >( mView[x] );
    return mView.at( x );    // <-- commit out of range access here
}

template < typename T >
void View< T >::FillAll( T value )
{
    for ( size_t i = 0; i < mView.size(); ++i )
    {
        for ( size_t j = 0; j < mView.at( i ).size(); ++j )
        {
            *( mView[i][j] ) = value;
        }
    }
}

#endif /* _VIEW_ */
