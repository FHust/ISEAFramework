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
#ifndef _THERMAL_SIMULATION_
#define _THERMAL_SIMULATION_


#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>

#include "../factory/thermal/thermal_factory.h"
#include "../factory/thermal/materialfactorybuilder.h"
#include "../factory/thermal/blockfactorybuilder.h"
#include "../factory/thermal/coolingfactorybuilder.h"
#include "../factory/thermal/coolingblockfactorybuilder.h"
#include "../factory/object/objectfactorybuilder.h"
#include "../factory/observer/createObserver.h"
#include "../factory/state/statefactorybuilder.h"

#include "../exceptions/error_proto.h"

#include "../thermal/blocks/thermal_block.h"
#include "../thermal/ode_system_thermal.h"
#include "../thermal/thermal_model.h"
#include "../time_series/time_series.h"

class TestSimulation;

namespace simulation
{

template < typename Matrix, typename T, bool FilterTypeChoice >
class ThermalSimulation
{
    friend class ::TestSimulation;

    public:
    /**
     * @param[in] rootXmlNode Root node of xml-file to parse
     * @param[in] recommendedDeltaTime The initial value if delta time for the equation solver
     * @param[in] simulationDuration Duration of the whole simulation in sec
     * @param[in] thermalStateStopCriterion Recalculation of equation system is triggered if the temperature of any cell
     * element has changed by thermalStateStopCriterion percent
     * @param[out] thermalVisualizer Returns thermal visualizer if it is not null
     * @param[in,out] thermalStates The thermal states created by the electrical simulation must be put in here. If it
     * is null, purely thermal simulation is assumed and thermal states are created in this function
     * @param[out] thermalStatesOfCellBlocks Returns all thermal states in cells ordered by the thermal block inside
     * which they are if it is not null
     */
    ThermalSimulation( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, T recommendedDeltaTime, T simulationDuration,
                       T thermalStateStopCriterion, boost::scoped_ptr< observer::ThermalObserver< T > > *thermalVisualizer,
                       std::vector< boost::shared_ptr< ThermalState< T > > > *thermalStates,
                       std::vector< std::vector< boost::shared_ptr< ThermalState< T > > > > *thermalStatesOfCellBlocks );
    /// Updates thermal equation system and sets temperature vector and air temperature
    void UpdateSystem();
    /// Returns true if current simulation step needs to end, if so it resets mDeltaTime for the next simulation step
    bool CheckLoopConditionAndSetDeltaTime( T loopStopTime );
    /// Returns true if mTime has exceeded mSimulationDuration
    bool CheckIfSumlationTimeHasNotEnded();
    /// Saves current temperatures into thermal states
    void UpdateAllThermalStatesTemperatures();
    /// Resets stop criterion before the start of a new simulation step
    void InitializeStopCriterion();
    /// Returns true if thermal state stop criterion is met
    bool IsStopCriterionFulfilled() const;

    // Internal data
    std::vector< boost::shared_ptr< ThermalState< T > > > mThermalStates;
    std::vector< boost::shared_ptr< ThermalState< T > > > mUnconnectedThermalStates;
    std::vector< boost::shared_ptr< thermal::Material< T > > > mMaterials;
    std::vector< boost::shared_ptr< thermal::Cooling< T > > > mCoolings;
    std::vector< thermal::ThermalElement< T > * > mConnectedThermalElements;
    std::vector< ::probe::ThermalProbe > mThermalProbes;
    // Thermal system with states
    boost::scoped_ptr< thermal::OdeSystemThermal< T > > mThermalSystem;
    vector< T > mTemperatures;
    // Simulation times
    T mTime;
    T mLastTime;
    T mDeltaTime;

    private:
    T mLastUnconstrainedDeltaTime;
    T mSimulationDuration;
    // Thermal state stop criterion
    vector< T > mThermalStateValuesFirstStep;
    T mThermalStateStopCriterion;
    geometry::Tolerance< T > mTolerance;
    // TemperatureHeatProfiles
    boost::scoped_ptr< electrical::TimeSeries< T, electrical::EvalLinearInterpolation > > mAirTemperaturesData;
};


template < typename Matrix, typename T, bool FilterTypeChoice >
ThermalSimulation< Matrix, T, FilterTypeChoice >::ThermalSimulation(
 const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, T recommendedDeltaTime, T simulationDuration,
 T thermalStateStopCriterion, boost::scoped_ptr< observer::ThermalObserver< T > > *thermalVisualizer,
 std::vector< boost::shared_ptr< ThermalState< T > > > *thermalStates,
 std::vector< std::vector< boost::shared_ptr< ThermalState< T > > > > *thermalStatesOfCellBlocks )
    : mTime( 0.0 )
    , mLastTime( 0.0 )
    , mDeltaTime( recommendedDeltaTime )
    , mLastUnconstrainedDeltaTime( recommendedDeltaTime )
    , mSimulationDuration( simulationDuration )
    , mThermalStateStopCriterion( 5.0 )
    , mTolerance(geometry::Tolerance< T >( 0.000001, geometry::Angle<>::Deg( 0.001 ), 0.1 ))
{
    // Evaluate Options node
    boost::shared_ptr< xmlparser::XmlParameter > optionsNode = rootXmlNode->GetElementChild( "Options" );

    // Air temperature
    T airTemperature = 0.0;
    if ( optionsNode->GetElementChild( "AirTemperature" )->GetElementAttributeBoolValue( "ReadFromFile", false ) )
    {
#ifndef __NO_STRING__
        std::ifstream airTemperatureFile( optionsNode->GetElementStringValue( "AirTemperature" ) );
        if ( airTemperatureFile.fail() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "FileInAirTemperatureCannotBeOpened" );
#ifdef __EXCEPTIONS__
        try
        {
#endif
            mAirTemperaturesData.reset( new electrical::TimeSeries< T, electrical::EvalLinearInterpolation >( airTemperatureFile ) );
#ifdef __EXCEPTIONS__
        }
        catch ( std::exception &e )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AirTemperatureFileDefinition", e.what() );
        }
        airTemperature = mAirTemperaturesData->GetValue();
#endif
#else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ReadFromFileInOptionsNonFalse" );
#endif
    }
    else
        airTemperature = optionsNode->GetElementDoubleValue( "AirTemperature" );

