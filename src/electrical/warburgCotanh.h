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
* File Name : warburgCotanh.h
* Creation Date : 05-02-2014
* Last Modified : Fr 07 Feb 2014 17:57:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _WARBURGCOTA_
#define _WARBURGCOTA_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "electricalelement.h"

/// DESCRIPTION
namespace electrical
{
template < typename T = myMatrixType >
class WarburgCotanh : public SerialTwoPort< T >
{
    public:
    WarburgCotanh( const bool observable = false );
    ~WarburgCotanh(){};
    virtual const char* GetName() const;

    private:
    protected:
};

template < typename T >
WarburgCotanh< T >::WarburgCotanh( const bool observable )
    : SerialTwoPort< T >( observable )
{
}


template < typename T >
const char* WarburgCotanh< T >::GetName() const
{
    return "WarburgCotanh";
}
} /*electrical */
#endif /* _WARBURGCOTA_ */
