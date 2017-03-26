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
* File Name : warburgTanh.h
* Creation Date : 05-02-2014
* Last Modified : Fr 07 Feb 2014 14:32:40 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _WARBURGTA_
#define _WARBURGTA_
// STD
#include <vector>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "serialtwoport.h"


/// DESCRIPTION
namespace electrical
{

template < typename T = myMatrixType >
class WarburgTanh : public SerialTwoPort< T >
{
    public:
    WarburgTanh( const bool observable = false );
    ~WarburgTanh(){};
    virtual const char* GetName() const;

    private:
    protected:
};

template < typename T >
WarburgTanh< T >::WarburgTanh( const bool observable )
    : SerialTwoPort< T >( observable )
{
}

template < typename T >
const char* WarburgTanh< T >::GetName() const
{
    return "WarburgTanh";
}

} /* electrical */
#endif /* _WARBURGTA_ */