    // Standard coolings
    typename thermal::ThermalModel< T >::AggregateAreasForConvection aggregateAreasForConvection =
     thermal::ThermalModel< T >::AGGREGATE_BY_PLANE;
    std::vector< boost::shared_ptr< thermal::DefaultConvection< T > > > convection( 3 );
    boost::shared_ptr< thermal::Radiation<> > radiation;
    if ( optionsNode->GetElementBoolValue( "ConvectionActivated" ) )
    {
        convection[thermal::TOP].reset( new thermal::ConvectionByFormula< T >( 0.71 ) );
        convection[thermal::SIDE].reset( new thermal::ConvectionByFormula< T >( 0.548 ) );
        if ( optionsNode->GetElementChild( "ConvectionActivated" )
              ->GetElementAttributeBoolValue( "advancedAreaAggregation", true ) )
            aggregateAreasForConvection = thermal::ThermalModel< T >::AGGREGATE_BY_PLANE_AND_BLOCKS;
    }
    if ( optionsNode->GetElementBoolValue( "RadiationActivated" ) )
        radiation.reset( new thermal::Radiation<> );

    // ThermalStopCriterion
    if ( thermalStateStopCriterion > 0.0 )
        mThermalStateStopCriterion = thermalStateStopCriterion;
    else
    {
        if ( optionsNode->HasElement( "ThermalStopCriterionInDegreeC" ) )
        {
            mThermalStateStopCriterion = optionsNode->GetElementDoubleValue( "ThermalStopCriterionInDegreeC" );
            if ( mThermalStateStopCriterion <= 0.0 )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "ThermalStopCriterionInDegreeCNegative" );
        }
    }

    // Tolerance
    if ( optionsNode->HasElement( "GeometricalTolerance" ) )
    {
        boost::shared_ptr< xmlparser::XmlParameter > geometricalToleranceNode =
         optionsNode->GetElementChild( "GeometricalTolerance" );
        mTolerance =
         geometry::Tolerance< T >( geometricalToleranceNode->GetElementAttributeDoubleValue( "length" ),
                                   geometry::Angle<>::Deg(
                                    geometricalToleranceNode->GetElementAttributeDoubleValue( "angleInDegrees" ) ),
                                   geometricalToleranceNode->GetElementAttributeDoubleValue( "percentOfQuantity" ) );
    }

    bool showLateralSurfaces = false;
    if ( optionsNode->HasElement( "ThermalObserver" ) &&
         optionsNode->GetElementChild( "ThermalObserver" )->HasElement( "ShowLateralSurfaces" ) )
        showLateralSurfaces =
         optionsNode->GetElementChild( "ThermalObserver" )->GetElementBoolValue( "ShowLateralSurfaces", false );


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
    std::vector< geometry::Area< T > > coolingAreas;
    boost::shared_ptr< thermal::Cooling< T > > cooling;
    std::vector< std::vector< thermal::TaylorData< T > > > coolingDataVector;
    std::vector< std::vector< thermal::TaylorData< T > > > dirichletDataVector;

    misc::StrCont description;

    // Create thermal model
    thermalFactory->CreateThermalModel( rootXmlNode, heatedBlocks, unheatedBlocks, coolingBlocks, thermalStates,
                                        thermalStatesOfCellBlocks, &mThermalProbes );
    thermal::ThermalModel< T > thermalModel( mTolerance, aggregateAreasForConvection );
    thermalModel.ClearAndSetNumberOfBlocksAndCoolings( heatedBlocks.size() + unheatedBlocks.size(), coolingBlocks.size() );
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
    BOOST_FOREACH ( boost::shared_ptr< thermal::CoolingBlock< T > > &coolingBlock, coolingBlocks )
    {
        cooling = coolingBlock->GetCooling();
        coolingBlock->GetCoolingAreas( coolingAreas );
        coolingBlock->GetBlockGeometry( blockGeometry );
        thermalModel.AddCooling( coolingAreas, blockGeometry, cooling );
    }

    // Get thermal states
    if ( thermalStates )
        mThermalStates = *thermalStates;
    else
    {
        mThermalStates.reserve( heatedBlocks.size() );
        BOOST_FOREACH ( boost::shared_ptr< thermal::ThermalBlock< T > > &block, heatedBlocks )
            mThermalStates.insert( mThermalStates.end(), block->GetThermalStates().begin(), block->GetThermalStates().end() );
    }
    if ( mThermalStateStopCriterion < thermal::globalMaxValue )
        mThermalStateValuesFirstStep.resize( mThermalStates.size() );
    mUnconnectedThermalStates.reserve( unheatedBlocks.size() );
    BOOST_FOREACH ( boost::shared_ptr< thermal::ThermalBlock< T > > &block, unheatedBlocks )
        mUnconnectedThermalStates.push_back( block->GetThermalStates().at( 0 ) );

    // Create thermal system
    thermalModel.CreateDataByFusingBlocks( thermalElements, conductivityMatrix, coolingDataVector, dirichletDataVector );
    materialFactory->GetObjects( mMaterials );
    coolingFactory->GetObjects( mCoolings );
    BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalElement< T > > &elem, thermalElements )
        if ( elem->HasThermalState() )    // Get connected thermal elements (that have a thermal state) before
                                          // thermalElements is swapped into mThermalSystem
            mConnectedThermalElements.push_back( elem.get() );
    mThermalSystem.reset( new thermal::OdeSystemThermal< T >( thermalElements, conductivityMatrix, coolingDataVector, dirichletDataVector,
                                                              convection, radiation, airTemperature, mThermalStates ) );
    mThermalSystem->GetTemperatureVector( mTemperatures );

    // Create thermal visualizer if desired
    vector< thermal::ThermalElement< double > * > thermalElementsOfAreas;
    vector< vector< size_t > > areas;
    vector< vector< size_t > > volumes;
    vector< misc::StrCont > volumeNames;
    vector< geometry::Cartesian< double > > vertices;
    thermalModel.CreateDataForVisualization( thermalElementsOfAreas, areas, volumes, volumeNames, vertices, showLateralSurfaces );

    if ( thermalVisualizer )
        thermalVisualizer->reset( CreateThermalObserver< double, FilterTypeChoice >( rootXmlNode.get(), thermalElementsOfAreas,
                                                                                     areas, volumes, volumeNames, vertices ) );
    thermalModel.ProbeThermalElemnts( mThermalProbes );
}

