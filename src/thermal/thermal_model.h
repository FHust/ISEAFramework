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
#ifndef _THERMAL_MODEL_
#define _THERMAL_MODEL_

// STD
#include <vector>
#include <list>
#include <algorithm>
#include <stdexcept>

// BOOST
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
// ETC
#include "thermal_structs.h"
#include "blocks/elements/thermal_element.h"
#include "boundaryConditions/cooling.h"

#include "../cstring/strlcat.h"

#include "../geometry/block_geometry.h"
#include "../geometry/tolerance.h"

#include "../exceptions/error_proto.h"

#include "../misc/get_position_in_vector.h"

#include "../probes/thermal_probe.h"

#ifndef __NO_STRING__
#include <fstream>
#endif


class TestThermalModels;

namespace thermal
{
using namespace geometry;
using boost::shared_ptr;
using std::vector;
using std::list;
using std::pair;

/// ThermalModel gathers and fuses all information of the thermal model and returns data usable by the equation solver
template < typename T = double >
class ThermalModel
{
    friend class ::TestThermalModels;

    struct BlockData
    {
        vector< shared_ptr< ThermalElement< T > > > mThermalElements;
        vector< vector< IndexedValue< T > > > mConductivityMatrix;
        vector< IndexedArea< T > > mSurfaceAreas;
        shared_ptr< BlockGeometry< T > > mBlockGeometry;
        misc::StrCont mVolumeName;
        vector< IndexedInnerArea< T > > mInnerSurfaceAreas;
    };
    struct CoolingData
    {
        vector< Area< T > > mCoolingAreas;
        shared_ptr< BlockGeometry< T > > mBlockGeometry;
        shared_ptr< Cooling< T > > mCooling;
    };
    struct ExternalAreaData
    {
        size_t mElementIndex;
        Area< T > *mAreaPointer;
        T mRestAreaValue;
        size_t mBlockIndex;
    };
    struct CooledAreaData
    {
        size_t mElementIndex;
        Area< T > *mAreaPointer;
        T mRestAreaValue;
        Cooling< T > *mCooling;
    };
    struct IndexedAreasByLocation
    {
        vector< IndexedArea< T > * > mTop;
        vector< IndexedArea< T > * > mSide;
        vector< IndexedArea< T > * > mBottom;
    };
    struct AreasByLocation
    {
        vector< Area< T > * > mTop;
        vector< Area< T > * > mSide;
        vector< Area< T > * > mBottom;
    };
    typedef pair< size_t, AdjacencyType > IndexedAdjacency;

    public:
    /// Determines in which way surface areas are aggregated into a common area. The characteristic length of the
    /// convection at the surface areas is calculated with the common area.
    enum AggregateAreasForConvection
    {
        AGGREGATE_BY_PLANE    ///<All areas, that are coplanar, are aggregated
        ,
        AGGREGATE_BY_PLANE_AND_BLOCKS    ///<In addition, the adjacency of thermal blocks, on which the surface areas are located, is taken into account
                                         //        , AGGREGATE_BY_PLANE_AND_BLOCKS_AND_AREAS///<///<In addition, the adjacency of the surface areas themselves is taken into account
    };
    explicit ThermalModel( const Tolerance< T > &tolerance, AggregateAreasForConvection aggregateAreasForConvection );
    virtual ~ThermalModel();
    void ClearAndSetNumberOfBlocksAndCoolings( size_t numberOfBlocks, size_t numberOfCoolings );
    void AddBlock( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                   vector< vector< IndexedValue< T > > > &conductivityMatrix, vector< IndexedArea< T > > &surfaceAreas,
                   shared_ptr< BlockGeometry< T > > &blockGeometry, misc::StrCont volumeName = "NoName" );
    void AddBlock( vector< shared_ptr< ThermalElement< T > > > &thermalElements, vector< vector< IndexedValue< T > > > &conductivityMatrix,
                   vector< IndexedArea< T > > &surfaceAreas, shared_ptr< BlockGeometry< T > > &blockGeometry,
                   vector< IndexedInnerArea< T > > &innerSurfaceAreas, misc::StrCont volumeName = "NoName" );
    void AddCooling( vector< Area< T > > &coolingAreas, shared_ptr< BlockGeometry< T > > &blockGeometry,
                     shared_ptr< Cooling< T > > &cooling );
    /// Outputs data usable by the equation solver
    void CreateDataByFusingBlocks( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                   vector< vector< IndexedValue< T > > > &conductivityMatrix,
                                   vector< vector< TaylorData< T > > > &coolingDataVector,
                                   vector< vector< TaylorData< T > > > &dirichletDataVector );
    void CreateDataForVisualization( vector< ThermalElement< T > * > &thermalElementsOfAreas, vector< vector< size_t > > &areas,
                                     vector< vector< size_t > > &volumes, vector< misc::StrCont > &volumeNames,
                                     vector< Cartesian< T > > &vertices, bool showInnerSurfaces = false ) const;
    /**
     * Checks each thermal block and cooling block for their dimension to be valid. That means:
     * 1. Extension in z-direction and smallest length in polygon must be bigger than tolerance and
     * 2. side areas of block must be distant enough to not be considered overlapping by tolerance
     */
    void BlocksDimensionTest() const;
    /// Checks if any of the thermal blocks or cooling blocks collide
    void BlocksCollisionTest() const;
    /// Connect Probe and Thermal Element
    void ProbeThermalElemnts( std::vector< ::probe::ThermalProbe > &thermalProbes ) const;

