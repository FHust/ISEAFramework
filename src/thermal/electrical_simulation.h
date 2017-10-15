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
#ifndef _ELECTRICAL_SIMULATION_
#define _ELECTRICAL_SIMULATION_


#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../factory/electrical/electricalfactorybuilder.h"
#include "../factory/object/objectfactorybuilder.h"
#include "../factory/state/statefactorybuilder.h"
#include "../factory/electricalEquationSystem/chooseElectricSystemType.h"

#include "../system/dae_sys.h"

#include "../factory/observer/createObserver.h"
#include "../observer/observer.h"
#include "../observer/observerException.h"
#include "../observer/filter/filter.h"
#include "../observer/filter/csvfilter.h"
#include "../exceptions/error_proto.h"

namespace thermal
{
extern const double globalMaxValue;
}


class TestSimulation;

namespace simulation
{

template < typename Matrix, typename T, bool matlabFilterOutput = true >
class ElectricalSimulation
{
    friend class ::TestSimulation;

    public:
    /**
     * @param[in] rootXmlNode Root node of xml-file to parse
     * @param[in] maxSimulationStepDuration Recalculation of equation system is triggered if maxSimulationStepDuration
     * seconds have been passed since last calculation, forcing a new simulation step
     * @param[in] simulationDuration Duration of the whole simulation in sec
     * @param[in] socStopCriterion Recalculation of equation system is triggered if the SoC of any cell element has
     * changed by socStopCriterion percent
     * @param[out] cells Returns all created cells if it is not null
     */
    ElectricalSimulation( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, T maxSimulationStepDuration,
                          T simulationDuration, T socStopCriterion,
                          std::vector< boost::shared_ptr< ::electrical::TwoPort< Matrix > > > *cells = 0 );
    /// Updates electrical equation system
    void UpdateSystem();
    /// Updates physical quantities in electrical two ports
    void UpdateSystemValues();
    /// Returns true if current simulation step needs to end, if so it resets mDeltaTime for the next simulation step
    bool CheckLoopConditionAndSetDeltaTime( T currentChangeTime );
    /// Returns true if mTime has exceeded mSimulationDuration, if so it resets step start time to current time
    bool CheckIfSumlationTimeHasNotEndedAndSetStepStartTime();
    /// Gets the time passed since mLoopStartTime (mLoopStartTime is the start time of this simulation step)
    T GetCurrentSimulationStepTime() const;
    /// Executes actions needed at the end of a simulation step
    void FinshStep();
    /// Saves power and time of all cell elements into their thermal states
    void UpdateAllThermalStatesPowerDissipation();
    /// Saves power of all cell elements into their thermal states
    void UpdateAllThermalStatesPowerDissipationWithoutTime();
    /// Clears power dissipation of all thermal states
    void ResetAllThermalStatesPowerDissipation();
    /// Resets SoC stop criterion before the start of a new simulation step
    void InitializeStopCriterion();
    /// Returns true if SoC stop criterion is met
    bool IsStopCriterionFulfilled() const;
    /// Saves states if stop criterion of thermal simulation is triggered
    void SaveStatesForLaterReset();
    /// Resets states and SoCs to states saved at a certain point of time
    void ResetStatesToPointOfTime( T time );

    void LoadCapacityForLaterReset();
    void SaveCapacityForLaterReset();
    std::vector< T > mCapcityValues;

    // Internal data
    boost::shared_ptr< electrical::TwoPort< Matrix > > mRootTwoPort;
    std::vector< boost::shared_ptr< ::electrical::Cellelement< Matrix > > > mCellElements;
    std::vector< boost::shared_ptr< ::electrical::state::Soc > > mSocStates;
    std::vector< boost::shared_ptr< ::state::ThermalState< T > > > mThermalStates;
    // Electrical system with states
    boost::shared_ptr< systm::GeneralizedSystem< Matrix > > mEqSystem;
    boost::scoped_ptr< observer::TwoPortObserver< Matrix > > mObserver;
    // boost::scoped_ptr<systm::DifferentialAlgebraicSystem< Matrix > > mEqSystem;
    systm::StateSystemGroup< myMatrixType > mStateSystemGroup;
    // Simulation times
    T mTime;
    T mDeltaTime;
    T mSimulationDuration;

