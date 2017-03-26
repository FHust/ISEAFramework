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
#ifndef _COOLINGBLOCKCLASSWRAPPER_
#define _COOLINGBLOCKCLASSWRAPPER_

#include <map>
#include <boost/shared_ptr.hpp>
#include "../factory.h"
#include "../classwrapper.h"

#include "../../xmlparser/xmlparameter.h"
#include "../../thermal/boundaryConditions/cooling_block.h"

#include "../thermal/coolingclasswrapper.h"
#include "parameter_description.h"
#include "../../exceptions/error_proto.h"


namespace factory
{

template < typename T >
struct ArgumentTypeCoolingBlock
{
    ArgumentTypeCoolingBlock()
        : mX( 0 )
        , mY( 0 )
        , mZ( 0 ){};
    T mX;
    T mY;
    T mZ;
};

/// Classwrapper base class for being used in a generic ::thermal::CoolingBlock Factory. This class has to be derived.
template < typename T >
class CoolingBlockClassWrapperBase : public ClassWrapperBase< thermal::CoolingBlock< T >, ArgumentTypeCoolingBlock< T > >
{
    public:
    /**
     * Classwrapper constructor.
     * @param[in] coolingFactory A Factory for classes derived from ::thermal::Cooling
     */
    CoolingBlockClassWrapperBase( Factory< thermal::Cooling< T > > *coolingFactory )
        : mCoolingFactory( coolingFactory )
    {
    }

    protected:
    /// Get a ::thermal::Cooling Factory.
    Factory< thermal::Cooling< T > > *GetCoolingFactory() { return mCoolingFactory; }

    /// Gets emplacment point from argument
    geometry::Cartesian< T > MakeEmplacementPoint( const ArgumentTypeCoolingBlock< T > *arg )
    {
        return geometry::Cartesian< T >( arg->mX, arg->mY, arg->mZ );
    }

    private:
    Factory< thermal::Cooling< T > > *const mCoolingFactory;
};


/// Classwrapper for thermal::CoolingBlock. This template class has to be specialized in order to create an instance of
/// a particular class.
template < typename T, template < typename > class TConcrete >
class CoolingBlockClassWrapper : public CoolingBlockClassWrapperBase< T >
{
    public:
    CoolingBlockClassWrapper( Factory< thermal::Cooling< T > > *coolingFactory )
        : CoolingBlockClassWrapperBase< T >( coolingFactory )
    {
    }
};

/// Classwrapper for thermal::CoolingHorizontalPlane
template < typename T >
class CoolingBlockClassWrapper< T, thermal::CoolingHorizontalPlane > : public CoolingBlockClassWrapperBase< T >
{
    public:
    CoolingBlockClassWrapper( Factory< thermal::Cooling< T > > *coolingFactory )
        : CoolingBlockClassWrapperBase< T >( coolingFactory )
    {
    }

    boost::shared_ptr< thermal::CoolingBlock< T > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeCoolingBlock< T > *arg = 0 )
    {
        const geometry::Cartesian< T > emplacementPoint = CoolingBlockClassWrapperBase< T >::MakeEmplacementPoint( arg );
        misc::StrCont description = ParameterDescription( param, "CoolingHorizontalPlane", emplacementPoint );
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "CoolingHorizontalPlaneDeprecated",
                                             param->GetElementName(), "CoolingHorizontalPlane", emplacementPoint.GetX(),
                                             emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );


        std::vector< std::vector< double > > doubleVecVec = param->GetElementDoubleVecVecValue( "Vertices" );
        BOOST_FOREACH ( std::vector< double > &twoDimensions, doubleVecVec )
        {
            twoDimensions[0] += emplacementPoint.GetX();
            twoDimensions[1] += emplacementPoint.GetY();
        }

        std::vector< geometry::TwoDim< T > > vertices;
        vertices.reserve( doubleVecVec.size() );
        BOOST_FOREACH ( const std::vector< double > doubleVec, doubleVecVec )
        {
            if ( doubleVec.size() != 2 )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TwoCoordinatesForTwoDim",
                                                     param->GetElementName(), "CoolingHorizontalPlane",
                                                     emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                     emplacementPoint.GetZ(), param->GetLineNumber() );
            vertices.push_back( geometry::TwoDim< T >( doubleVec[0], doubleVec[1] ) );
        }

        const T z = param->GetElementDoubleValue( "ZCoordinate" ) + emplacementPoint.GetZ();

        boost::shared_ptr< thermal::Cooling< T > > cooling(
         this->GetCoolingFactory()->CreateInstance( param->GetElementChild( "Cooling" ) ) );

        return boost::shared_ptr< thermal::CoolingBlock< T > >(
         new thermal::CoolingHorizontalPlane< T >( description, vertices, z, cooling ) );
    }
};

