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
* File Name : parallelrc.h
* Creation Date : 10-11-2012
* Last Modified : Mo 15 Jun 2015 16:10:36 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _PARALLELRC_
#define _PARALLELRC_

// STD
#include <cmath>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "electricalelement.h"

class TestElectricalElement;
class TestXML;

namespace electrical
{
/// This is an optimzed version of a parallel RC circuit. It mimics the equation  dU = ib/c - Uc /(RC).  This is one
/// equation less than through a parallel R and C Element
template < typename T = myMatrixType >
class ParallelRC : public ElectricalElement< T >
{
    friend class ::TestElectricalElement;
    friend class ::TestXML;

    public:
    ParallelRC( boost::shared_ptr< object::Object< double > > objR, boost::shared_ptr< object::Object< double > > objC,
                const bool observable = false );
    ParallelRC( object::Object< double >* objR, object::Object< double >* objC, const bool observable = false );

    virtual ~ParallelRC(){};

    virtual T* GetVoltage();                                   ///< returns the voltage over the RC element  Urc  = 1
    virtual void SetCurrent( const T current );                ///< Sets the current
    virtual void SetCurrent( const ScalarUnit currentval );    ///< Sets the current value
    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );
    virtual void CalculateStateDependentValues();
    virtual double GetValueC() const;    ///< Get Value of Object

    const object::Object< double >* GetTauObject() const;
    virtual ScalarUnit GetTauValue() const;    ///< Get value of tau Object

    virtual const char* GetName() const;

    private:
    ParallelRC();
    void SetVoltage();

    size_t mUID;

    boost::shared_ptr< object::Object< double > > mObjectTau;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup for capacity

    T mConstVoltageVector;
    T mDGLPart;

    protected:
};


template < typename T >
ParallelRC< T >::ParallelRC( boost::shared_ptr< object::Object< double > > objR,
                             boost::shared_ptr< object::Object< double > > objC, const bool observable )
    : ElectricalElement< T >( objR, observable )
    , mUID( 0 )
    , mObjectTau( objC )
    , mConstVoltageVector( T() )
    , mDGLPart( T() )
{
}

template < typename T >
ParallelRC< T >::ParallelRC( object::Object< double >* objR, object::Object< double >* objC, const bool observable )
    : ElectricalElement< T >( objR, observable )
    , mUID( 0 )
    , mObjectTau( boost::shared_ptr< object::Object< double > >( objC ) )
    , mConstVoltageVector( T() )
{
}

template < typename T >
void ParallelRC< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );
    SetVoltage();
}

template < typename T >
void ParallelRC< T >::SetCurrent( const ScalarUnit currentval )
{
    TwoPort< T >::SetCurrent( currentval );
}

template < typename T >
T* ParallelRC< T >::GetVoltage()
{

    if ( this->HasSystem() )
    {
        mDGLPart = this->mCurrent * this->GetValue() / GetTauValue();
        mDGLPart( this->mCurrent.n_rows - 1, mUID ) -= 1.0 / GetTauValue();
        this->mStateSystemGroup->mDglStateSystem.AddEquations( mUID, mDGLPart );
    }
    return TwoPort< T >::GetVoltage();
}

template < typename T >
void ParallelRC< T >::SetVoltage()
{
    mConstVoltageVector.zeros( this->mCurrent.n_rows, this->mCurrent.n_cols );
    mConstVoltageVector( 0, mUID ) = 1;
    this->mVoltage = mConstVoltageVector;
    mDGLPart.zeros( this->mCurrent.n_rows, this->mCurrent.n_cols );
}

template < typename T >
void ParallelRC< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    mUID = stateSystemGroup->mDglStateSystem.GetNewEquation();
    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
void ParallelRC< T >::CalculateStateDependentValues()
{
    this->CalculateVoltageValue();

    if ( this->mObservable )
        this->CalculateCurrentValue();

    this->mPowerValue = ::std::pow( this->mVoltageValue, static_cast< int >( 2 ) ) / this->GetValue();
}

template < typename T >
const char* ParallelRC< T >::GetName() const
{
    return "ParallelRC";
}

template < typename T >
double ParallelRC< T >::GetValueC() const
{
#ifndef _SYMBOLIC_
    return ( ( *mObjectTau )() / this->GetValue() );
#else
    return ( *mObjectTau )() / ( *ElectricalElement< T >::mObject )();
#endif
}

template < typename T >
const object::Object< double >* ParallelRC< T >::GetTauObject() const
{
    return mObjectTau.get();
}

template < typename T >
ScalarUnit ParallelRC< T >::GetTauValue() const
{
#ifndef _SYMBOLIC_
    return ( *mObjectTau )();
#else
    return symbolic::Symbolic( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjTau" );
#endif
}

}    // /* electrical */
#endif /* _PARALLELRC_ */
