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
#ifndef _COOLING_BLOCK_
#define _COOLING_BLOCK_

#include <boost/shared_ptr.hpp>
#include "cooling.h"
#include "../../geometry/block_geometry.h"
#include "../../geometry/area.h"
#include "../../geometry/two_dim.h"
#include "../../exceptions/error_proto.h"

class TestFactories;


namespace thermal
{
using namespace geometry;
using std::vector;
using boost::shared_ptr;

/// CoolingBlock is the subclass that returns geometry and physics data of cooling
template < typename T = double >
class CoolingBlock
{
    public:
    /**
     * @param[in] description A description of position, name and class that can be given out during error handling
     * @param[in] cooling The physics part of the cooling that calculates the heat transmission through the surfaces in
     * W/mm^2
     */
    CoolingBlock( const char *description, const shared_ptr< Cooling< T > > &cooling )
        : mDescription( description )
        , mCooling( cooling ){};
    virtual ~CoolingBlock(){};
    ///%Cooling areas that are adjacent to surface areas of thermal elements cause a cooling of a thermal block at these
    /// areas
    virtual void GetCoolingAreas( vector< Area< T > > &coolingAreas ) const = 0;
    /// Geometrical extension of cooling
    virtual void GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const = 0;
    /// physics data of cooling
    const shared_ptr< Cooling< T > > &GetCooling() const { return mCooling; }

    protected:
    const misc::StrCont mDescription;

    private:
    const shared_ptr< Cooling< T > > mCooling;
};


/// CoolingHorizontalPlane is a horizontal area that causes cooling
template < typename T = double >
class CoolingHorizontalPlane : public CoolingBlock< T >
{
    friend class ::TestFactories;

    public:
    /**
     * @param[in] description A description of position, name class that can be given out during error handling
     * @param[in] cooling The physics part of the cooling that calculates the heat transmission through the surfaces in
     * W/mm^2
     * @param[in] vertices Vertices of area in global xy-coordinate system
     * @param[in] z z-position of area in global coordinates
     */
    CoolingHorizontalPlane( const char *description, const vector< TwoDim< T > > &vertices, T z,
                            const shared_ptr< Cooling< T > > &cooling );
    virtual ~CoolingHorizontalPlane();
    void GetCoolingAreas( vector< Area< T > > &coolingAreas ) const;
    void GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const;

    private:
    vector< TwoDim< T > > mVertices;
    T mZ;
};

template < typename T >
CoolingHorizontalPlane< T >::CoolingHorizontalPlane( const char *description, const vector< TwoDim< T > > &vertices,
                                                     T z, const shared_ptr< Cooling< T > > &cooling )
    : CoolingBlock< T >( description, cooling )
    , mVertices( vertices )
    , mZ( z )
{
    if ( vertices.size() < 3 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoEnoughVerticesToFormArea" );
}

template < typename T >
CoolingHorizontalPlane< T >::~CoolingHorizontalPlane()
{
}

template < typename T >
void CoolingHorizontalPlane< T >::GetCoolingAreas( vector< Area< T > > &coolingAreas ) const
{
    coolingAreas.clear();

    coolingAreas.push_back( Area< T >( mVertices, Plane< T >( Cartesian< T >( 0.0, 0.0, mZ ), UnitVector< T >( 1.0, 0.0, 0.0 ),
                                                              UnitVector< T >( 0.0, 1.0, 0.0 ) ),
                                       0.0, 0.0, BOTTOM ) );

    coolingAreas.push_back( Area< T >( mVertices, Plane< T >( Cartesian< T >( 0.0, 0.0, mZ ), UnitVector< T >( 1.0, 0.0, 0.0 ),
                                                              UnitVector< T >( 0.0, 1.0, 0.0 ) ),
                                       0.0, 0.0, TOP ) );
}

template < typename T >
void CoolingHorizontalPlane< T >::GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    blockGeometry.reset( new BlockGeometry< T >( Geometry2D< T >( mVertices ), mZ, mZ, CoolingBlock< T >::mDescription ) );
}


/// CoolingVerticalPlane is a vertical area that causes cooling
template < typename T = double >
class CoolingVerticalPlane : public CoolingBlock< T >
{
    friend class ::TestFactories;

