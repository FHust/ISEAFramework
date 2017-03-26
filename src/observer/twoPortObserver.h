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
* File Name : twoPortObserver.h
* Creation Date : 31-08-2015
* Last Modified : Mi 02 Sep 2015 18:46:33 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TWOPORTOBSERVER_
#define _TWOPORTOBSERVER_
// STD
#include <vector>

// BOOST
#include <boost/shared_ptr.hpp>

#include "observer.h"

#include "../misc/matrixInclude.h"
#include "../electrical/twoport_withchild.h"


/// The observer namespace for handling the output
namespace observer
{
template < typename T = myMatrixType >
class TwoPortObserver : public Observer< T, electrical::TwoPort, PreparationType >
{

    public:
    typedef Filter< T, electrical::TwoPort, PreparationType > FilterT;

    TwoPortObserver( boost::shared_ptr< electrical::TwoPort< T > > rootPort );
    TwoPortObserver( std::vector< boost::shared_ptr< electrical::TwoPort< T > > >& observableTwoPorts );
    ~TwoPortObserver(){};
    virtual void operator()( double t );

    virtual void PrepareFilter( Filter< T, electrical::TwoPort, PreparationType >* filt );

    typename FilterT::Data_t& GetObservedTwoPortsPtr() { return mObservedTwoPortsPtr; };

    std::vector< boost::shared_ptr< electrical::TwoPort< T > > >& GetObservedTwoPorts() { return mObservableTwoPorts; };


    private:
    std::vector< boost::shared_ptr< electrical::TwoPort< T > > > mObservableTwoPorts;
    typedef typename std::vector< electrical::TwoPort< T >* > ProcessDataT;

    void RegisterObservableTwoPort( boost::shared_ptr< electrical::TwoPort< T > > tp );
    ProcessDataT mObservedTwoPortsPtr;


    protected:
};

template < typename T >
TwoPortObserver< T >::TwoPortObserver( boost::shared_ptr< electrical::TwoPort< T > > rootPort )
    : Observer< T, electrical::TwoPort, PreparationType >()
{
    RegisterObservableTwoPort( rootPort );
}

template < typename T >
TwoPortObserver< T >::TwoPortObserver( std::vector< boost::shared_ptr< electrical::TwoPort< T > > >& observableTwoPorts )
    : Observer< T, electrical::TwoPort, PreparationType >()
    , mObservableTwoPorts( observableTwoPorts )
{
    mObservedTwoPortsPtr.resize( mObservableTwoPorts.size() );
    for ( size_t i = 0; i < mObservableTwoPorts.size(); ++i )
    {
        mObservedTwoPortsPtr[i] = mObservableTwoPorts[i].get();
    }
}


template < typename T >
void TwoPortObserver< T >::PrepareFilter( Filter< T, electrical::TwoPort, PreparationType >* filt )
{
    PreparationType prepType( mObservableTwoPorts.size() );
    filt->PrepareFilter( prepType );
}

template < typename T >
void TwoPortObserver< T >::operator()( double t )
{
    if ( this->mBegin )
    {
        this->mBegin->ProcessData( mObservedTwoPortsPtr, t );
    }
}

template < typename T >
void TwoPortObserver< T >::RegisterObservableTwoPort( boost::shared_ptr< electrical::TwoPort< T > > tp )
{
    if ( tp->IsObservable() )
        mObservableTwoPorts.push_back( tp );

    if ( !tp->HasChildren() )
        return;

    boost::shared_ptr< electrical::TwoPortWithChild< T > > tpwc(
     boost::static_pointer_cast< electrical::TwoPortWithChild< T > >( tp ) );

    for ( size_t i = 0; i < tpwc->size(); ++i )
        RegisterObservableTwoPort( tpwc->shared_at( i ) );
}

} /* END NAMESPACE */
#endif /* _TWOPORTOBSERVER_ */
