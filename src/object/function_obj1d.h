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
* File Name : function_obj1d.h
* Creation Date : 14-05-2013
* Last Modified : Mo 24 Nov 2014 18:51:02 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _FUNCTION_OBJ1D_
#define _FUNCTION_OBJ1D_
#include "object.h"

#include "object.h"

namespace object
{
/// This Class allowes one to use function that take one arguement to be used by an object class
/// The invokation can be done through the operator(T) or GetValue(T)
template < typename T, T ( *FuncToCall )( T ) >
class FunctionObj : Object< T >
{
    public:
    FunctionObj();
    virtual ~FunctionObj() {}
    virtual T GetValue() const;
    virtual T operator()() const;
    virtual T GetValue( const T val );
    virtual T operator()( const T val );

    virtual const char* GetName() const;
}; /* FunctionObj */

template < typename T, T ( *FuncToCall )( T ) >
FunctionObj< T, FuncToCall >::FunctionObj()
    : Object< T >()
{
}

template < typename T, T ( *FuncToCall )( T ) >
T FunctionObj< T, FuncToCall >::operator()() const
{
    return this->mLastValue;
}

template < typename T, T ( *FuncToCall )( T ) >
T FunctionObj< T, FuncToCall >::GetValue() const
{
    return this->mLastValue;
}

template < typename T, T ( *FuncToCall )( T ) >
T FunctionObj< T, FuncToCall >::operator()( const T val )
{
    return FunctionObj< T, FuncToCall >::GetValue( val );
};

template < typename T, T ( *FuncToCall )( T ) >
T FunctionObj< T, FuncToCall >::GetValue( const T val )
{
    this->mLastValue = FuncToCall( val );
    return FunctionObj< T, FuncToCall >::GetValue();
}

template < typename T, T ( *FuncToCall )( T ) >
const char* FunctionObj< T, FuncToCall >::GetName() const
{
    return "FunctionObj";
}


} /* object */
#endif /* _FUNCTION_OBJ1D_ */
