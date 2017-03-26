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
#include "TestTimeSeries.h"
#include "../../time_series/time_series.h"
#include "../thermal/exception_tester.h"
#include <fstream>
#include <sstream>
#include <cstring>

using namespace electrical;
static const double sDelta = 0.000001;


void TestTimeSeries::TestEvalNoInterpolation()
{
    vector< double > timesData( 3 );
    timesData.at( 0 ) = 0.0;
    timesData.at( 1 ) = 1.0;
    timesData.at( 2 ) = 2.5;
    vector< double > valuesData( 3 );
    valuesData.at( 0 ) = 1.2;
    valuesData.at( 1 ) = 0.0;
    valuesData.at( 2 ) = -3.5;
    double time = 0.0;
    size_t index = 0;
    double value = 0.0;
    EvalNoInterpolation<> evaluator;

    evaluator.CalculateValue( true, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 1.2, sDelta );

    time = 0.5;
    index = 0;
    evaluator.CalculateValue( false, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 1.2, sDelta );

    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 0.0 ), 1.0, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 1.1 ), 1.0, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 1.2 ), 1.0, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 2.5 ), 2.5, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 4.7 ), 2.5, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 5.5 ),
                     std::numeric_limits< double >::max(), sDelta );

    time = 1.0;
    index = 1;
    evaluator.CalculateValue( true, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 0.0, sDelta );

    time = 1.5;
    index = 1;
    evaluator.CalculateValue( false, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 0.0, sDelta );

    time = 2.5;
    index = 2;
    evaluator.CalculateValue( true, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, -3.5, sDelta );

    time = 3.0;
    index = 2;
    evaluator.CalculateValue( false, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, -3.5, sDelta );
}

