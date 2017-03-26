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
#ifndef MATERIALCLASSWRAPPER_H_
#define MATERIALCLASSWRAPPER_H_

#ifdef __EXCEPTIONS__
#include <cstdio>
#endif

#include "../factory.h"
#include "../classwrapper.h"

#include "../../thermal/materials/material.h"
#include "parameter_description.h"
#include "../../exceptions/error_proto.h"


namespace factory
{

/// Classwrapper for thermal::Material without anisotropy
template < typename ValueT >
class MaterialClassWrapperIsotropic : public ClassWrapperBase< thermal::Material< ValueT > >
{
    public:
    boost::shared_ptr< thermal::Material< ValueT > > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentType * )
    {
        misc::StrCont description = ParameterDescription( param, "Material" );
        if ( !param->GetElementAttributeBoolValue( "cache", false ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EachMaterialNotCached",
                                                 param->GetElementName(), "Material", param->GetLineNumber() );

        const ValueT density = param->GetElementDoubleValue( "Density" );
        const ValueT specificCapacity = param->GetElementDoubleValue( "SpecificCapacity" );
        const ValueT conductivity = param->GetElementDoubleValue( "Conductivity" );

        boost::shared_ptr< thermal::Material< ValueT > > material;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            material.reset( new thermal::Material< ValueT >( density, specificCapacity, conductivity, conductivity,
                                                             conductivity, thermal::Material< ValueT >::ISOTROPIC ) );
#ifdef __EXCEPTIONS__
        }
        catch ( const std::runtime_error &e )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error79", e.what(),
                                                 param->GetElementName(), "Material", param->GetLineNumber() );
        }
#endif

        return material;
    }
};

/// Classwrapper for thermal::Material with anisotropic conductivity given in cartesian coordinates
template < typename ValueT >
class MaterialClassWrapperCartesian : public ClassWrapperBase< thermal::Material< ValueT > >
{
    public:
    boost::shared_ptr< thermal::Material< ValueT > > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentType * )
    {
        misc::StrCont description = ParameterDescription( param, "MaterialCartesian" );
        if ( !param->GetElementAttributeBoolValue( "cache", false ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EachMaterialNotCached",
                                                 param->GetElementName(), "MaterialCartesian", param->GetLineNumber() );

        const ValueT density = param->GetElementDoubleValue( "Density" );
        const ValueT specificCapacity = param->GetElementDoubleValue( "SpecificCapacity" );
        const ValueT conductivityX = param->GetElementDoubleValue( "ConductivityX" );
        const ValueT conductivityY = param->GetElementDoubleValue( "ConductivityY" );
        const ValueT conductivityZ = param->GetElementDoubleValue( "ConductivityZ" );

        boost::shared_ptr< thermal::Material< ValueT > > material;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            material.reset( new thermal::Material< ValueT >( density, specificCapacity, conductivityX, conductivityY,
                                                             conductivityZ, thermal::Material< ValueT >::CARTESIAN ) );
#ifdef __EXCEPTIONS__
        }
        catch ( const std::runtime_error &e )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error79", e.what(),
                                                 param->GetElementName(), "MaterialCartesian", param->GetLineNumber() );
        }
#endif

        return material;
    }
};

/// Classwrapper for thermal::Material with anisotropic conductivity given in cylindircal coordinates
template < typename ValueT >
class MaterialClassWrapperCylindric : public ClassWrapperBase< thermal::Material< ValueT > >
{
    public:
    boost::shared_ptr< thermal::Material< ValueT > > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentType * )
    {
        misc::StrCont description = ParameterDescription( param, "MaterialCylindric" );
        if ( !param->GetElementAttributeBoolValue( "cache", false ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EachMaterialNotCached",
                                                 param->GetElementName(), "MaterialCylindric", param->GetLineNumber() );

        const ValueT density = param->GetElementDoubleValue( "Density" );
        const ValueT specificCapacity = param->GetElementDoubleValue( "SpecificCapacity" );
        const ValueT conductivityRho = param->GetElementDoubleValue( "ConductivityRho" );
        const ValueT conductivityPhi = param->GetElementDoubleValue( "ConductivityPhi" );
        const ValueT conductivityZ = param->GetElementDoubleValue( "ConductivityZ" );

        boost::shared_ptr< thermal::Material< ValueT > > material;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            material.reset( new thermal::Material< ValueT >( density, specificCapacity, conductivityRho, conductivityPhi,
                                                             conductivityZ, thermal::Material< ValueT >::CYLINDRIC ) );
#ifdef __EXCEPTIONS__
        }
        catch ( const std::runtime_error &e )
        // catch(const thermal::InitializationError &e)
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error79", e.what(),
                                                 param->GetElementName(), "MaterialiCylindric", param->GetLineNumber() );
        }
#endif

        return material;
    }
};
}
#endif
