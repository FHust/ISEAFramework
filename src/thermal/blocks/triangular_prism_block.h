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
#ifndef _TRIANGULAR_PRISM_BLOCK_
#define _TRIANGULAR_PRISM_BLOCK_

#include <vector>
#include <cstddef>
#include <boost/shared_ptr.hpp>
#include "thermal_block.h"
#include "elements/triangular_element.h"
#include "../../geometry/area.h"
#include "../../exceptions/error_proto.h"

class TestFactories;


namespace thermal
{
using namespace geometry;
using boost::shared_ptr;
using std::vector;
using std::list;

/// TriangularPrismBlock is a thermal block with a triangular base in a plane parallel to the xy-plane and a height
template < typename T = double >
class TriangularPrismBlock : public ThermalBlock< T >
{
    friend class ::TestFactories;

    public:
    /**
     * @param[in] emplacementPoint Origin of construction
     * @param[in] vertex1,vertex2,vertex3 Vertices 1-3 define the base triangle of the block as two dimensional
     * coordinates in a plane parallel to the xy-plane with the emplacement point as origin
     * @param[in] height Extension of block in z-direction
     * @param[in] nZ Division of block in z-direction
     * @param[in] thermalStates Connection to electrical part of the model, default argument means that there is no
     * connection to cell element
     * @param[in] electricalDiscretizationZ Distribution of cell elements in thermal block in z-direction
     */
    TriangularPrismBlock( const char *description, const Cartesian< T > &emplacementPoint, const TwoDim< T > &vertex1,
                          const TwoDim< T > &vertex2, const TwoDim< T > &vertex3, T height, size_t nZ, const Material< T > *material,
                          T initialTemperature, const vector< shared_ptr< ::state::ThermalState< T > > > &thermalStates =
                                                 vector< shared_ptr< ::state::ThermalState< T > > >( 1 ),
                          size_t electricalDiscretizationZ = 1 );
    virtual ~TriangularPrismBlock();
    void CreateData( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                     vector< vector< IndexedValue< T > > > &conductivityMatrix,
                     vector< IndexedArea< T > > &surfaceAreas, shared_ptr< BlockGeometry< T > > &blockGeometry ) const;
    size_t GetNumberOfThermalElements() const;