void TestTimeSeries::TestEvalLinearInterpolation()
{
    vector< double > timesData( 4 );
    timesData.at( 0 ) = 0.0;
    timesData.at( 1 ) = 1.0;
    timesData.at( 2 ) = 1.5;
    timesData.at( 3 ) = 2.5;
    vector< double > valuesData( 4 );
    valuesData.at( 0 ) = 1.2;
    valuesData.at( 1 ) = 0.0;
    valuesData.at( 2 ) = 1.5;
    valuesData.at( 3 ) = -3.5;
    EvalLinearInterpolation<> evaluator;

    double offset, slope;
    evaluator.CalculateOffsetSlope( timesData, valuesData, 0, offset, slope );
    TS_ASSERT_DELTA( offset, 1.2, sDelta );
    TS_ASSERT_DELTA( slope, -1.2, sDelta );
    evaluator.CalculateOffsetSlope( timesData, valuesData, 1, offset, slope );
    TS_ASSERT_DELTA( offset, -3.0, sDelta );
    TS_ASSERT_DELTA( slope, 3.0, sDelta );
    evaluator.CalculateOffsetSlope( timesData, valuesData, 2, offset, slope );
    TS_ASSERT_DELTA( offset, 9.0, sDelta );
    TS_ASSERT_DELTA( slope, -5.0, sDelta );
    evaluator.CalculateOffsetSlope( timesData, valuesData, 3, offset, slope );
    TS_ASSERT_DELTA( offset, -3.5, sDelta );
    TS_ASSERT_DELTA( slope, 0.0, sDelta );


    double time = 0.0;
    size_t index = 0;
    double value = 0.0;


    evaluator.CalculateValue( true, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 1.2, sDelta );

    time = 0.5;
    index = 0;
    evaluator.CalculateValue( false, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 0.6, sDelta );

    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 0.0 ), 0.5, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 0.3 ), 0.75, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 0.599 ),
                     1.0 - 0.001 / 1.2, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 0.601 ),
                     1.4 + 0.001 / 3.0, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 0.899 ),
                     1.5 - 0.001 / 3.0, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 0.901 ),
                     1.86 + 0.001 / 5.0, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 1.6 ), 2.0, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 4.099 ),
                     2.5 - 0.001 / 5.0, sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 4.101 ),
                     std::numeric_limits< double >::max(), sDelta );
    TS_ASSERT_DELTA( evaluator.GetTimeUntilMaxValueDeviation( timesData, valuesData, time, value, index, 5.0 ),
                     std::numeric_limits< double >::max(), sDelta );

    time = 1.0;
    index = 1;
    evaluator.CalculateValue( true, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 0.0, sDelta );

    time = 1.2;
    index = 1;
    evaluator.CalculateValue( true, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 0.6, sDelta );

    time = 1.5;
    index = 2;
    evaluator.CalculateValue( false, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, 1.5, sDelta );

    time = 1.9;
    index = 2;
    evaluator.CalculateValue( true, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, -0.5, sDelta );

    time = 2.5;
    index = 3;
    evaluator.CalculateValue( true, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, -3.5, sDelta );

    time = 3.0;
    index = 3;
    evaluator.CalculateValue( false, timesData, valuesData, time, index, value );
    TS_ASSERT_DELTA( value, -3.5, sDelta );
}
void TestTimeSeries::TestTimeSeriesCreation()
{
    //#if defined(__EXCEPTIONS__)
    {
        electrical::TimeSeries< double, electrical::EvalNoInterpolation > timeSeries;
        vector< double > timesData( 1 );
        timesData.at( 0 ) = 0.0;
        vector< double > valuesData;

        TS_ASSERT_THROWS_EQUALS( timeSeries.ResetData( timesData, valuesData ), const std::runtime_error &e,
                                 strcmp( e.what(), "The size of mTimesData and mValuesData must be equal." ), 0 );
        timesData.pop_back();
        TS_ASSERT_THROWS_EQUALS( timeSeries.ResetData( timesData, valuesData ), const std::runtime_error &e,
                                 strcmp( e.what(), "mTimesData must have at least one element." ), 0 );

        timesData.resize( 5 );
        timesData.at( 0 ) = -1.0;
        timesData.at( 1 ) = 0.0;
        timesData.at( 2 ) = 0.0;
        timesData.at( 3 ) = 0.5;
        timesData.at( 4 ) = 1.5;
        valuesData.resize( 5 );
        valuesData.at( 0 ) = 1.0;
        valuesData.at( 1 ) = -1.0;
        valuesData.at( 2 ) = -0.5;
        valuesData.at( 3 ) = 0.0;
        valuesData.at( 4 ) = 2.5;

        TS_ASSERT_THROWS_EQUALS( timeSeries.ResetData( timesData, valuesData ), const std::runtime_error &e,
                                 strcmp( e.what(), "First point of time must not be negativ." ), 0 );
        timesData.erase( timesData.begin() );
        valuesData.erase( valuesData.begin() );
        TS_ASSERT_THROWS_EQUALS( timeSeries.ResetData( timesData, valuesData ), const std::runtime_error &e,
                                 strcmp( e.what(),
                                         "Points of time in must be in strictly ascending order: (0)th point of time "
                                         "smaller than (0+1)th point of time for all 0." ),
                                 0 );
        timesData.erase( timesData.begin() );
        valuesData.erase( valuesData.begin() );
        TS_ASSERT_THROWS_NOTHING( timeSeries.ResetData( timesData, valuesData ) );
    }
#ifndef __NO_STRING__
    {
        std::stringstream testStream;
        testStream << 0.0 << ", " << 0.0 << ", "
                   << "\n";
        TS_ASSERT_THROWS_EQUALS( electrical::TimeSeries< double > timeSeries( testStream ), const std::runtime_error &e,
                                 strcmp( e.what(), "Each line must be of the form: time, value." ), 0 );
    }
    {
        std::stringstream testStream;
        testStream << 0.0 << " " << 0.0 << "\n";
        TS_ASSERT_THROWS_EQUALS( electrical::TimeSeries< double > timeSeries( testStream ), const std::runtime_error &e,
                                 strcmp( e.what(), "Each line must be of the form: time, value." ), 0 );
    }
    {
        std::string line( " +1.34 , -.45645 " );
        electrical::TimeSeries< double > timeSeries;
        double time = 0, value = 0;
        TS_ASSERT_THROWS_NOTHING( timeSeries.ParseCsvLine( line, time, value ) );
        TS_ASSERT_DELTA( time, 1.34, sDelta );
        TS_ASSERT_DELTA( value, -0.45645, sDelta );
    }
#endif
//#endif
#ifndef __NO_STRING__
    {
        std::ofstream timeSeriesFileOut( "timeSeries.txt" );
        timeSeriesFileOut << 0.0 << ", " << 0.0 << "\n";
        timeSeriesFileOut << 1.0 << ", " << 4.5 << "\n";
        timeSeriesFileOut << 1.0 << ", " << 4.5 << "\n";
        timeSeriesFileOut << 1.1 << ", " << -6.3 << "\n";
        timeSeriesFileOut << 2.0 << ", " << -2.0 << "\n";
        timeSeriesFileOut << 2.0 << ", " << -2.0 << "\n";
        timeSeriesFileOut << 3.3 << ", " << 1.0 << std::endl;
        std::ifstream timeSeriesFileIn( "timeSeries.txt" );
        electrical::TimeSeries< double, electrical::EvalNoInterpolation > timeSeries( timeSeriesFileIn );

        TS_ASSERT_EQUALS( timeSeries.mTimesData.size(), 5 );
        TS_ASSERT_EQUALS( timeSeries.mValuesData.size(), 5 );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 1 ), 1.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 1 ), 4.5, sDelta );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 2 ), 1.1, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 2 ), -6.3, sDelta );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 3 ), 2.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 3 ), -2.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 4 ), 3.3, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 4 ), 1.0, sDelta );
    }
    {
        std::stringstream testStream;
        testStream << 0.0 << ", " << 0.0 << "\n";
        testStream << 1.0 << ", " << 4.5 << "\n";
        testStream << 1.0 << ", " << 4.5 << "\n";
        testStream << 1.1 << ", " << -6.3 << "\n";
        testStream << 2.0 << ", " << -2.0 << "\n";
        testStream << 2.0 << ", " << -2.0 << "\n";
        testStream << 3.3 << ", " << 1.0 << "\n";
        electrical::TimeSeries< double, electrical::EvalNoInterpolation > timeSeries( testStream );

        TS_ASSERT_EQUALS( timeSeries.mTimesData.size(), 5 );
        TS_ASSERT_EQUALS( timeSeries.mValuesData.size(), 5 );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 1 ), 1.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 1 ), 4.5, sDelta );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 2 ), 1.1, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 2 ), -6.3, sDelta );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 3 ), 2.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 3 ), -2.0, sDelta );
        TS_ASSERT_DELTA( timeSeries.mTimesData.at( 4 ), 3.3, sDelta );
        TS_ASSERT_DELTA( timeSeries.mValuesData.at( 4 ), 1.0, sDelta );
    }
