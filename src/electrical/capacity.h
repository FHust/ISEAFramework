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
* File Name : capacity.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 15:49:04 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CAPACITY_
#define _CAPACITY_

// ETC
#include "electricalelement.h"

class TestDaeSystem;
class TestXML;
class TestTwoPortsWithChildren;

/// The electrical namespace contains all electrical elements and other classes that define an electrical element, e.g
/// SOC
namespace electrical
{
/// This class describes the the behavour of a capacity.  The differential equation it mimics dU  = 1 / C current
template < typename T = myMatrixType >
class Capacity : public ElectricalElement< T >
{
    friend class ::TestDaeSystem;
    friend class ::TestTwoPortsWithChildren;
    friend class ::TestXML;

    public:
    explicit Capacity( boost::shared_ptr< object::Object< double > > obj, const bool observable = false,
                       typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));
    explicit Capacity( object::Object< double >* obj, const bool observable = false,
                       typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));

    virtual ~Capacity(){};

    virtual T* GetVoltage();    /// Returns Uc = 1

    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );

    virtual const char* GetName() const;

    private:
    void SetVoltage();

    size_t mUID;    /// Row number for DGL (DE)  system

    T mVoltageValue;

    bool mVoltageSet;

    protected:
};

template < typename T >
Capacity< T >::Capacity( boost::shared_ptr< object::Object< double > > obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
    , mUID( 0 )
    , mVoltageValue( T() )
    , mVoltageSet( false )
{
}

template < typename T >
Capacity< T >::Capacity( object::Object< double >* obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : Capacity< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
void Capacity< T >::SetVoltage()
{
    mVoltageValue.zeros( this->mCurrent.n_rows, this->mCurrent.n_cols );
    mVoltageValue( 0, mUID ) = 1;
    mVoltageSet = true;
}

template < typename T >
T* Capacity< T >::GetVoltage()
{
    if ( !mVoltageSet )
        SetVoltage();

    if ( this->HasSystem() )
        this->mStateSystemGroup->mDglStateSystem.AddEquations( mUID, this->mCurrent / this->GetValue() );

    this->mVoltage = mVoltageValue;
    return TwoPort< T >::GetVoltage();
}


template < typename T >
void Capacity< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    mUID = stateSystemGroup->mDglStateSystem.GetNewEquation();
    TwoPort< T >::SetSystem( stateSystemGroup );
}
template < typename T >
const char* Capacity< T >::GetName() const
{
    return "Capacity";
}
} /* END NAMESPACE */
#endif /* _CAPACITY_ */