template < typename Matrix, typename T, bool FilterTypeChoice >
void ThermalSimulation< Matrix, T, FilterTypeChoice >::UpdateSystem()
{
    if ( mAirTemperaturesData )
    {
        mAirTemperaturesData->SetTimeAndTriggerEvaluation( mTime );
        mThermalSystem->ResetAirTemperature( mAirTemperaturesData->GetValue() );
    }

    mThermalSystem->Update( mTime, mTime - mLastTime );
    mLastTime = mTime;
    mThermalSystem->SetTemperatureVector( mTemperatures );
}

template < typename Matrix, typename T, bool FilterTypeChoice >
bool ThermalSimulation< Matrix, T, FilterTypeChoice >::CheckLoopConditionAndSetDeltaTime( T loopStopTime )
{
    if ( mTime < loopStopTime )
    {
        mLastUnconstrainedDeltaTime = mDeltaTime;
        mDeltaTime = std::min( loopStopTime - mTime, mLastUnconstrainedDeltaTime );
        return true;
    }

    return false;
}

template < typename Matrix, typename T, bool FilterTypeChoice >
bool ThermalSimulation< Matrix, T, FilterTypeChoice >::CheckIfSumlationTimeHasNotEnded()
{
    return mTime < mSimulationDuration;
}

template < typename Matrix, typename T, bool FilterTypeChoice >
void ThermalSimulation< Matrix, T, FilterTypeChoice >::UpdateAllThermalStatesTemperatures()
{
    BOOST_FOREACH ( boost::shared_ptr< ::state::ThermalState< T > > &thermalstate, mThermalStates )
        thermalstate->ResetTemperature();
    BOOST_FOREACH ( thermal::ThermalElement< T > *thermalElement, mConnectedThermalElements )
        thermalElement->UpdateThermalStateTemperature();
}

template < typename Matrix, typename T, bool FilterTypeChoice >
void ThermalSimulation< Matrix, T, FilterTypeChoice >::InitializeStopCriterion()
{
    for ( size_t i = 0; i < mThermalStates.size(); ++i )
        mThermalStateValuesFirstStep[i] = mThermalStates[i]->GetValue();
}

template < typename Matrix, typename T, bool FilterTypeChoice >
bool ThermalSimulation< Matrix, T, FilterTypeChoice >::IsStopCriterionFulfilled() const
{
    bool isStop = false;
    for ( size_t i = 0; i < mThermalStates.size(); ++i )
    {
        if ( fabs( mThermalStateValuesFirstStep[i] - mThermalStates[i]->GetValue() ) > mThermalStateStopCriterion )
        {
            isStop = true;
            break;
        }
    }

    return isStop;
}
}
#endif