    private:
    template < typename AreaStruct >
    void CreateVisualizationDataForOneArea( vector< vector< size_t > > &areas, vector< Cartesian< T > > &vertices,
                                            const AreaStruct &actualArea ) const;
    void CreateAdjacencyMatrices( vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                                  vector< vector< IndexedAdjacency > > &adjacencyMatrixCoolings ) const;
    size_t DetermineOdeSystemSize() const;
    void CreateThermalElementVector( size_t odeSystemSize, vector< shared_ptr< ThermalElement< T > > > &thermalElementVector ) const;
    void CreateBlockStartIndices( vector< size_t > &blockStartIndices ) const;
    void FuseBlocksInnerConductivities( const vector< size_t > &blockStartIndices, size_t odeSystemSize,
                                        vector< vector< IndexedValue< T > > > &conductivityMatrix ) const;
    void FuseBlocksOuterConductivitiesAndFindExternalAreas( const vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                                                            const vector< vector< IndexedAdjacency > > &adjacencyMatrixCoolings,
                                                            const vector< size_t > &blockStartIndices,
                                                            vector< ExternalAreaData > &externalAreaDataVector,
                                                            vector< CooledAreaData > &cooledAreaDataVector,
                                                            vector< vector< IndexedValue< T > > > &conductivityMatrix );
    void SortAndCheckConductivityMatrix( vector< vector< IndexedValue< T > > > &conductivityMatrix );
    void CreateCoolingData( const vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                            const vector< ExternalAreaData > &externalAreaDataVector, const vector< CooledAreaData > &cooledAreaDataVector,
                            size_t odeSystemSize, vector< vector< TaylorData< T > > > &coolingDataVector,
                            vector< vector< TaylorData< T > > > &dirichletDataVector );
    /// Sort areas that are coplanar (on the same plane) and have the same location (TOP, BOTTOM or SIDE)
    void SortExternalAreasBySamePlane( const vector< ExternalAreaData > &externalAreaDataVector,
                                       list< vector< const ExternalAreaData * > > &areasSortedBySamePlane ) const;
    /// Further separate coplanar areas that are not connected by block adjacency
    void FurtherSortExternalAreasByBlockAdjacency( const vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                                                   list< vector< const ExternalAreaData * > > &areasSortedBySamePlane ) const;
    /// Further sort one vector of external areas that are not connected by block adjacency
    void SortOneExternalAreaVectorByBlockAdjacency( const vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                                                    const vector< const ExternalAreaData * > externalAreaDataVector,
                                                    vector< vector< size_t > > &sortedData ) const;
    /// Get data for explicitly defined cooling
    void CreateForcedCoolingData( const vector< CooledAreaData > &cooledAreaDataVector,
                                  vector< vector< TaylorData< T > > > &coolingDataVector,
                                  vector< vector< TaylorData< T > > > &dirichletDataVector );

    const Tolerance< T > mTolerance;
    AggregateAreasForConvection mAggregateAreasForConvection;
    vector< BlockData > mBlocks;
    vector< CoolingData > mCoolings;
};


template < typename T >
ThermalModel< T >::ThermalModel( const Tolerance< T > &tolerance, AggregateAreasForConvection aggregateAreasForConvection )
    : mTolerance( tolerance )
    , mAggregateAreasForConvection( aggregateAreasForConvection )
{
}

template < typename T >
ThermalModel< T >::~ThermalModel()
{
}

template < typename T >
void ThermalModel< T >::ClearAndSetNumberOfBlocksAndCoolings( size_t numberOfBlocks, size_t numberOfCoolings )
{
    mBlocks.clear();
    mBlocks.reserve( numberOfBlocks );
    mCoolings.clear();
    mCoolings.reserve( numberOfCoolings );
}

template < typename T >
void ThermalModel< T >::AddBlock( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                  vector< vector< IndexedValue< T > > > &conductivityMatrix, vector< IndexedArea< T > > &surfaceAreas,
                                  shared_ptr< BlockGeometry< T > > &blockGeometry, misc::StrCont volumeName )
{
    if ( thermalElements.size() != conductivityMatrix.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                             "thermalElementsconductivityMatrixUnEqual" );
    if ( std::max_element( surfaceAreas.begin(), surfaceAreas.end() )->mIndex >= thermalElements.size() )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfRangeIndex" );

    mBlocks.push_back( BlockData() );
    thermalElements.swap( mBlocks.back().mThermalElements );
    conductivityMatrix.swap( mBlocks.back().mConductivityMatrix );
    surfaceAreas.swap( mBlocks.back().mSurfaceAreas );
    blockGeometry.swap( mBlocks.back().mBlockGeometry );
    volumeName.swap( mBlocks.back().mVolumeName );
}

template < typename T >
void ThermalModel< T >::AddBlock( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                  vector< vector< IndexedValue< T > > > &conductivityMatrix,
                                  vector< IndexedArea< T > > &surfaceAreas, shared_ptr< BlockGeometry< T > > &blockGeometry,
                                  vector< IndexedInnerArea< T > > &innerSurfaceAreas, misc::StrCont volumeName )
{
    AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry, volumeName );
    if ( !innerSurfaceAreas.empty() )
        innerSurfaceAreas.swap( mBlocks.back().mInnerSurfaceAreas );
}

template < typename T >
void ThermalModel< T >::AddCooling( vector< Area< T > > &coolingAreas, shared_ptr< BlockGeometry< T > > &blockGeometry,
                                    shared_ptr< Cooling< T > > &cooling )
{
    mCoolings.push_back( CoolingData() );
    coolingAreas.swap( mCoolings.back().mCoolingAreas );
    blockGeometry.swap( mCoolings.back().mBlockGeometry );
    cooling.swap( mCoolings.back().mCooling );
}

