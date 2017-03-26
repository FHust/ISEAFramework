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
#ifndef _SIMULATION_DATA_OUTLINE_
#define _SIMULATION_DATA_OUTLINE_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "../factory/thermal/thermal_factory.h"
#include "../factory/thermal/materialfactorybuilder.h"
#include "../factory/thermal/blockfactorybuilder.h"
#include "../factory/thermal/coolingfactorybuilder.h"
#include "../factory/thermal/coolingblockfactorybuilder.h"
#include "../factory/object/objectfactorybuilder.h"
#include "../factory/state/statefactorybuilder.h"

#include "../thermal/blocks/thermal_block.h"
#include "../observer/thermal_observer.h"
#include "../observer/filter/csvfilter.h"
#include "../thermal/thermal_model.h"


namespace simulation
{

/// ThermalModelOutline() returns number of cells, number of thermal elements and the emplacement points of all thermal
/// blocks if the respective pointer is not set to null
template < typename T >
void ThermalModelOutline( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, size_t *numberOfCells,
                          size_t *numberOfThermalElements,
                          std::vector< geometry::Cartesian< T > > *emplacementPointsOfThermalBlocks, size_t *numberOfThermalProbes )
{
    // Build Factories
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory;
    boost::scoped_ptr< factory::Factory< object::Object< T >, factory::ArgumentTypeObject< T > > > objectFactory;
    boost::scoped_ptr< factory::Factory< thermal::Material< T > > > materialFactory;
    boost::scoped_ptr< factory::Factory< thermal::ThermalBlock< T >, factory::ArgumentTypeThermalBlock< T > > > blockFactory;
    boost::scoped_ptr< factory::Factory< thermal::Cooling< T > > > coolingFactory;
    boost::scoped_ptr< factory::Factory< thermal::CoolingBlock< T >, factory::ArgumentTypeCoolingBlock< T > > > coolingBlockFactory;
    boost::scoped_ptr< thermal::ThermalFactory< T > > thermalFactory;

    stateFactory.reset( factory::BuildStateFactory() );
    objectFactory.reset( factory::BuildObjectFactory< T >( stateFactory.get() ) );
    materialFactory.reset( factory::BuildMaterialFactory< T >() );
    blockFactory.reset( factory::BuildBlockFactory< T >( materialFactory.get() ) );
    coolingFactory.reset( factory::BuildCoolingFactory< T >( objectFactory.get() ) );
    coolingBlockFactory.reset( factory::BuildCoolingBlockFactory< T >( coolingFactory.get() ) );
    thermalFactory.reset( new thermal::ThermalFactory< T >( materialFactory.get(), blockFactory.get(),
                                                            coolingFactory.get(), coolingBlockFactory.get() ) );

    // Temporary variables to process creation of thermal system
    std::vector< boost::shared_ptr< thermal::ThermalBlock< T > > > heatedBlocks;
    std::vector< boost::shared_ptr< thermal::ThermalBlock< T > > > unheatedBlocks;
    std::vector< boost::shared_ptr< thermal::CoolingBlock< T > > > coolingBlocks;
    std::vector< std::vector< boost::shared_ptr< ::state::ThermalState< T > > > > thermalStatesOfCellBlocks;
    std::vector< ::probe::ThermalProbe > thermalProbes;

    // Create data
    thermalFactory->CreateThermalModel( rootXmlNode, heatedBlocks, unheatedBlocks, coolingBlocks, 0,
                                        &thermalStatesOfCellBlocks, &thermalProbes );

    // Evaluate data
    if ( numberOfCells )
        *numberOfCells = thermalStatesOfCellBlocks.size();
    if ( numberOfThermalElements )
    {
        size_t sum = 0;
        BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalBlock< T > > &block, heatedBlocks )
            sum += block->GetNumberOfThermalElements();
        BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalBlock< T > > &block, unheatedBlocks )
            sum += block->GetNumberOfThermalElements();
        *numberOfThermalElements = sum;
    }
    if ( emplacementPointsOfThermalBlocks )
    {
        emplacementPointsOfThermalBlocks->clear();
        emplacementPointsOfThermalBlocks->reserve( heatedBlocks.size() );
        BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalBlock< T > > &block, heatedBlocks )
            emplacementPointsOfThermalBlocks->push_back( block->GetEmplacementPoint() );
    }

    if ( numberOfThermalProbes )
    {
        *numberOfThermalProbes = thermalProbes.size();
    }
}


