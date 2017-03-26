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
#ifndef _THERMAL_VISUALIZER_
#define _THERMAL_VISUALIZER_

#include <vector>
#include <boost/foreach.hpp>
#include "../thermal/blocks/elements/thermal_element.h"


#include "observer.h"
#include "../misc/macros.h"


namespace observer
{
using std::vector;
using thermal::ThermalElement;
using thermal::Cartesian;


/// ThermalObserver creates output data as CSV files that can be processed to visualize the simulation
template < typename T = double >
class ThermalObserver : public Observer< T, ThermalElement, ThermalPreperation >
{
    public:
    /**
      * param[in] thermalElementsOfAreas Thermal elements belonging to areas, ordered by the same sequence, to later
     * extract temperature for areas
      * param[in] areas All areas to be displayed, depicted as a set of vertices
      * param[in] vertices All vertices from which areas can be constructed
      * param[in] fileNameTemperatures,fileNameVertices,fileNameAreas Data is written into these csv-files
      */
    ThermalObserver( vector< ThermalElement< T > * > thermalElementsOfAreas, vector< vector< size_t > > &areas,
                     vector< vector< size_t > > &volumes, vector< misc::StrCont > &volumeNames,
                     vector< geometry::Cartesian< T > > &vertices );

    virtual void PrepareFilter( Filter< T, ThermalElement, ThermalPreperation > *filt );

    virtual void operator()( double t );
    virtual ~ThermalObserver();

    private:
    ThermalPreperation mPrepareParameter;
    std::vector< ThermalElement< T > * > mThermalElementsOfAreas;
};

template < typename T >
ThermalObserver< T >::ThermalObserver( vector< ThermalElement< T > * > thermalElementsOfAreas,
                                       vector< vector< size_t > > &areas, vector< vector< size_t > > &volumes,
                                       vector< misc::StrCont > &volumeNames, vector< Cartesian< T > > &vertices )
    : mPrepareParameter( areas, volumes, volumeNames, vertices )
    , mThermalElementsOfAreas( thermalElementsOfAreas )
{
    BOOST_FOREACH ( const vector< size_t > &area, areas )
    {
        if ( area.size() == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyArea" );
    }

    if ( volumes.size() != volumeNames.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "volumeNamesVecNonEqualVolumesVec" );

    BOOST_FOREACH ( const vector< size_t > &volume, volumes )
    {
        if ( volume.size() == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyVolume" );
    }
}

template < typename T >
void ThermalObserver< T >::PrepareFilter( Filter< T, ThermalElement, ThermalPreperation > *filt )
{
    filt->PrepareFilter( mPrepareParameter );
}


template < typename T >
void ThermalObserver< T >::operator()( double t )
{
    if ( Observer< T, ThermalElement, ThermalPreperation >::mBegin )
    {
        Observer< T, ThermalElement, ThermalPreperation >::mBegin->ProcessData( mThermalElementsOfAreas, t );
    }
}

template < typename T >
ThermalObserver< T >::~ThermalObserver()
{
}
}
#endif
