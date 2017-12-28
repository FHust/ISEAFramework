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
* File Name : observer.h
* Creation Date : 18-11-2012
* Last Modified : Mi 02 Sep 2015 18:31:13 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _OBSERVER_
#define _OBSERVER_

// STD
#include <vector>

// BOOST
#include <boost/shared_ptr.hpp>

#include "filter/filter.h"
#include "../misc/macros.h"

/// The observer namespace for handling the output
namespace observer
{

/// This is the Generic observer class and should be specialized for further use.
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class Observer
{
    public:
    Observer();
    virtual ~Observer(){};
    virtual void operator()( double t );
    void AddFilter( Filter< T, TConcrete, ArgumentType >* filt );
    void AddFilter( boost::shared_ptr< Filter< T, TConcrete, ArgumentType > > filt );

    virtual void PrepareFilter( Filter< T, TConcrete, ArgumentType >* filt ) = 0;
    std::vector< boost::shared_ptr< Filter< T, TConcrete, ArgumentType > > >& GetFilterChain() { return mFilterChain; };

    private:
    std::vector< boost::shared_ptr< Filter< T, TConcrete, ArgumentType > > > mFilterChain;


    protected:
    Filter< T, TConcrete, ArgumentType >* mBegin;
    Filter< T, TConcrete, ArgumentType >* mEnd;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
Observer< T, TConcrete, ArgumentType >::Observer()
    : mBegin( 0 )
    , mEnd( 0 )
{
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Observer< T, TConcrete, ArgumentType >::AddFilter( boost::shared_ptr< Filter< T, TConcrete, ArgumentType > > filt )
{
    if ( !mBegin )
    {
        mBegin = filt.get();
        mEnd = filt.get();
    }
    else
    {
        mEnd->SetNext( filt.get() );
        mEnd = filt.get();
    }
    mFilterChain.push_back( filt );
    PrepareFilter( filt.get() );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Observer< T, TConcrete, ArgumentType >::AddFilter( Filter< T, TConcrete, ArgumentType >* filt )
{
    this->AddFilter( boost::shared_ptr< Filter< T, TConcrete, ArgumentType > >( filt ) );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Observer< T, TConcrete, ArgumentType >::operator()( double t )
{
    UNUSED( t );
}

} /* END NAMESPACE */
#endif /* _OBSERVER_ */
