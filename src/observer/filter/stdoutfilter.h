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
* File Name : stdoutfilter.h
* Creation Date : 18-11-2012
* Last Modified : Fr 04 Sep 2015 10:35:07 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _STDOUTFILTER_
#define _STDOUTFILTER_

#ifdef __EXCEPTIONS__
// BOOST
#include <boost/shared_ptr.hpp>

// STD
#include <iostream>

#include "filter.h"

#include "../../electrical/cellelement.h"

namespace observer
{

template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType >
class StdoutFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;
    StdoutFilter(){};
    virtual ~StdoutFilter(){};

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        std::cout << "Time: " << t << std::endl;
        for ( size_t i = 0; i < data.size(); ++i )
        {
            std::cout << data[i] << std::endl;
        }
    };

    private:
    protected:
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class StdoutFilterBase : public StdoutFilter< T, TConcrete, ArgumentType >
{
    public:
    StdoutFilterBase()
        : StdoutFilter< T, TConcrete, ArgumentType >(){};
};


template < typename T >
class StdoutFilterBase< T, electrical::TwoPort, PreparationType > : public StdoutFilter< T, electrical::TwoPort, PreparationType >
{
    public:
    StdoutFilterBase()
        : StdoutFilter< T, electrical::TwoPort, PreparationType >(){};

    typedef Filter< T, electrical::TwoPort, PreparationType > FilterT;

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        std::cout << "Time: " << t << "\n";
        for ( size_t i = 0; i < data.size(); ++i )
        {
            std::cout << " Element " << i + 1;

            electrical::TwoPort< T >* port = data[i];
            std::cout << " Voltage: " << port->GetVoltageValue();
            std::cout << " Current: " << port->GetCurrentValue();
            std::cout << " Power: " << port->GetPowerValue();

            if ( port->IsCellelement() )
            {
                electrical::Cellelement< T >* cell = static_cast< electrical::Cellelement< T >* >( port );
                std::cout << " Soc: " << cell->GetSocStateValue();
            }

            std::cout << "\n";
        }

        std::cout << "\n\n\n";

        FilterT::ProcessData( data, t );
    }
};


template < typename T >
using StdoutFilterTwoPort = StdoutFilterBase< T, electrical::TwoPort, PreparationType >;


} /* END NAMESPACE */
#endif /* __EXCEPTIONS__ */
#endif /* _STDOUTFILTER_ */
