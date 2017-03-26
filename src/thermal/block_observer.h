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
#ifndef _BLOCK_OBSERVER_
#define _BLOCK_OBSERVER_

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include "../states/thermal_state.h"
#include "../electrical/twoport.h"
#include "../electrical/cellelement.h"
#include "../exceptions/error_proto.h"

#ifndef __NO_STRING__
#include <fstream>
#endif


namespace thermal
{
using std::vector;
using boost::shared_ptr;

/// BlockObserver outputs thermal/electrical data of blocks as CSV files by evaluating data from thermal states
template < typename Matrix, typename T >
class BlockObserver
{
    public:
    BlockObserver( vector< shared_ptr< ::electrical::TwoPort< Matrix > > > &cells,
                   vector< vector< shared_ptr< ::state::ThermalState< T > > > > &thermalStatesOfBlocks,
                   const char fileName[] = "Data_i_timeEl_cur_volt_soc_heat_temp.csv" );
    virtual ~BlockObserver();
    /// Must be called before operator(). Values outputed by operator() are averaged between times passed two both
    /// operator() and Initialize().
    void Initialize( T time );
    ///@return In every line: position in thermal states vector, time, current, voltage, SoC, heat generation, average
    ///temperature of cell/thermal block
    void operator()( T time );

    private:
#ifndef __NO_STRING__
    vector< ::electrical::TwoPortWithChild< Matrix > * > mCells;
    vector< vector< ::state::ThermalState< T > * > > mThermalStatesOfBlocks;
    vector< T > mLastCurrent;
    vector< T > mLastVoltage;
    vector< T > mLastTemperature;
    vector< T > mLastSoc;
    T mLastTime;
    vector< T > mThisCurrent;
    vector< T > mThisVoltage;
    vector< T > mThisTemperature;
    vector< T > mThisSoc;
    std::ofstream mCsvFile;
    bool mIsInitialized;
#endif
};

template < typename Matrix, typename T >
BlockObserver< Matrix, T >::BlockObserver(
#ifndef __NO_STRING__
 vector< shared_ptr< ::electrical::TwoPort< Matrix > > > &cells,
 vector< vector< shared_ptr< ::state::ThermalState< T > > > > &thermalStatesOfBlocks, const char fileName[] )
#else
 vector< shared_ptr< ::electrical::TwoPort< Matrix > > > &,
 vector< vector< shared_ptr< ::state::ThermalState< T > > > > &, const char * )
#endif
#ifndef __NO_STRING__
    : mLastCurrent( cells.size() )
    , mLastVoltage( cells.size() )
    , mLastTemperature( cells.size() )
    , mLastSoc( cells.size() )
    , mThisCurrent( cells.size() )
    , mThisVoltage( cells.size() )
    , mThisTemperature( cells.size() )
    , mThisSoc( cells.size() )
    , mCsvFile( fileName )
    , mIsInitialized( false )
#endif
{
#ifndef __NO_STRING__
    if ( cells.size() != thermalStatesOfBlocks.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NumberOfCellsAndThermalblocksUnequal" );

    mCells.reserve( cells.size() );
    BOOST_FOREACH ( const boost::shared_ptr< ::electrical::TwoPort< Matrix > > &cell, cells )
    {
        if ( !cell->CanHaveChildren() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "CellsNonTwoportsWithChild" );
        mCells.push_back( static_cast< ::electrical::TwoPortWithChild< Matrix > * >( cell.get() ) );
    }

    mThermalStatesOfBlocks.resize( thermalStatesOfBlocks.size() );
    for ( size_t i = 0; i < thermalStatesOfBlocks.size(); ++i )
    {
        mThermalStatesOfBlocks[i].reserve( thermalStatesOfBlocks[i].size() );
        BOOST_FOREACH ( const boost::shared_ptr< ::state::ThermalState< T > > &thermalState, thermalStatesOfBlocks[i] )
        {
            mThermalStatesOfBlocks[i].push_back( thermalState.get() );
        }
    }

    mCsvFile.close();
    mCsvFile.open( fileName, std::ios::app );
#endif
}

template < typename Matrix, typename T >
BlockObserver< Matrix, T >::~BlockObserver()
{
}

template < typename Matrix, typename T >
void BlockObserver< Matrix, T >::Initialize(
#ifndef __NO_STRING__
 T time )
#else
 T )
