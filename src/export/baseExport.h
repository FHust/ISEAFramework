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
* File Name : baseExport.h
* Creation Date : 14-01-2014
* Last Modified : Mo 06 Jun 2016 11:44:18 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _BASEEXPORT_
#define _BASEEXPORT_

#include <cstdlib>
#include <stdexcept>

#include "../electrical/twoport.h"

/// Baseclass for all exports
/// Exports take framework data and create another kind of data
/// e.g. create a spice model

template < typename matType >
class BaseExport
{
    public:
    BaseExport( const electrical::TwoPort< matType > *rootTwoport );
    virtual void WritePreamble() = 0;
    virtual void LoopOverAllTwoPorts( const electrical::TwoPort< matType > *twoPort ) = 0;
    virtual void WriteEnding() = 0;
    virtual ~BaseExport(){};

    private:
    protected:
    BaseExport() { abort(); };
    const electrical::TwoPort< matType > *mRootPort;
};

template < typename matType >
BaseExport< matType >::BaseExport( const electrical::TwoPort< matType > *rootTwoport )
    : mRootPort( rootTwoport )
{
    if ( !this->mRootPort )
    {
#ifdef __EXCEPTIONS__
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoRoot" );
#else
        abort();
#endif
    }
}

#endif /* _BASEEXPORT_ */
