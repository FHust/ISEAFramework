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
* File Name : cellelement.h
* Creation Date : 10-11-2012
* Last Modified : Mi 02 Jul 2014 13:18:07 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CELLELEMENT_
#define _CELLELEMENT_
// STD

#include "serialtwoport.h"

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../states/soc.h"
#include "../states/surface_soc.h"
#include "../states/thermal_state.h"

#include "../object/lookup_obj2d_with_state.h"


namespace electrical
{

/// Basic behaviour of a cellelement with Temperature, Soc and an electrical circuit which is represented through a
/// SerialTwoPort
template < typename T = myMatrixType >
class Cellelement : public SerialTwoPort< T >
{
    public:
    Cellelement( boost::shared_ptr< ::state::ThermalState< double > > thermalState,
                 boost::shared_ptr< electrical::state::Soc > socState, const bool observable = false );
    virtual ~Cellelement(){};
    virtual bool IsCellelement() const;    ///< Is this element a cellelement?
    virtual void SetCurrentFromActiveBalancer( ScalarUnit current );    ///< This function gives the chance to actively balance a battery with a current

    double GetSocStateValue() const;
    double GetSurfaceSocStateValue() const;

#ifndef _SYMBOLIC_
    virtual void LoadInternalData( std::vector< double > &dataVector );
    virtual void SaveInternalData( std::vector< double > &dataVector );
#endif

    virtual void CalculateStateDependentValues();

    virtual void SetSurfaceSOC( boost::shared_ptr< state::SurfaceSoc > surfaceSoc );

    virtual const char *GetName() const;

    void UpdateThermalStatePowerDissipation( double time );    ///<Stores power and time of this cell elements into its thermal state
    void UpdateThermalStatePowerDissipation();    ///<Stores power of this cell elements into its thermal state
    void ResetThermalStatePowerDissipation();     ///<Resets power dissipation data of thermal state

    const boost::shared_ptr< state::Soc > &GetSoc() const { return mSoc; }
    const boost::shared_ptr< ::state::ThermalState< double > > &GetThermalState() const { return mThermalState; }

    bool mSurfaceSocSet;

    protected:
    private:
    boost::shared_ptr< ::state::ThermalState< double > > mThermalState;
    boost::shared_ptr< state::Soc > mSoc;
    boost::shared_ptr< state::SurfaceSoc > mSurfaceSoc;
};

template < typename T >
Cellelement< T >::Cellelement( boost::shared_ptr< ::state::ThermalState< double > > thermalState,
                               boost::shared_ptr< electrical::state::Soc > socState, const bool observable )
    : SerialTwoPort< T >( observable )
    , mSurfaceSocSet( false )
    , mThermalState( thermalState )
    , mSoc( socState )
{
}

#ifndef _SYMBOLIC_
template < typename T >
void Cellelement< T >::LoadInternalData( std::vector< double > &dataVector )
{
    mSoc->SetCapacity( dataVector.back() );
    dataVector.pop_back();

    TwoPort< T >::LoadInternalData( dataVector );
}

template < typename T >
void Cellelement< T >::SaveInternalData( std::vector< double > &dataVector )
{
    TwoPort< T >::SaveInternalData( dataVector );
    dataVector.push_back( mSoc->GetValue() );
}
#endif

template < typename T >
void Cellelement< T >::CalculateStateDependentValues()
{
    SerialTwoPort< T >::CalculateStateDependentValues();

    if ( !this->IsObservable() )
        this->CalculateCurrentValue();

#ifndef _SYMBOLIC_
    // Update the Soc state. Therefore calculate the charge which is stored/released in the previous simulation step
    // period,
    this->mSoc->UpdateCapacity( this->mCurrentValue * this->mStateSystemGroup->mDt );
    if ( mSurfaceSocSet )
        mSurfaceSoc->UpdateLookUp();
#endif
}

template < typename T >
bool Cellelement< T >::IsCellelement() const
{
    return true;
}


template < typename T >
double Cellelement< T >::GetSurfaceSocStateValue() const
{
    return mSurfaceSoc->GetValue();
}

template < typename T >
void Cellelement< T >::SetSurfaceSOC( boost::shared_ptr< state::SurfaceSoc > surfaceSoc )
{
    mSurfaceSoc = surfaceSoc;
    mSurfaceSocSet = true;
}

template < typename T >
void Cellelement< T >::SetCurrentFromActiveBalancer( ScalarUnit currentval )
{
    currentval += this->mCurrent( 0, this->mCurrent.n_cols - 1 );
    this->SetCurrent( currentval );
}

template < typename T >
double Cellelement< T >::GetSocStateValue() const
{
    return mSoc->GetValue();
}

template < typename T >
const char *Cellelement< T >::GetName() const
{
    return "Cellelement";
}

#ifndef _SYMBOLIC_
template < typename T >
void Cellelement< T >::UpdateThermalStatePowerDissipation( double time )
{
#ifndef _SYMBOLIC_
    mThermalState->AddPowerDissipation( this->GetPowerValue(), time );
#endif
}

template < typename T >
void Cellelement< T >::UpdateThermalStatePowerDissipation()
{
#ifndef _SYMBOLIC_
    mThermalState->SetFixedPowerDissipation( this->GetPowerValue() );
#endif
}

template < typename T >
void Cellelement< T >::ResetThermalStatePowerDissipation()
{
#ifndef _SYMBOLIC_
    mThermalState->ResetPowerDissipation();
#endif
}
#endif

} /* END NAMESPACE */
#endif /* _CELLELEMENT_ */
