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
 * objectclasswrapper.h
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#ifndef OBJECTCLASSWRAPPER_H_
#define OBJECTCLASSWRAPPER_H_

// BOOST
#include <boost/static_assert.hpp>

// STD
#include <stdexcept>
#include <cstring>

// ETC
#include "../factory.h"

#include "../classwrapper.h"

#include "../../states/soc.h"
#include "../../states/thermal_state.h"

#include "../../object/const_obj.h"
#include "../../object/lookup_obj1d.h"
#include "../../object/lookup_obj2d.h"
#include "../../object/lookup_obj1d_with_state.h"
#include "../../object/lookup_obj2d_with_state.h"
#include "../../object/function_obj1d.h"

#include "../../operators/vectorOperator.h"
#include "../../exceptions/error_proto.h"

#include "../../misc/cmp_str.h"

#include "../state/stateclasswrapper.h"

using namespace object;

namespace factory
{

template < typename ValueT >
struct ArgumentTypeObject
{
    ArgumentTypeObject()
        : mObjectFactor( 1.0 ){};
    std::map< misc::StrCont, double, misc::cmp_str > mDoubleMap;
    ValueT mObjectFactor;
};

// DIRTY DIRTY HACK!
const double UNDEFINED_BEHAVIOR = -1;
const double SCALA_MULUTPLICATION = 0;
const double OBJECT_MULTIPLICATION = 1;

/// Classwrapper base class for being used in a generic ::object::Object Factory. This class has to be derived.
template < typename ValueT >
class ObjectClassWrapperBase : public ClassWrapperBase< Object< ValueT >, ArgumentTypeObject< ValueT > >
{
    // This is needed for HACK_1
    BOOST_STATIC_ASSERT( sizeof( double ) >=
                         sizeof( double* ) );    // This only works if pointers are smaller than doubles
    public:
    /**
     * Classwrapper constructor.
     * The factory is supplied via parameter and is used for other instances ctor parameters.
     * @param stateFactory A Factory for class in the ::state namespace
     */
    ObjectClassWrapperBase( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : mStateFactory( stateFactory )
    {
    }

    protected:
    /// Get a ::state::Dgl_state Factory.
    Factory< ::state::Dgl_state, ArgumentTypeState >* GetStateFactory() { return mStateFactory; }

    void ConvertDouble2ObjectPointer( double address, Object< ValueT >* obj )
    {
        memcpy( static_cast< void* >( &obj ), static_cast< void* >( &address ), sizeof( double* ) );
    }

    double GetOperand( const ArgumentTypeObject< ValueT >* arg ) const
    {
        const char* mapEntry = "Operand";
        DoubleMapConstIterator it = arg->mDoubleMap.find( mapEntry );
        if ( it == arg->mDoubleMap.end() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "MultiplierNotFound" );

        return it->second;
    }

    double GetOperator( const ArgumentTypeObject< ValueT >* arg ) const
    {
        const char* mapEntry = "Operator";

        DoubleMapConstIterator it = arg->mDoubleMap.find( mapEntry );
        if ( it == arg->mDoubleMap.end() )
            return UNDEFINED_BEHAVIOR;

        return it->second;
    }

    double GetMultiplier( const ArgumentTypeObject< ValueT >* arg ) const
    {
        const char* mapEntry = "Multiplier";
        DoubleMapConstIterator it = arg->mDoubleMap.find( mapEntry );
        if ( it == arg->mDoubleMap.end() )
        {
            return 1;
        }
        return it->second;
    }

    void CheckStateIsCacherefed( const xmlparser::XmlParameter* param ) const
    {
        if ( !param->HasElementAttribute( "cacheref" ) )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "CacherefAttribute",
                                                 param->GetElementName(), param->GetLineNumber() );
        }
    }

    /** Get the LookupType from xml file, e.g. LinearInterpolation
    * @param    param Pointer of type xmlparser::XmlParameter. This is the desired LookupType in the xml-file.
    * @return   Integer representing a LookupType: 0 => "LinearInterpolation", 1 => NearestNeighbour", 2 =>
    * ConstantUntilThreshold, 3 => SplineInterpolation
    */
    int GetLookupType( const xmlparser::XmlParameter* param )
    {
        if ( !param )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidXML" );
        }
        else
        {
            // Get the user defined LookupType
            if ( param->HasElementAttribute( "LookupType" ) )
            {
                const char* myLookupTypeString = param->GetElementAttribute( "LookupType" );
                misc::equal_str myCompareObject;
                if ( myCompareObject( myLookupTypeString, "LinearInterpolation" ) )
                    return 0;
                else if ( myCompareObject( myLookupTypeString, "NearestNeighbour" ) )
                    return 1;
                else if ( myCompareObject( myLookupTypeString, "ConstantUntilThreshold" ) )
                    return 2;
                else if ( myCompareObject( myLookupTypeString, "SplineInterpolation" ) )
                    return 3;
                else
                {
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownLookupType" );
                }
            }
        }
        return 0;    // Set LookupType to default 0 = LinearInterpolation
    }

    private:
    Factory< ::state::Dgl_state, ArgumentTypeState >* const mStateFactory;
};

/// Classwrapper for ::object namespace. This template class has to be specialized in order to create an instance of a
/// particular class.
template < typename ValueT, template < typename > class TConcrete >
class ObjectClassWrapper : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory )
    {
    }
};

