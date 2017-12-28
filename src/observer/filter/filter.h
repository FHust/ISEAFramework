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
* File Name : filter.h
* Creation Date : 18-11-2012
* Last Modified : Di 13 Jun 2017 19:02:35 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _FILTER_
#define _FILTER_

// STD
#include <vector>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../../misc/matrixInclude.h"
#include "../../misc/StrCont.h"
#include "../../misc/macros.h"
#include "../../geometry/cartesian.h"

namespace electrical
{
template < class T >
class TwoPort;
}

namespace observer
{
template < typename T = myMatrixType >
struct PreparationType
{
    public:
    PreparationType( const size_t numberOfElements, electrical::TwoPort< T > *rootPort = 0 )
        : mNumberOfElements( numberOfElements )
        , mRootPort( rootPort ){};

    const size_t mNumberOfElements;
    electrical::TwoPort< T > *mRootPort;
};

struct ThermalPreperation
{
    ThermalPreperation( std::vector< std::vector< size_t > > &areas, std::vector< std::vector< size_t > > &volumes,
                        std::vector< misc::StrCont > &volumeNames, std::vector< geometry::Cartesian< double > > &vertices )
        : mAreas( areas )
        , mVolumes( volumes )
        , mVolumeNames( volumeNames )
        , mVertices( vertices ){};

    public:
    std::vector< std::vector< size_t > > &mAreas;
    std::vector< std::vector< size_t > > &mVolumes;
    std::vector< misc::StrCont > &mVolumeNames;
    std::vector< geometry::Cartesian< double > > &mVertices;
};


/// Base Class for filtering the results of the observer

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class Filter
{
    public:
    typedef typename std::vector< TConcrete< T > * > Data_t;

    Filter();
    virtual ~Filter(){};
    virtual void ProcessData( const typename Filter< T, TConcrete, ArgumentType >::Data_t &data, const double t );
    virtual void PrepareFilter( ArgumentType &prePareData ) { UNUSED( prePareData ); };

    void SetNext( Filter< T, TConcrete, ArgumentType > *newNext );

    private:
    protected:
    Filter< T, TConcrete, ArgumentType > *mNext;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
Filter< T, TConcrete, ArgumentType >::Filter()
    : mNext( 0 )
{
}


template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Filter< T, TConcrete, ArgumentType >::ProcessData( const typename Filter< T, TConcrete, ArgumentType >::Data_t &data, double t )
{
    if ( mNext )
        mNext->ProcessData( data, t );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Filter< T, TConcrete, ArgumentType >::SetNext( Filter< T, TConcrete, ArgumentType > *newNext )
{
    mNext = newNext;
}

} /* END NAMESPACE */
#endif /* _FILTER_ */