/// Classwrapper for thermal::CoolingVerticalPlane
template < typename T >
class CoolingBlockClassWrapper< T, thermal::CoolingVerticalPlane > : public CoolingBlockClassWrapperBase< T >
{
    public:
    CoolingBlockClassWrapper( Factory< thermal::Cooling< T > > *coolingFactory )
        : CoolingBlockClassWrapperBase< T >( coolingFactory )
    {
    }

    boost::shared_ptr< thermal::CoolingBlock< T > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeCoolingBlock< T > *arg = 0 )
    {
        const geometry::Cartesian< T > emplacementPoint = CoolingBlockClassWrapperBase< T >::MakeEmplacementPoint( arg );
        misc::StrCont description = ParameterDescription( param, "CoolingVerticalPlane", emplacementPoint );
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "CoolingVerticalPlaneDeprecated",
                                             param->GetElementName(), "CoolingVerticalPlane", emplacementPoint.GetX(),
                                             emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );

        std::vector< std::vector< double > > doubleVecVec = param->GetElementDoubleVecVecValue( "Vertices" );
        BOOST_FOREACH ( std::vector< double > &twoDimensions, doubleVecVec )
        {
            twoDimensions[0] += emplacementPoint.GetX();
            twoDimensions[1] += emplacementPoint.GetY();
        }

        std::vector< geometry::TwoDim< T > > vertices;
        vertices.reserve( doubleVecVec.size() );
        BOOST_FOREACH ( const std::vector< double > doubleVec, doubleVecVec )
        {
            if ( doubleVec.size() != 2 )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TwoCoordinatesForTwoDim",
                                                     param->GetElementName(), "CoolingVerticalPlane",
                                                     emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                     emplacementPoint.GetZ(), param->GetLineNumber() );
            vertices.push_back( geometry::TwoDim< T >( doubleVec[0], doubleVec[1] ) );
        }

        const T zLower = param->GetElementDoubleValue( "LowerZCoordinate" ) + emplacementPoint.GetZ();
        const T zUpper = param->GetElementDoubleValue( "UpperZCoordinate" ) + emplacementPoint.GetZ();

        boost::shared_ptr< thermal::Cooling< T > > cooling(
         this->GetCoolingFactory()->CreateInstance( param->GetElementChild( "Cooling" ) ) );

        return boost::shared_ptr< thermal::CoolingBlock< T > >(
         new thermal::CoolingVerticalPlane< T >( description, vertices, zLower, zUpper, cooling ) );
    }
};

/// Classwrapper for thermal::CoolingPrismatic
template < typename T >
class CoolingBlockClassWrapper< T, thermal::CoolingPrismatic > : public CoolingBlockClassWrapperBase< T >
{
    public:
    CoolingBlockClassWrapper( Factory< thermal::Cooling< T > > *coolingFactory )
        : CoolingBlockClassWrapperBase< T >( coolingFactory )
    {
    }

    boost::shared_ptr< thermal::CoolingBlock< T > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeCoolingBlock< T > *arg = 0 )
    {
        const geometry::Cartesian< T > emplacementPoint = CoolingBlockClassWrapperBase< T >::MakeEmplacementPoint( arg );
        misc::StrCont description = ParameterDescription( param, "CoolingPrismatic", emplacementPoint );

        std::vector< std::vector< double > > doubleVecVec = param->GetElementDoubleVecVecValue( "Vertices" );
        BOOST_FOREACH ( std::vector< double > &twoDimensions, doubleVecVec )
        {
            twoDimensions[0] += emplacementPoint.GetX();
            twoDimensions[1] += emplacementPoint.GetY();
        }

        std::vector< geometry::TwoDim< T > > vertices;
        vertices.reserve( doubleVecVec.size() );
        BOOST_FOREACH ( const std::vector< double > doubleVec, doubleVecVec )
        {
            if ( doubleVec.size() != 2 )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TwoCoordinatesForTwoDim",
                                                     param->GetElementName(), "CoolingPrismatic", emplacementPoint.GetX(),
                                                     emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
            vertices.push_back( geometry::TwoDim< T >( doubleVec[0], doubleVec[1] ) );
        }

        const T zLower = param->GetElementDoubleValue( "LowerZCoordinate" ) + emplacementPoint.GetZ();
        const T zUpper = param->GetElementDoubleValue( "UpperZCoordinate" ) + emplacementPoint.GetZ();

        boost::shared_ptr< thermal::Cooling< T > > cooling(
         this->GetCoolingFactory()->CreateInstance( param->GetElementChild( "Cooling" ) ) );

        boost::shared_ptr< thermal::CoolingBlock< T > > coolingBlock;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            coolingBlock.reset( new thermal::CoolingPrismatic< T >( description, vertices, zLower, zUpper, cooling ) );
#ifdef __EXCEPTIONS__
        }
        catch ( const std::runtime_error &e )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error77", e.what(),
                                                 param->GetElementName(), "CoolingPrismatic", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }
#endif

        return coolingBlock;
    }
};
}
#endif
