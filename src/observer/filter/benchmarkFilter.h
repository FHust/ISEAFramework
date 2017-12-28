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
* File Name : benchmarkFilter.h
* Creation Date : 07-10-2015
* Last Modified : Mo 06 Jun 2016 13:25:55 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _BENMARKFILTER_
#define _BENMARKFILTER_


#ifndef __NO_STRING__
// STD
#include <iostream>

// BOOST
#define BOOST_DATE_TIME_NO_LIB
#include "boost/date_time.hpp"

// ETC
#include "../observer.h"
#include "../observerException.h"
#include "filter.h"

#include "../../electrical/twoport.h"
#include "../../thermal/blocks/elements/thermal_element.h"


namespace observer
{
/// The BenchmarkFilter allows to time the intervall between creation and destruction and also the amount of times the
/// function ProcessData is called
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class BenchmarkFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;
    BenchmarkFilter();             //< start the timing process
    virtual ~BenchmarkFilter();    //< ends the timing process and produces an output

    virtual void
    ProcessData( const typename FilterT::Data_t& data, const double t );    //< this call increases a counter

    private:
    size_t mCounter;
    boost::posix_time::ptime mStart;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
BenchmarkFilter< T, TConcrete, ArgumentType >::BenchmarkFilter()
    : mCounter( 0 )
    , mStart( boost::posix_time::microsec_clock::local_time() )
{
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void BenchmarkFilter< T, TConcrete, ArgumentType >::ProcessData( const typename FilterT::Data_t& data, const double t )
{
    ++mCounter;
    FilterT::ProcessData( data, t );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
BenchmarkFilter< T, TConcrete, ArgumentType >::~BenchmarkFilter()
{
    boost::posix_time::ptime stop = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration elapsed = stop - mStart;
    std::cout << "Mittelwert: " << elapsed.total_microseconds() / 1000.0 / double( mCounter ) << std::endl;
    std::cout << "Cycles: " << mCounter << std::endl;
}

template < typename T >
using BenchmarkFilterTwoPort = BenchmarkFilter< T, electrical::TwoPort, PreparationType< T > >;

template < typename T >
using BenchmarkFilterThermal = BenchmarkFilter< T, thermal::ThermalElement, ThermalPreperation >;
}

#endif /* __NO_STRING__ */
#endif /* _BENMARKFILTER_ */
