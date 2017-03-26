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
#ifndef _COOLINGCLASSWRAPPER_
#define _COOLINGCLASSWRAPPER_

#include <map>
#include <boost/shared_ptr.hpp>
#include "../factory.h"
#include "../classwrapper.h"

#include "../../xmlparser/xmlparameter.h"
#include "../../thermal/boundaryConditions/cooling.h"

#include "../object/objectclasswrapper.h"
#include "parameter_description.h"
#include "../../exceptions/error_proto.h"


namespace factory
{

/// Classwrapper base class for being used in a generic ::thermal::Cooling Factory. This class has to be derived.
template < typename T >
class CoolingClassWrapperBase : public ClassWrapperBase< thermal::Cooling< T > >
{
    public:
    /**
     * Classwrapper constructor.
     * @param[in] objectFactory A Factory for class in the ::object namespace
     */
    CoolingClassWrapperBase( Factory< object::Object< T >, ArgumentTypeObject< T > > *objectFactory )
        : mObjectFactory( objectFactory )
    {
    }

    protected:
    /// Get an ::object::Object Factory.
    Factory< object::Object< T >, ArgumentTypeObject< T > > *GetObjectFactory() { return mObjectFactory; }

    private:
    Factory< object::Object< T >, ArgumentTypeObject< T > > *const mObjectFactory;
};


/// Classwrapper for thermal::Cooling. This template class has to be specialized in order to create an instance of a
/// particular class.
template < typename T, template < typename > class TConcrete >
class CoolingClassWrapper : public CoolingClassWrapperBase< T >
{
    public:
    CoolingClassWrapper( Factory< object::Object< T >, ArgumentTypeObject< T > > *objectFactory )
        : CoolingClassWrapperBase< T >( objectFactory )
    {
    }
};

/// Classwrapper for thermal::DirichletBoundaryCondition
template < typename T >
class CoolingClassWrapper< T, thermal::DirichletBoundaryCondition > : public CoolingClassWrapperBase< T >
{
    public:
    CoolingClassWrapper( Factory< object::Object< T >, ArgumentTypeObject< T > > *objectFactory )
        : CoolingClassWrapperBase< T >( objectFactory )
    {
    }

    boost::shared_ptr< thermal::Cooling< T > > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentType * )
    {
        return boost::shared_ptr< thermal::Cooling< T > >(
         new thermal::DirichletBoundaryCondition< T >( param->GetElementDoubleValue( "SurfaceTemperature" ) ) );
    }
};

/// Classwrapper for thermal::CoolingByLookUp
template < typename T >
class CoolingClassWrapper< T, thermal::CoolingByLookUp > : public CoolingClassWrapperBase< T >
{
    public:
    CoolingClassWrapper( Factory< object::Object< T >, ArgumentTypeObject< T > > *objectFactory )
        : CoolingClassWrapperBase< T >( objectFactory )
    {
    }

    boost::shared_ptr< thermal::Cooling< T > > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentType * )
    {
        const boost::shared_ptr< object::Object< T > > offsetLookUp(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookUpTable" ) ) );


        std::vector< double > measurementPoints =
         param->GetElementChild( "LookUpTable" )->GetElementDoubleVecValue( "MeasurementPoints" );
        std::vector< double > lookUpData =
         param->GetElementChild( "LookUpTable" )->GetElementDoubleVecValue( "LookupData" );
        std::vector< double > slopeMeasurementPoints;
        slopeMeasurementPoints.reserve( 2 * measurementPoints.size() );
        std::vector< double > slopeLookUpData;
        slopeLookUpData.reserve( 2 * measurementPoints.size() );

        slopeLookUpData.push_back( 0.0 );
        slopeMeasurementPoints.push_back( measurementPoints.front() - 0.1 );
        for ( size_t i = 0; i < measurementPoints.size() - 1; ++i )
        {
            const double timeIntervall = measurementPoints[i + 1] - measurementPoints[i];
            if ( timeIntervall < 1.0 )
            {
                slopeMeasurementPoints.push_back( measurementPoints[i] + timeIntervall * 0.1 );
                slopeMeasurementPoints.push_back( measurementPoints[i + 1] - timeIntervall * 0.1 );
            }
            else
            {
                slopeMeasurementPoints.push_back( measurementPoints[i] + 0.1 );
                slopeMeasurementPoints.push_back( measurementPoints[i + 1] - 0.1 );
            }

            const double slopeValue = ( lookUpData[i + 1] - lookUpData[i] ) / timeIntervall;
            slopeLookUpData.push_back( slopeValue );
            slopeLookUpData.push_back( slopeValue );
        }
        slopeLookUpData.push_back( 0.0 );
        slopeMeasurementPoints.push_back( measurementPoints.back() + 0.1 );
        boost::shared_ptr< object::Object< T > > slopeLookUp( new object::LookupObj1D< T >( slopeLookUpData, slopeMeasurementPoints ) );

        boost::shared_ptr< thermal::Cooling< T > > cooling;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            cooling.reset( new thermal::CoolingByLookUp< T >( offsetLookUp, slopeLookUp ) );
#ifdef __EXCEPTIONS__
        }
        catch ( const std::runtime_error &e )
        // catch(const thermal::InitializationError &e)
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error79", e.what(),
                                                 param->GetElementName(), "CollingByLookUp", param->GetLineNumber() );
        }
#endif

        return cooling;
    }
};

/// Classwrapper for thermal::CoolingByConstantValue
template < typename T >
class CoolingClassWrapper< T, thermal::CoolingByConstantValue > : public CoolingClassWrapperBase< T >
{
    public:
    CoolingClassWrapper( Factory< object::Object< T >, ArgumentTypeObject< T > > *objectFactory )
        : CoolingClassWrapperBase< T >( objectFactory )
    {
    }

    boost::shared_ptr< thermal::Cooling< T > > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentType * )
    {
        return boost::shared_ptr< thermal::Cooling< T > >(
         new thermal::CoolingByConstantValue< T >( param->GetElementDoubleValue( "Value" ) ) );
    }
};

/// Classwrapper for thermal::CoolingByLookUp which always returns zero cooling
template < typename T >
class CoolingClassWrapperThermalIsolation : public CoolingClassWrapperBase< T >
{
    public:
    CoolingClassWrapperThermalIsolation( Factory< object::Object< T >, ArgumentTypeObject< T > > *objectFactory )
        : CoolingClassWrapperBase< T >( objectFactory )
    {
    }

    boost::shared_ptr< thermal::Cooling< T > > CreateInstance( const xmlparser::XmlParameter *, const ArgumentType * )
    {
        return boost::shared_ptr< thermal::Cooling< T > >( new thermal::CoolingByConstantValue< T >( 0.0 ) );
    }
};
}
#endif