template < typename T >
void ThermalModel< T >::CreateDataByFusingBlocks( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                                  vector< vector< IndexedValue< T > > > &conductivityMatrix,
                                                  vector< vector< TaylorData< T > > > &coolingDataVector,
                                                  vector< vector< TaylorData< T > > > &dirichletDataVector )
{
    BlocksDimensionTest();
    BlocksCollisionTest();

    conductivityMatrix.clear();
    coolingDataVector.clear();
    dirichletDataVector.clear();

    const size_t odeSystemSize = DetermineOdeSystemSize();

    CreateThermalElementVector( odeSystemSize, thermalElements );

    vector< size_t > blockStartIndices;
    CreateBlockStartIndices( blockStartIndices );

    FuseBlocksInnerConductivities( blockStartIndices, odeSystemSize, conductivityMatrix );

    vector< vector< IndexedAdjacency > > adjacencyMatrix;
    vector< vector< IndexedAdjacency > > adjacencyMatrixCoolings;
    CreateAdjacencyMatrices( adjacencyMatrix, adjacencyMatrixCoolings );

    vector< ExternalAreaData > externalAreaDataVector;
    vector< CooledAreaData > cooledAreaDataVector;
    FuseBlocksOuterConductivitiesAndFindExternalAreas( adjacencyMatrix, adjacencyMatrixCoolings, blockStartIndices,
                                                       externalAreaDataVector, cooledAreaDataVector, conductivityMatrix );
    SortAndCheckConductivityMatrix( conductivityMatrix );

    CreateCoolingData( adjacencyMatrix, externalAreaDataVector, cooledAreaDataVector, odeSystemSize, coolingDataVector,
                       dirichletDataVector );
}

template < typename T >
void ThermalModel< T >::CreateDataForVisualization( vector< ThermalElement< T > * > &thermalElementsOfAreas,
                                                    vector< vector< size_t > > &areas, vector< vector< size_t > > &volumes,
                                                    vector< misc::StrCont > &volumeNames,
                                                    vector< Cartesian< T > > &vertices, bool showInnerSurfaces ) const
{
    thermalElementsOfAreas.clear();
    volumes.clear();
    volumeNames.clear();
    areas.clear();
    vertices.clear();

    size_t numberOfAreas = 0;
    const size_t numberOfVolumes = mBlocks.size();
    BOOST_FOREACH ( const BlockData &block, mBlocks )
    {
        numberOfAreas += block.mSurfaceAreas.size();
    }
    areas.reserve( numberOfAreas );
    thermalElementsOfAreas.reserve( numberOfAreas );
    volumes.reserve( numberOfVolumes );
    volumeNames.reserve( numberOfVolumes );

    size_t areaToVolumeIndex = 0;
    BOOST_FOREACH ( const BlockData &block, mBlocks )
    {
        volumes.push_back( vector< size_t >() );
        volumeNames.push_back( block.mVolumeName );
        BOOST_FOREACH ( const IndexedArea< T > &surfaceArea, block.mSurfaceAreas )
        {
            volumes.back().push_back( ++areaToVolumeIndex );
            thermalElementsOfAreas.push_back( block.mThermalElements[surfaceArea.mIndex].get() );

            CreateVisualizationDataForOneArea< IndexedArea< T > >( areas, vertices, surfaceArea );
        }
        if ( showInnerSurfaces && !block.mInnerSurfaceAreas.empty() )
        {
            volumes.push_back( vector< size_t >() );
            volumeNames.push_back( misc::StrCont( "INNER: " ) + block.mVolumeName );
            BOOST_FOREACH ( const IndexedInnerArea< T > &innerSurfaceArea, block.mInnerSurfaceAreas )
            {
                volumes.back().push_back( ++areaToVolumeIndex );
                thermalElementsOfAreas.push_back( block.mThermalElements[innerSurfaceArea.mIndex].get() );

                CreateVisualizationDataForOneArea< IndexedInnerArea< T > >( areas, vertices, innerSurfaceArea );
            }
        }
    }
}

template < typename T >
template < typename AreaStruct >
void ThermalModel< T >::CreateVisualizationDataForOneArea( vector< vector< size_t > > &areas, vector< Cartesian< T > > &vertices,
                                                           const AreaStruct &actualArea ) const
{

    vector< Cartesian< T > > verticesOfCurrentArea;
    actualArea.mArea.GetVertices( verticesOfCurrentArea );
    areas.push_back( vector< size_t >() );

    BOOST_FOREACH ( const Cartesian< T > &vertexOfCurrentArea, verticesOfCurrentArea )
    {
        bool vertexFound = false;

        for ( size_t i = 0; i < vertices.size(); ++i )
            if ( vertexOfCurrentArea.Equals( vertices[i], mTolerance.mLength ) )
            {
                areas.back().push_back( i + 1 );
                vertexFound = true;
                break;
            }

        if ( !vertexFound )
        {
            vertices.push_back( vertexOfCurrentArea );
            areas.back().push_back( vertices.size() );
        }
    }
}

template < typename T >
void ThermalModel< T >::BlocksDimensionTest() const
{
    for ( size_t i = 0; i < mBlocks.size(); ++i )
        if ( mBlocks[i].mBlockGeometry->HasInsufficientDimension( mTolerance ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DimensionSmallerTolerance",
                                                 mBlocks[i].mBlockGeometry->GetDescription() );

    for ( size_t i = 0; i < mCoolings.size(); ++i )
        if ( mCoolings[i].mBlockGeometry->HasInsufficientDimension( mTolerance ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DimensionSmallerTolerance",
                                                 mCoolings[i].mBlockGeometry->GetDescription() );
}

template < typename T >
void ThermalModel< T >::BlocksCollisionTest() const
{
    for ( size_t i = 0; i < mBlocks.size(); ++i )
    {
        for ( size_t j = i + 1; j < mBlocks.size(); ++j )
            if ( mBlocks[i].mBlockGeometry->CollidesWith( *mBlocks[j].mBlockGeometry, mTolerance ) )
            {
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "CollisionThermalBlocks",
                                                     mBlocks[i].mBlockGeometry->GetDescription(),
                                                     mBlocks[j].mBlockGeometry->GetDescription() );
            }


        for ( size_t j = 0; j < mCoolings.size(); ++j )
            if ( mBlocks[i].mBlockGeometry->CollidesWith( *mCoolings[j].mBlockGeometry, mTolerance ) )
            {
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "CollisionThermalCoolingBlocks",
                                                     mBlocks[i].mBlockGeometry->GetDescription(),
                                                     mCoolings[j].mBlockGeometry->GetDescription() );
            }
    }

    for ( size_t i = 0; i < mCoolings.size(); ++i )
        for ( size_t j = i + 1; j < mCoolings.size(); ++j )
            if ( mCoolings[i].mBlockGeometry->CollidesWith( *mCoolings[j].mBlockGeometry, mTolerance ) )
            {
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "CollisionCoolingBlocks",
                                                     mCoolings[i].mBlockGeometry->GetDescription(),
                                                     mCoolings[j].mBlockGeometry->GetDescription() );
            }
}

