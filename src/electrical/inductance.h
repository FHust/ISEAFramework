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
* File Name : inductance.h
* Creation Date : 22-04-2015
* Last Modified : Di 13 Okt 2015 17:51:04 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _INDUCTANCE_
#define _INDUCTANCE_
#include "electricalelement.h"

namespace electrical
{
/// This class defines the basic behaviour of an Inductance  and does not work! It is just for display
template < typename T = myMatrixType >
class Inductance : public ElectricalElement< T >
{
    public:
    Inductance( boost::shared_ptr< object::Object< double > > obj, const bool observable = false );
    Inductance( object::Object< double >* obj, const bool observable = false );
    virtual ~Inductance(){};
    virtual T* GetVoltage();    ///< Abort the simulation

    virtual const char* GetName() const;

    private:
    protected:
};

template < typename T >
Inductance< T >::Inductance( boost::shared_ptr< object::Object< double > > obj, const bool observable )
    : ElectricalElement< T >( obj, observable )
{
}

template < typename T >
Inductance< T >::Inductance( object::Object< double >* obj, const bool observable )
    : ElectricalElement< T >( obj, observable )
{
}

template < typename T >
T* Inductance< T >::GetVoltage()
{
    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoSimulationPossible", GetName() );
    return 0;
}

template < typename T >
const char* Inductance< T >::GetName() const
{
    return "Inductance";
}
}
#endif /* _INDUCTANCE_ */
