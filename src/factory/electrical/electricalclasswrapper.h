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
 * electricalclasswrapper.h
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

// STD
#include <cmath>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../factory.h"
#include "../get_electrical_discretization.h"
#include "../check_positive_int.h"
#include "../classwrapper.h"

#include "../../states/soc.h"
#include "../../states/thermal_state.h"

#include "../../electrical/capacity.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/constantphaseelement.h"
#include "../../electrical/inductance.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/parallelRCAlg.h"
#include "../../electrical/paralleltwoport.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/warburgTanh.h"
#include "../../electrical/warburgCotanh.h"
#include "../../electrical/zarc.h"
#include "../../electrical/zarcalg.h"

#include "../object/objectclasswrapper.h"
#include "../state/stateclasswrapper.h"

#ifdef __EXCEPTIONS__
#include "../exceptions/nochildrenexception.h"
#endif


#include "../../exceptions/error_proto.h"

using namespace electrical;

namespace factory
{

struct ArgumentTypeElectrical
{
    ArgumentTypeElectrical()
        : mCellDiscretization( 1 ){};
    size_t mCellDiscretization;
};

/// Classwrapper base class for being used in a generic ::electrical::TwoPort Factory. This class has to be derived.
template < typename MatrixT >
class ElectricalClassWrapperBase : public ClassWrapperBase< TwoPort< MatrixT >, ArgumentTypeElectrical >
{
    public:
    /**
     * Classwrapper constructor.
     * The three different factories are supplied via parameter and are used for other instances ctor parameters.
     * @param electricalFactory A Factory for class in the ::electrical namespace
     * @param objectFactory A Factory for class in the ::object namespace
     * @param stateFactory A Factory for class in the ::state namespace
     */
    ElectricalClassWrapperBase( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                                Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                                Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : mElectricalFactory( electricalFactory )
        , mObjectFactory( objectFactory )
        , mStateFactory( stateFactory )
    {
    }

    protected:
    /// Get an ::electrical::TwoPort Factory.
    Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *GetElectricalFactory()
    {
        return mElectricalFactory;
    }

    /// Get an ::object::Object Factory.
    Factory< object::Object< double >, ArgumentTypeObject< double > > *GetObjectFactory() { return mObjectFactory; }

    /// Get a ::state::Dgl_state Factory.
    Factory< ::state::Dgl_state, ArgumentTypeState > *GetStateFactory() { return mStateFactory; }

    /// Returns ArgumentType for adjusting ::object::Object creation if cell consist of more than one cell element
    ArgumentTypeObject< double > *SetObjectFactorToArg( const ArgumentTypeElectrical *argElectrical, bool multiply,
                                                        boost::scoped_ptr< ArgumentTypeObject< double > > &argObject )
    {
        if ( argElectrical )
            if ( argElectrical->mCellDiscretization != 1 )
            {
                if ( !argObject )
                    argObject.reset( new ArgumentTypeObject< double > );

                argObject->mObjectFactor = argElectrical->mCellDiscretization;
                if ( !multiply )
                    argObject->mObjectFactor = 1.0 / argObject->mObjectFactor;
            }

        return argObject.get();
    }

    bool SimplifyRC( const xmlparser::XmlParameter *param, double tauLookup )
    {
        return tauLookup < GetTauForSimplification( param );
    }

    private:
    Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *const mElectricalFactory;
    Factory< object::Object< double >, ArgumentTypeObject< double > > *const mObjectFactory;
    Factory< ::state::Dgl_state, ArgumentTypeState > *const mStateFactory;

    double GetTauForSimplification( const xmlparser::XmlParameter *param )
    {
        double minDt = 1.0e-6;    // Default Initializer if Samplerate is not given in XML file
        if ( param->GetConfigurationRoot()->HasElement( "Options" ) &&
             param->GetConfigurationRoot()->GetElementChild( "Options" )->HasElement( "SampleRate" ) )
        {
            double samp =
             param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementDoubleValue( "SampleRate", 1000 );
            if ( samp > 1.0e-8 )    // Check if greater 0
                minDt = 1.0 / samp;
            else
                ErrorFunction< std::range_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );

            if ( param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementChild( "SampleRate" )->HasElementAttribute(
                  "SimplificationType" ) )
            {
                const char *mySimplificationType =
                 param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementChild( "SampleRate" )->GetElementAttribute(
                  "SimplificationType" );
                misc::equal_str myCompareObject;
                if ( myCompareObject( mySimplificationType, "Euler" ) )
                {
                    return 0.50001 * minDt;    // Exakt Stability limit at 0.5*minDt, safe side given with 0.50001
                }
            }
        }
        return minDt / 3.0;
    }
};

/// Classwrapper for electrical namespace. This template class has to be specialized in order to create an instance of a
/// particular class.
template < typename MatrixT, template < typename > class TConcrete >
class ElectricalClassWrapper : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }
};