template < typename T >
void ThermalModel< T >::CreateAdjacencyMatrices( vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                                                 vector< vector< IndexedAdjacency > > &adjacencyMatrixCoolings ) const
{
    adjacencyMatrix.resize( mBlocks.size() );
    adjacencyMatrixCoolings.resize( mBlocks.size() );


    for ( size_t i = 0; i < mBlocks.size(); ++i )
    {
        for ( size_t j = i + 1; j < mBlocks.size(); ++j )
        {
            AdjacencyType adjacencyType = mBlocks[i].mBlockGeometry->IsAdjacentTo( *mBlocks[j].mBlockGeometry, mTolerance );
            if ( adjacencyType != NOT_ADJACENT )
                adjacencyMatrix[i].push_back( pair< size_t, AdjacencyType >( j, adjacencyType ) );
        }

        for ( size_t j = 0; j < mCoolings.size(); ++j )
        {
            AdjacencyType adjacencyType = mBlocks[i].mBlockGeometry->IsAdjacentTo( *mCoolings[j].mBlockGeometry, mTolerance );
            if ( adjacencyType != NOT_ADJACENT )
                adjacencyMatrixCoolings[i].push_back( pair< size_t, AdjacencyType >( j, adjacencyType ) );
        }
    }
}

template < typename T >
size_t ThermalModel< T >::DetermineOdeSystemSize() const
{
    size_t odeSystemSize = 0;

    BOOST_FOREACH ( const BlockData &block, mBlocks )
        odeSystemSize += block.mThermalElements.size();

    return odeSystemSize;
}

template < typename T >
void ThermalModel< T >::CreateThermalElementVector( size_t odeSystemSize,
                                                    vector< shared_ptr< ThermalElement< T > > > &thermalElementVector ) const
{
    thermalElementVector.clear();
    thermalElementVector.reserve( odeSystemSize );

    BOOST_FOREACH ( const BlockData &block, mBlocks )
        thermalElementVector.insert( thermalElementVector.end(), block.mThermalElements.begin(), block.mThermalElements.end() );
}

template < typename T >
void ThermalModel< T >::CreateBlockStartIndices( vector< size_t > &blockStartIndices ) const
{
    if ( mBlocks.empty() )
        return;

    blockStartIndices.resize( mBlocks.size() );
    blockStartIndices[0] = 0;

    for ( size_t i = 1; i < blockStartIndices.size(); ++i )
        blockStartIndices[i] = blockStartIndices[i - 1] + mBlocks[i - 1].mThermalElements.size();
}

template < typename T >
void ThermalModel< T >::FuseBlocksInnerConductivities( const vector< size_t > &blockStartIndices, size_t odeSystemSize,
                                                       vector< vector< IndexedValue< T > > > &conductivityMatrix ) const
{
    conductivityMatrix.clear();
    conductivityMatrix.reserve( odeSystemSize );

    for ( size_t i = 0; i < mBlocks.size(); ++i )
    {
        conductivityMatrix.insert( conductivityMatrix.end(), mBlocks[i].mConductivityMatrix.begin(),
                                   mBlocks[i].mConductivityMatrix.end() );

        for ( size_t j = blockStartIndices[i]; j < conductivityMatrix.size(); ++j )
            BOOST_FOREACH ( IndexedValue< T > &conductivity, conductivityMatrix[j] )
                conductivity.mIndex += blockStartIndices[i];
    }
}