/// Visualizes thermal model at t=0.0 and prints to console if there are any collision in it
template < typename T >
void QuickVisualizeThermalModel( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode )
{
    bool showLateralSurfaces = false;
    // Evaluate Options node
    boost::shared_ptr< xmlparser::XmlParameter > optionsNode = rootXmlNode->GetElementChild( "Options" );
    if ( optionsNode->HasElement( "ThermalVisualizer" ) &&
         optionsNode->GetElementChild( "ThermalVisualizer" )->HasElement( "ShowLateralSurfaces" ) )
        showLateralSurfaces =
         optionsNode->GetElementChild( "ThermalVisualizer" )->GetElementBoolValue( "ShowLateralSurfaces", false );

    // Build Factories
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory;
    boost::scoped_ptr< factory::Factory< object::Object< T >, factory::ArgumentTypeObject< T > > > objectFactory;
    boost::scoped_ptr< factory::Factory< thermal::Material< T > > > materialFactory;
    boost::scoped_ptr< factory::Factory< thermal::ThermalBlock< T >, factory::ArgumentTypeThermalBlock< T > > > blockFactory;
    boost::scoped_ptr< factory::Factory< thermal::Cooling< T > > > coolingFactory;
    boost::scoped_ptr< factory::Factory< thermal::CoolingBlock< T >, factory::ArgumentTypeCoolingBlock< T > > > coolingBlockFactory;
    boost::scoped_ptr< thermal::ThermalFactory< T > > thermalFactory;

    stateFactory.reset( factory::BuildStateFactory() );
    objectFactory.reset( factory::BuildObjectFactory< T >( stateFactory.get() ) );
    materialFactory.reset( factory::BuildMaterialFactory< T >() );
    blockFactory.reset( factory::BuildBlockFactory< T >( materialFactory.get() ) );
    coolingFactory.reset( factory::BuildCoolingFactory< T >( objectFactory.get() ) );
    coolingBlockFactory.reset( factory::BuildCoolingBlockFactory< T >( coolingFactory.get() ) );
    thermalFactory.reset( new thermal::ThermalFactory< T >( materialFactory.get(), blockFactory.get(),
                                                            coolingFactory.get(), coolingBlockFactory.get() ) );

    // Temporary variables to process creation of thermal system
    std::vector< boost::shared_ptr< thermal::ThermalBlock< T > > > heatedBlocks;
    std::vector< boost::shared_ptr< thermal::ThermalBlock< T > > > unheatedBlocks;
    std::vector< boost::shared_ptr< thermal::CoolingBlock< T > > > coolingBlocks;

    std::vector< boost::shared_ptr< thermal::ThermalElement< T > > > thermalElements;
    std::vector< std::vector< thermal::IndexedValue< T > > > conductivityMatrix;
    std::vector< thermal::IndexedArea< T > > surfaceElements;
    std::vector< thermal::IndexedInnerArea< T > > innerSurfaceElements;
    boost::shared_ptr< thermal::BlockGeometry< T > > blockGeometry;

    misc::StrCont description;

    // Create thermal model
    thermalFactory->CreateThermalModel( rootXmlNode, heatedBlocks, unheatedBlocks, coolingBlocks, 0, 0 );
    thermal::Tolerance<> tolerance( 0.000001, geometry::Angle<>::Deg( 0.001 ), 0.0003 );
    thermal::ThermalModel< T > thermalModel( tolerance, thermal::ThermalModel< T >::AGGREGATE_BY_PLANE );
    thermalModel.ClearAndSetNumberOfBlocksAndCoolings( heatedBlocks.size() + unheatedBlocks.size(), 0 );
    BOOST_FOREACH ( boost::shared_ptr< thermal::ThermalBlock< T > > &block, heatedBlocks )
    {
        block->CreateData( thermalElements, conductivityMatrix, surfaceElements, blockGeometry );
        block->GetDescription( description );
        if ( showLateralSurfaces )
            block->GetInnerSurfaceAreas( innerSurfaceElements );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceElements, blockGeometry, innerSurfaceElements, description );
    }
    BOOST_FOREACH ( boost::shared_ptr< thermal::ThermalBlock< T > > &block, unheatedBlocks )
    {
        block->CreateData( thermalElements, conductivityMatrix, surfaceElements, blockGeometry );
        block->GetDescription( description );
        if ( showLateralSurfaces )
            block->GetInnerSurfaceAreas( innerSurfaceElements );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceElements, blockGeometry, innerSurfaceElements, description );
    }

    // Visualize
    vector< thermal::ThermalElement< double > * > thermalElementsOfAreas;
    vector< vector< size_t > > areas;
    vector< vector< size_t > > volumes;
    vector< misc::StrCont > volumeNames;
    vector< geometry::Cartesian< double > > vertices;
    thermalModel.CreateDataForVisualization( thermalElementsOfAreas, areas, volumes, volumeNames, vertices, showLateralSurfaces );
    observer::ThermalObserver< T > thermalVisualizer( thermalElementsOfAreas, areas, volumes, volumeNames, vertices );
    thermalVisualizer.AddFilter( boost::make_shared< observer::CsvFilterThermal< double > >() );

#ifdef __EXCEPTIONS__
    try
    {
        thermalModel.BlocksDimensionTest();
        thermalModel.BlocksCollisionTest();
    }
    catch ( const std::runtime_error &e )
    {
        printf( "%s", e.what() );
    }
#endif
}
}
#endif    // Include guard