/// Classwrapper for electrical::Capacity
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Capacity > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject;
        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, false, argObject ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new Capacity< MatrixT >( obj, observable ) );
    }
};

/// Classwrapper for electrical::OhmicResistance
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, OhmicResistance > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject;
        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, true, argObject ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new OhmicResistance< MatrixT >( obj, observable ) );
    }
};

/// Classwrapper for electrical::ConstantPhaseElement dummy
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ConstantPhaseElement > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject;
        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, true, argObject ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new ConstantPhaseElement< MatrixT >( obj, observable ) );
    }
};

/// Classwrapper for electrical::Inductance dummy
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Inductance > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject;
        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, true, argObject ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new Inductance< MatrixT >( obj, observable ) );
    }
};


/// Classwrapper for electrical::VoltageSource
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, VoltageSource > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical * /* arg */ = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::shared_ptr< object::Object< double > > obj( this->GetObjectFactory()->CreateInstance( objparam ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new VoltageSource< MatrixT >( obj, observable ) );
    }
};

/// Classwrapper for electrical::ParallelRC
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ParallelRC > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );

        boost::shared_ptr< xmlparser::XmlParameter > robjparam( param->GetElementChild( "LookupOhmicResistance" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject;
        boost::shared_ptr< object::Object< double > > rObj(
         this->GetObjectFactory()->CreateInstance( robjparam, this->SetObjectFactorToArg( arg, true, argObject ) ) );

        boost::shared_ptr< xmlparser::XmlParameter > cobjparam( param->GetElementChild( "LookupTau" ) );
        boost::shared_ptr< object::Object< double > > cObj( this->GetObjectFactory()->CreateInstance( cobjparam ) );

        double maxRelaxationTime = cObj.get()->GetMaxValueOfLookup();

        if ( ElectricalClassWrapperBase< MatrixT >::SimplifyRC( param, maxRelaxationTime ) )
        {
            return boost::shared_ptr< TwoPort< MatrixT > >( new OhmicResistance< MatrixT >( rObj, observable ) );
        }
        return boost::shared_ptr< TwoPort< MatrixT > >( new ParallelRC< MatrixT >( rObj, cObj, observable ) );
    }
};

