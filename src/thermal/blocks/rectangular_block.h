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
#ifndef _RECTANGULAR_BLOCK_
#define _RECTANGULAR_BLOCK_

#include <vector>
#include <cstddef>
#include <boost/shared_ptr.hpp>
#include "thermal_block.h"
#include "elements/rectangular_element.h"
#include "../../geometry/area.h"
#include "../../exceptions/error_proto.h"

class TestFactories;


namespace thermal
{
using namespace geometry;
using boost::shared_ptr;
using std::vector;
using std::list;

/// RectangularBlock constructs a rectangular block of rectangular elements, either with or without heat power
/// dissipation
template < typename T = double >
class RectangularBlock : public ThermalBlock< T >
{
    friend class ::TestFactories;

    public:
    /**
     * @param[in] emplacementPoint Vertex of Block with the lowest x-, y- and z-coordinate
     * @param[in] length Extension of block in x-direction
     * @param[in] width Extension of Block in y-direction
     * @param[in] height Extension of Block in z-direction
     * @param[in] nX,nY,nZ Division of Block in x-, y- and z-direction
     * @param[in] thermalStates Connection to electrical part of the model, default argument means that there is no
     * connection to cell element
     * @param[in] electricalDiscretizationX,electricalDiscretizationY,electricalDiscretizationZ Distribution of cell
     * elements in thermal block in x-, y- and z-direction
     */
    RectangularBlock( const char *description, const Cartesian< T > &emplacementPoint, T length, T width, T height,
                      size_t nX, size_t nY, size_t nZ, const Material< T > *material, T initialTemperature,
                      const vector< shared_ptr< ::state::ThermalState< T > > > &thermalState =
                       vector< shared_ptr< ::state::ThermalState< T > > >( 1 ),
                      size_t electricalDiscretizationX = 1, size_t electricalDiscretizationY = 1,
                      size_t electricalDiscretizationZ = 1 );
    virtual ~RectangularBlock();
    void CreateData( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                     vector< vector< IndexedValue< T > > > &conductivityMatrix,
                     vector< IndexedArea< T > > &surfaceAreas, shared_ptr< BlockGeometry< T > > &blockGeometry ) const;
    size_t GetNumberOfThermalElements() const;

    private:
    void GetThermalElements( vector< shared_ptr< ThermalElement< T > > > &thermalElements ) const;
    void GetConductivityMatrix( vector< vector< IndexedValue< T > > > &conductivityMatrix ) const;
    void GetSurfaceAreas( vector< IndexedArea< T > > &surfaceAreas ) const;
    void GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const;
    size_t Index( size_t x, size_t y, size_t z ) const;
    const Cartesian< T > mEmplacementPoint;
    const size_t mNX;
    const size_t mNY;
    const size_t mNZ;
    const T mDimX;
    const T mDimY;
    const T mDimZ;
    const Material< T > *const mMaterial;
    const T mInitialTemperature;
    const size_t mElectricalDiscretizationX;
    const size_t mElectricalDiscretizationY;
    const size_t mElectricalDiscretizationZ;

