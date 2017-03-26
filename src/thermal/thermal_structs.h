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
#ifndef _THERMAL_STRUCTS_
#define _THERMAL_STRUCTS_

#include "../misc/aligned_to_cache_line.h"
#include "../geometry/area.h"
#include "../geometry/innerArea.h"


namespace thermal
{
using namespace geometry;

template < typename T >
class Cooling;
/// TaylorData contains information used for Taylor approximation and calculation of cooling of thermal elements
template < typename T >
struct TaylorData
{
    T mCharacteristicLength;
    // T mDistanceFromLeadingEdge
    T mA_cool;
    ALIGNED_TO_64_BYTE T mTempSurfLastStep;
    Location mLocation;
    T mConductivity;
    T mDistanceToGridVertex;
    Cooling< T > *mCooling;
};

/// IndexedValue is a surface area with an index, as each thermal element is stored in a vector
template < typename T >
struct IndexedValue
{
    bool operator<( const IndexedValue< T > &rhs ) const { return mIndex < rhs.mIndex; }
    size_t mIndex;
    T mValue;
};

/**
 * MakeIndexedValue is an external constructor for IndexedValue<>
 *
 * If IndexedValue had a constructor, it would no longer be a POD type, which would add implicit functions like
 *destructors, copy constructors, etc. That could slow down the program if these are not erased by compiler
 *optimization.
 * This function becomes obsolete with C++11, as initializer lists can be used: vector<IndexedValue> vec;
 *vec.push_back({1, 1.0});
 **/
template < typename T >
IndexedValue< T > MakeIndexedValue( const size_t &index, const T &value )
{
    IndexedValue< T > indexedValue = {index, value};
    return indexedValue;
}

/// IndexedArea is a value with an index to a thermal element that is stored in a vector. mAreaNotCovered is later used
/// for calculation.
template < typename T >
struct IndexedArea
{
    IndexedArea( const size_t &index, const Area< T > &area )
        : mIndex( index )
        , mAreaNotCovered( 0.0 )
        , mArea( area )
    {
    }
    bool operator<( const IndexedArea< T > &rhs ) const { return mIndex < rhs.mIndex; }
    size_t mIndex;
    T mAreaNotCovered;
    Area< T > mArea;
};

/// IndexedArea is a value with an index to a thermal element that is stored in a vector. mAreaNotCovered is later used
/// for calculation.
template < typename T >
struct IndexedInnerArea
{
    IndexedInnerArea( const size_t &index, const InnerArea< T > &innerArea )
        : mIndex( index )
        , mArea( innerArea )
    {
    }
    bool operator<( const IndexedArea< T > &rhs ) const { return mIndex < rhs.mIndex; }
    size_t mIndex;
    InnerArea< T > mArea;
};

/// Linearization contains the values for linearization by first order Taylor approximation
template < typename T >
struct Linearization
{
    Linearization( const T &offset, const T &slope )
        : mOffset( offset )
        , mSlope( slope )
    {
    }
    void Add( const Linearization< T > &other )
    {
        mOffset += other.mOffset;
        mSlope += other.mSlope;
    }
    T mOffset;
    T mSlope;
};


/// IndexIs is a one parameter predicate that returns true, if IndexedValue has a certain index
template < typename T >
struct IndexIs
{
    explicit IndexIs( size_t index )
        : mIndex( index )
    {
    }
    bool operator()( const IndexedValue< T > &compare ) const { return mIndex == compare.mIndex; }

    private:
    size_t mIndex;
};

/// IndexedValuesEqual is a two parameter predicate that returns true, if both indexed values have the same index and
/// the diffrence of both values is smaller than delta
template < typename T >
struct IndexedValuesEqual
{
    explicit IndexedValuesEqual( T delta )
        : mDelta( delta )
    {
    }
    bool operator()( const IndexedValue< T > &lhs, const IndexedValue< T > &rhs ) const
    {
        return lhs.mIndex == rhs.mIndex && fabs( lhs.mValue - rhs.mValue ) < mDelta;
    }

    private:
    T mDelta;
};


/// Use this if C++11 is not available
template < typename T >
inline void ShrinkToFit( vector< T > &vec )
{
    vector< T >( vec ).swap( vec );
}
}
#endif
