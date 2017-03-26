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
* File Name : valueStateWrapper.h
* Creation Date : 19-02-2014
* Last Modified : Mo 08 Sep 2014 15:27:43 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _VALUESTATEWRAPPER_
#define _VALUESTATEWRAPPER_

#include <limits>
#include "dgl_state.h"

namespace state
{
// typedef double ValueT;
/// This states wrapps an arbitrary pointer within and evalutes it

template < typename ValueT >
class ValueStateWrapper : public Dgl_state
{
    public:
    ValueStateWrapper( ValueT *value );
    virtual ~ValueStateWrapper(){};

    virtual double GetMaxDt() const
    {
        return ::std::numeric_limits< double >::max();
    };    // returns the Maximum dt step that can be done before any major change in the state/lookup occures

    virtual double GetValue() const { return *static_cast< double * >( mValue ); };

    virtual double operator()() const { return *static_cast< double * >( mValue ); };

    private:
    ValueStateWrapper();
    ValueT *mValue;

    protected:
};

template < typename ValueT >
ValueStateWrapper< ValueT >::ValueStateWrapper( ValueT *value )
    : Dgl_state()
    , mValue( value )
{
}

} /*state*/
#endif /* _VALUESTATEWRAPPER_ */
