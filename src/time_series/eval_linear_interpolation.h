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
#ifndef _EVAL_LINEAR_INTERPOLATION_
#define _EVAL_LINEAR_INTERPOLATION_

#include <vector>
#include <limits>
#include <cmath>

class TestTimeSeries;


namespace electrical
{
using std::vector;

template < typename T = double >
class EvalLinearInterpolation
{
    friend class ::TestTimeSeries;

    public:
    inline void CalculateValue( bool hasIndexChanged, const vector< T > &timesData, const vector< T > &valuesData,
                                const T &time, const size_t &index, T &value )
    {
        if ( hasIndexChanged )
            CalculateOffsetSlope( timesData, valuesData, index, mOffset, mSlope );
        value = mOffset + mSlope * time;
    }
    inline T GetTimeUntilMaxValueDeviation( const vector< T > &timesData, const vector< T > &valuesData, const T &time,
                                            const T &value, const size_t &index, const T &maxDeltaValue ) const
    {
        for ( size_t maxDeviationIndex = index; maxDeviationIndex < timesData.size() - 1; ++maxDeviationIndex )
        {
            if ( fabs( valuesData[maxDeviationIndex + 1] - value ) > maxDeltaValue )
            {
                T slope, offset;
                if ( maxDeviationIndex == index )
                {
                    offset = mOffset;
                    slope = mSlope;
                }
                else
                    CalculateOffsetSlope( timesData, valuesData, maxDeviationIndex, offset, slope );

                T timeUntilMaxDeltaValue = ( value + maxDeltaValue - offset ) / slope;
                if ( timeUntilMaxDeltaValue < timesData[maxDeviationIndex] ||
                     timeUntilMaxDeltaValue > timesData[maxDeviationIndex + 1] || timeUntilMaxDeltaValue < time )
                    timeUntilMaxDeltaValue = ( value - maxDeltaValue - offset ) / slope;
                return timeUntilMaxDeltaValue;
            }
        }
        return std::numeric_limits< T >::max();
    }

    private:
    void CalculateOffsetSlope( const vector< T > &timesData, const vector< T > &valuesData, const size_t &index,
                               T &offset, T &slope ) const
    {
        if ( index + 1 == timesData.size() )
        {
            slope = 0.0;
            offset = valuesData[index];
        }
        else
        {
            slope = ( valuesData[index + 1] - valuesData[index] ) / ( timesData[index + 1] - timesData[index] );
            offset = valuesData[index] - timesData[index] * slope;
        }
    }
    T mOffset;
    T mSlope;
};

}    // namespace
#endif    // Include guard
