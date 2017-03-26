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
#ifndef _HEXAGONAL_CELL_BLOCK_
#define _HEXAGONAL_CELL_BLOCK_

#include <vector>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include "thermal_block.h"
#include "elements/cylindric_element.h"
#include "elements/cutcyl_element.h"
#include "../../geometry/area.h"
#include "../thermal_structs.h"

#include "../../exceptions/error_function.h"


class TestFactories;


namespace thermal
{
using namespace geometry;
using boost::shared_ptr;
using std::vector;

/// HexagonalCellBlock constructs a hexagonal prismatic block with hexagonal prismatic base area containg a cylindric
/// battery cell in the center of the block, from top to bottom, surrounded by filling material. Two Edges of the
/// hexagon are parallel to x-vector. The x- and y- coordinates of the emplacement point are respectivley the lowest of
/// all edge vertices.
template < typename T = double >
class HexagonalCellBlock : public ThermalBlock< T >
{
    friend class ::TestFactories;

    public:
    /**
     * @param[in] emplacementPoint Emplacement point
     * @param[in] length Length of an edge of the hexagon that is the base of this cell
     * @param[in] radiusCell Radius of battery cell
     * @param[in] height Extension of block in z-Direction
     * @param[in] nRhoCell Division of battery cell in rho-direction
     * @param[in] nPhi,nZ Division of block in phi- and z-direction
     * @param[in] arcPolygonEdgesLength Supposes a length for the edge of a polygon representing an arc or a circle
     * @param[in] thermalStates Connection to electrical part of the model, must not be empty as a battery must have an
     * electric part
     * @param[in] electricalDiscretizationPhi,electricalDiscretizationZ Distribution of cell elements in thermal block
     * in phi- and z-direction
     */
    HexagonalCellBlock( const char *description, const Cartesian< T > &emplacementPoint, T length, T radiusCell,
                        T height, size_t nRhoCell, size_t nPhi, size_t nZ, T arcPolygonEdgesLength,
                        const Material< T > *cellMaterial, const Material< T > *fillMaterial, T initialTemperature,
                        const vector< shared_ptr< ::state::ThermalState< T > > > &thermalStates,
                        size_t electricalDiscretizationPhi = 1, size_t electricalDiscretizationZ = 1 );
    virtual ~HexagonalCellBlock();
    void CreateData( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                     vector< vector< IndexedValue< T > > > &conductivityMatrix,
                     vector< IndexedArea< T > > &surfaceAreas, shared_ptr< BlockGeometry< T > > &blockGeometry ) const;
    size_t GetNumberOfThermalElements() const;

    private:
    ///@param[out] helpElements Used to help constructing thermal elements by granting access to geometrical functions
    /// of CutCylElement
    void GetThermalElements( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                             vector< CutCylElement< T > * > &helpElements ) const;
    ///@param[in] helpElements Used to help constructing thermal elements by granting access to geometrical functions of
    /// CutCylElement
    void GetConductivityMatrix( const vector< CutCylElement< T > * > &helpElements,
                                vector< vector< IndexedValue< T > > > &conductivityMatrix ) const;
    ///@param[in] helpElements Used to help constructing thermal elements by granting access to geometrical functions of
    /// CutCylElement
    void GetSurfaceAreas( const vector< CutCylElement< T > * > &helpElements, vector< IndexedArea< T > > &surfaceAreas ) const;
    void GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const;

    Cartesian< T > mRadialEmplacementPoint;
    const T mLength;
    const T mRadiusCell;
    const T mHeight;
    const size_t mNRhoCell;
    const size_t mNPhi;
    const size_t mNZ;
    const T mArcPolygonEdgesLength;
    const Material< T > *const mCellMaterial;
    const Material< T > *const mFillMaterial;
    const T mInitialTemperature;
    const size_t mElectricalDiscretizationPhi;
    const size_t mElectricalDiscretizationZ;

