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
#ifndef _THERMAL_BLOCK_
#define _THERMAL_BLOCK_

#include <vector>
#include <boost/shared_ptr.hpp>
#include "elements/rectangular_element.h"
#include "../../geometry/area.h"
#include "../../geometry/innerArea.h"
#include "../../geometry/block_geometry.h"
#include "../../states/thermal_state.h"
#include "../thermal_structs.h"

#include "../../misc/macros.h"

namespace thermal
{
using boost::shared_ptr;
using std::vector;
using std::pair;

/// ThermalBlock is an interface class, whose subclasses contain information to build different parts of the thermal
/// part of the model
template < typename T = double >
class ThermalBlock
{
    public:
    /**
     * @param[in] thermalStates Thermal States (connections to electrical model) that have to be distributed inside
     * block
     * @param[in] emplacementPoint The emplacement point of each block is determined thus: Take the x-, y- and
     * z-coordinates of all edges and combine the lowest x-, y- and z-coordinate into a point
     * @param[in] description A description of position, name and class that can be given out during error handling
     */
    ThermalBlock( const vector< shared_ptr< ::state::ThermalState< T > > > &thermalStates,
                  const Cartesian< T > &emplacementPoint, const char *description );
    virtual ~ThermalBlock();
    /**
     *
     * @param[out] thermalElements Instances of ThermalElement in block, position in vector determines index
     * @param[out] conductivityMatrix List of thermal conductivity between thermal elements with indices: From index as
     *vector position to index in IndexedValue
     * @param[out] surfaceAreas Indexed surfaces areas, indices show to which thermal element area belongs to
     * @param[out] blockGeometry Geometrical extension of block used for collision and adjacency tests
     */
    virtual void CreateData( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                             vector< vector< IndexedValue< T > > > &conductivityMatrix, vector< IndexedArea< T > > &surfaceAreas,
                             shared_ptr< BlockGeometry< T > > &blockGeometry ) const = 0;
    const vector< shared_ptr< ::state::ThermalState< T > > > &GetThermalStates() const;
    const Cartesian< T > &GetEmplacementPoint() const;
    const char *GetDescription() const;
    void GetDescription( misc::StrCont &description );
    virtual size_t GetNumberOfThermalElements() const = 0;
    virtual void GetInnerSurfaceAreas( vector< IndexedInnerArea< T > > &innerSurfaceAreas ) const;

    protected:
    const vector< shared_ptr< ::state::ThermalState< T > > > mThermalStates;
    const Cartesian< T > mPassedEmplacementPoint;
    const misc::StrCont mDescription;
};


template < typename T >
ThermalBlock< T >::ThermalBlock( const vector< shared_ptr< ::state::ThermalState< T > > > &thermalStates,
                                 const Cartesian< T > &emplacementPoint, const char *description )
    : mThermalStates( thermalStates )
    , mPassedEmplacementPoint( emplacementPoint )
    , mDescription( description )
{
}

template < typename T >
ThermalBlock< T >::~ThermalBlock()
{
}

template < typename T >
const vector< shared_ptr< ::state::ThermalState< T > > > &ThermalBlock< T >::GetThermalStates() const
{
    return mThermalStates;
}

template < typename T >
const Cartesian< T > &ThermalBlock< T >::GetEmplacementPoint() const
{
    return mPassedEmplacementPoint;
}

template < typename T >
const char *ThermalBlock< T >::GetDescription() const
{
    return mDescription;
}

template < typename T >
void ThermalBlock< T >::GetDescription( misc::StrCont &description )
{
    description = mDescription;
}

template < typename T >
void ThermalBlock< T >::GetInnerSurfaceAreas( vector< IndexedInnerArea< T > > &innerSurfaceAreas ) const
{
    UNUSED( innerSurfaceAreas );
    return;
}
}
#endif