template < typename T >
void ThermalModel< T >::FuseBlocksOuterConductivitiesAndFindExternalAreas(
 const vector< vector< IndexedAdjacency > > &adjacencyMatrix, const vector< vector< IndexedAdjacency > > &adjacencyMatrixCoolings,
 const vector< size_t > &blockStartIndices, vector< ExternalAreaData > &externalAreaDataVector,
 vector< CooledAreaData > &cooledAreaDataVector, vector< vector< IndexedValue< T > > > &conductivityMatrix )
{
    externalAreaDataVector.clear();
    cooledAreaDataVector.clear();

    vector< IndexedAreasByLocation > sortedSurfaceAreas( mBlocks.size() );
    for ( size_t i = 0; i < mBlocks.size(); ++i )
        BOOST_FOREACH ( IndexedArea< T > &surfaceArea, mBlocks[i].mSurfaceAreas )
        {
            // mAreaNotCovered determines the area of the surface areas, that is not covered by other areas
            surfaceArea.mAreaNotCovered = surfaceArea.mArea.GetAreaValue();

            // Order surfaceAreas by location for faster evaluation
            switch ( surfaceArea.mArea.GetLocation() )
            {
                case TOP:
                    sortedSurfaceAreas[i].mTop.push_back( &surfaceArea );
                    break;
                case SIDE:
                    sortedSurfaceAreas[i].mSide.push_back( &surfaceArea );
                    break;
                case BOTTOM:
                    sortedSurfaceAreas[i].mBottom.push_back( &surfaceArea );
                    break;
                default:
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "IndexOutOfDefinition1" );
                    break;
            }
        }

    vector< AreasByLocation > sortedCoolingAreas( mCoolings.size() );
    for ( size_t i = 0; i < mCoolings.size(); ++i )
    {
        BOOST_FOREACH ( Area< T > &coolingArea, mCoolings[i].mCoolingAreas )
        {
            // Order coolingAreas by location for faster evaluation
            switch ( coolingArea.GetLocation() )
            {
                case TOP:
                    sortedCoolingAreas[i].mTop.push_back( &coolingArea );
                    break;
                case SIDE:
                    sortedCoolingAreas[i].mSide.push_back( &coolingArea );
                    break;
                case BOTTOM:
                    sortedCoolingAreas[i].mBottom.push_back( &coolingArea );
                    break;
                default:
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "IndexOutOfDefinition2" );
                    break;
            }
        }
    }

    // Compare surface areas with adjacent surface areas
    for ( size_t i = 0; i < adjacencyMatrix.size(); ++i )
        BOOST_FOREACH ( const IndexedAdjacency &adjacency, adjacencyMatrix[i] )
        {
            const size_t &j = adjacency.first;    // index of other block

            // Find the areas to compare using adjacencyMatrix
            vector< IndexedArea< T > * > surfaceAreas;
            vector< IndexedArea< T > * > otherSurfaceAreas;
            switch ( adjacency.second )
            {
                case ADJACENT_BOTTOM_TOP:
                    surfaceAreas = sortedSurfaceAreas[i].mBottom;
                    otherSurfaceAreas = sortedSurfaceAreas[j].mTop;
                    break;
                case ADJACENT_SIDE_SIDE:
                    surfaceAreas = sortedSurfaceAreas[i].mSide;
                    otherSurfaceAreas = sortedSurfaceAreas[j].mSide;
                    break;
                case ADJACENT_TOP_BOTTOM:
                    surfaceAreas = sortedSurfaceAreas[i].mTop;
                    otherSurfaceAreas = sortedSurfaceAreas[j].mBottom;
                    break;
                default:
                    break;
            }

            // Iterate over both area sets
            BOOST_FOREACH ( IndexedArea< T > *area, surfaceAreas )
                BOOST_FOREACH ( IndexedArea< T > *otherArea, otherSurfaceAreas )
                {
                    const T overlap = area->mArea.OverlapWith( otherArea->mArea, mTolerance );
                    if ( overlap > 0.0 )
                    {
                        // If areas overlap, the overlap needs to be subtracted from both areas
                        area->mAreaNotCovered -= overlap;
                        otherArea->mAreaNotCovered -= overlap;

                        // If areas overlap, they are connected mutually by conductivity
                        T conductivity = 0.0;
                        if ( fabs( area->mArea.GetConductivity() / otherArea->mArea.GetConductivity() - 1.0 ) <
                             mTolerance.mPercentOfQuantity / 100.0 )    // if both conductivities are equal
                            conductivity = overlap * area->mArea.GetConductivity() /
                                           ( area->mArea.GetDistanceToGridVertex() + otherArea->mArea.GetDistanceToGridVertex() );
                        else
                            conductivity =
                             overlap / ( area->mArea.GetDistanceToGridVertex() / area->mArea.GetConductivity() +
                                         otherArea->mArea.GetDistanceToGridVertex() / otherArea->mArea.GetConductivity() );

                        // Write date into conductivity matrix
                        const size_t elementIndex = area->mIndex + blockStartIndices[i];
                        const size_t otherElementIndex = otherArea->mIndex + blockStartIndices[j];
                        // Mutual conductivities
                        conductivityMatrix[elementIndex].push_back( MakeIndexedValue< T >( otherElementIndex, conductivity ) );
                        conductivityMatrix[otherElementIndex].push_back( MakeIndexedValue< T >( elementIndex, conductivity ) );
                        // Self conductivity
                        typename vector< IndexedValue< T > >::iterator it =
                         std::find_if( conductivityMatrix[elementIndex].begin(), conductivityMatrix[elementIndex].end(),
                                       IndexIs< T >( elementIndex ) );
                        if ( it != conductivityMatrix[elementIndex].end() )
                            it->mValue -= conductivity;
                        else
                            conductivityMatrix[elementIndex].push_back( MakeIndexedValue< T >( elementIndex, -conductivity ) );
                        // Other self conductivity
                        it = std::find_if( conductivityMatrix[otherElementIndex].begin(),
                                           conductivityMatrix[otherElementIndex].end(), IndexIs< T >( otherElementIndex ) );
                        if ( it != conductivityMatrix[otherElementIndex].end() )
                            it->mValue -= conductivity;
                        else
                            conductivityMatrix[otherElementIndex].push_back( MakeIndexedValue< T >( otherElementIndex, -conductivity ) );
                    }
                }
        }

    // Compare surface areas with adjacent cooling areas
    for ( size_t i = 0; i < adjacencyMatrixCoolings.size(); ++i )
        BOOST_FOREACH ( const IndexedAdjacency &adjacency, adjacencyMatrixCoolings[i] )
        {
            // Find the areas to compare using adjacencyMatrixCoolings
            vector< IndexedArea< T > * > surfaceAreas;
            vector< Area< T > * > coolingAreas;
            switch ( adjacency.second )
            {
                case ADJACENT_BOTTOM_TOP:
                    surfaceAreas = sortedSurfaceAreas[i].mBottom;
                    coolingAreas = sortedCoolingAreas[adjacency.first].mTop;
                    break;
                case ADJACENT_SIDE_SIDE:
                    surfaceAreas = sortedSurfaceAreas[i].mSide;
                    coolingAreas = sortedCoolingAreas[adjacency.first].mSide;
                    break;
                case ADJACENT_TOP_BOTTOM:
                    surfaceAreas = sortedSurfaceAreas[i].mTop;
                    coolingAreas = sortedCoolingAreas[adjacency.first].mBottom;
                    break;
                default:
                    break;
            }

            // Iterate over both area sets
            BOOST_FOREACH ( Area< T > *coolingArea, coolingAreas )
                BOOST_FOREACH ( IndexedArea< T > *surfaceArea, surfaceAreas )
                {
                    const T overlap = surfaceArea->mArea.OverlapWith( *coolingArea, mTolerance );

                    // If surface area and cooling area overlap, the corresponding cooling data is created
                    if ( overlap > 0.0 )
                    {
                        const CooledAreaData cooledArea = {surfaceArea->mIndex + blockStartIndices[i], &surfaceArea->mArea,
                                                           overlap, mCoolings[adjacency.first].mCooling.get()};
                        cooledAreaDataVector.push_back( cooledArea );
                        surfaceArea->mAreaNotCovered -= overlap;
                    }
                }
        }

    // Check remaining uncovered parts of areas and create externalAreaDataVector
    for ( size_t i = 0; i < mBlocks.size(); ++i )
    {
        BOOST_FOREACH ( IndexedArea< T > &area, mBlocks[i].mSurfaceAreas )
        {
            T areaTolerance = area.mArea.GetAreaValue() * mTolerance.mPercentOfQuantity / 100.0;

            if ( area.mAreaNotCovered < -areaTolerance )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "OverlappingAreaBiggerActualOne" );

            // If area is not completely covered, default convection and radiation will be applied
            if ( area.mAreaNotCovered > areaTolerance )
            {
                const ExternalAreaData externalArea = {area.mIndex + blockStartIndices[i], &area.mArea, area.mAreaNotCovered, i};
                externalAreaDataVector.push_back( externalArea );
            }
        }
    }
}