    const size_t mNumberOfElements;
};


template < typename T >
RectangularBlock< T >::RectangularBlock( const char *description, const Cartesian< T > &emplacementPoint, T length,
                                         T width, T height, size_t nX, size_t nY, size_t nZ, const Material< T > *material,
                                         T initialTemperature, const vector< shared_ptr< ::state::ThermalState< T > > > &thermalStates,
                                         size_t electricalDiscretizationX, size_t electricalDiscretizationY,
                                         size_t electricalDiscretizationZ )
    : ThermalBlock< T >( thermalStates, emplacementPoint, description )
    , mEmplacementPoint( emplacementPoint )
    , mNX( nX == 0 ? 1 : nX )
    , mNY( nY == 0 ? 1 : nY )
    , mNZ( nZ == 0 ? 1 : nZ )
    , mDimX( length / mNX )
    , mDimY( width / mNY )
    , mDimZ( height / mNZ )
    , mMaterial( material )
    , mInitialTemperature( initialTemperature )
    , mElectricalDiscretizationX( electricalDiscretizationX )
    , mElectricalDiscretizationY( electricalDiscretizationY )
    , mElectricalDiscretizationZ( electricalDiscretizationZ )
    , mNumberOfElements( nX * nY * nZ )
{
    if ( length <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ElementsDimensionsNegative", description );

    if ( width <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ElementsDimensionsNegative", description );
    if ( height <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ElementsDimensionsNegative", description );
    if ( nX % electricalDiscretizationX != 0 || nY % electricalDiscretizationY != 0 ||
         nZ % electricalDiscretizationZ != 0 || nX == 0 || nY == 0 || nZ == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DivisibleByElectricalDiscretization", description );
    if ( electricalDiscretizationX * electricalDiscretizationY * electricalDiscretizationZ != thermalStates.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error3" );
}

template < typename T >
RectangularBlock< T >::~RectangularBlock()
{
}

template < typename T >
void RectangularBlock< T >::CreateData( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                        vector< vector< IndexedValue< T > > > &conductivityMatrix,
                                        vector< IndexedArea< T > > &surfaceAreas, shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    GetThermalElements( thermalElements );
    GetConductivityMatrix( conductivityMatrix );
    GetSurfaceAreas( surfaceAreas );
    GetBlockGeometry( blockGeometry );
}

template < typename T >
void RectangularBlock< T >::GetThermalElements( vector< shared_ptr< ThermalElement< T > > > &thermalElements ) const
{
    thermalElements.clear();
    thermalElements.reserve( GetNumberOfThermalElements() );
    const size_t discDivX = mNX / mElectricalDiscretizationX;
    const size_t discDivY = mNY / mElectricalDiscretizationY;
    const size_t discDivZ = mNZ / mElectricalDiscretizationZ;

    for ( size_t k = 0; k < mNZ; ++k )
        for ( size_t j = 0; j < mNY; ++j )
            for ( size_t i = 0; i < mNX; ++i )
                thermalElements.push_back( shared_ptr< ThermalElement< T > >( new RectangularElement< T >(
                 Cartesian< T >( i * mDimX, j * mDimY, k * mDimZ ) + mEmplacementPoint,
                 Cartesian< T >( mDimX, mDimY, mDimZ ), mInitialTemperature, mMaterial,
                 ThermalBlock< T >::mThermalStates.at( i / discDivX + mElectricalDiscretizationX * ( j / discDivY ) +
                                                       mElectricalDiscretizationX * mElectricalDiscretizationY * ( k / discDivZ ) ).get(),
                 1.0 / ( discDivX * discDivY * discDivZ ) ) ) );
}

template < typename T >
size_t RectangularBlock< T >::GetNumberOfThermalElements() const
{
    return mNumberOfElements;
}

template < typename T >
void RectangularBlock< T >::GetConductivityMatrix( vector< vector< IndexedValue< T > > > &conductivityMatrix ) const
{
    conductivityMatrix.clear();
    conductivityMatrix.resize( GetNumberOfThermalElements() );
    if ( GetNumberOfThermalElements() == 1 )
        return;

    const T conductivityX = mDimY * mDimZ / mDimX * mMaterial->GetConductivity( 0 );
    const T conductivityY = mDimX * mDimZ / mDimY * mMaterial->GetConductivity( 1 );
    const T conductivityZ = mDimX * mDimY / mDimZ * mMaterial->GetConductivity( 2 );

    // if x-, y- or z-neigbour is there, create conductivty to index of that thermal element
    for ( size_t i = 0; i < mNX; ++i )
        for ( size_t j = 0; j < mNY; ++j )
            for ( size_t k = 0; k < mNZ; ++k )
            {
                T totalConductivity = 0.0;

                bool iLow = i > 0, iUpp = i< mNX - 1, jLow = j > 0, jUpp = j< mNY - 1, kLow = k > 0, kUpp = k < mNZ - 1;
                conductivityMatrix[Index( i, j, k )].reserve(
                 static_cast< unsigned int >( iLow ) + static_cast< unsigned int >( iUpp ) +
                 static_cast< unsigned int >( jLow ) + static_cast< unsigned int >( jUpp ) + static_cast< unsigned int >( kLow ) +
                 static_cast< unsigned int >( kUpp ) );    // false -> 0, true -> 1 by C++ standard


                if ( i > 0 )
                {
                    conductivityMatrix[Index( i, j, k )].push_back( MakeIndexedValue< T >( Index( i - 1, j, k ), conductivityX ) );
                    totalConductivity += conductivityX;
                }
                if ( i < mNX - 1 )
                {
                    conductivityMatrix[Index( i, j, k )].push_back( MakeIndexedValue< T >( Index( i + 1, j, k ), conductivityX ) );
                    totalConductivity += conductivityX;
                }
                if ( j > 0 )
                {
                    conductivityMatrix[Index( i, j, k )].push_back( MakeIndexedValue< T >( Index( i, j - 1, k ), conductivityY ) );
                    totalConductivity += conductivityY;
                }
                if ( j < mNY - 1 )
                {
                    conductivityMatrix[Index( i, j, k )].push_back( MakeIndexedValue< T >( Index( i, j + 1, k ), conductivityY ) );
                    totalConductivity += conductivityY;
                }
                if ( k > 0 )
                {
                    conductivityMatrix[Index( i, j, k )].push_back( MakeIndexedValue< T >( Index( i, j, k - 1 ), conductivityZ ) );
                    totalConductivity += conductivityZ;
                }
                if ( k < mNZ - 1 )
                {
                    conductivityMatrix[Index( i, j, k )].push_back( MakeIndexedValue< T >( Index( i, j, k + 1 ), conductivityZ ) );
                    totalConductivity += conductivityZ;
                }

                conductivityMatrix[Index( i, j, k )].push_back( MakeIndexedValue< T >( Index( i, j, k ), -totalConductivity ) );
            }
}

template < typename T >
void RectangularBlock< T >::GetSurfaceAreas( vector< IndexedArea< T > > &surfaceAreas ) const
{
    surfaceAreas.clear();
    surfaceAreas.reserve( 2 * ( mNX * mNY + mNX * mNZ + mNY * mNZ ) );


    const UnitVector< T > xAxis( 1.0, 0.0, 0.0 );
    const UnitVector< T > yAxis( 0.0, 1.0, 0.0 );
    const UnitVector< T > zAxis( 0.0, 0.0, 1.0 );

    // const UnitVector<T> xAxisUpsideDown(-1.0, 0.0, 0.0);
    // const UnitVector<T> zAxisUpsideDown(0.0, 0.0, -1.0);

    vector< TwoDim< T > > vertices( 4 );
    vertices[0] = TwoDim< T >( 0.0, 0.0 );
    vertices[1] = TwoDim< T >( mDimX, 0.0 );
    vertices[2] = TwoDim< T >( mDimX, mDimY );
    vertices[3] = TwoDim< T >( 0.0, mDimY );

    // vertices für areas in negative coordinate direction:
    vector< TwoDim< T > > verticesFlipped( 4 );
    verticesFlipped[0] = TwoDim< T >( 0.0, 0.0 );
    verticesFlipped[1] = TwoDim< T >( 0.0, mDimY );
    verticesFlipped[2] = TwoDim< T >( mDimX, mDimY );
    verticesFlipped[3] = TwoDim< T >( mDimX, 0.0 );


    for ( size_t i = 0; i < mNX; ++i )
        for ( size_t j = 0; j < mNY; ++j )
        {
            // Top areas
            surfaceAreas.push_back(
             IndexedArea< T >( Index( i, j, mNZ - 1 ),
                               Area< T >( vertices, Plane< T >( Cartesian< T >( i * mDimX, j * mDimY, mNZ * mDimZ ) + mEmplacementPoint,
                                                                xAxis, yAxis ),
                                          0.5 * mDimZ, mMaterial->GetConductivity( 2 ), TOP ) ) );

            // Bottom areas
            surfaceAreas.push_back(
             IndexedArea< T >( Index( i, j, 0 ),
                               Area< T >( verticesFlipped,
                                          Plane< T >( Cartesian< T >( i * mDimX, j * mDimY, 0.0 ) + mEmplacementPoint, xAxis, yAxis ),
                                          mDimZ * 0.5, mMaterial->GetConductivity( 2 ), BOTTOM ) ) );
        }


    for ( size_t k = 0; k < mNZ; ++k )
    {
        vertices[1] = TwoDim< T >( 0.0, mDimZ );
        vertices[2] = TwoDim< T >( mDimX, mDimZ );
        vertices[3] = TwoDim< T >( mDimX, 0.0 );

        // verticesFlipped for side areas in Y-direction:

        verticesFlipped[1] = TwoDim< T >( mDimX, 0.0 );
        verticesFlipped[2] = TwoDim< T >( mDimX, mDimZ );
        verticesFlipped[3] = TwoDim< T >( 0.0, mDimZ );

        for ( size_t i = 0; i < mNX; ++i )
        {
            // Side areas in Y-direction

            surfaceAreas.push_back(
             IndexedArea< T >( Index( i, mNY - 1, k ),
                               Area< T >( vertices, Plane< T >( Cartesian< T >( i * mDimX, mNY * mDimY, k * mDimZ ) + mEmplacementPoint,
                                                                xAxis, zAxis ),
                                          mDimY * 0.5, mMaterial->GetConductivity( 1 ), SIDE ) ) );

            surfaceAreas.push_back(
             IndexedArea< T >( Index( i, 0, k ),
                               Area< T >( verticesFlipped,
                                          Plane< T >( Cartesian< T >( i * mDimX, 0.0, k * mDimZ ) + mEmplacementPoint, xAxis, zAxis ),
                                          mDimY * 0.5, mMaterial->GetConductivity( 1 ), SIDE ) ) );
        }


        // Side areas in X-direction
        vertices[1] = TwoDim< T >( mDimY, 0.0 );
        vertices[2] = TwoDim< T >( mDimY, mDimZ );
        vertices[3] = TwoDim< T >( 0.0, mDimZ );

        // verticesFlipped for side areas in X-Direction:

        verticesFlipped[1] = TwoDim< T >( 0.0, mDimZ );
        verticesFlipped[2] = TwoDim< T >( mDimY, mDimZ );
        verticesFlipped[3] = TwoDim< T >( mDimY, 0.0 );

        for ( size_t j = 0; j < mNY; ++j )
        {
            surfaceAreas.push_back(
             IndexedArea< T >( Index( mNX - 1, j, k ),
                               Area< T >( vertices, Plane< T >( Cartesian< T >( mNX * mDimX, j * mDimY, k * mDimZ ) + mEmplacementPoint,
                                                                yAxis, zAxis ),
                                          mDimX * 0.5, mMaterial->GetConductivity( 0 ), SIDE ) ) );

            surfaceAreas.push_back(
             IndexedArea< T >( Index( 0, j, k ),
                               Area< T >( verticesFlipped,
                                          Plane< T >( Cartesian< T >( 0.0, j * mDimY, k * mDimZ ) + mEmplacementPoint, yAxis, zAxis ),
                                          mDimX * 0.5, mMaterial->GetConductivity( 0 ), SIDE ) ) );
        }
    }
}


template < typename T >
void RectangularBlock< T >::GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    // Base area of Block
    vector< TwoDim< T > > vertices( 4 );
    vertices[0] = TwoDim< T >( mEmplacementPoint.GetX() + 0.0, mEmplacementPoint.GetY() + 0.0 );
    vertices[1] = TwoDim< T >( mEmplacementPoint.GetX() + mNX * mDimX, mEmplacementPoint.GetY() + 0.0 );
    vertices[2] = TwoDim< T >( mEmplacementPoint.GetX() + mNX * mDimX, mEmplacementPoint.GetY() + mNY * mDimY );
    vertices[3] = TwoDim< T >( mEmplacementPoint.GetX() + 0.0, mEmplacementPoint.GetY() + mNY * mDimY );
    const Geometry2D< T > geometry2D( vertices );

    blockGeometry.reset( new BlockGeometry< T >( geometry2D, mEmplacementPoint.GetZ(),
                                                 mEmplacementPoint.GetZ() + mNZ * mDimZ, ThermalBlock< T >::mDescription ) );
}

template < typename T >
size_t RectangularBlock< T >::Index( size_t x, size_t y, size_t z ) const
{
    return x + y * mNX + z * mNY * mNX;
}
}
#endif
