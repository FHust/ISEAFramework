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
#include "TestSimulation.h"

#include "../../thermal/thermal_simulation.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"
#include "../../misc/StrCont.h"
#include "../../misc/matrixInclude.h"
#include "../../thermal/electrical_simulation.h"
#include "../../thermal/block_observer.h"
#include "exception_tester.h"
#include <fstream>

#include "../../misc/macros.h"

static const double sDelta = 0.000001;


void TestSimulation::RunScenario( double electricalStopCriterion, double thermalStopCriterion, bool stopCriterionTriggered )
{
    UNUSED( stopCriterionTriggered );
    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double > > electricalSimulation;
    boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > thermalSimulation;
    boost::scoped_ptr< observer::TwoPortObserver< myMatrixType > > electricalObserver;
    boost::scoped_ptr< thermal::BlockObserver< myMatrixType, double > > blockObserver;
    boost::scoped_ptr< observer::ThermalObserver< double > > thermalVisualizer;

    try
    {
        boost::scoped_ptr< xmlparser::XmlParser > parser;
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromFile( "./TestPouchBag.xml" );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double >( rootXmlNode, 10.0, 100.0, electricalStopCriterion, &cells ) );
        electricalObserver.reset( new observer::TwoPortObserver< myMatrixType >( cells ) );
#ifndef __NO_STRING__
        electricalObserver->AddFilter( new observer::CsvFilterTwoPort< myMatrixType >( "electricalStates.csv" ) );
#endif

        std::vector< std::vector< boost::shared_ptr< ThermalState< double > > > > thermalStatesOfCellBlocks;
        thermalSimulation.reset(
         new simulation::ThermalSimulation< myMatrixType, double, true >( rootXmlNode, 10.0, 100.0, thermalStopCriterion,
                                                                          &thermalVisualizer, &electricalSimulation->mThermalStates,
                                                                          &thermalStatesOfCellBlocks ) );
        blockObserver.reset( new thermal::BlockObserver< myMatrixType, double >( cells, thermalStatesOfCellBlocks ) );
    }
    catch ( std::exception &e )
    {
        TS_FAIL( e.what() );
        return;
    }
    catch ( ... )
    {
        TS_FAIL( "Unittest failed, because an unknown exception occurred." );
        return;
    }


    TS_ASSERT_DELTA( thermalSimulation->mThermalSystem->mAirTemperature, 25.0, sDelta );
    if ( thermalStopCriterion <= 0.0 )
    {
        TS_ASSERT_DELTA( thermalSimulation->mThermalStateStopCriterion, 3.5, sDelta );
    }
    if ( electricalStopCriterion <= 0.0 )
    {
        TS_ASSERT_DELTA( electricalSimulation->mSocStopCriterion, 4.1, sDelta );
    }

    TS_ASSERT_DELTA( thermalSimulation->mTolerance.mLength, 0.00000101, sDelta * sDelta );
    TS_ASSERT_DELTA( thermalSimulation->mTolerance.mAngle.GetDeg(), 0.00101, sDelta * sDelta );
    TS_ASSERT_DELTA( thermalSimulation->mTolerance.mPercentOfQuantity, 1.00001, sDelta * sDelta );


#if defined( SPARSE_MATRIX_FORMAT ) || defined( _EIGEN_ )
    electricalSimulation->CheckIfSumlationTimeHasNotEndedAndSetStepStartTime();
    electricalSimulation->mRootTwoPort->SetCurrent( 1.0 );
    electricalSimulation->UpdateSystem();
    electricalSimulation->ResetAllThermalStatesPowerDissipation();
    electricalSimulation->CheckLoopConditionAndSetDeltaTime( 0.0 );
    electricalSimulation->UpdateSystemValues();
    electricalSimulation->UpdateAllThermalStatesPowerDissipation();
    ( *electricalObserver )( electricalSimulation->mTime );
    thermalSimulation->CheckLoopConditionAndSetDeltaTime( electricalSimulation->mTime );
    thermalSimulation->UpdateSystem();
    if ( thermalVisualizer )
        ( *thermalVisualizer )( thermalSimulation->mTime );

    thermalSimulation->UpdateAllThermalStatesTemperatures();
    electricalSimulation->FinshStep();
    blockObserver->Initialize( electricalSimulation->mTime );

    electricalSimulation->mRootTwoPort->SetCurrent( -2.0 );
    electricalSimulation->mTime = 10.0;
    electricalSimulation->UpdateSystem();
    ( *blockObserver )( electricalSimulation->mTime );