template < typename T >
void ThermalModel< T >::SortAndCheckConductivityMatrix( vector< vector< IndexedValue< T > > > &conductivityMatrix )
{
    for ( size_t i = 0; i < conductivityMatrix.size(); ++i )
    {
        std::sort( conductivityMatrix[i].begin(), conductivityMatrix[i].end() );

        const size_t *lastIndex = 0;
        BOOST_FOREACH ( const IndexedValue< T > &conductivity, conductivityMatrix[i] )
        {
            if ( lastIndex )
                if ( *lastIndex == conductivity.mIndex )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "IndexOccuredTwice" );
            lastIndex = &conductivity.mIndex;

            if ( conductivity.mIndex == i )
            {
                if ( conductivity.mValue >= 0.0 )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SelfConductivityPositive" );
            }
            else
            {
                if ( conductivity.mValue <= 0.0 )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NegativeConductivity" );
            }
        }
    }
}

template < typename T >
void ThermalModel< T >::SortOneExternalAreaVectorByBlockAdjacency( const vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                                                                   const vector< const ExternalAreaData * > externalAreaDataVector,
                                                                   vector< vector< size_t > > &sortedData ) const
{
    typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS > Graph;

    // This would sort areas by adjacency of the area itself
    /* size_t numberOfVertices = externalAreaDataVector.size();
    Graph graph(numberOfVertices);

    for(size_t i = 0; i < externalAreaDataVector.size(); ++i)
        for(size_t j = i + 1; i < externalAreaDataVector.size(); ++i)
            if(externalAreaDataVector[i]->mAreaPointer->IsAdjacentTo(*externalAreaDataVector[j]->mAreaPointer,
    mTolerance)
            || externalAreaDataVector[i]->mAreaPointer->OverlapWith(*externalAreaDataVector[j]->mAreaPointer,
    mTolerance) == 0.0)
                boost::add_edge(i, j, graph);

    vector<int> commonPlanesIndexedExternalArea(numberOfVertices);
    size_t numberOfCommonPlanes = boost::connected_components(graph, &commonPlanesIndexedExternalArea[0]);

    sortedData.clear();
    sortedData.resize(numberOfCommonPlanes);
    for(size_t i = 0; i < numberOfVertices; ++i)
        sortedData[commonPlanesIndexedExternalArea[i]].push_back(i);*/

    // Get all block indices of externalAreaDataVector
    // The position of the block indices in the vector mappes the blocks for the graph by assigning each block index a
    // number, from 0 to blockIndices.size()
    vector< size_t > blockIndices;
    BOOST_FOREACH ( const ExternalAreaData *externalAreaDatum, externalAreaDataVector )
        if ( misc::GetPositionInVector( blockIndices, externalAreaDatum->mBlockIndex ) == blockIndices.size() )
            blockIndices.push_back( externalAreaDatum->mBlockIndex );


    // Put block indices into a graph structure that represents the connection of the areas on the outside of the
    // thermal blocks
    size_t numberOfVertices = blockIndices.size();
    Graph graph( numberOfVertices );

    for ( size_t i = 0; i < blockIndices.size(); ++i )
    {
        BOOST_FOREACH ( const IndexedAdjacency &adjacency, adjacencyMatrix[blockIndices[i]] )
        {
            size_t j = misc::GetPositionInVector( blockIndices, adjacency.first );

            if ( j != blockIndices.size() )
            {
                // Connection scheme is different for SIDE and TOP/BOTTOM areas
                if ( externalAreaDataVector.at( 0 )->mAreaPointer->GetLocation() == SIDE )
                {
                    if ( adjacency.second == ADJACENT_TOP_BOTTOM || adjacency.second == ADJACENT_BOTTOM_TOP )
                        boost::add_edge( i, j, graph );
                }
                else    // GetLocation() == TOP || GetLocation() == BOTTOM
                {
                    if ( adjacency.second == ADJACENT_SIDE_SIDE )
                        boost::add_edge( i, j, graph );
                }
            }
        }
    }


    // Use graph structure to determine connections
    vector< int > commonPlanesIndexedByBlock( numberOfVertices );
    size_t numberOfCommonPlanes = connected_components( graph, &commonPlanesIndexedByBlock[0] );

    // Get connections into sorted data for output
    sortedData.clear();
    sortedData.resize( numberOfCommonPlanes );
    for ( size_t i = 0; i < externalAreaDataVector.size(); ++i )
    {
        size_t mappedBlockIndex = misc::GetPositionInVector( blockIndices, externalAreaDataVector[i]->mBlockIndex );
        sortedData[commonPlanesIndexedByBlock[mappedBlockIndex]].push_back( i );
    }
}

