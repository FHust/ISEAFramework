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
* File Name : serialtwoport.h
* Creation Date : 30-10-2012
* Last Modified : Mo 15 Jun 2015 17:28:04 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SERIALTWOPORT_
#define _SERIALTWOPORT_

// ETC
#include "twoport_withchild.h"

class TestTwoPortsWithChildren;
class TestDaeSystem;

namespace electrical
{
/// Serial Connector for all Elements. Can have any TwoPort as a child
template < typename T = myMatrixType >
class SerialTwoPort : public TwoPortWithChild< T >
{
    friend class ::TestTwoPortsWithChildren;
    friend class ::TestDaeSystem;

    public:
    SerialTwoPort( const bool observable = false );
    virtual ~SerialTwoPort(){};

    virtual T* GetVoltage();

    virtual bool IsSerialTwoPort() const;
    virtual const char* GetName() const;

    private:
    protected:
};

template < typename T >
SerialTwoPort< T >::SerialTwoPort( const bool observable )
    : TwoPortWithChild< T >( observable )
{
}

template < typename T >
T* SerialTwoPort< T >::GetVoltage()
{
    this->mVoltage = *( this->mChildren.at( 0 )->GetVoltage() );
    for ( size_t i = 1; i < this->mChildren.size(); ++i )
        this->mVoltage += *( this->mChildren[i]->GetVoltage() );
    return TwoPort< T >::GetVoltage();
}

template < typename T >
bool SerialTwoPort< T >::IsSerialTwoPort() const
{
    return true;
}

template < typename T >
const char* SerialTwoPort< T >::GetName() const
{
    return "SerialTwoPort";
}
} /* END NAMESPACE */
#endif /* _SERIALTWOPORT_ */