    private:
    T mLastUnconstrainedDeltaTime;
    T mMaxSimulationStepDuration;
    T mStepStartTime;
    // Soc stop criterion
    T mSocStopCriterion;
    std::vector< std::vector< T > > mSocValuesSteps;
    // If electrical is supposed to be resettable
    std::vector< Matrix > mStatesSteps;
    std::vector< T > mTimeSteps;
    size_t mNumberOfSteps;
    // CurrentVoltageProfiles
};


template < typename Matrix, typename T, bool matlabFilterOutput >
ElectricalSimulation< Matrix, T, matlabFilterOutput >::ElectricalSimulation(
 const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, T maxSimulationStepDuration, T simulationDuration,
 T socStopCriterion, std::vector< boost::shared_ptr< ::electrical::TwoPort< Matrix > > > *cells )
    : mTime( 0.0 )
    , mDeltaTime( maxSimulationStepDuration )
    , mSimulationDuration( simulationDuration )
    , mLastUnconstrainedDeltaTime( maxSimulationStepDuration )
    , mMaxSimulationStepDuration( maxSimulationStepDuration )
    , mStepStartTime( 0.0 )
    , mSocStopCriterion( 5.0 )
    , mNumberOfSteps( 0 )
{
    // Build Factories
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory;
    boost::scoped_ptr< factory::Factory< object::Object< T >, factory::ArgumentTypeObject< T > > > objectFactory;
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< Matrix >, factory::ArgumentTypeElectrical > > electricalFactory;
    stateFactory.reset( factory::BuildStateFactory() );
    objectFactory.reset( factory::BuildObjectFactory< T >( stateFactory.get() ) );
    electricalFactory.reset( factory::BuildElectricalFactory< Matrix, T >( objectFactory.get(), stateFactory.get() ) );

    // Create electrical system
    mRootTwoPort = electricalFactory->CreateInstance( rootXmlNode->GetElementChild( "RootElement" ) );
#ifdef _SYMBOLIC_
    mRootTwoPort->SetID( 0 );
    size_t iDCounter = 1;

    std::vector< boost::shared_ptr< electrical::TwoPort< Matrix > > > electricalElements;
    electricalFactory->GetObjects( electricalElements );
    for ( size_t i = 0; i < electricalElements.size(); ++i )
        if ( electricalElements.at( i ).get() != mRootTwoPort.get() )
            electricalElements.at( i )->SetID( iDCounter++ );

    try
    {
        BOOST_FOREACH ( boost::shared_ptr< electrical::TwoPort< Matrix > > &elem,
                        electricalFactory->GetObjectsOfClass( "WarburgCotanh" ) )
        {
            SerialTwoPort< Matrix > *serialTwoPort = dynamic_cast< SerialTwoPort< Matrix > * >( elem.get() );
            for ( size_t i = 0; i < serialTwoPort->size(); ++i )
                serialTwoPort->at( i )->SetID( iDCounter++ );
        }
    }
    catch ( ... )
    {
    }
    try
    {
        BOOST_FOREACH ( boost::shared_ptr< electrical::TwoPort< Matrix > > &elem,
                        electricalFactory->GetObjectsOfClass( "WarburgTanh" ) )
        {
            SerialTwoPort< Matrix > *serialTwoPort = dynamic_cast< SerialTwoPort< Matrix > * >( elem.get() );
            for ( size_t i = 0; i < serialTwoPort->size(); ++i )
                serialTwoPort->at( i )->SetID( iDCounter++ );
        }
    }
    catch ( ... )
    {
    }
#endif
    mRootTwoPort->SetSystem( &mStateSystemGroup );
    mStateSystemGroup.Initialize();
#ifndef _SYMBOLIC_
    mRootTwoPort->SetInitialCurrent( 0.0 );
#else
    mRootTwoPort->SetInitialCurrent( symbolic::Symbolic( "InputCurrent" ) );
#endif
    mRootTwoPort->UpdateStateSystemGroup();
    mEqSystem = factory::ChooseElectricEquationSystemType( &mStateSystemGroup );

    // Store thermal states, SoC states and cell elements from electrical state factory
    mThermalStates.reserve( stateFactory->GetObjectsOfClass( "ThermalState" ).size() );
    BOOST_FOREACH ( const boost::shared_ptr< ::state::Dgl_state > &thermalStateFromFactory,
                    stateFactory->GetObjectsOfClass( "ThermalState" ) )
        mThermalStates.push_back( boost::static_pointer_cast< ::state::ThermalState< T >, ::state::Dgl_state >( thermalStateFromFactory ) );

    const size_t numberOfSocObject = stateFactory->GetObjectsOfClass( "Soc" ).size();
    mSocStates.reserve( numberOfSocObject );
    mCapcityValues.resize( numberOfSocObject );

    BOOST_FOREACH ( const boost::shared_ptr< ::state::Dgl_state > &socStateFromFactory,
                    stateFactory->GetObjectsOfClass( "Soc" ) )
        mSocStates.push_back( boost::static_pointer_cast< electrical::state::Soc, ::state::Dgl_state >( socStateFromFactory ) );

    mCellElements.reserve( electricalFactory->GetObjectsOfClass( "CellElement" ).size() );
    BOOST_FOREACH ( const boost::shared_ptr< ::electrical::TwoPort< Matrix > > &twoPortFromFactory,
                    electricalFactory->GetObjectsOfClass( "CellElement" ) )
    {
        if ( twoPortFromFactory->IsCellelement() )
            mCellElements.push_back(
             boost::static_pointer_cast< ::electrical::Cellelement< Matrix >, ::electrical::TwoPort< Matrix > >( twoPortFromFactory ) );
        else
        {
            ::electrical::ParallelTwoPort< Matrix > *cellFromFactory =
             static_cast< ::electrical::ParallelTwoPort< Matrix > * >( twoPortFromFactory.get() );
            for ( size_t i = 0; i < cellFromFactory->GetParallelChildren(); ++i )
                mCellElements.push_back(
                 boost::static_pointer_cast< ::electrical::Cellelement< Matrix > >( cellFromFactory->shared_at( i ) ) );
        }
    }

    // Evaluate Options node
    boost::shared_ptr< xmlparser::XmlParameter > optionsNode = rootXmlNode->GetElementChild( "Options" );
    // SocStopCriterion
    if ( socStopCriterion > 0.0 )
        mSocStopCriterion = socStopCriterion;
    else
    {
        if ( optionsNode->HasElement( "SocStopCriterionInPercent" ) )
        {
            mSocStopCriterion = optionsNode->GetElementDoubleValue( "SocStopCriterionInPercent" );
            if ( mSocStopCriterion <= 0.0 )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "SocStopCriterionInPercentNegative" );
        }
    }

