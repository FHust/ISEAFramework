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
* File Name : const_obj.h
* Creation Date : 31-10-2012
* Last Modified : Mo 24 Nov 2014 18:23:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CONST_OBJ_
#define _CONST_OBJ_
#include "object.h"

namespace object
{
/// Simple value class, which always return the internal value it was initialized with
template < typename T = double >
class ConstObj : public Object< T >
{
    public:
    ConstObj( T value );
    ConstObj( T value, ConstObj< T >* CloneObj );

    ConstObj( const char* name, T value, ConstObj< T >* CloneObj );

    virtual ~ConstObj(){};

    virtual T GetValue() const;
    virtual T operator()() const;

    inline virtual T GetMaxValueOfLookup() { return this->mLastValue; };
    inline virtual T GetMinValueOfLookup() { return this->mLastValue; };

    virtual const char* GetName() const;
};

template < typename T >
ConstObj< T >::ConstObj( T value )
    : Object< T >()
{
    this->mLastValue = value;
}

template < typename T >
ConstObj< T >::ConstObj( T value, ConstObj< T >* CloneObj )
    : Object< T >()
{
    this->mLastValue = value * CloneObj->mLastValue;
}

template < typename T >
T ConstObj< T >::GetValue() const
{
    return this->mLastValue;
}

template < typename T >
T ConstObj< T >::operator()() const
{
    return this->mLastValue;
}

template < typename T >
const char* ConstObj< T >::GetName() const
{
    return "ConstObject";
}


} /* END NAMESPACE */
#endif /* _CONST_OBJ_ */