template < typename T >
void ThermalModel< T >::CreateCoolingData( const vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                                           const vector< ExternalAreaData > &externalAreaDataVector,
                                           const vector< CooledAreaData > &cooledAreaDataVector, size_t odeSystemSize,
                                           vector< vector< TaylorData< T > > > &coolingDataVector,
                                           vector< vector< TaylorData< T > > > &dirichletDataVector )
{
    coolingDataVector.clear();
    coolingDataVector.resize( odeSystemSize );
    dirichletDataVector.clear();
    dirichletDataVector.resize( odeSystemSize );


    list< vector< const ExternalAreaData * > > areasSortedBySamePlane;
    SortExternalAreasBySamePlane( externalAreaDataVector, areasSortedBySamePlane );
    if ( mAggregateAreasForConvection == AGGREGATE_BY_PLANE_AND_BLOCKS )
        FurtherSortExternalAreasByBlockAdjacency( adjacencyMatrix, areasSortedBySamePlane );


// Get data for default convection and radiation and write characteristic length data to files
#ifndef __NO_STRING__
    std::ofstream mCharacteristicLengthData( "CharacteristicLengthData.txt" );
    std::ofstream mCharacteristicLengthDataVertices( "CharacteristicLengthVisualization_Patch_Vertices.csv" );
    std::ofstream mCharacteristicLengthDataAreas( "CharacteristicLengthVisualization_Patch_Areas.csv" );
    std::ofstream mCharacteristicLengthDataTemperatures( "CharacteristicLengthVisualization_Patch_Temperatures.csv" );
    mCharacteristicLengthDataTemperatures << "0";
    size_t areaCounter = 0;
#endif
    BOOST_FOREACH ( const vector< const ExternalAreaData * > &areasOnTheSamePlane, areasSortedBySamePlane )
    {
        set< Area< T > * > areasToAggregate;
        BOOST_FOREACH ( const ExternalAreaData *externalAreaData, areasOnTheSamePlane )
            if ( !areasToAggregate.insert( externalAreaData->mAreaPointer ).second )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AreaFoundTwice" );

        // Aggregate areas: Get common area data
        TwoDim< T > lowestEdge, highestEdge;
        map< Area< T > *, TwoDim< T > > centroids;
        Area< T > *constructingArea = *areasToAggregate.begin();
        constructingArea->Aggregate( areasToAggregate, lowestEdge, highestEdge, centroids, mTolerance );

        // Calculate characteristic length
        T characteristicLength;
        if ( constructingArea->IsHorizontal( mTolerance ) )
        {
            const T a = highestEdge.Get1() - lowestEdge.Get1();
            const T b = highestEdge.Get2() - lowestEdge.Get2();
            characteristicLength = 0.5 * a * b / ( a + b );
        }
        else
            characteristicLength = highestEdge.Get2() - lowestEdge.Get2();

        // create cooling data for default coolings at external areas
        BOOST_FOREACH ( const ExternalAreaData *externalAreaData, areasOnTheSamePlane )
        {
            TaylorData< T > data;
            data.mCharacteristicLength = characteristicLength;
            // data.mDistanceToLeadingEdge = centroids.find(externalArea.mAreaPointer).mArea.Distance(whatever)
            data.mA_cool = externalAreaData->mRestAreaValue;
            data.mLocation = externalAreaData->mAreaPointer->GetLocation();
            data.mDistanceToGridVertex = externalAreaData->mAreaPointer->GetDistanceToGridVertex();
            data.mConductivity = externalAreaData->mAreaPointer->GetConductivity();
            data.mCooling = 0;
            coolingDataVector[externalAreaData->mElementIndex].push_back( data );
        }

// Output data of common area into file
#ifndef __NO_STRING__
        const Cartesian< T > globalLowestEdge = constructingArea->GlobalCoordinates( lowestEdge );
        const Cartesian< T > globalHighestEdge = constructingArea->GlobalCoordinates( highestEdge );

        mCharacteristicLengthData << "Char. length: " << characteristicLength << ", Location:";
        if ( constructingArea->GetLocation() == TOP )
            mCharacteristicLengthData << " TOP, spanning from ";
        else if ( constructingArea->GetLocation() == BOTTOM )
            mCharacteristicLengthData << " BOTTOM, spanning from ";
        else if ( constructingArea->GetLocation() == SIDE )
            mCharacteristicLengthData << " SIDE, spanning from ";
        mCharacteristicLengthData << "(" << globalLowestEdge.GetX() << "," << globalLowestEdge.GetY() << ","
                                  << globalLowestEdge.GetZ() << ") to (" << globalHighestEdge.GetX() << ","
                                  << globalHighestEdge.GetY() << "," << globalHighestEdge.GetZ() << ")"
                                  << "\n";

        const Cartesian< T > globalEdge1 =
         constructingArea->GlobalCoordinates( TwoDim< T >( lowestEdge.Get1(), highestEdge.Get2() ) );
        const Cartesian< T > globalEdge2 =
         constructingArea->GlobalCoordinates( TwoDim< T >( highestEdge.Get1(), lowestEdge.Get2() ) );
        mCharacteristicLengthDataVertices << globalLowestEdge << "\n" << globalEdge1 << "\n" << globalHighestEdge
                                          << "\n" << globalEdge2 << "\n";
        mCharacteristicLengthDataAreas << areaCounter * 4 << "," << areaCounter * 4 + 1 << ", " << areaCounter * 4 + 2
                                       << ", " << areaCounter * 4 + 3 << "\n";
        mCharacteristicLengthDataTemperatures << " ," << characteristicLength;
        ++areaCounter;
#endif
    }
#ifndef __NO_STRING__
    mCharacteristicLengthDataTemperatures << "\n";
    mCharacteristicLengthData.close();
    mCharacteristicLengthDataVertices.close();
    mCharacteristicLengthDataAreas.close();
    mCharacteristicLengthDataTemperatures.close();
#endif


    CreateForcedCoolingData( cooledAreaDataVector, coolingDataVector, dirichletDataVector );
}