/// Classwrapper for object::ConstObj
template < typename ValueT >
class ObjectClassWrapper< ValueT, ConstObj > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        ValueT value = param->GetElementDoubleValue( "Value" );

        if ( arg != 0 )
        {
            value *= this->GetMultiplier( arg );

            if ( arg->mObjectFactor != 1.0 )
                value *= arg->mObjectFactor;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {
                Object< ValueT >* cObj = NULL;

                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::shared_ptr< Object< ValueT > >(
                 new ConstObj< ValueT >( value, static_cast< ConstObj< ValueT >* >( cObj ) ) );
            }
        }

        return boost::shared_ptr< Object< ValueT > >( new ConstObj< ValueT >( value ) );
    }
};

/// Classwrapper for object::LookupObj1D
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj1D > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        std::vector< ValueT > lookupData = param->GetElementDoubleVecValue( "LookupData" );
        std::vector< ValueT > measurementPoints = param->GetElementDoubleVecValue( "MeasurementPoints" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {

                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::shared_ptr< Object< ValueT > >(
                 new LookupObj1D< ValueT >( lookupData, measurementPoints, static_cast< LookupObj1D< ValueT >* >( cObj ),
                                            lookup::LookupType( this->GetLookupType( param ) ) ) );
            }
        }
        return boost::shared_ptr< Object< ValueT > >(
         new LookupObj1D< ValueT >( lookupData, measurementPoints, lookup::LookupType( this->GetLookupType( param ) ) ) );
    }
};

/// Classwrapper for object::LookupObj1dWithState
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj1dWithState > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        ObjectClassWrapperBase< ValueT >::CheckStateIsCacherefed( param->GetElementChild( "State" ).get() );
        boost::shared_ptr< ::state::Dgl_state > state(
         this->GetStateFactory()->CreateInstance( param->GetElementChild( "State" ) ) );
        std::vector< ValueT > lookupData = param->GetElementDoubleVecValue( "LookupData" );
        std::vector< ValueT > measurementPoints = param->GetElementDoubleVecValue( "MeasurementPoints" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {
                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::shared_ptr< Object< ValueT > >(
                 new LookupObj1dWithState< ValueT >( lookupData, measurementPoints, state,
                                                     static_cast< LookupObj1dWithState< ValueT >* >( cObj ),
                                                     lookup::LookupType( this->GetLookupType( param ) ) ) );
            }
        }
        return boost::shared_ptr< Object< ValueT > >(
         new LookupObj1dWithState< ValueT >( lookupData, measurementPoints, state,
                                             lookup::LookupType( this->GetLookupType( param ) ) ) );
    }
};

/// Classwrapper for object::LookupObj2D
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj2D > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        std::vector< std::vector< ValueT > > lookupData = param->GetElementDoubleVecVecValue( "LookupData" );
        std::vector< ValueT > measurementPointsRow = param->GetElementDoubleVecValue( "MeasurementPointsRow" );
        std::vector< ValueT > measurementPointsColumn = param->GetElementDoubleVecValue( "MeasurementPointsColumn" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {
                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::shared_ptr< Object< ValueT > >(
                 new LookupObj2D< ValueT >( lookupData, measurementPointsRow, measurementPointsColumn,
                                            static_cast< LookupObj2D< ValueT >* >( cObj ),
                                            lookup::LookupType( this->GetLookupType( param ) ) ) );
            }
        }

        return boost::shared_ptr< Object< ValueT > >(
         new LookupObj2D< ValueT >( lookupData, measurementPointsRow, measurementPointsColumn,
                                    lookup::LookupType( this->GetLookupType( param ) ) ) );
    }
};

/// Classwrapper for object::LookupObj2dWithState
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj2dWithState > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        ObjectClassWrapperBase< ValueT >::CheckStateIsCacherefed( param->GetElementChild( "RowState" ).get() );
        ObjectClassWrapperBase< ValueT >::CheckStateIsCacherefed( param->GetElementChild( "ColState" ).get() );
        boost::shared_ptr< ::state::Dgl_state > rowstate(
         this->GetStateFactory()->CreateInstance( param->GetElementChild( "RowState" ) ) );
        boost::shared_ptr< ::state::Dgl_state > colstate(
         this->GetStateFactory()->CreateInstance( param->GetElementChild( "ColState" ) ) );
        std::vector< std::vector< ValueT > > lookupData = param->GetElementDoubleVecVecValue( "LookupData" );
        std::vector< ValueT > measurementPointsRow = param->GetElementDoubleVecValue( "MeasurementPointsRow" );
        std::vector< ValueT > measurementPointsColumn = param->GetElementDoubleVecValue( "MeasurementPointsColumn" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {

                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );

                return boost::shared_ptr< Object< ValueT > >(
                 new LookupObj2dWithState< ValueT >( lookupData, measurementPointsRow, measurementPointsColumn, rowstate,
                                                     colstate, static_cast< LookupObj2dWithState< ValueT >* >( cObj ),
                                                     lookup::LookupType( this->GetLookupType( param ) ) ) );
            }
        }

        return boost::shared_ptr< Object< ValueT > >(
         new LookupObj2dWithState< ValueT >( lookupData, measurementPointsRow, measurementPointsColumn, rowstate,
                                             colstate, lookup::LookupType( this->GetLookupType( param ) ) ) );
    }
};

} /* namespace factory */
#endif /* OBJECTCLASSWRAPPER_H_ */
