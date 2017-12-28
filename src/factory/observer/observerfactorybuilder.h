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
* File Name : observerfactorybuilder.h
* Creation Date : 10-08-2015
* Last Modified : Mo 06 Jun 2016 13:45:09 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _OBSERVERFACTORYBUILDER_
#define _OBSERVERFACTORYBUILDER_

// ETC
#include "../../misc/matrixInclude.h"
#include "../../misc/macros.h"
#include "../../observer/observer.h"
#include "observerclasswrapper.h"
namespace factory
{
/// Add MatlabFilter as default
template < typename MatrixT, bool matlabSupport >
void AddExternalFilterThermal( Factory< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >* fct )
{
    fct->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::MatlabFilterThermal >, "MatlabFilter" );
    fct->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::BenchmarkFilterThermal >, "BenchmarkFilter" );
    fct->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::ElementCounterFilterThermal >,
                     "ElementCounterFilter" );
}

/// Workaround function for matlab S-Functions
template <>
void AddExternalFilterThermal< double, false >(
 Factory< observer::Filter< double, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >* fct );


/// Add MatlabFilter as default
template < typename MatrixT, bool matlabSupport >
void AddExternalFilterTwoPort(
 Factory< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >* fct )
{
    fct->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::MatlabFilterTwoPort >, "MatlabFilter" );
    fct->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::BenchmarkFilterTwoPort >, "BenchmarkFilter" );
    fct->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::ElementCounterFilterTwoPort >,
                     "ElementCounterFilter" );
}

/// Workaround function for matlab S-Functions
template <>
void AddExternalFilterTwoPort< myMatrixType, false >(
 Factory< observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > >, ArgumentTypeObserver >* fct );

/// Builds a Factory for ::observer::filter
template < typename MatrixT, bool matlabSupport >
Factory< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >*
BuildObserverFactoryTwoPort()
{
    Factory< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >* observerFactory =
     new Factory< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >();

    observerFactory->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::CsvFilterTwoPort >, "CSVFilter" );
    observerFactory->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::StdoutFilterTwoPort >,
                                 "StdoutFilter" );
    observerFactory->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::DecimateFilterTwoPort >,
                                 "DecimateFilter" );
    AddExternalFilterTwoPort< MatrixT, matlabSupport >( observerFactory );
    return observerFactory;
}

/// Builds a Factory for ::observer::filter
template < typename MatrixT, bool matlabSupport >
Factory< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >*
BuildObserverFactoryThermal()
{
    Factory< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >* observerFactory =
     new Factory< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >();

    observerFactory->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::CsvFilterThermal >, "CSVFilter" );
    observerFactory->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::ElementCounterFilterThermal >,
                                 "ElementCounterFilter" );
    observerFactory->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::DecimateFilterThermal >,
                                 "DecimateFilter" );
    AddExternalFilterThermal< MatrixT, matlabSupport >( observerFactory );
    return observerFactory;
}


} /* namespace factory */
#endif /* _OBSERVERFACTORYBUILDER_ */