#else
    // Simulate
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > > >::type stepperThermal =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType > >::type stepperElectrical =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType >() );

    // Initialize Current profile
    double currentChangeTime = 0.0;
    electrical::TimeSeries< double, electrical::EvalNoInterpolation > currentProfile;
    vector< double > timesData( 3 );
    timesData.at( 0 ) = 0.0;
    timesData.at( 1 ) = 40.0;
    timesData.at( 2 ) = 70.0;
    vector< double > valuesData( 3 );
    valuesData.at( 0 ) = 10.0;
    valuesData.at( 1 ) = -8.0;
    valuesData.at( 2 ) = 0.0;
    currentProfile.ResetData( timesData, valuesData );

    while ( electricalSimulation->CheckIfSumlationTimeHasNotEndedAndSetStepStartTime() )
    {
        // Current profile
        if ( electricalSimulation->mTime >= currentChangeTime )
        {
            currentProfile.SetTimeAndTriggerEvaluation( electricalSimulation->mTime );
            electricalSimulation->mRootTwoPort->SetCurrent( currentProfile.GetValue() );
            currentChangeTime = currentProfile.GetTimeUntilMaxValueDeviation( 0.0 );
        }


        // Electrical
        electricalSimulation->UpdateSystem();
        // Initialize block Observer
        electricalSimulation->UpdateSystemValues();
        blockObserver->Initialize( electricalSimulation->mTime );
        electricalSimulation->ResetAllThermalStatesPowerDissipation();
        electricalSimulation->InitializeStopCriterion();
        while ( electricalSimulation->CheckLoopConditionAndSetDeltaTime( currentChangeTime ) )
        {
            // Handle stop criterion
            if ( electricalSimulation->IsStopCriterionFulfilled() )
                break;

            electricalSimulation->mStateSystemGroup.mDt = electricalSimulation->mDeltaTime;
#ifdef _EIGEN_
            myMatrixType state = electricalSimulation->mStateSystemGroup.mStateVector.topRows(
             electricalSimulation->mStateSystemGroup.mStateVector.n_rows - 1 );
#else
            myMatrixType state = electricalSimulation->mStateSystemGroup.mStateVector.submat(
             0, 0, electricalSimulation->mStateSystemGroup.mStateVector.n_rows - 2, 0 );    // mElectricalState
#endif
            while ( stepperElectrical.try_step( boost::ref( *electricalSimulation->mEqSystem )
                                                // mElectricalStates
                                                ,
                                                state, electricalSimulation->mTime,
                                                electricalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
            {
                electricalSimulation->mStateSystemGroup.mDt = electricalSimulation->mDeltaTime;
            }

            electricalSimulation->UpdateSystemValues();
            electricalSimulation->UpdateAllThermalStatesPowerDissipation();
            ( *electricalObserver )( electricalSimulation->mTime );
            electricalSimulation->SaveStatesForLaterReset();
        }

        // Thermal
        thermalSimulation->InitializeStopCriterion();
        while ( thermalSimulation->CheckLoopConditionAndSetDeltaTime( electricalSimulation->mTime ) )
        {
            // Handle stop criterion
            if ( thermalSimulation->IsStopCriterionFulfilled() )
            {
                electricalSimulation->ResetStatesToPointOfTime( thermalSimulation->mTime );
                break;
            }

            while ( stepperThermal.try_step( boost::ref( *thermalSimulation->mThermalSystem ),
                                             thermalSimulation->mTemperatures, thermalSimulation->mTime,
                                             thermalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
            {
            }

            thermalSimulation->UpdateSystem();
            ( *thermalVisualizer )( thermalSimulation->mTime );
            thermalSimulation->UpdateAllThermalStatesTemperatures();
        }
        // thermalSimulation->UpdateAllThermalStatesTemperatures(); //if no thermal state stop criterion is used, this
        // belongs here

        if ( stopCriterionTriggered && currentProfile.GetValue() != 0.0 )
        {
            TS_ASSERT_LESS_THAN( electricalSimulation->GetCurrentSimulationStepTime(), 10.0 );
        }
        else
        {
            TS_ASSERT_DELTA( electricalSimulation->GetCurrentSimulationStepTime(), 10.0, sDelta );
        }

        // Finish step
        electricalSimulation->FinshStep();
        ( *blockObserver )( electricalSimulation->mTime );
    }


    // Actual testing happens here
    std::ifstream csvFile( "Data_i_timeEl_cur_volt_soc_heat_temp.csv" );
    std::string line;
    std::vector< observerOutput > dataFromObserver;
    // std::cout << std::endl;
    while ( std::getline( csvFile, line ) )
    {
        dataFromObserver.push_back( observerOutput() );
        EvaluateCsvFile( line, dataFromObserver.back().mI, dataFromObserver.back().mTimeEl,
                         dataFromObserver.back().mCur, dataFromObserver.back().mVolt, dataFromObserver.back().mSoc,
                         dataFromObserver.back().mHeat, dataFromObserver.back().mTemp );
        // std::cout << dataFromObserver.back().mI << ", " << dataFromObserver.back().mTimeEl << ", " <<
        // dataFromObserver.back().mCur << ", " << dataFromObserver.back().mVolt << ", " << dataFromObserver.back().mSoc
        // << ", " << dataFromObserver.back().mHeat << ", " << dataFromObserver.back().mTemp << std::endl;
    }

    TS_ASSERT_EQUALS( dataFromObserver.size() % 2, 0 );
    for ( size_t thermalBlockIndex = 0; thermalBlockIndex < 2; ++thermalBlockIndex )
    {
        size_t simulationCounter = 0;
        double lastHeatBeforeCurrentChange = 0.0;

        for ( size_t lineNumber = thermalBlockIndex + 2; lineNumber < dataFromObserver.size(); lineNumber = lineNumber + 2 )
        {
            observerOutput &thisOutput = dataFromObserver.at( lineNumber );
            observerOutput &previousOutput = dataFromObserver.at( lineNumber - 2 );
            TS_ASSERT_EQUALS( thisOutput.mI, thermalBlockIndex );

            if ( simulationCounter == 0 )
            {
                if ( thisOutput.mTimeEl >= 40.0 + sDelta )
                {
                    lastHeatBeforeCurrentChange = previousOutput.mHeat;
                    ++simulationCounter;
                }
            }
            else if ( simulationCounter == 1 )
            {
                if ( thisOutput.mTimeEl >= 70.0 + sDelta )
                {
                    lastHeatBeforeCurrentChange = previousOutput.mHeat;
                    ++simulationCounter;
                }
            }
            else if ( simulationCounter == 2 )
            {
                if ( thisOutput.mTemp < previousOutput.mTemp )
                    ++simulationCounter;
            }

            if ( simulationCounter == 0 )
            {
                if ( thermalBlockIndex == 0 )
                {
                    TS_ASSERT_DELTA( thisOutput.mCur, 10.0 / 2.0 + 0.1, 0.2 );
                    TS_ASSERT_DELTA( thisOutput.mCur + dataFromObserver.at( lineNumber + 1 ).mCur, 10.0, sDelta );
                }
                else
                {
                    TS_ASSERT_DELTA( thisOutput.mCur, 10.0 / 2.0 - 0.1, 0.2 );
                }
                TS_ASSERT_LESS_THAN( previousOutput.mVolt, thisOutput.mVolt );
                TS_ASSERT_LESS_THAN( previousOutput.mSoc, thisOutput.mSoc );
                if ( previousOutput.mTimeEl >= 10.0 &&
                     thermalStopCriterion > 0.0 )    // TODO: Why is the first step not as expected
                {
                    TS_ASSERT_LESS_THAN( previousOutput.mTemp, thisOutput.mTemp );
                }
            }
            else if ( simulationCounter == 1 )
            {
                if ( thermalBlockIndex == 0 )
                {
                    TS_ASSERT_DELTA( thisOutput.mCur, -8.0 / 2.0 + 0.1, 0.2 );
                    TS_ASSERT_DELTA( thisOutput.mCur + dataFromObserver.at( lineNumber + 1 ).mCur, -8.0, sDelta );
                }
                else
                {
                    TS_ASSERT_DELTA( thisOutput.mCur, -8.0 / 2.0 - 0.1, 0.2 );
                }
                TS_ASSERT_LESS_THAN( thisOutput.mVolt, previousOutput.mVolt );
                TS_ASSERT_LESS_THAN( thisOutput.mSoc, previousOutput.mSoc );
                TS_ASSERT_LESS_THAN( thisOutput.mHeat, lastHeatBeforeCurrentChange );
                TS_ASSERT_LESS_THAN( previousOutput.mTemp, thisOutput.mTemp );
            }
            else
            {

                if ( thermalBlockIndex == 0 )
                {
                    TS_ASSERT_DELTA( thisOutput.mCur, 0.0 + 0.1, 0.2 );
                    TS_ASSERT_DELTA( thisOutput.mCur + dataFromObserver.at( lineNumber + 1 ).mCur, 0.0, sDelta );
                }
                else
                {
                    TS_ASSERT_DELTA( thisOutput.mCur, 0.0 - 0.1, 0.2 );
                }
                TS_ASSERT_LESS_THAN( previousOutput.mVolt, thisOutput.mVolt );
                TS_ASSERT_DELTA( previousOutput.mSoc, thisOutput.mSoc, 0.2 );
                TS_ASSERT_LESS_THAN( thisOutput.mHeat, lastHeatBeforeCurrentChange / 100.0 );
                if ( simulationCounter == 2 )
                {
                    TS_ASSERT_LESS_THAN( previousOutput.mTemp, thisOutput.mTemp );
                }
                else
                {
                    TS_ASSERT_LESS_THAN( thisOutput.mTemp, previousOutput.mTemp );
                }
            }
        }
    }
// Thermal Block with index 1
#endif
}

void TestSimulation::TestScenario()
{
    RunScenario( -1.0, -1.0, false );
    RunScenario( 0.06, -1.0, true );
    RunScenario( -1.0, 0.0000000000001, true );
}

void TestSimulation::TestErrorHandlingAtConstruction()
{
    //#ifdef __EXCEPTIONS__
    std::ifstream TestPouchBagCollision( "./TestPouchBag.xml" );
    if ( !TestPouchBagCollision.is_open() )
    {
        TS_FAIL( "Couldn't open file TestPouchBag.xml" );
        return;
    }
    std::string inputString( ( std::istreambuf_iterator< char >( TestPouchBagCollision ) ), std::istreambuf_iterator< char >() );
    size_t pos = inputString.rfind( "<Children dy=\"0.05\" count=\"2\">" );
    size_t len = strlen( "<Children dy=\"0.05\" count=\"2\">" );
    inputString.replace( pos, len, "<Children dy=\"0.04\" count=\"2\">" );
    bool exceptionCaught = false;

    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double > > electricalSimulation;
    boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > thermalSimulation;
    boost::scoped_ptr< observer::TwoPortObserver< myMatrixType > > electricalObserver;
    boost::scoped_ptr< thermal::BlockObserver< myMatrixType, double > > blockObserver;
    boost::scoped_ptr< observer::ThermalObserver< double > > thermalVisualizer;

    try
    {
        boost::scoped_ptr< xmlparser::XmlParser > parser;
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromMem( inputString.c_str() );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double >( rootXmlNode, 10.0, 100.0, 5.0, &cells ) );
        electricalObserver.reset( new observer::TwoPortObserver< myMatrixType >( cells ) );
#ifndef __NO_STRING__
        electricalObserver->AddFilter( new observer::CsvFilterTwoPort< myMatrixType >( "electricalStates.csv" ) );
#endif

        std::vector< std::vector< boost::shared_ptr< ThermalState< double > > > > thermalStatesOfCellBlocks;
        thermalSimulation.reset(
         new simulation::ThermalSimulation< myMatrixType, double, true >( rootXmlNode, 10.0, 100.0, 5.0, &thermalVisualizer,
                                                                          &electricalSimulation->mThermalStates,
                                                                          &thermalStatesOfCellBlocks ) );
        blockObserver.reset( new thermal::BlockObserver< myMatrixType, double >( cells, thermalStatesOfCellBlocks ) );
    }
    catch ( std::runtime_error &e )
    {
        TS_ASSERT_EQUALS( strcmp( e.what(),
                                  "Collision between thermal block PouchBagThermal of class RectangularBlock at "
                                  "(0.000000,0.000000,0.000000) in xml-file line 211 and thermal block PouchBagThermal "
                                  "of class RectangularBlock at (0.000000,0.040000,0.000000) in xml-file line 211." ),
                          0 );
        exceptionCaught = true;
    }
    catch ( ... )
    {
        TS_FAIL( "Unittest failed, because an unknown exception occurred." );
    }

    TS_ASSERT( exceptionCaught );
    //#endif
}

void TestSimulation::EvaluateCsvFile( const std::string &line, size_t &i, double &timeEl, double &cur, double &volt,
                                      double &soc, double &heat, double &temp )
{
    istringstream lineStream( line );
    lineStream >> i;
    lineStream.ignore( 1 );
    lineStream >> timeEl;
    lineStream.ignore( 1 );
    lineStream >> cur;
    lineStream.ignore( 1 );
    lineStream >> volt;
    lineStream.ignore( 1 );
    lineStream >> soc;
    lineStream.ignore( 1 );
    lineStream >> heat;
    lineStream.ignore( 1 );
    lineStream >> temp;
}

void TestSimulation::TestParallelRC()    // TODO: If necessary
{
    ;
}