    const size_t mNZLayer;                 ///<Number of thermal elements in in one layer parallel to xy-plane
    const T mRadiusCellDelta;              ///<Extension of one thermal element in battery cell in rho-direction
    const T mPhiDelta;                     ///<///<Extension of one thermal element in block in phi-direction
    const T mZDelta;                       ///<///<Extension of one thermal element in block in z-direction
    const size_t mNumberOfCellElements;    ///<Number of thermal elements in in battery cell
};


template < typename T >
HexagonalCellBlock< T >::HexagonalCellBlock( const char *description, const Cartesian< T > &emplacementPoint, T length,
                                             T radiusCell, T height, size_t nRhoCell, size_t nPhi, size_t nZ,
                                             T arcPolygonEdgesLength, const Material< T > *cellMaterial,
                                             const Material< T > *fillMaterial, T initialTemperature,
                                             const vector< shared_ptr< ::state::ThermalState< T > > > &thermalStates,
                                             size_t electricalDiscretizationPhi, size_t electricalDiscretizationZ )
    : ThermalBlock< T >( thermalStates, emplacementPoint, description )
    , mRadialEmplacementPoint( emplacementPoint + Cartesian< T >( length, sqrt( 0.75 ) * length, 0.0 ) )
    , mLength( length )
    , mRadiusCell( radiusCell )
    , mHeight( height )
    , mNRhoCell( nRhoCell == 0 ? 1 : nRhoCell )
    , mNPhi( nPhi == 0 ? 1 : nPhi )
    , mNZ( nZ == 0 ? 1 : nZ )
    , mArcPolygonEdgesLength( arcPolygonEdgesLength )
    , mCellMaterial( cellMaterial )
    , mFillMaterial( fillMaterial )
    , mInitialTemperature( initialTemperature )
    , mElectricalDiscretizationPhi( electricalDiscretizationPhi )
    , mElectricalDiscretizationZ( electricalDiscretizationZ )
    , mNZLayer( mNRhoCell * mNPhi + mNPhi )
    , mRadiusCellDelta( radiusCell / mNRhoCell )
    , mPhiDelta( 2.0 * Angle< T >::pi / mNPhi )
    , mZDelta( height / mNZ )
    , mNumberOfCellElements( mNZLayer * mNZ )
{
    if ( length <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "LengthNegative", description );
    if ( radiusCell <= 0.0 || radiusCell >= length * sqrt( 0.75 ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "RadiusCellDoesNotFitInHexagonal", description );
    if ( nPhi % 6 != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "PhiDivisionNonDivisible6", description );
    if ( height <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "HeightNegative", description );
    if ( mArcPolygonEdgesLength <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ArcPolygonizationNegative", description );
    if ( nZ % electricalDiscretizationZ != 0 || nPhi % electricalDiscretizationPhi != 0 || nZ == 0 || nPhi == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DivisibleByElectricalDiscretization", description );
    if ( electricalDiscretizationZ * electricalDiscretizationPhi != thermalStates.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error3", description );
}

template < typename T >
HexagonalCellBlock< T >::~HexagonalCellBlock()
{
}

template < typename T >
void HexagonalCellBlock< T >::CreateData( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                          vector< vector< IndexedValue< T > > > &conductivityMatrix,
                                          vector< IndexedArea< T > > &surfaceAreas,
                                          shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    vector< CutCylElement< T > * > helpElements;

    GetThermalElements( thermalElements, helpElements );
    GetConductivityMatrix( helpElements, conductivityMatrix );
    GetSurfaceAreas( helpElements, surfaceAreas );
    GetBlockGeometry( blockGeometry );
}

template < typename T >
size_t HexagonalCellBlock< T >::GetNumberOfThermalElements() const
{
    return mNumberOfCellElements;
}

template < typename T >
void HexagonalCellBlock< T >::GetThermalElements( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                                  vector< CutCylElement< T > * > &helpElements ) const
{
    helpElements.clear();
    helpElements.reserve( mNPhi );
    thermalElements.clear();
    thermalElements.reserve( GetNumberOfThermalElements() );
    const T thermalStatesVolume =
     mHeight * mRadiusCell * mRadiusCell * Angle<>::pi / ( mElectricalDiscretizationPhi * mElectricalDiscretizationZ );
    const size_t discDivPhi = mNPhi / mElectricalDiscretizationPhi;
    const size_t discDivZ = mNZ / mElectricalDiscretizationZ;

    for ( size_t k = 0; k < mNZ; ++k )
    {
        // Construction of thermal elements in battery cell
        for ( size_t i = 0; i < mNRhoCell; ++i )
            for ( size_t j = 0; j < mNPhi; ++j )
                thermalElements.push_back( shared_ptr< ThermalElement< T > >( new CylindricElement< T >(
                 mRadialEmplacementPoint, Cylindric< T >( mRadiusCellDelta * i, Angle< T >::Rad( mPhiDelta * j ), mZDelta * k ),
                 Cylindric< T >( mRadiusCellDelta, Angle< T >::Rad( mPhiDelta ), mZDelta ), mInitialTemperature, mCellMaterial,
                 ThermalBlock< T >::mThermalStates.at( j / discDivPhi + mElectricalDiscretizationPhi * ( k / discDivZ ) ).get(),
                 mZDelta * mRadiusCellDelta * mRadiusCellDelta * ( ( i + 1 ) * ( i + 1 ) - i * i ) * mPhiDelta * 0.5 /
                  thermalStatesVolume ) ) );

        // Construction of thermal elements outside of battery cell
        for ( size_t j = 0; j < mNPhi; ++j )
        {
            TwoDim< T > cutLineEmplacement;
            TwoDim< T > cutLineDirection;

            size_t section = j / ( mNPhi / 6 );
            switch ( section )
            {
                case 0:
                    cutLineEmplacement = TwoDim< T >( mLength, 0.0 );
                    cutLineDirection = TwoDim< T >( -1.0, sqrt( 3.0 ) );
                    break;
                case 1:
                    cutLineEmplacement = TwoDim< T >( 0.5 * mLength, sqrt( 0.75 ) * mLength );
                    cutLineDirection = TwoDim< T >( 1.0, 0.0 );
                    break;
                case 2:
                    cutLineEmplacement = TwoDim< T >( -0.5 * mLength, sqrt( 0.75 ) * mLength );
                    cutLineDirection = TwoDim< T >( 1.0, sqrt( 3.0 ) );
                    break;
                case 3:
                    cutLineEmplacement = TwoDim< T >( -mLength, 0.0 );
                    cutLineDirection = TwoDim< T >( -1.0, sqrt( 3.0 ) );
                    break;
                case 4:
                    cutLineEmplacement = TwoDim< T >( -0.5 * mLength, -sqrt( 0.75 ) * mLength );
                    cutLineDirection = TwoDim< T >( 1.0, 0.0 );
                    break;
                case 5:
                    cutLineEmplacement = TwoDim< T >( 0.5 * mLength, -sqrt( 0.75 ) * mLength );
                    cutLineDirection = TwoDim< T >( 1.0, sqrt( 3.0 ) );
                    break;
            }

            // Thermal elements outside of battery cell are accesible by their subclass pointer to grant access to
            // subclass geometrical functions later
            helpElements.push_back( new CutCylElement< T >( mRadialEmplacementPoint,
                                                            Cylindric< T >( mRadiusCell, Angle< T >::Rad( mPhiDelta * j ), mZDelta * k ),
                                                            Angle< T >::Rad( mPhiDelta ), mZDelta, cutLineEmplacement,
                                                            cutLineDirection, mInitialTemperature, mFillMaterial ) );
            thermalElements.push_back( shared_ptr< ThermalElement< T > >( helpElements.back() ) );
        }
    }
}

template < typename T >
void HexagonalCellBlock< T >::GetConductivityMatrix( const vector< CutCylElement< T > * > &helpElements,
                                                     vector< vector< IndexedValue< T > > > &conductivityMatrix ) const
{
    conductivityMatrix.clear();
    conductivityMatrix.resize( GetNumberOfThermalElements() );

    // Conductivities in phi-direction
    for ( size_t k = 0; k < mNZ; ++k )
    {
        size_t index = k * mNZLayer;
        // In battery cell
        for ( size_t i = 0; i < mNRhoCell; ++i )
        {
            const T conductivity = mCellMaterial->GetConductivity( 1 ) * mRadiusCellDelta * mZDelta /
                                   ( ( 0.5 + i ) * mRadiusCellDelta * mPhiDelta );
            for ( size_t j = 0; j < mNPhi; ++j )
            {
                size_t nextIndex = index + 1;
                if ( j == mNPhi - 1 )
                    nextIndex -= mNPhi;

                conductivityMatrix[index].push_back( MakeIndexedValue< T >( nextIndex, conductivity ) );
                conductivityMatrix[nextIndex].push_back( MakeIndexedValue< T >( index, conductivity ) );
                ++index;
            }
        }

        // Outside of battery cell
        for ( size_t j = 0; j < mNPhi; ++j )
        {
            const T help1 = helpElements[j]->GetRhoNegDistance() + helpElements[j]->GetRhoPosDistance();
            const T help2 = helpElements[j]->GetStraightEdgeVertexDelta().Length() - mRadiusCell;
            const T help3 =
             helpElements[( j + 1 ) % mNPhi]->GetRhoNegDistance() + helpElements[( j + 1 ) % mNPhi]->GetRhoPosDistance();
            const T area = mZDelta * pow( help1 * help2 * help2 * help3, 0.25 );
            const T distance = helpElements[j]->GetPhiPosDistance() + helpElements[( j + 1 ) % mNPhi]->GetPhiNegDistance();
            const T conductivity = mFillMaterial->GetConductivity( 1 ) * area / distance;

            size_t nextIndex = index + 1;
            if ( j == mNPhi - 1 )
                nextIndex -= mNPhi;

            conductivityMatrix[index].push_back( MakeIndexedValue< T >( nextIndex, conductivity ) );
            conductivityMatrix[nextIndex].push_back( MakeIndexedValue< T >( index, conductivity ) );
            ++index;
        }
    }


    // Conductivities in rho-direction
    for ( size_t k = 0; k < mNZ; ++k )
    {
        size_t index = k * mNZLayer;
        // In battery cell
        for ( size_t i = 0; i < mNRhoCell - 1; ++i )
        {
            const T conductivity =
             mCellMaterial->GetConductivity( 0 ) * ( mRadiusCellDelta * mPhiDelta * ( i + 1 ) ) * mZDelta / mRadiusCellDelta;
            for ( size_t j = 0; j < mNPhi; ++j )
            {
                conductivityMatrix[index].push_back( MakeIndexedValue< T >( index + mNPhi, conductivity ) );
                conductivityMatrix[index + mNPhi].push_back( MakeIndexedValue< T >( index, conductivity ) );
                ++index;
            }
        }

        // From inside to outside of battery cell
        for ( size_t j = 0; j < mNPhi; ++j )
        {
            const T conductivity = mRadiusCell * mPhiDelta * mZDelta /
                                   ( mRadiusCellDelta * 0.5 / mCellMaterial->GetConductivity( 0 ) +
                                     helpElements[j]->GetRhoNegDistance() / mFillMaterial->GetConductivity( 0 ) );

            conductivityMatrix[index].push_back( MakeIndexedValue< T >( index + mNPhi, conductivity ) );
            conductivityMatrix[index + mNPhi].push_back( MakeIndexedValue< T >( index, conductivity ) );
            ++index;
        }
    }


    // Conductivities in z-direction
    for ( size_t k = 0; k < mNZ - 1; ++k )
    {
        size_t index = k * mNZLayer;
        // In battery cell
        for ( size_t i = 0; i < mNRhoCell; ++i )
        {
            const T conductivity = mCellMaterial->GetConductivity( 2 ) * mRadiusCellDelta * mRadiusCellDelta *
                                   ( ( i + 1 ) * ( i + 1 ) - i * i ) * Angle< T >::pi / mNPhi / mZDelta;
            for ( size_t j = 0; j < mNPhi; ++j )
            {
                conductivityMatrix[index].push_back( MakeIndexedValue< T >( index + mNZLayer, conductivity ) );
                conductivityMatrix[index + mNZLayer].push_back( MakeIndexedValue< T >( index, conductivity ) );
                ++index;
            }
        }

        // Outside of battery cell
        for ( size_t j = 0; j < mNPhi; ++j )
        {
            const T conductivity = mFillMaterial->GetConductivity( 2 ) * helpElements[j]->GetZAreaValue() / mZDelta;
            conductivityMatrix[index].push_back( MakeIndexedValue< T >( index + mNZLayer, conductivity ) );
            conductivityMatrix[index + mNZLayer].push_back( MakeIndexedValue< T >( index, conductivity ) );
            ++index;
        }
    }


    // Add negative self conductivities
    for ( size_t i = 0; i < conductivityMatrix.size(); ++i )
    {
        T totalConductivity = 0.0;
        BOOST_FOREACH ( IndexedValue< T > &conductivity, conductivityMatrix[i] )
            totalConductivity += conductivity.mValue;

        conductivityMatrix[i].push_back( MakeIndexedValue< T >( i, -totalConductivity ) );
        std::sort( conductivityMatrix[i].begin(), conductivityMatrix[i].end() );
    }
}

template < typename T >
void HexagonalCellBlock< T >::GetSurfaceAreas( const vector< CutCylElement< T > * > &helpElements,
                                               vector< IndexedArea< T > > &surfaceAreas ) const
{
    surfaceAreas.clear();
    surfaceAreas.reserve( mNPhi * ( 2 + mNZ + 2 * mNRhoCell ) );

    // Plane for lower base area of block
    const Plane< T > lowerPlane( mRadialEmplacementPoint, UnitVector< T >( 1.0, 0.0, 0.0 ), UnitVector< T >( 0.0, 1.0, 0.0 ) );
    // Plane for upper base area of block
    const Plane< T > upperPlane( mRadialEmplacementPoint + Cartesian< T >( 0.0, 0.0, mHeight ),
                                 UnitVector< T >( 1.0, 0.0, 0.0 ), UnitVector< T >( 0.0, 1.0, 0.0 ) );
    vector< TwoDim< T > > vertices;
    vertices.reserve( 4 );
    map< size_t, TwoDim< T > > arcs;


    size_t index = 0;
    vertices.resize( 3 );
    arcs[1] = TwoDim< T >( 0.0, 0.0 );
    // Construction of surface areas at lower and upper base area of block that are adjacent to cylindric symmetry axis
    // of battery cell
    for ( size_t j = 0; j < mNPhi; ++j )
    {
        vertices[0] = TwoDim<>( 0.0, 0.0 );
        vertices[1] = TwoDim<>( mRadiusCellDelta, Angle<>::Rad( mPhiDelta * j ) );
        vertices[2] = TwoDim<>( mRadiusCellDelta, Angle<>::Rad( mPhiDelta * ( j + 1 ) ) );

        surfaceAreas.push_back(
         IndexedArea< T >( index, Area< T >( vertices, lowerPlane, mZDelta * 0.5, mCellMaterial->GetConductivity( 2 ),
                                             BOTTOM, arcs, mArcPolygonEdgesLength ) ) );
        surfaceAreas.push_back( IndexedArea< T >( index + mNZLayer * ( mNZ - 1 ),
                                                  Area< T >( vertices, upperPlane, mZDelta * 0.5, mCellMaterial->GetConductivity( 2 ),
                                                             TOP, arcs, mArcPolygonEdgesLength ) ) );
        ++index;
    }

    vertices.resize( 4 );
    arcs.clear();
    arcs[0] = arcs[2] = TwoDim< T >( 0.0, 0.0 );
    // Construction of surface areas at lower and upper base area of block that are adjacent to battery cell
    for ( size_t i = 1; i < mNRhoCell; ++i )
    {
        for ( size_t j = 0; j < mNPhi; ++j )
        {
            vertices[0] = TwoDim<>( mRadiusCellDelta * i, Angle<>::Rad( mPhiDelta * j ) );
            vertices[1] = TwoDim<>( mRadiusCellDelta * i, Angle<>::Rad( mPhiDelta * ( j + 1 ) ) );
            vertices[2] = TwoDim<>( mRadiusCellDelta * ( i + 1 ), Angle<>::Rad( mPhiDelta * ( j + 1 ) ) );
            vertices[3] = TwoDim<>( mRadiusCellDelta * ( i + 1 ), Angle<>::Rad( mPhiDelta * j ) );

            surfaceAreas.push_back(
             IndexedArea< T >( index, Area< T >( vertices, lowerPlane, mZDelta * 0.5, mCellMaterial->GetConductivity( 2 ),
                                                 BOTTOM, arcs, mArcPolygonEdgesLength ) ) );
            surfaceAreas.push_back(
             IndexedArea< T >( index + mNZLayer * ( mNZ - 1 ),
                               Area< T >( vertices, upperPlane, mZDelta * 0.5, mCellMaterial->GetConductivity( 2 ), TOP,
                                          arcs, mArcPolygonEdgesLength ) ) );
            ++index;
        }
    }

    vertices.resize( 4 );
    arcs.clear();
    arcs[0] = TwoDim< T >( 0.0, 0.0 );
    // Construction of surface areas at lower and upper base area of block that are not adjacent to battery cell
    for ( size_t j = 0; j < mNPhi; ++j )
    {
        vertices[0] = TwoDim<>( mRadiusCell, Angle<>::Rad( mPhiDelta * j ) );
        vertices[1] = TwoDim<>( mRadiusCell, Angle<>::Rad( mPhiDelta * ( j + 1 ) ) );
        vertices[2] = helpElements[j]->GetStraightEdgeVertexDelta();
        vertices[3] = helpElements[j]->GetStraightEdgeVertexStart();

        surfaceAreas.push_back(
         IndexedArea< T >( index, Area< T >( vertices, lowerPlane, mZDelta * 0.5, mFillMaterial->GetConductivity( 2 ),
                                             BOTTOM, arcs, mArcPolygonEdgesLength ) ) );
        surfaceAreas.push_back( IndexedArea< T >( index + mNZLayer * ( mNZ - 1 ),
                                                  Area< T >( vertices, upperPlane, mZDelta * 0.5, mFillMaterial->GetConductivity( 2 ),
                                                             TOP, arcs, mArcPolygonEdgesLength ) ) );
        ++index;
    }


    vertices[0] = TwoDim< T >( 0.0, 0.0 );
    const UnitVector< T > zAxis( 0.0, 0.0, 1.0 );
    // Construction of surface areas at side areas of block
    for ( size_t i = 0; i < mNZ; ++i )
        for ( size_t j = 0; j < mNPhi; ++j )
        {
            TwoDim< T > horizontalEdge =
             helpElements[j]->GetStraightEdgeVertexDelta() - helpElements[j]->GetStraightEdgeVertexStart();
            vertices[1] = TwoDim< T >( horizontalEdge.Length(), 0.0 );
            vertices[2] = TwoDim< T >( horizontalEdge.Length(), mZDelta );
            vertices[3] = TwoDim< T >( 0.0, mZDelta );

            surfaceAreas.push_back( IndexedArea< T >(
             ( i + 1 ) * mNZLayer - mNPhi + j,
             Area< T >( vertices, Plane< T >( mRadialEmplacementPoint +
                                               Cartesian< T >( helpElements[j]->GetStraightEdgeVertexStart().Get1(),
                                                               helpElements[j]->GetStraightEdgeVertexStart().Get2(), mZDelta * i ),
                                              UnitVector< T >( horizontalEdge.Get1(), horizontalEdge.Get2(), 0.0 ), zAxis ),
                        helpElements[j]->GetRhoPosDistance(), mFillMaterial->GetConductivity( 0 ), SIDE ) ) );
        }
}

template < typename T >
void HexagonalCellBlock< T >::GetBlockGeometry( shared_ptr< BlockGeometry< T > > &blockGeometry ) const
{
    // Base area of block
    vector< TwoDim< T > > vertices( 6 );
    vertices[0] = TwoDim< T >( mRadialEmplacementPoint.GetX() + mLength, mRadialEmplacementPoint.GetY() );
    vertices[1] =
     TwoDim< T >( mRadialEmplacementPoint.GetX() + mLength * 0.5, mRadialEmplacementPoint.GetY() + mLength * sqrt( 0.75 ) );
    vertices[2] =
     TwoDim< T >( mRadialEmplacementPoint.GetX() - mLength * 0.5, mRadialEmplacementPoint.GetY() + mLength * sqrt( 0.75 ) );
    vertices[3] = TwoDim< T >( mRadialEmplacementPoint.GetX() - mLength, mRadialEmplacementPoint.GetY() );
    vertices[4] =
     TwoDim< T >( mRadialEmplacementPoint.GetX() - mLength * 0.5, mRadialEmplacementPoint.GetY() - mLength * sqrt( 0.75 ) );
    vertices[5] =
     TwoDim< T >( mRadialEmplacementPoint.GetX() + mLength * 0.5, mRadialEmplacementPoint.GetY() - mLength * sqrt( 0.75 ) );

    blockGeometry.reset( new BlockGeometry< T >( Geometry2D< T >( vertices ), mRadialEmplacementPoint.GetZ(),
                                                 mRadialEmplacementPoint.GetZ() + mHeight, ThermalBlock< T >::mDescription ) );
}
}
#endif