    public:
    /**
     * @param[in] description A description of position, name class that can be given out during error handling
     * @param[in] cooling The physics part of the cooling that calculates the heat transmission through the surfaces in
     * W/mm^2
     * @param[in] vertices Lower edge of area in global xy-coordinate system
     * @param[in] zLower Lower edge of area in z-direction in global coordinates
     * @param[in] zUpper Upper edge of area in z-direction in global coordinates
     */
    CoolingVerticalPlane( const char *description, const vector< TwoDim< T > > &vertices, T zLower, T zUpper,
                          const shared_ptr< Cooling< T > > &cooling );
    virtual ~CoolingVerticalPlane();
    void GetCoolingAreas( vector< Area< T > > &coolingAreas ) const;
    void GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const;

    private:
    const vector< TwoDim< T > > mVertices;
    const T mZLower;
    const T mZUpper;
};


template < typename T >
CoolingVerticalPlane< T >::CoolingVerticalPlane( const char *description, const vector< TwoDim< T > > &vertices,
                                                 T zLower, T zUpper, const shared_ptr< Cooling< T > > &cooling )
    : CoolingBlock< T >( description, cooling )
    , mVertices( vertices )
    , mZLower( zLower )
    , mZUpper( zUpper )
{
    if ( mZLower >= mZUpper )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UpperZCoordinateSmallerLowerZ" );
}

template < typename T >
CoolingVerticalPlane< T >::~CoolingVerticalPlane()
{
}

template < typename T >
void CoolingVerticalPlane< T >::GetCoolingAreas( vector< Area< T > > &coolingAreas ) const
{
    coolingAreas.clear();
    coolingAreas.reserve( mVertices.size() - 1 );

    vector< TwoDim< T > > areaVertices( 4 );
    areaVertices[0] = TwoDim< T >( 0.0, 0.0 );
    areaVertices[1] = TwoDim< T >( 0.0, mZUpper - mZLower );

    for ( size_t i = 0; i < mVertices.size() - 1; ++i )
    {
        areaVertices[2] = TwoDim< T >( mVertices[i].DistanceTo( mVertices[i + 1] ), mZUpper - mZLower );
        areaVertices[3] = TwoDim< T >( mVertices[i].DistanceTo( mVertices[i + 1] ), 0.0 );

        coolingAreas.push_back(
         Area< T >( areaVertices, Plane< T >( Cartesian< T >( mVertices[i].Get1(), mVertices[i].Get2(), mZLower ),
                                              UnitVector< T >( mVertices[i + 1].Get1() - mVertices[i].Get1(),
                                                               mVertices[i + 1].Get2() - mVertices[i].Get2(), 0.0 ),
                                              UnitVector< T >( 0.0, 0.0, 1.0 ) ),
                    0.0, 0.0, SIDE ) );
    }
}

template < typename T >
void CoolingVerticalPlane< T >::GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    vector< TwoDim< T > > line;
    line.reserve( 2 * mVertices.size() - 2 );
    line.insert( line.end(), mVertices.begin(), mVertices.end() );

    /*if(mVertices.size() > 2)
        for(int i = mVertices.size() - 2; i > 0; --i)
            line.push_back(mVertices[i]);*/

    blockGeometry.reset( new BlockGeometry< T >( Geometry2D< T >( line ), mZLower, mZUpper, CoolingBlock< T >::mDescription ) );
}


/// CoolingPrismatic is prismatic block (polygon as base area with height in Z direction) that is cooled
template < typename T = double >
class CoolingPrismatic : public CoolingBlock< T >
{
    friend class ::TestFactories;

