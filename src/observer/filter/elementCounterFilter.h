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
* File Name : elementCounterFilter.h
* Creation Date : 08-10-2015
* Last Modified : Mo 06 Jun 2016 13:26:20 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ELEMENTCOUNTERFILTER_
#define _ELEMENTCOUNTERFILTER_

#ifndef __NO_STRING__
// STD
#include <iostream>

// ETC
#include "../observer.h"
#include "../observerException.h"
#include "filter.h"

#include "../../electrical/twoport.h"
#include "../../thermal/blocks/elements/thermal_element.h"


namespace observer
{
/// ElementCounterFilterBase counts the number of observed elements
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType >
class ElementCounterFilterBase : public Filter< T, TConcrete, ArgumentType >
{
    typedef Filter< T, TConcrete, ArgumentType > FilterT;

    public:
    ElementCounterFilterBase()
        : FilterT(){};
    virtual ~ElementCounterFilterBase() { std::cout << "Observed Elements: " << mObservedElements << std::endl; };

    private:
    protected:
    size_t mObservedElements = 0;    // C++11
};

/// Empty class, needs to be filled by template class specialization
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType >
class ElementCounterFilter : public ElementCounterFilterBase< T, TConcrete, ArgumentType >
{
};

/// Class specialization for thermal::ThermalElement
template < typename T >
class ElementCounterFilter< T, thermal::ThermalElement, ThermalPreperation >
 : public ElementCounterFilterBase< T, thermal::ThermalElement, ThermalPreperation >
{
    typedef ElementCounterFilterBase< T, thermal::ThermalElement, ThermalPreperation > FilterT;

    public:
    ElementCounterFilter()
        : FilterT(){};
    virtual void PrepareFilter( ThermalPreperation& prepData ) { this->mObservedElements = prepData.mAreas.size(); };
    virtual ~ElementCounterFilter(){};
};

/// Class specialization for electrical::TwoPort
template < typename T >
class ElementCounterFilter< T, electrical::TwoPort, PreparationType >
 : public ElementCounterFilterBase< T, electrical::TwoPort, PreparationType >
{
    typedef ElementCounterFilterBase< T, electrical::TwoPort, PreparationType > FilterT;

    public:
    ElementCounterFilter()
        : FilterT(){};
    virtual void PrepareFilter( PreparationType& prepData ) { this->mObservedElements = prepData.mNumberOfElements; };
    virtual ~ElementCounterFilter(){};
};


template < typename T >
using ElementCounterFilterTwoPort = ElementCounterFilter< T, electrical::TwoPort, PreparationType >;

template < typename T >
using ElementCounterFilterThermal = ElementCounterFilter< T, thermal::ThermalElement, ThermalPreperation >;
}
#endif /* __NO_STRING__ */
#endif /* _ELEMENTCOUNTERFILTER_ */
