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
* File Name : createObserver.h
* Creation Date : 11-08-2015
* Last Modified : 25.02.2016 16:56:39
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CREATEOBSERVER_
#define _CREATEOBSERVER_

// STD
#include <memory>

// ETC
#include "observerfactorybuilder.h"
#include "observerclasswrapper.h"
#include "../../observer/twoPortObserver.h"
#include "../../observer/thermal_observer.h"
#include "../../observer/filter/vcpfilter.h"
#include "../../misc/matrixInclude.h"

template < class containerType, typename matrixType, bool FilterTypeChoice >
observer::TwoPortObserver< matrixType >*
CreateTwoPortObserver( containerType& observablePorts, const xmlparser::XmlParameter* rootparam,
                       real_T* voltageOutputVec = 0, real_T * currentOutputVec = 0, real_T * powerOutputVec = 0,
                       real_T * socOutputVec = 0, real_T * socSurfaceOutputVec = 0 )
{

    std::auto_ptr< observer::TwoPortObserver< matrixType > > obs( new observer::TwoPortObserver< matrixType >( observablePorts ) );
    observer::TwoPortObserver< matrixType >* obs_ptr = obs.get();

    if ( FilterTypeChoice )
    {
        if ( rootparam )
        {
            boost::scoped_ptr< factory::Factory< observer::Filter< matrixType, electrical::TwoPort, observer::PreparationType >, factory::ArgumentTypeObserver > > fct(
             factory::BuildObserverFactoryTwoPort< matrixType, FilterTypeChoice >() );

            if ( rootparam->HasElement( "Observer" ) &&
                 rootparam->GetElementChild( "Observer" )->HasElement( "Electrical" ) )
            {
                std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
                 rootparam->GetElementChild( "Observer" )->GetElementChildren( "Electrical" );
                for ( size_t i = 0; i < children.size(); ++i )
                {
                    obs->AddFilter( fct->CreateInstance( children.at( i ) ) );
                }
            }
        }
    }
    else if ( voltageOutputVec && currentOutputVec && powerOutputVec && socOutputVec && socSurfaceOutputVec )
    {
        obs->AddFilter( new observer::VCPFilter< matrixType >( voltageOutputVec, currentOutputVec, powerOutputVec,
                                                               socOutputVec, socSurfaceOutputVec ) );
    }
    obs.release();
    return obs_ptr;
}

template < typename matrixType, bool FilterTypeChoice >
observer::ThermalObserver< matrixType >*
CreateThermalObserver( const xmlparser::XmlParameter* rootparam,
                       std::vector< thermal::ThermalElement< matrixType >* > thermalElementsOfAreas,
                       std::vector< std::vector< size_t > >& areas, std::vector< std::vector< size_t > >& volumes,
                       std::vector< misc::StrCont >& volumeNames, std::vector< geometry::Cartesian< matrixType > >& vertices )
{

    std::auto_ptr< observer::ThermalObserver< matrixType > > obs(
     new observer::ThermalObserver< matrixType >( thermalElementsOfAreas, areas, volumes, volumeNames, vertices ) );
    observer::ThermalObserver< matrixType >* obs_ptr = obs.get();

    boost::scoped_ptr< factory::Factory< observer::Filter< matrixType, thermal::ThermalElement, observer::ThermalPreperation >, factory::ArgumentTypeObserver > > fct(
     factory::BuildObserverFactoryThermal< matrixType, FilterTypeChoice >() );

    if ( FilterTypeChoice )
    {
        if ( rootparam->HasElement( "Observer" ) && rootparam->GetElementChild( "Observer" )->HasElement( "Thermal" ) )
        {

            std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
             rootparam->GetElementChild( "Observer" )->GetElementChildren( "Thermal" );
            for ( size_t i = 0; i < children.size(); ++i )
            {
                obs->AddFilter( fct->CreateInstance( children.at( i ) ) );
            }
        }
    }
    obs.release();
    return obs_ptr;
}

#endif /* _CREATEOBSERVER_ */