/// Classwrapper for electrical::WarburgCotanh
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, WarburgCotanh > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
        // Nedded for HACK_1
        assert( sizeof( double ) >= sizeof( double * ) );    // This only works if pointers are smaller than doubles
    }

    double CalculateCoefficent( size_t elementNumber ) const
    {
        return 4.0 / std::pow( ( elementNumber * 3.141592653589793 ), 2.0 );
    }

    bool SaneInput( int maxElements )
    {
        if ( maxElements <= 0 )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "negativeValue", maxElements );
        }
        return true;
    }


    void AddResidualOhmicResistance( boost::shared_ptr< SerialTwoPort< MatrixT > > tp,
                                     boost::shared_ptr< xmlparser::XmlParameter > cObjParam,
                                     ArgumentTypeObject< double > *arg, size_t start, size_t end, bool observable )
    {
        double residualOhmicResistance = 0;

        for ( ; start < end; ++start )
            residualOhmicResistance += CalculateCoefficent( start + 1 );

        // ArgumentType::iterator it = arg->find("Multiplier");
        arg->mDoubleMap["Multiplier"] = residualOhmicResistance;

        // it->second = residualOhmicResistance;
        boost::shared_ptr< object::Object< double > > rObj( this->GetObjectFactory()->CreateInstance( cObjParam, arg ) );    // R
        tp->AddChild( new OhmicResistance< MatrixT >( rObj, observable ) );
    }

    double ConvertObjectPointer2double( object::Object< double > *objP )
    {
        double address = 0;
        memcpy( static_cast< void * >( &address ), static_cast< void * >( &objP ), sizeof( double * ) );
        return address;
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );
        const bool withCapacity = param->GetElementAttributeBoolValue( "withCapacity", true );
        int iMaxElements = param->GetElementAttributeIntValue( "RCCounter", 5 );    // default is 5 RC Elements
        size_t maxElements( iMaxElements );

        SaneInput( iMaxElements );    // throws

        boost::shared_ptr< WarburgCotanh< MatrixT > > tp( new WarburgCotanh< MatrixT >( observable ) );

        // HACK: GIGANTIC HACK
        // After reading these lines you should feel dirty

        boost::scoped_ptr< ArgumentTypeObject< double > > argument( new ArgumentTypeObject< double > );
        argument->mDoubleMap.insert( std::pair< const char *, double >( "Operator", factory::OBJECT_MULTIPLICATION ) );

        boost::shared_ptr< xmlparser::XmlParameter > sigmaObjParam( param->GetElementChild( "Sigma" ) );
        boost::shared_ptr< object::Object< double > > sigmaObj( this->GetObjectFactory()->CreateInstance( sigmaObjParam ) );

        // From here we are in nomansland where Object are mere bytes and the address of and Obj is represented as a
        // double...
        // pack our objectaddress into a nice little double
        // no easy conversion is known

        double addressR = ConvertObjectPointer2double( sigmaObj.get() );
        argument->mDoubleMap["Operand"] = addressR;

        // sigma^2
        boost::shared_ptr< object::Object< double > > sigmaSquared(
         this->GetObjectFactory()->CreateInstance( sigmaObjParam, argument.get() ) );


        boost::shared_ptr< xmlparser::XmlParameter > cObjParam( param->GetElementChild( "Clim" ) );


        // Add the packed to our map
        for ( size_t counter = 0; counter < maxElements; ++counter )
        {
            argument->mDoubleMap["Multiplier"] = CalculateCoefficent( counter + 1 );

            double addressSigmaSquared = ConvertObjectPointer2double( sigmaSquared.get() );
            argument->mDoubleMap["Operand"] = addressSigmaSquared;

            boost::shared_ptr< object::Object< double > > rObj(
             this->GetObjectFactory()->CreateInstance( cObjParam, this->SetObjectFactorToArg( arg, true, argument ) ) );    // R
            argument->mObjectFactor = 1.0;

            double addressTau = ConvertObjectPointer2double( rObj.get() );

            argument->mDoubleMap["Operand"] = addressTau;
            argument->mDoubleMap["Multiplier"] = 0.5;

            boost::shared_ptr< object::Object< double > > tauObj(
             this->GetObjectFactory()->CreateInstance( cObjParam, this->SetObjectFactorToArg( arg, false, argument ) ) );    // Tau
            argument->mObjectFactor = 1.0;

            double maxRelaxationTime = tauObj.get()->GetMaxValueOfLookup();

            if ( ElectricalClassWrapperBase< MatrixT >::SimplifyRC( param, maxRelaxationTime ) )
            {
                argument->mDoubleMap["Operand"] = addressSigmaSquared;
                argument->mDoubleMap["Multiplier"] = 1.0;
                AddResidualOhmicResistance( tp, cObjParam, this->SetObjectFactorToArg( arg, true, argument ), counter,
                                            maxElements, observable );
                argument->mObjectFactor = 1.0;
                break;
            }

            tp->AddChild( new ParallelRC< MatrixT >( rObj, tauObj, observable ) );
        }

        if ( withCapacity )
        {
            boost::scoped_ptr< ArgumentTypeObject< double > > argForCapacity;
            boost::shared_ptr< object::Object< double > > cObj(
             this->GetObjectFactory()->CreateInstance( cObjParam, this->SetObjectFactorToArg( arg, false, argForCapacity ) ) );    // C
            tp->AddChild( new Capacity< MatrixT >( cObj, observable ) );
        }
        return tp;
    }
};

