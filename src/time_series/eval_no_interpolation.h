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
#ifndef _EVAL_NO_INTERPOLATION_
#define _EVAL_NO_INTERPOLATION_

#include <vector>
#include <limits>
#include <cmath>


namespace electrical
{
using std::vector;

template < typename T = double >
class EvalNoInterpolation
{
    public:
    inline void CalculateValue( bool hasIndexChanged, const vector< T > &, const vector< T > &valuesData, const T &,
                                const size_t &index, T &value )
    {
        if ( hasIndexChanged )
            value = valuesData[index];
    }
    inline T GetTimeUntilMaxValueDeviation( const vector< T > &timesData, const vector< T > &valuesData, const T &,
                                            const T &value, const size_t &index, const T &maxDeltaValue ) const
    {
        for ( size_t maxDeviationIndex = index + 1; maxDeviationIndex < timesData.size(); ++maxDeviationIndex )
            if ( fabs( valuesData[maxDeviationIndex] - value ) >= maxDeltaValue )
                return timesData[maxDeviationIndex];
        return std::numeric_limits< T >::max();
    }
};

}    // namespace
#endif    // Include guard