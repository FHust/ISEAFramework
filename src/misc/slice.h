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
* File Name : slice.h
* Creation Date : 08-04-2015
* Last Modified : Do 25 Jun 2015 11:19:58 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SLICE_
#define _SLICE_

// STD
#include <vector>

/// SliceView gives the ability to operate on a vector<T*> elements like on
template < typename T >
class SliceView
{
    public:
    SliceView( std::vector< T * > &slice );

    /// Change the element persistent
    T &operator[]( size_t x );

    /// Give back an element that does not change the slicedview
    T operator[]( size_t x ) const;

    // SliceView<T>& operator=(const &SliceView<T>);
    private:
    std::vector< T * > &mSlice;
};

template < typename T >
SliceView< T >::SliceView( std::vector< T * > &slice )
    : mSlice( slice ){};

template < typename T >
T &SliceView< T >::operator[]( size_t x )
{
    if ( x < mSlice.size() )
        return *mSlice[x];
    return *mSlice.at( x );
}

template < typename T >
T SliceView< T >::operator[]( size_t x ) const
{
    if ( x < mSlice.size() )
        return *mSlice[x];
    return *mSlice.at( x );
};
#endif /* _SLICE_ */