/// Classwrapper for electrical::WarburgTanh
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, WarburgTanh > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    double CalculateCoefficent( size_t elementNumber ) const { return std::pow( ( 2 * elementNumber ) - 1, 2.0 ); }

    bool SaneInput( int maxElements )
    {
        if ( maxElements <= 0 )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "negativeValue", maxElements );
        }
        return true;
    }


    void AddResidualOhmicResistance( boost::shared_ptr< SerialTwoPort< MatrixT > > tp,
                                     boost::shared_ptr< xmlparser::XmlParameter > robjparam, size_t start, size_t end, bool observable )
    {
        double residualOhmicResistance = 0;
        for ( ; start < end; ++start )
            residualOhmicResistance += 1 / CalculateCoefficent( start + 1 );

        boost::scoped_ptr< ArgumentTypeObject< double > > argument( new ArgumentTypeObject< double > );
        const char *mapEntry = "Multiplier";
        argument->mDoubleMap.insert( std::pair< const char *, double >( mapEntry, residualOhmicResistance ) );

        boost::shared_ptr< object::Object< double > > rObj( this->GetObjectFactory()->CreateInstance( robjparam, argument.get() ) );

        tp->AddChild( new OhmicResistance< MatrixT >( rObj, observable ) );
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );
        int iMaxElements = param->GetElementAttributeIntValue( "RCCounter", 5 );    // default is 5 RC Elements
        size_t maxElements( iMaxElements );

        SaneInput( iMaxElements );    // throws

        boost::shared_ptr< WarburgTanh< MatrixT > > tp( new WarburgTanh< MatrixT >( observable ) );


        for ( size_t counter = 0; counter < maxElements; ++counter )
        {
            boost::scoped_ptr< ArgumentTypeObject< double > > argument( new ArgumentTypeObject< double > );
            const char *mapEntry = "Multiplier";
            argument->mDoubleMap.insert( std::pair< const char *, double >( mapEntry, 1.0 / CalculateCoefficent( counter + 1 ) ) );

            boost::shared_ptr< xmlparser::XmlParameter > robjparam( param->GetElementChild( "OhmicResistance" ) );

            boost::shared_ptr< object::Object< double > > rObj(
             this->GetObjectFactory()->CreateInstance( robjparam, this->SetObjectFactorToArg( arg, true, argument ) ) );
            argument->mObjectFactor = 1.0;

            boost::shared_ptr< xmlparser::XmlParameter > cobjparam( param->GetElementChild( "Tau" ) );
            boost::shared_ptr< object::Object< double > > tauObj(
             this->GetObjectFactory()->CreateInstance( cobjparam, argument.get() ) );

            double maxRelaxationTime = tauObj.get()->GetMaxValueOfLookup();

            if ( ElectricalClassWrapperBase< MatrixT >::SimplifyRC( param, maxRelaxationTime ) )
            {
                AddResidualOhmicResistance( tp, robjparam, counter, maxElements, observable );
                break;
            }

            tp->AddChild( new ParallelRC< MatrixT >( rObj, tauObj, observable ) );
        }
        return tp;
    }
};