#endif
}

void TestTimeSeries::TestTimeSeriesNoInterpolation()
{
    vector< double > timesData( 3 );
    timesData.at( 0 ) = 0.0;
    timesData.at( 1 ) = 1.0;
    timesData.at( 2 ) = 2.5;
    vector< double > valuesData( 3 );
    valuesData.at( 0 ) = 1.2;
    valuesData.at( 1 ) = 0.0;
    valuesData.at( 2 ) = -3.5;
    electrical::TimeSeries< double, electrical::EvalNoInterpolation > timeSeries;

    timeSeries.ResetData( timesData, valuesData );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 0 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 0.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 1.2, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 0.5 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 0 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 0.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 1.2, sDelta );

    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 0.0 ), 1.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 1.1 ), 1.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 1.2 ), 1.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 2.5 ), 2.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 4.7 ), 2.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 5.5 ), std::numeric_limits< double >::max(), sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 1.0 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 1 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 1.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 0.0, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 1.5 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 1 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 1.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 0.0, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 2.5 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 2 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 2.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), -3.5, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 3.0 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 2 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 3.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), -3.5, sDelta );
}

void TestTimeSeries::TestTimeSeriesWithLinearInterpolation()
{
    vector< double > timesData( 4 );
    timesData.at( 0 ) = 0.0;
    timesData.at( 1 ) = 1.0;
    timesData.at( 2 ) = 1.5;
    timesData.at( 3 ) = 2.5;
    vector< double > valuesData( 4 );
    valuesData.at( 0 ) = 1.2;
    valuesData.at( 1 ) = 0.0;
    valuesData.at( 2 ) = 1.5;
    valuesData.at( 3 ) = -3.5;
    electrical::TimeSeries< double, electrical::EvalLinearInterpolation > timeSeries;

    timeSeries.ResetData( timesData, valuesData );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 0 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 0.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 1.2, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 0.5 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 0 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 0.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 0.6, sDelta );

    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 0.0 ), 0.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 0.3 ), 0.75, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 0.599 ), 1.0 - 0.001 / 1.2, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 0.601 ), 1.4 + 0.001 / 3.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 0.899 ), 1.5 - 0.001 / 3.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 0.901 ), 1.86 + 0.001 / 5.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 1.6 ), 2.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 4.099 ), 2.5 - 0.001 / 5.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 4.101 ), std::numeric_limits< double >::max(), sDelta );
    TS_ASSERT_DELTA( timeSeries.GetTimeUntilMaxValueDeviation( 5.0 ), std::numeric_limits< double >::max(), sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 1.0 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 1 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 1.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 0.0, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 1.2 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 1 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 1.2, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 0.6, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 1.5 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 2 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 1.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), 1.5, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 1.9 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 2 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 1.9, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), -0.5, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 2.5 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 3 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 2.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), -3.5, sDelta );

    timeSeries.SetTimeAndTriggerEvaluation( 3.0 );
    TS_ASSERT_EQUALS( timeSeries.mIndex, 3 );
    TS_ASSERT_DELTA( timeSeries.GetTime(), 3.0, sDelta );
    TS_ASSERT_DELTA( timeSeries.GetValue(), -3.5, sDelta );
}

