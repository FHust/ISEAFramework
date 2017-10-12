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
* File Name : electricalelement.h
* Creation Date : 30-10-2012
* Last Modified : Do 10 Mär 2016 10:40:24 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ELECTRICALELEMENT_
#define _ELECTRICALELEMENT_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "twoport.h"
#include "../object/object.h"

// Friend
class TestElectricalFactory;

namespace electrical
{
/// This class defines the basic behaviour of Electrical element, e.g. having an Object that defines the the behaviour
/// (const, lookup). This is saved in a shared_ptr mObject
template < typename T = myMatrixType >
class ElectricalElement : public TwoPort< T >
{
    friend class ::TestElectricalFactory;


    public:
    explicit ElectricalElement(
     boost::shared_ptr< object::Object< double > > obj, const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));
    explicit ElectricalElement(
     object::Object< double >* obj, const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));
    virtual ~ElectricalElement(){};

    virtual ScalarUnit GetValue() const;    ///< Get Value of Object

    object::Object< double >* operator()();               ///< Get Object
    const object::Object< double >* GetObject() const;    ///< Get Object if instance is const
    virtual const char* GetName() const;

    private:
    protected:
    boost::shared_ptr< object::Object< double > > mObject;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
};

template < typename T >
ElectricalElement< T >::ElectricalElement( boost::shared_ptr< object::Object< double > > obj, const bool observable,
                                           typename TwoPort< T >::DataType dataValues )
    : TwoPort< T >( observable, dataValues )
    , mObject( obj )
{
}

template < typename T >
ElectricalElement< T >::ElectricalElement( object::Object< double >* obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
ScalarUnit ElectricalElement< T >::GetValue() const
{
#ifndef _SYMBOLIC_
    return ( *mObject )();
#else
    return symbolic::Symbolic( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjBase" );
#endif
}
template < typename T >
object::Object< double >* ElectricalElement< T >::operator()()
{
    return mObject.get();
}

template < typename T >
const object::Object< double >* ElectricalElement< T >::GetObject() const
{
    return mObject.get();
}

template < typename T >
const char* ElectricalElement< T >::GetName() const
{
    return "ElectricalElement";
}
} /* END NAMESPACE */
#endif /* _ELECTRICALELEMENT_ */