/// Classwrapper for electrical::ParallelRCAlg
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ParallelRCAlg > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );

        boost::shared_ptr< xmlparser::XmlParameter > robjparam( param->GetElementChild( "RObject" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject;
        boost::shared_ptr< object::Object< double > > rObj(
         this->GetObjectFactory()->CreateInstance( robjparam, this->SetObjectFactorToArg( arg, true, argObject ) ) );

        boost::shared_ptr< xmlparser::XmlParameter > cobjparam( param->GetElementChild( "CObject" ) );
        argObject.reset();
        boost::shared_ptr< object::Object< double > > cObj(
         this->GetObjectFactory()->CreateInstance( cobjparam, this->SetObjectFactorToArg( arg, false, argObject ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new ParallelRCAlg< MatrixT >( rObj, cObj, observable ) );
    }
};

/// Classwrapper for electrical::Cellelement
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Cellelement > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< electrical::state::SurfaceSoc >
    CreateSurfaceSoc( const xmlparser::XmlParameter *param, const ArgumentTypeState *arg = 0 )
    {
        typedef electrical::state::SurfaceSoc SurfaceSocState_t;
        return boost::static_pointer_cast< SurfaceSocState_t >(
         this->GetStateFactory()->CreateInstance( param->GetElementChild( "SurfaceSoc" ), arg ) );
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical * /* arg */ = 0 )
    {
        typedef ::state::ThermalState< double > ThermalState_t;
        typedef electrical::state::Soc SocState_t;

        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children = param->GetElementChildren( "Children" );
        if ( children.empty() )
        {
            ErrorFunction< factory::NoChildrenException >( __FUNCTION__, __LINE__, __FILE__, "NoChildrenException",
                                                           "Cellelement" );
        }
        const size_t count =
         CheckPositiveInt( param->GetElementChild( "Children" )->GetElementAttributeIntValue( "count", 1 ),
                           param->GetElementChild( "Children" ), "count" );

        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );
        const size_t numberOfCellElements =
         param->HasElement( "ThermalBlock" ) ?
          factory::GetElectricalDiscretization( param->GetElementChild( "ThermalBlock" ) ) :
          1;

        boost::scoped_ptr< ArgumentTypeState > argState;
        boost::scoped_ptr< ArgumentTypeElectrical > argElectrical;
        if ( numberOfCellElements != 1 )
        {
            argState.reset( new ArgumentTypeState );
            argState->mCellDiscretization = numberOfCellElements;
            argElectrical.reset( new ArgumentTypeElectrical );
            argElectrical->mCellDiscretization = numberOfCellElements;
        }
        boost::shared_ptr< ParallelTwoPort< MatrixT > > parallelCellElements( new ParallelTwoPort< MatrixT >( observable ) );

        for ( size_t i = 0; i < numberOfCellElements; ++i )
        {
            boost::shared_ptr< ThermalState_t > tempObj( boost::static_pointer_cast< ThermalState_t >(
             this->GetStateFactory()->CreateInstance( param->GetElementChild( "ThermalState" ), argState.get() ) ) );
            boost::shared_ptr< SocState_t > socObj( boost::static_pointer_cast< SocState_t >(
             this->GetStateFactory()->CreateInstance( param->GetElementChild( "Soc" ), argState.get() ) ) );

            boost::shared_ptr< electrical::state::SurfaceSoc > surfaceSoc;
            if ( param->HasElement(
                  "SurfaceSoc" ) )    // TODO: Should be probably checked if it is set to True or not False
            {
                surfaceSoc = CreateSurfaceSoc( param, argState.get() );
            }

            boost::shared_ptr< Cellelement< MatrixT > > cellelement( new Cellelement< MatrixT >( tempObj, socObj, observable ) );
            std::vector< electrical::TwoPort< myMatrixType > * > surfaceEffectedElements;
            for ( size_t i = 0; i < count; ++i )
            {
                for ( std::vector< boost::shared_ptr< xmlparser::XmlParameter > >::iterator it = children.begin();
                      it != children.end(); ++it )
                {
                    boost::shared_ptr< electrical::TwoPort< myMatrixType > > element(
                     this->GetElectricalFactory()->CreateInstance( it->get(), argElectrical.get() ) );
                    if ( ( *it )->HasElementAttribute( "SurfaceEffect" ) )
                        surfaceEffectedElements.push_back( element.get() );

                    cellelement->AddChild( element );
                }
            }

            if ( param->HasElement(
                  "SurfaceSoc" ) )    // TODO: Should be probably checked if it is set to True or not False
            {
                surfaceSoc->SetElementsForLookup( surfaceEffectedElements );
                surfaceSoc->SetReverseLookup( this->GetObjectFactory()->CreateInstance(
                 param->GetElementChild( "SurfaceSoc" )->GetElementChild( "ReverseLookup" ) ) );
                cellelement->SetSurfaceSOC( surfaceSoc );
            }

            parallelCellElements->AddChild( cellelement );
        }

        boost::shared_ptr< TwoPort< MatrixT > > cell;
        if ( numberOfCellElements == 1 )
            cell = parallelCellElements->shared_at( 0 );
        else
            cell = parallelCellElements;
        return cell;
    }
};

