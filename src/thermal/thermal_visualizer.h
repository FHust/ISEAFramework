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

// STD
#include <vector>
// BOOST
#include <boost/foreach.hpp>

// ETC
#include "blocks/elements/thermal_element.h"
#include "../misc/StrCont.h"

#include "../exceptions/error_proto.h"

#ifndef __NO_STRING__
#include <fstream>
#endif

#include "../misc/macros.h"


namespace thermal
{
using std::vector;

/// ThermalVisualizer creates output data as CSV files that can be processed to visualize the simulation
template < typename T = double >
class ThermalVisualizer
{
    public:
    /**
      * param[in] thermalElementsOfAreas Thermal elements belonging to areas, ordered by the same sequence, to later
     * extract temperature for areas
      * param[in] areas All areas to be displayed, depicted as a set of vertices
      * param[in] vertices All vertices from which areas can be constructed
      * param[in] fileNameTemperatures,fileNameVertices,fileNameAreas Data is written into these csv-files
      */
    ThermalVisualizer( vector< ThermalElement< T > * > thermalElementsOfAreas, vector< vector< size_t > > &areas,
                       vector< vector< size_t > > &volumes, vector< misc::StrCont > &volumeNames,
                       vector< Cartesian< T > > &vertices, const char fileNameTemperatures[] = "Patch_Temperatures.csv",
                       const char fileNameVertices[] = "Patch_Vertices.csv",
                       const char fileNameAreas[] = "Patch_Areas.csv",
                       const char fileNameVolumes[] = "Patch_Volumes.csv",
                       const char fileNameVolumeNames[] = "Patch_VolumeNames.csv" );

    virtual ~ThermalVisualizer();
    void WriteCsvForTemperatureUpdates( T time );

    private:
    vector< ThermalElement< T > * > mThermalElementsOfAreas;
#ifndef __NO_STRING__
    std::ofstream mFileTemperatures;
#endif
};

template < typename T >
ThermalVisualizer< T >::ThermalVisualizer( vector< ThermalElement< T > * > thermalElementsOfAreas,
                                           vector< vector< size_t > > &areas, vector< vector< size_t > > &volumes,
                                           vector< misc::StrCont > &volumeNames, vector< Cartesian< T > > &vertices,
                                           const char fileNameTemperatures[], const char fileNameVertices[],
                                           const char fileNameAreas[], const char fileNameVolumes[],
                                           const char fileNameVolumeNames[] )

    : mThermalElementsOfAreas( thermalElementsOfAreas )
#ifndef __NO_STRING__
    , mFileTemperatures( fileNameTemperatures )
#endif
{
    UNUSED( areas );
    UNUSED( vertices );
    UNUSED( volumes );
    UNUSED( volumeNames );
    UNUSED( fileNameTemperatures );
    UNUSED( fileNameVertices );
    UNUSED( fileNameAreas );
    UNUSED( fileNameVolumes );
#ifndef __NO_STRING__
    mFileTemperatures.close();
    mFileTemperatures.open( fileNameTemperatures, std::ios::app );
    std::ofstream fileVertices( fileNameVertices );
    std::ofstream fileAreas( fileNameAreas );
    std::ofstream fileVolumes( fileNameVolumes );
    std::ofstream fileVolumeNames( fileNameVolumeNames );

    BOOST_FOREACH ( const Cartesian< T > &vertex, vertices )
        fileVertices << vertex.GetX() << ", " << vertex.GetY() << ", " << vertex.GetZ() << "\n";

    BOOST_FOREACH ( const vector< size_t > &area, areas )
    {
        if ( area.size() == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyArea" );

        vector< size_t >::const_iterator it = area.begin();
        fileAreas << *it;
        for ( ++it; it != area.end(); ++it )
            fileAreas << ", " << *it;
        fileAreas << "\n";
    }

    if ( volumes.size() != volumeNames.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "volumeNamesVecNonEqualVolumesVec" );

    BOOST_FOREACH ( const vector< size_t > &volume, volumes )
    {
        if ( volume.size() == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyVolume" );

        vector< size_t >::const_iterator it = volume.begin();
        fileVolumes << *it;
        for ( ++it; it != volume.end(); ++it )
            fileVolumes << ", " << *it;
        fileVolumes << "\n";
    }

    BOOST_FOREACH ( misc::StrCont &volumeName, volumeNames )
    {
        fileVolumeNames << volumeName << "\n";
    }

    fileVertices.close();
    fileAreas.close();
    fileVolumes.close();
    fileVolumeNames.close();

    WriteCsvForTemperatureUpdates( 0.0 );
    mFileTemperatures << std::flush;
#endif
}

template < typename T >
ThermalVisualizer< T >::~ThermalVisualizer()
{
}

template < typename T >
void ThermalVisualizer< T >::WriteCsvForTemperatureUpdates( T time )
{
    UNUSED( time );
#ifndef __NO_STRING__
    mFileTemperatures << time;
    BOOST_FOREACH ( const ThermalElement< T > *thermalElement, mThermalElementsOfAreas )
        mFileTemperatures << ", " << thermalElement->GetTemperature();
    mFileTemperatures << "\n";
#endif
}
}
#endif
