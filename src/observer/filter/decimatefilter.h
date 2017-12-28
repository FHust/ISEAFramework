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
* File Name : decimatefilter.h
* Creation Date : 18-11-2012
* Last Modified : Fr 04 Sep 2015 11:34:30 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _DECIMATEFILTER_
#define _DECIMATEFILTER_

// STD
#include <limits>
#include <cmath>
#include "filter.h"
#include "../../electrical/twoport.h"
#include "../../thermal/blocks/elements/thermal_element.h"

namespace observer
{

/// Decimates the incoming data with a least stepsize of timeDifference
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class DecimateFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;
    DecimateFilter( double timeDifference )
        : FilterT()
        , mTimeDifference( timeDifference )
        , mLastPassedTime( -10 ){};
    virtual ~DecimateFilter(){};

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        if ( std::abs( t - mLastPassedTime ) >= mTimeDifference )
        {
            FilterT::ProcessData( data, t );
            mLastPassedTime = t;
        }
    };

    private:
    double mTimeDifference;
    double mLastPassedTime;

    protected:
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class DecimateFilterBase : public DecimateFilter< T, TConcrete, ArgumentType >
{
    public:
    DecimateFilterBase( double timeDifference )
        : DecimateFilter< T, TConcrete, ArgumentType >( timeDifference ){};
};

template < typename T >
using DecimateFilterTwoPort = DecimateFilterBase< T, electrical::TwoPort, PreparationType< T > >;

template < typename T >
using DecimateFilterThermal = DecimateFilterBase< T, thermal::ThermalElement, ThermalPreperation >;

} /* END NAMESPACE */
#endif /* _DECIMATEFILTER_ */