/// Classwrapper for electrical::Zarc
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Zarc > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        typedef object::LookupObj2dWithState< double > Obj2D_t;

        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );
        double dtValue = 0.001;

        if ( param->GetConfigurationRoot()->HasElement( "Options" ) &&
             param->GetConfigurationRoot()->GetElementChild( "Options" )->HasElement( "SampleRate" ) )
        {
            double samp =
             param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementDoubleValue( "SampleRate", 1000 );
            if ( samp > 0.0 )
                dtValue = 1.0 / samp;
            else
                ErrorFunction< std::range_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );
        }

        boost::shared_ptr< Obj2D_t > tauObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupTau" ) ) ) );

        boost::scoped_ptr< ArgumentTypeObject< double > > argObject;
        boost::shared_ptr< Obj2D_t > ohmObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupOhmicResistance" ),
                                                   this->SetObjectFactorToArg( arg, true, argObject ) ) ) );

        boost::shared_ptr< Obj2D_t > phiObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupPhi" ) ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new Zarc< MatrixT >( tauObj, ohmObj, phiObj, dtValue, observable ) );
    }
};

/// Classwrapper for electrical::ZarcAlg
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ZarcAlg > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        typedef object::LookupObj2dWithState< double > Obj2D_t;

        const bool observable =
         param->GetElementAttributeBoolValue( "observable" ) || param->GetElementAttributeBoolValue( "SurfaceEffect" );
        double sampleRate = 0.001;
        if ( param->GetConfigurationRoot()->HasElement( "Options" ) &&
             param->GetConfigurationRoot()->GetElementChild( "Options" )->HasElement( "SampleRate" ) )
        {
            double samp =
             param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementDoubleValue( "SampleRate", 1000 );
            if ( samp > 0.0 )
                sampleRate = samp;
            else
                ErrorFunction< std::range_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );
        }


        boost::shared_ptr< Obj2D_t > tauObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupTau" ) ) ) );

        boost::scoped_ptr< ArgumentTypeObject< double > > argObject;
        boost::shared_ptr< Obj2D_t > ohmObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupOhmicResistance" ),
                                                   this->SetObjectFactorToArg( arg, true, argObject ) ) ) );

        boost::shared_ptr< Obj2D_t > phiObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupPhi" ) ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new ZarcAlg< MatrixT >( tauObj, ohmObj, phiObj, sampleRate, observable ) );
    }
};

/// Classwrapper for electrical::ParallelTwoPort
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ParallelTwoPort > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical * /* arg */ = 0 )
    {
        boost::shared_ptr< ParallelTwoPort< MatrixT > > tp( new ParallelTwoPort< MatrixT >() );

        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children = param->GetElementChildren( "Children" );

        if ( children.empty() )
        {
            ErrorFunction< factory::NoChildrenException >( __FUNCTION__, __LINE__, __FILE__, "NoChildrenException" );
        }

        const size_t count =
         CheckPositiveInt( param->GetElementChild( "Children" )->GetElementAttributeIntValue( "count", 1 ),
                           param->GetElementChild( "Children" ), "count" );

        for ( size_t i = 0; i < count; ++i )
            for ( std::vector< boost::shared_ptr< xmlparser::XmlParameter > >::iterator it = children.begin();
                  it != children.end(); ++it )
            {
                boost::shared_ptr< TwoPort< MatrixT > > childtp( this->GetElectricalFactory()->CreateInstance( it->get() ) );
                tp->AddChild( childtp );
            }

        return tp;
    }
};

/// Classwrapper for electrical::SerialTwoPort
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, SerialTwoPort > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical * /* arg */ = 0 )
    {
        boost::shared_ptr< SerialTwoPort< MatrixT > > tp( new SerialTwoPort< MatrixT >() );

        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children = param->GetElementChildren( "Children" );

        if ( children.empty() )
        {
            ErrorFunction< factory::NoChildrenException >( __FUNCTION__, __LINE__, __FILE__, "NoChildrenException",
                                                           "SerialTwoPort" );
        }

        const size_t count =
         CheckPositiveInt( param->GetElementChild( "Children" )->GetElementAttributeIntValue( "count", 1 ),
                           param->GetElementChild( "Children" ), "count" );

        for ( size_t i = 0; i < count; ++i )
            for ( std::vector< boost::shared_ptr< xmlparser::XmlParameter > >::iterator it = children.begin();
                  it != children.end(); ++it )
            {
                boost::shared_ptr< TwoPort< MatrixT > > childtp( this->GetElectricalFactory()->CreateInstance( it->get() ) );
                tp->AddChild( childtp );
            }

        return tp;
    }
};
}
