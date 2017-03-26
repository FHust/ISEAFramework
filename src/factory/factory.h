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
/*
 * factory.h
 *
 *  Created on: 14.09.2013
 *      Author: chris
 */

#ifndef FACTORY_H_
#define FACTORY_H_

// TODO: This should be deleted when the RT-Board port of the STL correctly contains std::runtime
#ifdef __EXCEPTIONS__
#include <stdexcept>
#include <string>
#endif

// STD
#include <cstring>
#include <map>
#include <stdexcept>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../misc/cmp_str.h"
#include "../xmlparser/xmlparameter.h"
#include "../misc/StrCont.h"
#include "classwrapper.h"


#ifdef __EXCEPTIONS__
#include "exceptions/classnotcachedexception.h"
#include "exceptions/classnotfoundexception.h"
#endif


#include "../exceptions/error_proto.h"
/// The ::factory namespace contains a complete non-intrusive factory for ::electrical, ::object, ::state and ::thermal
/// and is further expandable
namespace factory
{

/// The Factory class can create instances from names. It also can cache instances for sharing until they may be
/// overwritten.
template < typename TBase, typename Argument = ArgumentType >
class Factory
{

    public:
    /// Constructor. Creates an empty factory.
    Factory() {}

    virtual ~Factory() {}

    /// Adds a new classwrapper to the Factory. This will register a new class. If we can't add wrapper, it is going to
    /// be consumed.
    void AddWrapper( ClassWrapperBase< TBase, Argument >* wrapper, const char* name );

    /// Creates a class instance by xmlparser::XMLParameter. Throws ClassNotFoundException.
    boost::shared_ptr< TBase > CreateInstance( const xmlparser::XmlParameter* param, const Argument* arg = 0 );

    /// Creates a class instance by xmlparser::XMLParameter.
    boost::shared_ptr< TBase > CreateInstance( boost::shared_ptr< xmlparser::XmlParameter > param, const Argument* arg = 0 );

    /// Returns all objects that have been created.
    void GetObjects( std::vector< boost::shared_ptr< TBase > >& objects ) const;

    /// Returns all objects of class classname that have been created.
    std::vector< boost::shared_ptr< TBase > >& GetObjectsOfClass( const char* classname );

    private:
    /// Caches an instance via boost::shared_ptr in a std::map.
    void CacheInstance( const char* name, boost::shared_ptr< TBase > obj );

    /// Retrieves the cached instance by name. Throws ClassNotFoundException.
    boost::shared_ptr< TBase > GetCachedInstance( const char* name );

    std::map< misc::StrCont, boost::shared_ptr< ClassWrapperBase< TBase, Argument > >, misc::cmp_str > mWrapperMap;
    std::map< misc::StrCont, boost::shared_ptr< TBase >, misc::cmp_str > mCacheMap;
    std::map< misc::StrCont, std::vector< boost::shared_ptr< TBase > >, misc::cmp_str > mObjectVectorMap;
};

template < typename TBase, typename Argument >
void Factory< TBase, Argument >::AddWrapper( ClassWrapperBase< TBase, Argument >* wrapper, const char* name )
{
    auto shrd_ptr = boost::shared_ptr< ClassWrapperBase< TBase, Argument > >( wrapper );
    if ( mWrapperMap.count( name ) )
        return;
    mWrapperMap.insert( std::pair< misc::StrCont, boost::shared_ptr< ClassWrapperBase< TBase, Argument > > >(
     name, boost::shared_ptr< ClassWrapperBase< TBase, Argument > >( shrd_ptr ) ) );
    mObjectVectorMap.insert(
     std::pair< misc::StrCont, std::vector< boost::shared_ptr< TBase > > >( name, std::vector< boost::shared_ptr< TBase > >() ) );
}

template < typename TBase, typename Argument >
boost::shared_ptr< TBase > Factory< TBase, Argument >::CreateInstance( const xmlparser::XmlParameter* param, const Argument* arg )
{

    if ( !param )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidXML" );

    if ( param->HasElementAttribute( "cacheref" ) )
        return GetCachedInstance( param->GetElementAttribute( "cacheref" ) );


    const char* classname = param->GetElementAttribute( "class" );
    typename std::map< misc::StrCont, boost::shared_ptr< ClassWrapperBase< TBase, Argument > >, misc::cmp_str >::iterator wrapper =
     mWrapperMap.find( classname );
    if ( wrapper == mWrapperMap.end() )
        ErrorFunction< ClassNotFoundException >( __FUNCTION__, __LINE__, __FILE__, "RequestedClassNotFound", classname );
    boost::shared_ptr< TBase > obj( wrapper->second->CreateInstance( param, arg ) );

    if ( param->GetElementAttributeBoolValue( "cache", false ) )
        CacheInstance( param->GetElementName(), obj );

    mObjectVectorMap.find( classname )->second.push_back( obj );

    return obj;
}

template < typename TBase, typename Argument >
boost::shared_ptr< TBase >
Factory< TBase, Argument >::CreateInstance( boost::shared_ptr< xmlparser::XmlParameter > param, const Argument* arg )
{
    return CreateInstance( param.get(), arg );
}

template < typename TBase, typename Argument >
void Factory< TBase, Argument >::CacheInstance( const char* name, boost::shared_ptr< TBase > obj )
{
    typename std::map< misc::StrCont, boost::shared_ptr< TBase >, misc::cmp_str >::iterator cache = mCacheMap.find( name );
    if ( cache == mCacheMap.end() )
        mCacheMap.insert( std::pair< misc::StrCont, boost::shared_ptr< TBase > >( name, obj ) );
    else
        cache->second = obj;
}

template < typename TBase, typename Argument >
void Factory< TBase, Argument >::GetObjects( std::vector< boost::shared_ptr< TBase > >& objects ) const
{
    typedef typename std::map< misc::StrCont, std::vector< boost::shared_ptr< TBase > >, misc::cmp_str >::const_iterator vectorMapIterator;

    size_t numberOfObjects = 0;
    for ( vectorMapIterator it = mObjectVectorMap.begin(); it != mObjectVectorMap.end(); ++it )
        numberOfObjects += it->second.size();

    objects.clear();
    objects.reserve( numberOfObjects );

    for ( vectorMapIterator it = mObjectVectorMap.begin(); it != mObjectVectorMap.end(); ++it )
        objects.insert( objects.end(), it->second.begin(), it->second.end() );
}

template < typename TBase, typename Argument >
std::vector< boost::shared_ptr< TBase > >& Factory< TBase, Argument >::GetObjectsOfClass( const char* classname )
{
    if ( !mObjectVectorMap.count( classname ) )
        ErrorFunction< ClassNotFoundException >( __FUNCTION__, __LINE__, __FILE__, "RequestedClassNotFound", classname );

    return mObjectVectorMap.find( classname )->second;
}

template < typename TBase, typename Argument >
boost::shared_ptr< TBase > Factory< TBase, Argument >::GetCachedInstance( const char* name )
{
    typename std::map< misc::StrCont, boost::shared_ptr< TBase >, misc::cmp_str >::iterator cache = mCacheMap.find( name );
    if ( cache == mCacheMap.end() )
        ErrorFunction< ClassNotCachedException >( __FUNCTION__, __LINE__, __FILE__, "NotCachedClass", name );

    return cache->second;
}
}

#endif /* FACTORY_H_ */
