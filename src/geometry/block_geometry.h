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
#ifndef _BLOCK_GEOMETRY_
#define _BLOCK_GEOMETRY_

#include "geometry2D.h"
#include "../misc/StrCont.h"


namespace geometry
{

/// AdjacencyType describes the ways in which two blocks can be adjacent to each other
enum AdjacencyType
{
    NOT_ADJACENT    ///<Two blocks are not adjacent
    ,
    ADJACENT_TOP_BOTTOM    ///<The two blocks are adjacent with the first block being over the second
    ,
    ADJACENT_SIDE_SIDE    ///<The two blocks are adjacent with the first block being under the second
    ,
    ADJACENT_BOTTOM_TOP    ///<The two blocks are adjacent by their side areas
    ,
    NOT_ADJACENT_BUT_TANGENT_SIDE_SIDE    ///<The two blocks are not adjacent, but are still touching (edge touching other edge or face) each other by their sides
};

/// BlockGeometry is used to test if two volumes collide or are adjacent, it contains a base area and an expansion in Z
/// direction
template < typename T = double >
class BlockGeometry
{
    public:
    /**
     * @param[in] base Base area in xy-plane in global cartesian coordinates
     * @param[in] zLower Lower end of extension in z-direction
     * @param[in] zUpper Upper end of extension in z-direction
     * @param[in] description A description of this instance that can be given out during error handling
     */
    BlockGeometry( const Geometry2D< T > &base, T zLower, T zUpper, const char *description );
    virtual ~BlockGeometry();
    /// Returns true if this instance and rhs collide (occupy the same space)
    bool CollidesWith( const BlockGeometry< T > &rhs, const Tolerance< T > tolerance ) const;
    /// Returns the AdjacencyType relationship between this instance and rhs
    AdjacencyType IsAdjacentTo( const BlockGeometry< T > &rhs, const Tolerance< T > tolerance ) const;
    /// Returns true of the dimensionality of the block is insufficient, i.e. if any lengths or angle are so small that
    /// errors would be made while creating the thermal model
    bool HasInsufficientDimension( const Tolerance< T > tolerance ) const;
    const char *GetDescription() const;
    /// Returns base area and the extension in z-direction
    const Geometry2D< T > *GetInternalData( T &zLower, T &zUpper ) const;    // UnitTest only
    private:
    Geometry2D< T > mBase;
    T mZLower;
    T mZUpper;
    misc::StrCont mDescription;
};


template < typename T >
BlockGeometry< T >::BlockGeometry( const Geometry2D< T > &base, T zLower, T zUpper, const char *description )
    : mBase( base )
    , mZLower( zLower )
    , mZUpper( zUpper )
    , mDescription( description )
{
    if ( zLower > zUpper )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "zLowerToSmall" );
}

template < typename T >
BlockGeometry< T >::~BlockGeometry()
{
}

template < typename T >
bool BlockGeometry< T >::CollidesWith( const BlockGeometry< T > &rhs, const Tolerance< T > tolerance ) const
{
    // If this instance and rhs cannot collide because oh their expansion in z-direction
    if ( rhs.mZLower + tolerance.mLength > mZUpper || rhs.mZUpper - tolerance.mLength < mZLower )
        return false;

    vector< Geometry2D< T > > intersection;
    mBase.Intersection( rhs.mBase, intersection, tolerance );
    return !intersection.empty();
}

template < typename T >
AdjacencyType BlockGeometry< T >::IsAdjacentTo( const BlockGeometry< T > &rhs, const Tolerance< T > tolerance ) const
{
    // If this instance and rhs cannot be adjacent because of their expansion in z-direction
    if ( rhs.mZLower - tolerance.mLength > mZUpper || rhs.mZUpper + tolerance.mLength < mZLower )
    {
        return NOT_ADJACENT;
    }
    // If they do not collide, but are adjacent concerning their expansion in z-direction
    else if ( rhs.mZLower + tolerance.mLength > mZUpper )
    {
        vector< Geometry2D< T > > intersection;
        mBase.Intersection( rhs.mBase, intersection, tolerance );
        return ( !intersection.empty() ) ? ADJACENT_TOP_BOTTOM : NOT_ADJACENT;
    }
    else if ( rhs.mZUpper - tolerance.mLength < mZLower )
    {
        vector< Geometry2D< T > > intersection;
        mBase.Intersection( rhs.mBase, intersection, tolerance );
        return ( !intersection.empty() ) ? ADJACENT_BOTTOM_TOP : NOT_ADJACENT;
    }
    else
    {
        if ( mBase.IsAdjacentTo( rhs.mBase, tolerance ) )
            return ADJACENT_SIDE_SIDE;
        else if ( mBase.IsTangentTo( rhs.mBase, tolerance ) )
            return NOT_ADJACENT_BUT_TANGENT_SIDE_SIDE;
        else
            return NOT_ADJACENT;
    }
}

template < typename T >
bool BlockGeometry< T >::HasInsufficientDimension( const Tolerance< T > tolerance ) const
{
    if ( mZUpper - mZLower < tolerance.mLength )
        return true;
    if ( mBase.IsNeglectable( tolerance ) )
        return true;
    if ( mBase.IsSelfAdjacent( tolerance ) )
        return true;

    return false;
}

template < typename T >
const char *BlockGeometry< T >::GetDescription() const
{
    return mDescription;
}

template < typename T >
const Geometry2D< T > *BlockGeometry< T >::GetInternalData( T &zLower, T &zUpper ) const
{
    zLower = mZLower;
    zUpper = mZUpper;
    return &mBase;
}
}
#endif
