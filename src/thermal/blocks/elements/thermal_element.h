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
#ifndef _THERMAL_ELEMENT_
#define _THERMAL_ELEMENT_

#include <boost/shared_ptr.hpp>
#include "../../../geometry/cartesian.h"
#include "../../materials/material.h"
#include "../../../states/thermal_state.h"
#include "../../../geometry/area.h"
#include "../../thermal_structs.h"

class TestThermalElements;
class TestBlocks;


namespace thermal
{
using boost::shared_ptr;
using namespace geometry;

/// ThermalElement represents a finite volume for thermal simulation
template < typename T = double >
class ThermalElement
{
    friend class ::TestThermalElements;
    friend class ::TestBlocks;

    public:
    virtual ~ThermalElement();
    void SetTemperature( T temperature );
    T GetTemperature() const;
    /// Returns the average power dissipation between the points of time (time-dt) and (time)
    T GetPowerDissipation( T time, T dt ) const;
    const Material< T >* GetMaterial() const;
    const Cartesian< T >& GetGridVertex() const;
    T GetVolume() const;
    /// Saves the current temperature and volume of this instance into thermal state. The thermal state later uses this
    /// information to calculate its average temperature.
    void UpdateThermalStateTemperature();
    /// Returns true if this instance has a thermal state (connection to electrical model), otherwise false
    bool HasThermalState() const;
    double GetThermalStateFactor() const { return mThermalStateFactor; }
    const ::state::ThermalState< T >* GetThermalState() const { return mThermalState; }

    protected:
    ThermalElement();
    /**
     * SetElement() is the substitute constructor function that sets the member variables and neds to be called in the
     * constructors of the sub classes
     * @param[in] gridVertex Using the finite volume method, each finite volume has a grid vertex
     * @param[in] thermalState Connection to electrical part of the model, null pointer means that there is no
     * connection
     * @param[in] thermalStateFactor Heat power dissipation from thermalState is multiplied by this factor
     */
    void SetElement( T initialTemperatureValue, const Material< T >* material, const Cartesian< T >& gridVertex,
                     T volume, ::state::ThermalState< T >* thermalState = 0, T thermalStateFactor = 1.0 );

    private:
    T mTemperature;
    const Material< T >* mMaterial;
    Cartesian< T > mGridVertex;
    T mVolume;
    ::state::ThermalState< T >* mThermalState;
    double mThermalStateFactor;
};


template < typename T >
ThermalElement< T >::ThermalElement()
    : mTemperature( 0.0 )
    , mMaterial( 0 )
    , mGridVertex( Cartesian< T >( 0.0, 0.0, 0.0 ) )
    , mVolume( 0.0 )
    , mThermalState( 0 )
    , mThermalStateFactor( 0.0 )
{
}

template < typename T >
ThermalElement< T >::~ThermalElement()
{
}

template < typename T >
void ThermalElement< T >::SetElement( T initialTemperatureValue, const Material< T >* material, const Cartesian< T >& gridVertex,
                                      T volume, ::state::ThermalState< T >* thermalState, T thermalStateFactor )
{
    mTemperature = initialTemperatureValue;
    mMaterial = material;
    mGridVertex = gridVertex;
    mVolume = volume;
    mThermalState = thermalState;
    mThermalStateFactor = thermalStateFactor;

    if ( !( thermalStateFactor > 0.0 ) || thermalStateFactor > 1.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "errorThermalStateFactor" );
    if ( volume < 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "errorVolZero2" );

    if ( mThermalState && volume != 0.0 )
        mThermalState->AddTemperature( mTemperature, mVolume );
}

template < typename T >
void ThermalElement< T >::SetTemperature( T temperature )
{
    mTemperature = temperature;
}

template < typename T >
T ThermalElement< T >::GetTemperature() const
{
    return mTemperature;
}

template < typename T >
T ThermalElement< T >::GetPowerDissipation( T time, T dt ) const
{
    return mThermalState ? mThermalState->GetPowerDissipation( time, dt ) * mThermalStateFactor : 0.0;
}

template < typename T >
const Material< T >* ThermalElement< T >::GetMaterial() const
{
    return mMaterial;
}

template < typename T >
const Cartesian< T >& ThermalElement< T >::GetGridVertex() const
{
    return mGridVertex;
}

template < typename T >
T ThermalElement< T >::GetVolume() const
{
    return mVolume;
}

template < typename T >
void ThermalElement< T >::UpdateThermalStateTemperature()
{
    mThermalState->AddTemperature( mTemperature, mVolume );
}

template < typename T >
bool ThermalElement< T >::HasThermalState() const
{
    return mThermalState != 0;
}
}
#endif