template < typename T >
void ThermalModel< T >::SortExternalAreasBySamePlane( const vector< ExternalAreaData > &externalAreaDataVector,
                                                      list< vector< const ExternalAreaData * > > &areasSortedBySamePlane ) const
{
    BOOST_FOREACH ( const ExternalAreaData &externalAreaData, externalAreaDataVector )
    {
        bool assigned = false;
        BOOST_FOREACH ( vector< const ExternalAreaData * > &areasOnTheSamePlane, areasSortedBySamePlane )
        {
            if ( areasOnTheSamePlane.front()->mAreaPointer->IsCoplanarTo( *externalAreaData.mAreaPointer, mTolerance ) &&
                 areasOnTheSamePlane.front()->mAreaPointer->GetLocation() == externalAreaData.mAreaPointer->GetLocation() )
            {
                areasOnTheSamePlane.push_back( &externalAreaData );
                assigned = true;
                break;
            }
        }

        if ( !assigned )
            areasSortedBySamePlane.push_back( vector< const ExternalAreaData * >( 1, &externalAreaData ) );
    }
}

template < typename T >
void ThermalModel< T >::FurtherSortExternalAreasByBlockAdjacency( const vector< vector< IndexedAdjacency > > &adjacencyMatrix,
                                                                  list< vector< const ExternalAreaData * > > &areasSortedBySamePlane ) const
{
    typename list< vector< const ExternalAreaData * > >::const_iterator it = areasSortedBySamePlane.begin();
    size_t iEnd = areasSortedBySamePlane.size();
    for ( size_t i = 0; i < iEnd; ++i )
    {
        // Get data about which areas are connected
        vector< vector< size_t > > sortedData;
        SortOneExternalAreaVectorByBlockAdjacency( adjacencyMatrix, *it, sortedData );

        // Sort areas according to data
        if ( sortedData.size() == 1 )
        {
            ++it;
            areasSortedBySamePlane.splice( areasSortedBySamePlane.end(), areasSortedBySamePlane, areasSortedBySamePlane.begin() );
        }
        else
        {
            BOOST_FOREACH ( const vector< size_t > &conectedAreasIndices, sortedData )
            {
                areasSortedBySamePlane.push_back( vector< const ExternalAreaData * >() );
                areasSortedBySamePlane.back().reserve( conectedAreasIndices.size() );
                BOOST_FOREACH ( const size_t &index, conectedAreasIndices )
                    areasSortedBySamePlane.back().push_back( it->at( index ) );
            }
            ++it;
            areasSortedBySamePlane.pop_front();
        }
    }
}

template < typename T >
void ThermalModel< T >::CreateForcedCoolingData( const vector< CooledAreaData > &cooledAreaDataVector,
                                                 vector< vector< TaylorData< T > > > &coolingDataVector,
                                                 vector< vector< TaylorData< T > > > &dirichletDataVector )
{
    BOOST_FOREACH ( const CooledAreaData &cooledAreaData, cooledAreaDataVector )
    {
        if ( cooledAreaData.mCooling->IsDirichletBoundaryCondition() )
        {
            TaylorData< T > data;
            data.mCharacteristicLength = -1.0;
            // data.mDistanceToLeadingEdge = centroids.find(externalArea.mAreaPointer).mArea.Distance(whatever)
            data.mA_cool = cooledAreaData.mRestAreaValue;
            data.mTempSurfLastStep = cooledAreaData.mCooling->GetOffsetSlope( 0.0, 0.0 ).mOffset;
            data.mLocation = cooledAreaData.mAreaPointer->GetLocation();
            data.mDistanceToGridVertex = cooledAreaData.mAreaPointer->GetDistanceToGridVertex();
            data.mConductivity = cooledAreaData.mAreaPointer->GetConductivity();
            data.mCooling = cooledAreaData.mCooling;
            dirichletDataVector[cooledAreaData.mElementIndex].push_back( data );
        }
        else
        {
            TaylorData< T > data;
            data.mCharacteristicLength = -1.0;
            // data.mDistanceToLeadingEdge = centroids.find(externalArea.mAreaPointer).mArea.Distance(whatever)
            data.mA_cool = cooledAreaData.mRestAreaValue;
            data.mLocation = cooledAreaData.mAreaPointer->GetLocation();
            data.mDistanceToGridVertex = cooledAreaData.mAreaPointer->GetDistanceToGridVertex();
            data.mConductivity = cooledAreaData.mAreaPointer->GetConductivity();
            data.mCooling = cooledAreaData.mCooling;
            coolingDataVector[cooledAreaData.mElementIndex].push_back( data );
        }
    }
}

template < typename T >
void ThermalModel< T >::ProbeThermalElemnts( std::vector< ::probe::ThermalProbe > &thermalProbes ) const
{
    for ( size_t probeCount = 0; probeCount < thermalProbes.size(); ++probeCount )
    {
        double distance = 10000000000000;
        double localDistance = 0;

        boost::shared_ptr< ThermalElement< T > > bestMatch;
        for ( size_t blockCounter = 0; blockCounter < mBlocks.size(); ++blockCounter )
        {
            for ( size_t thermalElementCount = 0; thermalElementCount < mBlocks[blockCounter].mThermalElements.size(); ++thermalElementCount )
            {
                localDistance = mBlocks[blockCounter].mThermalElements[thermalElementCount]->GetGridVertex().Distance(
                 thermalProbes[probeCount].GetCoordinates() );
                if ( localDistance < distance )
                {
                    distance = localDistance;
                    bestMatch = mBlocks[blockCounter].mThermalElements[thermalElementCount];
                }
            }
        }
        if ( bestMatch )
            thermalProbes[probeCount].SetCorrespondingThermalElement( bestMatch );
        else
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ThermalProbeError" );
    }
}
}
#endif
