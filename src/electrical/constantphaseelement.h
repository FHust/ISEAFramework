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
* File Name : cpe.h
* Creation Date : 22-04-2015
* Last Modified : Di 13 Okt 2015 17:51:29 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ConstantPhaseElement_
#define _ConstantPhaseElement_
#include "electricalelement.h"

namespace electrical
{
/// This class defines the basic behaviour of a ConstantPhaseElement and does not work! It is just for display
template < typename T = myMatrixType >
class ConstantPhaseElement : public ElectricalElement< T >
{
    public:
    ConstantPhaseElement( boost::shared_ptr< object::Object< double > > obj, const bool observable = false );
    ConstantPhaseElement( object::Object< double >* obj, const bool observable = false );
    virtual ~ConstantPhaseElement(){};
    virtual T* GetVoltage();    ///< Abort the simulation

    virtual const char* GetName() const;

    private:
    protected:
};

template < typename T >
ConstantPhaseElement< T >::ConstantPhaseElement( boost::shared_ptr< object::Object< double > > obj, const bool observable )
    : ElectricalElement< T >( obj, observable )
{
}

template < typename T >
ConstantPhaseElement< T >::ConstantPhaseElement( object::Object< double >* obj, const bool observable )
    : ElectricalElement< T >( obj, observable )
{
}

template < typename T >
T* ConstantPhaseElement< T >::GetVoltage()
{
    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoSimulationPossible", GetName() );
    return 0;
}

template < typename T >
const char* ConstantPhaseElement< T >::GetName() const
{
    return "ConstantPhaseElement";
}
}
#endif /* _ConstantPhaseElement_ */