#endif
{
#ifndef __NO_STRING__
    // Calculate last values
    mLastTime = time;
    for ( size_t i = 0; i < mThermalStatesOfBlocks.size(); ++i )
    {
        mLastCurrent[i] = mCells[i]->GetCurrentValue();
        mLastVoltage[i] = mCells[i]->GetVoltageValue();

        mLastSoc[i] = 0.0;
        if ( mCells[i]->IsCellelement() )
            mLastSoc[i] = static_cast< electrical::Cellelement< Matrix > * >( mCells[i] )->GetSocStateValue();
        else
        {
            for ( size_t j = 0; j < mCells.size(); ++j )
                mLastSoc[i] += static_cast< electrical::Cellelement< Matrix > * >( mCells[i]->at( j ) )->GetSocStateValue();
            mLastSoc[i] /= mCells.size();
        }

        mLastTemperature[i] = 0.0;
        BOOST_FOREACH ( ::state::ThermalState< T > *thermalState, mThermalStatesOfBlocks[i] )
            mLastTemperature[i] += thermalState->GetValue();
        mLastTemperature[i] /= mThermalStatesOfBlocks[i].size();
    }

    // Set initialized-flag
    mIsInitialized = true;
#endif
}

template < typename Matrix, typename T >
void BlockObserver< Matrix, T >::operator()(
#ifndef __NO_STRING__
 T time )
#else
 T )
#endif
{
#ifndef __NO_STRING__
    // If initialized-flag is not set, thats an error, bacause Initialize needs to be called once before operator()
    if ( !mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "BlockObserverNonInitialized" );

    // Calculate current values
    for ( size_t i = 0; i < mThermalStatesOfBlocks.size(); ++i )
    {
        mThisCurrent[i] = mCells[i]->GetCurrentValue();
        mThisVoltage[i] = mCells[i]->GetVoltageValue();

        mThisSoc[i] = 0.0;
        if ( mCells[i]->IsCellelement() )
            mThisSoc[i] = static_cast< electrical::Cellelement< Matrix > * >( mCells[i] )->GetSocStateValue();
        else
        {
            for ( size_t j = 0; j < mCells.size(); ++j )
                mThisSoc[i] += static_cast< electrical::Cellelement< Matrix > * >( mCells[i]->at( j ) )->GetSocStateValue();
            mThisSoc[i] /= mCells.size();
        }

        mThisTemperature[i] = 0.0;
        BOOST_FOREACH ( ::state::ThermalState< T > *thermalState, mThermalStatesOfBlocks[i] )
            mThisTemperature[i] += thermalState->GetValue();
        mThisTemperature[i] /= mThermalStatesOfBlocks[i].size();
    }

    // Write output for time period between this and last call of this method
    for ( size_t i = 0; i < mThermalStatesOfBlocks.size(); ++i )
    {
        T powerDissipation = 0.0;
        BOOST_FOREACH ( ::state::ThermalState< T > *thermalState, mThermalStatesOfBlocks[i] )
        {
            powerDissipation += thermalState->GetPowerDissipation( time, time - mLastTime );
        }

        mCsvFile << i << ", " << ( mLastTime + time ) * 0.5 << ", " << ( mLastCurrent[i] + mThisCurrent[i] ) * 0.5
                 << ", " << ( mLastVoltage[i] + mThisVoltage[i] ) * 0.5 << ", " << ( mLastSoc[i] + mThisSoc[i] ) * 0.5
                 << ", " << powerDissipation << ", " << ( mLastTemperature[i] + mThisTemperature[i] ) * 0.5 << "\n";
    }
#endif
}
}
#endif