    private:
    void GetThermalElements( vector< shared_ptr< ThermalElement< T > > > &thermalElements ) const;
    void GetConductivityMatrix( vector< vector< IndexedValue< T > > > &conductivityMatrix ) const;
    void GetSurfaceAreas( vector< IndexedArea< T > > &surfaceAreas ) const;
    void GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const;
    /**Calculates the distance between a point and a line in a two dimensional coordinate system
     * @param[in] point Coordinates of the point
     * @param[in] line1,line2 Definition of the line by two points which are part of the line
     */
    T DistanceTwoDimPointToLine( const TwoDim< T > &point, const TwoDim< T > &line1, const TwoDim< T > &line2 ) const;
    T CalculateConductivityNormalToSurfaceArea( const TwoDim< T > &vertex1, const TwoDim< T > &vertex2 ) const;
    const Cartesian< T > mEmplacementPoint;
    const TwoDim< T > mVertex1;
    const TwoDim< T > mVertex2;
    const TwoDim< T > mVertex3;
    const T mDimZ;
    const size_t mNZ;
    const Material< T > *const mMaterial;
    const T mInitialTemperature;
    const size_t mElectricalDiscretizationZ;
};


template < typename T >
TriangularPrismBlock< T >::TriangularPrismBlock( const char *description, const Cartesian< T > &emplacementPoint,
                                                 const TwoDim< T > &vertex1, const TwoDim< T > &vertex2, const TwoDim< T > &vertex3,
                                                 T height, size_t nZ, const Material< T > *material, T initialTemperature,
                                                 const vector< shared_ptr< ::state::ThermalState< T > > > &thermalStates,
                                                 size_t electricalDiscretizationZ )
    : ThermalBlock< T >( thermalStates, emplacementPoint, description )
    , mEmplacementPoint( emplacementPoint )
    , mVertex1( vertex1 )
    , mVertex2( vertex2 )
    , mVertex3( vertex3 )
    , mDimZ( height / nZ )
    , mNZ( nZ )
    , mMaterial( material )
    , mInitialTemperature( initialTemperature )
    , mElectricalDiscretizationZ( electricalDiscretizationZ )
{
    if ( height <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "HeightNegative", description );
    if ( nZ % electricalDiscretizationZ != 0 || nZ == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DivisibleByElectricalDiscretization", description );
    if ( electricalDiscretizationZ != thermalStates.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error3", description );
}

template < typename T >
TriangularPrismBlock< T >::~TriangularPrismBlock()
{
}

template < typename T >
void TriangularPrismBlock< T >::CreateData( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                            vector< vector< IndexedValue< T > > > &conductivityMatrix,
                                            vector< IndexedArea< T > > &surfaceAreas,
                                            shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    GetThermalElements( thermalElements );
    GetConductivityMatrix( conductivityMatrix );
    GetSurfaceAreas( surfaceAreas );
    GetBlockGeometry( blockGeometry );
}

template < typename T >
void TriangularPrismBlock< T >::GetThermalElements( vector< shared_ptr< ThermalElement< T > > > &thermalElements ) const
{
    thermalElements.clear();
    thermalElements.reserve( GetNumberOfThermalElements() );
    const size_t discDivZ = mNZ / mElectricalDiscretizationZ;

    for ( size_t i = 0; i < mNZ; ++i )
    {
        thermalElements.push_back( shared_ptr< ThermalElement< T > >(
         new TriangularElement< T >( mEmplacementPoint + Cartesian< T >( 0.0, 0.0, mDimZ * i ), mVertex1, mVertex2,
                                     mVertex3, mDimZ, mInitialTemperature, mMaterial,
                                     ThermalBlock< T >::mThermalStates.at( i / discDivZ ).get(), 1.0 / discDivZ ) ) );
    }
}

template < typename T >
size_t TriangularPrismBlock< T >::GetNumberOfThermalElements() const
{
    return mNZ;
}

template < typename T >
void TriangularPrismBlock< T >::GetConductivityMatrix( vector< vector< IndexedValue< T > > > &conductivityMatrix ) const
{
    conductivityMatrix.clear();
    conductivityMatrix.resize( GetNumberOfThermalElements() );
    if ( GetNumberOfThermalElements() == 1 )
        return;

    const T conductivity = 0.5 * fabs( ( ( mVertex1.Get2() + mVertex2.Get2() ) * ( mVertex1.Get1() - mVertex2.Get1() ) +
                                         ( mVertex2.Get2() + mVertex3.Get2() ) * ( mVertex2.Get1() - mVertex3.Get1() ) +
                                         ( mVertex3.Get2() + mVertex1.Get2() ) * ( mVertex3.Get1() - mVertex1.Get1() ) ) ) /
                           mDimZ * mMaterial->GetConductivity( 2 );

    // if x-, y- or z-neigbour is there, create conductivty to index of that thermal element
    for ( size_t k = 0; k < mNZ; ++k )
    {
        T totalConductivity = 0.0;

        bool kLow = k > 0, kUpp = k < mNZ - 1;
        conductivityMatrix[k].reserve( static_cast< unsigned int >( kLow ) + static_cast< unsigned int >( kUpp ) );    // false -> 0, true -> 1 by C++ standard


        if ( kLow )
        {
            conductivityMatrix[k].push_back( MakeIndexedValue< T >( k - 1, conductivity ) );
            totalConductivity += conductivity;
        }
        if ( kUpp )
        {
            conductivityMatrix[k].push_back( MakeIndexedValue< T >( k + 1, conductivity ) );
            totalConductivity += conductivity;
        }

        conductivityMatrix[k].push_back( MakeIndexedValue< T >( k, -totalConductivity ) );
    }
}

template < typename T >
void TriangularPrismBlock< T >::GetSurfaceAreas( vector< IndexedArea< T > > &surfaceAreas ) const
{
    surfaceAreas.clear();
    surfaceAreas.reserve( 3 * mNZ + 2 );


    vector< TwoDim< T > > vertices( 4 );
    const UnitVector< T > xAxis( 1.0, 0.0, 0.0 );
    const UnitVector< T > yAxis( 0.0, 1.0, 0.0 );
    const UnitVector< T > zAxis( 0.0, 0.0, 1.0 );


    vertices.resize( 3 );
    vertices[0] = mVertex1;
    vertices[1] = mVertex3;
    vertices[2] = mVertex2;

    // Bottom area
    surfaceAreas.push_back( IndexedArea< T >( 0, Area< T >( vertices, Plane< T >( mEmplacementPoint, xAxis, yAxis ),
                                                            mDimZ * 0.5, mMaterial->GetConductivity( 2 ), BOTTOM ) ) );
    // Umgekehrte Reihenfolge fuer Top Areas:

    vertices[1] = mVertex2;
    vertices[2] = mVertex3;


    // Top area
    surfaceAreas.push_back(
     IndexedArea< T >( mNZ - 1, Area< T >( vertices, Plane< T >( Cartesian< T >( 0.0, 0.0, mNZ * mDimZ ) + mEmplacementPoint, xAxis, yAxis ),
                                           0.5 * mDimZ, mMaterial->GetConductivity( 2 ), TOP ) ) );


    vertices.resize( 4 );
    vertices[0] = TwoDim< T >( 0.0, 0.0 );
    vertices[1] = TwoDim< T >( mVertex1.DistanceTo( mVertex2 ), 0.0 );
    vertices[2] = TwoDim< T >( mVertex1.DistanceTo( mVertex2 ), mDimZ );
    vertices[3] = TwoDim< T >( 0.0, mDimZ );


    vector< TwoDim< T > > vertices2( 4 );
    vertices2[0] = TwoDim< T >( 0.0, 0.0 );
    vertices2[1] = TwoDim< T >( mVertex2.DistanceTo( mVertex3 ), 0.0 );
    vertices2[2] = TwoDim< T >( mVertex2.DistanceTo( mVertex3 ), mDimZ );
    vertices2[3] = TwoDim< T >( 0.0, mDimZ );

    vector< TwoDim< T > > vertices3( 4 );
    vertices3[0] = TwoDim< T >( 0.0, 0.0 );
    vertices3[1] = TwoDim< T >( mVertex3.DistanceTo( mVertex1 ), 0.0 );
    vertices3[2] = TwoDim< T >( mVertex3.DistanceTo( mVertex1 ), mDimZ );
    vertices3[3] = TwoDim< T >( 0.0, mDimZ );


    // Side areas
    for ( size_t k = 0; k < mNZ; ++k )
    {
        surfaceAreas.push_back( IndexedArea< T >(
         k, Area< T >( vertices,
                       Plane< T >( Cartesian< T >( mVertex1.Get1(), mVertex1.Get2(), k * mDimZ ) + mEmplacementPoint,
                                   Cartesian<>( mVertex2.Get1() - mVertex1.Get1(), mVertex2.Get2() - mVertex1.Get2(), 0.0 ), zAxis ),
                       DistanceTwoDimPointToLine( ( mVertex1 + mVertex2 + mVertex3 ) / 3.0, mVertex1, mVertex2 ),
                       CalculateConductivityNormalToSurfaceArea( mVertex1, mVertex2 ), SIDE ) ) );

        surfaceAreas.push_back( IndexedArea< T >(
         k, Area< T >( vertices2,
                       Plane< T >( Cartesian< T >( mVertex2.Get1(), mVertex2.Get2(), k * mDimZ ) + mEmplacementPoint,
                                   Cartesian<>( mVertex3.Get1() - mVertex2.Get1(), mVertex3.Get2() - mVertex2.Get2(), 0.0 ), zAxis ),
                       DistanceTwoDimPointToLine( ( mVertex1 + mVertex2 + mVertex3 ) / 3.0, mVertex2, mVertex3 ),
                       CalculateConductivityNormalToSurfaceArea( mVertex2, mVertex3 ), SIDE ) ) );

        surfaceAreas.push_back( IndexedArea< T >(
         k, Area< T >( vertices3,
                       Plane< T >( Cartesian< T >( mVertex3.Get1(), mVertex3.Get2(), k * mDimZ ) + mEmplacementPoint,
                                   Cartesian<>( mVertex1.Get1() - mVertex3.Get1(), mVertex1.Get2() - mVertex3.Get2(), 0.0 ), zAxis ),
                       DistanceTwoDimPointToLine( ( mVertex1 + mVertex2 + mVertex3 ) / 3.0, mVertex3, mVertex1 ),
                       CalculateConductivityNormalToSurfaceArea( mVertex3, mVertex1 ), SIDE ) ) );
    }
}


template < typename T >
void TriangularPrismBlock< T >::GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    // Base area of Block
    vector< TwoDim< T > > vertices( 3 );
    vertices[0] = TwoDim< T >( mEmplacementPoint.GetX() + mVertex1.Get1(), mEmplacementPoint.GetY() + mVertex1.Get2() );
    vertices[1] = TwoDim< T >( mEmplacementPoint.GetX() + mVertex2.Get1(), mEmplacementPoint.GetY() + mVertex2.Get2() );
    vertices[2] = TwoDim< T >( mEmplacementPoint.GetX() + mVertex3.Get1(), mEmplacementPoint.GetY() + mVertex3.Get2() );
    const Geometry2D< T > geometry2D( vertices );

    blockGeometry.reset( new BlockGeometry< T >( geometry2D, mEmplacementPoint.GetZ(),
                                                 mEmplacementPoint.GetZ() + mNZ * mDimZ, ThermalBlock< T >::mDescription ) );
}

template < typename T >
T TriangularPrismBlock< T >::DistanceTwoDimPointToLine( const TwoDim< T > &point, const TwoDim< T > &line1,
                                                        const TwoDim< T > &line2 ) const
{
    TwoDim< T > helpVector1 = point - line1;
    TwoDim< T > helpVector2 = line2 - line1;
    helpVector2 = helpVector2 / helpVector2.Length();

    return fabs( helpVector1.Get1() * helpVector2.Get2() - helpVector1.Get2() * helpVector2.Get1() );
}

template < typename T >
T TriangularPrismBlock< T >::CalculateConductivityNormalToSurfaceArea( const TwoDim< T > &vertex1, const TwoDim< T > &vertex2 ) const
{
    TwoDim< T > deltaVertex = vertex1 - vertex2;
    TwoDim< T > areaNorm = TwoDim< T >( deltaVertex.Get2(), -deltaVertex.Get1() ) / deltaVertex.Length();
    return mMaterial->GetConductivity( 0 ) * areaNorm.Get1() * areaNorm.Get1() +
           mMaterial->GetConductivity( 1 ) * areaNorm.Get2() * areaNorm.Get2();
}
}
#endif
