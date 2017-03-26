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
/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : soc.cpp
* Creation Date : 01-10-2012
* Last Modified : Di 13 Okt 2015 16:56:19 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "soc.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include "../exceptions/error_proto.h"

namespace electrical
{
namespace state
{


Soc::Soc( double maxCapcity /* [Ah] */, double currentState /*[%]*/, std::vector< double > measurmentpoints )
    : ::state::Dgl_state()
    , mMaxCapAsec( maxCapcity * 3600 )
    , mCurCapAsec( mMaxCapAsec * currentState / 100 )
    , mCurrent( 0 )
    , mMeasurementpoints( measurmentpoints )
{
    std::vector< double >::iterator it;
    std::sort( mMeasurementpoints.begin(), mMeasurementpoints.end() );
    it = std::unique( mMeasurementpoints.begin(), mMeasurementpoints.end() );
    mMeasurementpoints.resize( it - mMeasurementpoints.begin() );
}


void Soc::SetCapacity( double capacity )
{
    mCurCapAsec = capacity;
    if ( mCurCapAsec > mMaxCapAsec )
        mCurCapAsec = mMaxCapAsec;
    else if ( mCurCapAsec < 0 )
        mCurCapAsec = 0;
}


void Soc::UpdateCapacity( double deltaCapacity )
{
    mCurCapAsec += deltaCapacity;
    if ( mCurCapAsec > mMaxCapAsec )
        mCurCapAsec = mMaxCapAsec;
    else if ( mCurCapAsec < 0 )
        mCurCapAsec = 0;
}


double Soc::GetMaxDt() const
{
    std::vector< double > measurements( mMeasurementpoints );    // Unsolved mistery why mMeasurementpoints(
                                                                 // parser::Make1DLookupTable<  double
                                                                 // >(measurmentpoints) ) is not enough
    std::vector< double >::iterator it;

    const double absSoc = mCurCapAsec / mMaxCapAsec;

    if ( mCurrent < 0 )
    {
        it = ::std::lower_bound( measurements.begin(), measurements.end(), absSoc * 100 );
        if ( it != measurements.begin() )
            --it;
    }
    else if ( mCurrent > 0 )
    {
        it = ::std::upper_bound( measurements.begin(), measurements.end(), absSoc * 100 );
    }
    else if ( mCurrent == 0 )
        return ::std::numeric_limits< double >::max();
    else
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DefaultError" );
    }

    return ::std::abs( ( ( *it ) / 100 - absSoc ) ) * mMaxCapAsec / ::std::abs( mCurrent );
}


double Soc::GetCapacity() const { return mCurCapAsec; }


double Soc::GetMaxCapacity() const { return mMaxCapAsec; }


double Soc::GetValue() const { return mCurCapAsec / mMaxCapAsec * 100; }
}
}