    public:
    /**
     * @param[in] description A description of position, name class that can be given out during error handling
     * @param[in] cooling The physics part of the cooling that calculates the heat transmission through the surfaces in
     * W/mm^2
     * @param[in] vertices Vertices of base area in global xy-coordinate system
     * @param[in] zLower Lower front of block in z-direction in global coordinates
     * @param[in] zUpper Upper front of block in z-direction in global coordinates
     */
    CoolingPrismatic( const char *description, const vector< TwoDim< T > > &vertices, T zLower, T zUpper,
                      const shared_ptr< Cooling< T > > &cooling );
    virtual ~CoolingPrismatic();
    void GetCoolingAreas( vector< Area< T > > &coolingAreas ) const;
    void GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const;

    private:
    const vector< TwoDim< T > > mVertices;
    const T mZLower;
    const T mZUpper;
};


template < typename T >
CoolingPrismatic< T >::CoolingPrismatic( const char *description, const vector< TwoDim< T > > &vertices, T zLower,
                                         T zUpper, const shared_ptr< Cooling< T > > &cooling )
    : CoolingBlock< T >( description, cooling )
    , mVertices( vertices )
    , mZLower( zLower )
    , mZUpper( zUpper )
{
    if ( mZLower >= mZUpper )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UpperZCoordinateSmallerLowerZ" );
    if ( vertices.size() < 3 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoEnoughVerticesToFormArea" );
#ifdef __EXCEPTIONS__
    try
    {
#endif
        Geometry2D< T > geometry( vertices );
#ifdef __EXCEPTIONS__
    }
    catch ( const std::runtime_error &e )
    {

        if ( !strstr( e.what(), "Vertices create an overlapping polygon.Vertices create an overlapping polygon." ) )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "polygonOverlap" );
        }
        else    // unknown error
        {
            throw( e );
        }
    }
#endif
}

template < typename T >
CoolingPrismatic< T >::~CoolingPrismatic()
{
}

template < typename T >
void CoolingPrismatic< T >::GetCoolingAreas( vector< Area< T > > &coolingAreas ) const
{
    coolingAreas.clear();
    coolingAreas.reserve( mVertices.size() + 2 );

    // Construction of cooling areas at lower and upper base area of cooling block
    coolingAreas.push_back( Area< T >( mVertices, Plane< T >( Cartesian< T >( 0.0, 0.0, mZLower ),
                                                              UnitVector< T >( 1.0, 0.0, 0.0 ), UnitVector< T >( 0.0, 1.0, 0.0 ) ),
                                       0.0, 0.0, BOTTOM ) );
    coolingAreas.push_back( Area< T >( mVertices, Plane< T >( Cartesian< T >( 0.0, 0.0, mZUpper ),
                                                              UnitVector< T >( 1.0, 0.0, 0.0 ), UnitVector< T >( 0.0, 1.0, 0.0 ) ),
                                       0.0, 0.0, TOP ) );

    vector< TwoDim< T > > areaVertices( 4 );
    areaVertices[0] = TwoDim< T >( 0.0, 0.0 );
    // Construction of cooling areas at side areas of cooling block
    for ( size_t i = 0; i < mVertices.size(); ++i )
    {
        size_t j = ( i + 1 ) % mVertices.size();

        areaVertices[1] = TwoDim< T >( 0.0, mZUpper - mZLower );
        areaVertices[2] = TwoDim< T >( mVertices[i].DistanceTo( mVertices[j] ), mZUpper - mZLower );
        areaVertices[3] = TwoDim< T >( mVertices[i].DistanceTo( mVertices[j] ), 0.0 );

        coolingAreas.push_back( Area< T >( areaVertices,
                                           Plane< T >( Cartesian< T >( mVertices[i].Get1(), mVertices[i].Get2(), mZLower ),
                                                       UnitVector< T >( mVertices[j].Get1() - mVertices[i].Get1(),
                                                                        mVertices[j].Get2() - mVertices[i].Get2(), 0.0 ),
                                                       UnitVector< T >( 0.0, 0.0, 1.0 ) ),
                                           0.0, 0.0, SIDE ) );
    }
}

template < typename T >
void CoolingPrismatic< T >::GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    blockGeometry.reset( new BlockGeometry< T >( Geometry2D< T >( mVertices ), mZLower, mZUpper, CoolingBlock< T >::mDescription ) );
}
}
#endif