void TestTimeSeries::TestTimeSeriesScaling()
{
    vector< double > timesData( 4 );
    timesData.at( 0 ) = 0.0;
    timesData.at( 1 ) = 1.0;
    timesData.at( 2 ) = 1.5;
    timesData.at( 3 ) = 2.5;
    vector< double > valuesData( 4 );
    valuesData.at( 0 ) = 1.2;
    valuesData.at( 1 ) = 0.0;
    valuesData.at( 2 ) = 1.5;
    valuesData.at( 3 ) = -3.5;
    electrical::TimeSeries< double, electrical::EvalLinearInterpolation > timeSeries;
    //#if defined(__EXCEPTIONS__)
    TS_ASSERT_THROWS_EQUALS( timeSeries.ResetData( timesData, valuesData, -1.5, 2.5 ), const std::runtime_error &e,
                             strcmp( e.what(), "timeScalingFactor must be bigger than zero." ), 0 );
    //#endif
    timeSeries.ResetData( timesData, valuesData, 1.5, -0.5 );

    TS_ASSERT_DELTA( timeSeries.mTimesData.at( 0 ), 0.0 * 1.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.mTimesData.at( 1 ), 1.0 * 1.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.mTimesData.at( 2 ), 1.5 * 1.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.mTimesData.at( 3 ), 2.5 * 1.5, sDelta );
    TS_ASSERT_DELTA( timeSeries.mValuesData.at( 0 ), 1.2 * ( -0.5 ), sDelta );
    TS_ASSERT_DELTA( timeSeries.mValuesData.at( 1 ), 0.0 * ( -0.5 ), sDelta );
    TS_ASSERT_DELTA( timeSeries.mValuesData.at( 2 ), 1.5 * ( -0.5 ), sDelta );
    TS_ASSERT_DELTA( timeSeries.mValuesData.at( 3 ), -3.5 * ( -0.5 ), sDelta );

#ifndef __NO_STRING__
    //#if defined(__EXCEPTIONS__)
    {
        std::stringstream testStream;
        testStream << 0.0 << ", " << 0.0 << "\n";
        testStream << 1.0 << ", " << 4.5 << "\n";
        testStream << 2.0 << ", " << -2.0 << "\n";
        typedef electrical::TimeSeries< double, electrical::EvalNoInterpolation > CurProf;
        TS_ASSERT_THROWS_EQUALS( CurProf( testStream, -0.5, 2.23 ), const std::runtime_error &e,
                                 strcmp( e.what(), "timeScalingFactor must be bigger than zero." ), 0 );
    }
    //#endif
    std::stringstream testStream;
    testStream << 0.0 << ", " << 0.0 << "\n";
    testStream << 1.0 << ", " << 4.5 << "\n";
    testStream << 2.0 << ", " << -2.0 << "\n";
    electrical::TimeSeries< double, electrical::EvalNoInterpolation > timeSeriesByStream( testStream, 0.5, 2.23 );

    TS_ASSERT_DELTA( timeSeriesByStream.mTimesData.at( 0 ), 0.0 * 0.5, sDelta );
    TS_ASSERT_DELTA( timeSeriesByStream.mTimesData.at( 1 ), 1.0 * 0.5, sDelta );
    TS_ASSERT_DELTA( timeSeriesByStream.mTimesData.at( 2 ), 2.0 * 0.5, sDelta );
    TS_ASSERT_DELTA( timeSeriesByStream.mValuesData.at( 0 ), 0.0 * 2.23, sDelta );
    TS_ASSERT_DELTA( timeSeriesByStream.mValuesData.at( 1 ), 4.5 * 2.23, sDelta );
    TS_ASSERT_DELTA( timeSeriesByStream.mValuesData.at( 2 ), -2.0 * 2.23, sDelta );
#endif
}