    // Give out cells if wanted
    if ( cells )
    {
        *cells = electricalFactory->GetObjectsOfClass( "Cellelement" );
		if ( ! mRootTwoPort->IsCellelement() )
			cells->push_back(mRootTwoPort);
        mObserver.reset( CreateTwoPortObserver< std::vector< boost::shared_ptr< ::electrical::TwoPort< Matrix > > >,
                                                Matrix, matlabFilterOutput >( *cells, rootXmlNode.get() ) );
    }
    else
        mObserver.reset( CreateTwoPortObserver< boost::shared_ptr< electrical::TwoPort< Matrix > >, Matrix, matlabFilterOutput >(
         mRootTwoPort, rootXmlNode.get() ) );
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::UpdateSystem()
{
    // Current must be set before this function
    mRootTwoPort->UpdateStateSystemGroup();
    mEqSystem->PrepareEquationSystem();
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::UpdateSystemValues()
{
    mRootTwoPort->CalculateStateDependentValues();
#if defined( _ARMADILLO_ ) && defined( SPARSE_MATRIX_FORMAT )
    mStateSystemGroup.ResetSystem();
#endif
}

template < typename Matrix, typename T, bool matlabFilterOutput >
bool ElectricalSimulation< Matrix, T, matlabFilterOutput >::CheckLoopConditionAndSetDeltaTime( T currentChangeTime )
{
    if ( mTime < mStepStartTime + mMaxSimulationStepDuration && mTime < currentChangeTime )
    {
        mLastUnconstrainedDeltaTime = mDeltaTime;
        mDeltaTime = std::min( currentChangeTime - mTime, mLastUnconstrainedDeltaTime );
        mDeltaTime = std::min( mStepStartTime + mMaxSimulationStepDuration - mTime, mDeltaTime );
        return true;
    }

    return false;
}

template < typename Matrix, typename T, bool matlabFilterOutput >
bool ElectricalSimulation< Matrix, T, matlabFilterOutput >::CheckIfSumlationTimeHasNotEndedAndSetStepStartTime()
{
    mStepStartTime = mTime;
    return mTime < mSimulationDuration;
}

template < typename Matrix, typename T, bool matlabFilterOutput >
T ElectricalSimulation< Matrix, T, matlabFilterOutput >::GetCurrentSimulationStepTime() const
{
    return mTime - mStepStartTime;
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::FinshStep()
{
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::UpdateAllThermalStatesPowerDissipation()
{
    BOOST_FOREACH ( boost::shared_ptr< electrical::Cellelement< Matrix > > &elem, mCellElements )
        elem->UpdateThermalStatePowerDissipation( mTime );
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::UpdateAllThermalStatesPowerDissipationWithoutTime()
{
    BOOST_FOREACH ( boost::shared_ptr< electrical::Cellelement< Matrix > > &elem, mCellElements )
        elem->UpdateThermalStatePowerDissipation();
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::ResetAllThermalStatesPowerDissipation()
{
    BOOST_FOREACH ( boost::shared_ptr< electrical::Cellelement< Matrix > > &elem, mCellElements )
        elem->ResetThermalStatePowerDissipation();
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::InitializeStopCriterion()
{
    mNumberOfSteps = 0;
    SaveStatesForLaterReset();
}

template < typename Matrix, typename T, bool matlabFilterOutput >
bool ElectricalSimulation< Matrix, T, matlabFilterOutput >::IsStopCriterionFulfilled() const
{
    bool isStop = false;
    for ( size_t i = 0; i < mSocStates.size(); ++i )
    {
        if ( fabs( mSocValuesSteps[0][i] - mSocStates[i]->GetValue() ) > mSocStopCriterion )
        {
            isStop = true;
            break;
        }
    }

    return isStop;
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::SaveStatesForLaterReset()
{
    if ( mTimeSteps.size() == mNumberOfSteps )
        mTimeSteps.resize( mNumberOfSteps + 1 );
    if ( mStatesSteps.size() == mNumberOfSteps )
        mStatesSteps.resize( mNumberOfSteps + 1 );
    if ( mSocValuesSteps.size() == mNumberOfSteps )
    {
        mSocValuesSteps.resize( mNumberOfSteps + 1 );
        mSocValuesSteps[mNumberOfSteps].resize( mSocStates.size() );
    }

    mTimeSteps[mNumberOfSteps] = mTime;
    mStatesSteps[mNumberOfSteps] = mStateSystemGroup.mStateVector;
    for ( size_t i = 0; i < mSocStates.size(); ++i )
        mSocValuesSteps[mNumberOfSteps][i] = mSocStates[i]->GetValue();

    mNumberOfSteps++;
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::ResetStatesToPointOfTime( T time )
{
    if ( time <= mTimeSteps[0] || time > mTimeSteps[mNumberOfSteps - 1] || mNumberOfSteps < 2 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                             "errorResetStatesToPointOfTimeExecution" );
    if ( time == mTimeSteps[mNumberOfSteps - 1] )    // No need to reset if thermal stop criterion is triggered after
                                                     // full step time time has been simulated
        return;

    // Get index, which is the index for the vectors mStateSystemGroup and mSocValuesSteps before time
    size_t index = 0;
    for ( size_t i = 1; i < mNumberOfSteps; ++i )
        if ( mTimeSteps[i] > time )
        {
            index = i - 1;
            break;
        }

    // Calculate factor1 and factor2, which are used to scale linearly between the point of time for index and (index+1)
    const T deltaTime = mTimeSteps[index + 1] - mTimeSteps[index];
    const T factor1 = ( mTimeSteps[index + 1] - time ) / deltaTime;
    const T factor2 = ( time - mTimeSteps[index] ) / deltaTime;

    // Reset time, states and SoCs
    mTime = time;
    mStateSystemGroup.mStateVector = mStatesSteps[index] * factor1 + mStatesSteps[index] * factor2;
    for ( size_t i = 0; i < mSocStates.size(); ++i )
    {
        const T newSocValue = mSocValuesSteps[index][i] * factor1 + mSocValuesSteps[index + 1][i] * factor2;
        const T deltaCapacityASec =
         ( newSocValue - mSocValuesSteps[mNumberOfSteps - 1][i] ) / 100.0 * mSocStates[i]->GetMaxCapacity();
        mSocStates[i]->UpdateCapacity( deltaCapacityASec );
    }
    UpdateSystemValues();

    BOOST_FOREACH ( boost::shared_ptr< ::state::ThermalState< T > > &thermalState, mThermalStates )
        thermalState->ResetPowerDissipationToTime( time );
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::SaveCapacityForLaterReset()
{
    for ( size_t i = 0; i < mSocStates.size(); ++i )
        mCapcityValues[i] = mSocStates[i]->GetCapacity();
}

template < typename Matrix, typename T, bool matlabFilterOutput >
void ElectricalSimulation< Matrix, T, matlabFilterOutput >::LoadCapacityForLaterReset()
{
    for ( size_t i = 0; i < mSocStates.size(); ++i )
        mSocStates[i]->SetCapacity( mCapcityValues[i] );
}
}
#endif
