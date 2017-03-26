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
* File Name : voltagesource.h
* Creation Date : 30-10-2012
* Last Modified : Mi 13 Aug 2014 18:07:19 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _VOLTAGESOURCE_
#define _VOLTAGESOURCE_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../object/object.h"
#include "electricalelement.h"

class TestSurfaceSoc;

namespace electrical
{

/// This TwoPort describes a VoltageSource e.g. OCV
template < typename T = myMatrixType >
class VoltageSource : public ElectricalElement< T >
{
    friend class ::TestSurfaceSoc;

    public:
    VoltageSource( boost::shared_ptr< object::Object< double > > obj, const bool observable = false );
    VoltageSource( object::Object< double >* obj, const bool observable = false );
    virtual ~VoltageSource(){};

    virtual T* GetVoltage();
    virtual void CalculateStateDependentValues();
    virtual const char* GetName() const;

    private:
    protected:
};

template < typename T >
VoltageSource< T >::VoltageSource( boost::shared_ptr< object::Object< double > > obj, const bool observable )
    : ElectricalElement< T >( obj, observable )
{
}

template < typename T >
VoltageSource< T >::VoltageSource( object::Object< double >* obj, const bool observable )
    : ElectricalElement< T >( obj, observable )
{
}

template < typename T >
void VoltageSource< T >::CalculateStateDependentValues()
{
    if ( this->mObservable )
        this->CalculateCurrentValue();
}


template < typename T >
T* VoltageSource< T >::GetVoltage()
{
    this->mVoltageValue = this->GetValue();
    this->mVoltage( 0, this->mCurrent.n_cols - 1 ) = this->mVoltageValue;
    return TwoPort< T >::GetVoltage();
}

template < typename T >
const char* VoltageSource< T >::GetName() const
{
    return "VoltageSource";
}

} /* END NAMESPACE */
#endif /* _VOLTAGESOURCE_ */
