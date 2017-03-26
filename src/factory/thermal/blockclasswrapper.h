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
#ifndef _BLOCKCLASSWRAPPER_
#define _BLOCKCLASSWRAPPER_

#include <map>
#include <boost/shared_ptr.hpp>
#include "../factory.h"
#include "../classwrapper.h"

#include "../../xmlparser/xmlparameter.h"
#include "../../thermal/blocks/thermal_block.h"
#include "../../thermal/blocks/rectangular_block.h"
#include "../../thermal/blocks/quadratic_cell_block.h"
#include "../../thermal/blocks/supercap.h"
#include "../../thermal/blocks/hexagonal_cell_block.h"
#include "../../thermal/blocks/triangular_prism_block.h"

#include "../../geometry/cartesian.h"
#include "../../states/thermal_state.h"
#include "../thermal/materialclasswrapper.h"
#include "parameter_description.h"
#include "../../exceptions/error_proto.h"

namespace factory
{

template < typename T >
struct ArgumentTypeThermalBlock
{
    ArgumentTypeThermalBlock()
        : mX( 0 )
        , mY( 0 )
        , mZ( 0 ){};
    std::vector< boost::shared_ptr< ::state::ThermalState< T > > > mThermalStates;
    T mX;
    T mY;
    T mZ;
};

/// Classwrapper base class for being used in a generic ::thermal::ThermalBlock Factory. This class has to be derived.
template < typename T >
class BlockClassWrapperBase : public ClassWrapperBase< thermal::ThermalBlock< T >, ArgumentTypeThermalBlock< T > >
{
    public:
    /**
     * Classwrapper constructor.
     * @param materialFactory A Factory for classes derived from ::thermal::Material.
     */
    BlockClassWrapperBase( Factory< thermal::Material< T > > *materialFactory )
        : mMaterialFactory( materialFactory )
    {
    }

    protected:
    /// Get a ::thermal::Material Factory.
    Factory< thermal::Material< T > > *GetMaterialFactory() { return mMaterialFactory; }

    /// Gets thermal states from argument
    std::vector< boost::shared_ptr< ::state::ThermalState< T > > >
    MakeThermalStatesVector( const xmlparser::XmlParameter *param, const ArgumentTypeThermalBlock< T > *arg )
    {
        std::vector< boost::shared_ptr< ::state::ThermalState< T > > > thermalStates;
        if ( arg->mThermalStates.empty() )
        {
            thermalStates.push_back( boost::shared_ptr< ::state::ThermalState< T > >() );
            if ( param->HasElement( "Heat" ) )
            {
                thermalStates.at( 0 ).reset( new ::state::ThermalState< T > );
                thermalStates.at( 0 )->SetFixedPowerDissipation( param->GetElementDoubleValue( "Heat" ) );
            }
        }
        else
            thermalStates = arg->mThermalStates;
        return thermalStates;
    }

    /// Gets emplacment point from argument
    geometry::Cartesian< T > MakeEmplacementPoint( const ArgumentTypeThermalBlock< T > *arg )
    {
        return geometry::Cartesian< T >( arg->mX, arg->mY, arg->mZ );
    }

    private:
    Factory< thermal::Material< T > > *const mMaterialFactory;
};


/// Classwrapper for thermal::ThermalBlock. This template class has to be specialized in order to create an instance of
/// a particular class.
template < typename T, template < typename > class TConcrete >
class BlockClassWrapper : public BlockClassWrapperBase< T >
{
    public:
    BlockClassWrapper( Factory< thermal::Material< T > > *materialFactory )
        : BlockClassWrapperBase< T >( materialFactory )
    {
    }
};

/// Classwrapper for thermal::RectangularBlock
template < typename T >
class BlockClassWrapper< T, thermal::RectangularBlock > : public BlockClassWrapperBase< T >
{
    public:
    BlockClassWrapper( Factory< thermal::Material< T > > *materialFactory )
        : BlockClassWrapperBase< T >( materialFactory )
    {
    }

    boost::shared_ptr< thermal::ThermalBlock< T > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeThermalBlock< T > *arg = 0 )
    {
        using namespace thermal;
        using namespace geometry;

        if ( arg == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "argNullPointer" );
        const geometry::Cartesian< T > emplacementPoint = BlockClassWrapperBase< T >::MakeEmplacementPoint( arg );
        misc::StrCont description = ParameterDescription( param, "RectangularBlock", emplacementPoint );

        const boost::shared_ptr< xmlparser::XmlParameter > materialParam( param->GetElementChild( "Material" ) );
        const Material< T > *const material = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( material->GetMaterialType() != thermal::Material< T >::CARTESIAN &&
             material->GetMaterialType() != thermal::Material< T >::ISOTROPIC )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "MaterialNonCartesianOrIsotropic",
                                                 param->GetElementName(), "RectangularBlock", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );

        const vector< T > dimensions = param->GetElementDoubleVecValue( "Dimensions" );
        const size_t sizeX = abs( param->GetElementIntValue( "SizeX" ) );
        const size_t sizeY = abs( param->GetElementIntValue( "SizeY" ) );
        const size_t sizeZ = abs( param->GetElementIntValue( "SizeZ" ) );
        const T temperature = param->GetElementDoubleValue( "InitialTemperature" );


        size_t discX = 1, discY = 1, discZ = 1;
        if ( param->HasElement( "ElectricalDiscretization" ) && !arg->mThermalStates.empty() )
        {
            boost::shared_ptr< xmlparser::XmlParameter > discParam =
             param->GetElementChild( "ElectricalDiscretization" );
            discX = abs( discParam->GetElementAttributeIntValue( "x", 1 ) );
            discY = abs( discParam->GetElementAttributeIntValue( "y", 1 ) );
            discZ = abs( discParam->GetElementAttributeIntValue( "z", 1 ) );
            if ( discParam->HasElementAttribute( "phi" ) || discParam->HasElementAttribute( "rho" ) )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "InElectricalDiscretizationInRectBlockXyZAllowed", param->GetElementName(),
                                                     "RectangularBlock", emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                     emplacementPoint.GetZ(), param->GetLineNumber() );
        }


        boost::shared_ptr< thermal::ThermalBlock< T > > thermalBlock;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            thermalBlock.reset( new thermal::RectangularBlock< T >(
             description, emplacementPoint, dimensions[0], dimensions[1], dimensions[2], sizeX, sizeY, sizeZ, material,
             temperature, BlockClassWrapperBase< T >::MakeThermalStatesVector( param, arg ), discX, discY, discZ ) );
#ifdef __EXCEPTIONS__
        }
        catch ( const std::runtime_error &e )
        // catch(const thermal::InitializationError &e)
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error77", e.what(),
                                                 param->GetElementName(), "RectangularBlock", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }
#endif
        return thermalBlock;
    }
};

/// Classwrapper for thermal::QuadraticCellBlock
template < typename T >
class BlockClassWrapper< T, thermal::QuadraticCellBlock > : public BlockClassWrapperBase< T >
{
    public:
    BlockClassWrapper( Factory< thermal::Material< T > > *materialFactory )
        : BlockClassWrapperBase< T >( materialFactory )
    {
    }

    boost::shared_ptr< thermal::ThermalBlock< T > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeThermalBlock< T > *arg = 0 )
    {
        using namespace thermal;
        using namespace geometry;

        if ( arg == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "argNullPointer" );
        const geometry::Cartesian< T > emplacementPoint = BlockClassWrapperBase< T >::MakeEmplacementPoint( arg );
        misc::StrCont description = ParameterDescription( param, "QuadraticCellBlock", emplacementPoint );

        boost::shared_ptr< xmlparser::XmlParameter > materialParam( param->GetElementChild( "CellMaterial" ) );
        const Material< T > *const cellMaterial = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( cellMaterial->GetMaterialType() != thermal::Material< T >::CYLINDRIC &&
             cellMaterial->GetMaterialType() != thermal::Material< T >::ISOTROPIC )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "CellMaterialNonCylindricOrIsotropic", param->GetElementName(),
                                                 "QuadraticCellBlock", emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                 emplacementPoint.GetZ(), param->GetLineNumber() );

        materialParam = param->GetElementChild( "FillMaterial" );
        const Material< T > *const fillMaterial = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( fillMaterial->GetMaterialType() != thermal::Material< T >::CYLINDRIC &&
             fillMaterial->GetMaterialType() != thermal::Material< T >::ISOTROPIC )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "FillMaterialNonCylindricOrIsotropic", param->GetElementName(),
                                                 "QuadraticCellBlock", emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                 emplacementPoint.GetZ(), param->GetLineNumber() );

        const T length = param->GetElementDoubleValue( "Length" );
        const T radiusCell = param->GetElementDoubleValue( "RadiusCell" );
        const T height = param->GetElementDoubleValue( "Height" );
        const size_t nRhoCell = abs( param->GetElementIntValue( "CellRhoDivision" ) );
        const size_t nPhi = abs( param->GetElementIntValue( "PhiDivision" ) );
        const size_t nZ = abs( param->GetElementIntValue( "HeightDivision" ) );
        const T arcPolygonEdgesLength = param->GetElementDoubleValue( "ArcPolygonization" );
        const T temperature = param->GetElementDoubleValue( "InitialTemperature" );


        size_t discPhi = 1, discZ = 1;
        if ( param->HasElement( "ElectricalDiscretization" ) && !arg->mThermalStates.empty() )
        {
            boost::shared_ptr< xmlparser::XmlParameter > discParam =
             param->GetElementChild( "ElectricalDiscretization" );
            discPhi = abs( discParam->GetElementAttributeIntValue( "phi", 1 ) );
            discZ = abs( discParam->GetElementAttributeIntValue( "z", 1 ) );
            if ( discParam->HasElementAttribute( "x" ) || discParam->HasElementAttribute( "y" ) ||
                 discParam->HasElementAttribute( "rho" ) )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "InElectricalDiscretizationInQuadrCellBlockPhiOrZAllowed",
                                                     param->GetElementName(), "QuadraticCellBlock", emplacementPoint.GetX(),
                                                     emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }


        boost::shared_ptr< thermal::ThermalBlock< T > > thermalBlock;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            thermalBlock.reset(
             new thermal::QuadraticCellBlock< T >( description, emplacementPoint, length, radiusCell, height, nRhoCell,
                                                   nPhi, nZ, arcPolygonEdgesLength, cellMaterial, fillMaterial, temperature,
                                                   BlockClassWrapperBase< T >::MakeThermalStatesVector( param, arg ),
                                                   discPhi, discZ ) );
#ifdef __EXCEPTIONS__
        }
        catch ( const std::runtime_error &e )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error77", e.what(),
                                                 param->GetElementName(), "QuadraticCellBlock", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }
#endif
        return thermalBlock;
    }
};

/// Classwrapper for thermal::Supercap
template < typename T >
class BlockClassWrapper< T, thermal::Supercap > : public BlockClassWrapperBase< T >
{
    public:
    BlockClassWrapper( Factory< thermal::Material< T > > *materialFactory )
        : BlockClassWrapperBase< T >( materialFactory )
    {
    }

    boost::shared_ptr< thermal::ThermalBlock< T > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeThermalBlock< T > *arg = 0 )
    {
        using namespace thermal;
        using namespace geometry;

        if ( arg == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "argNullPointer" );
        const geometry::Cartesian< T > emplacementPoint = BlockClassWrapperBase< T >::MakeEmplacementPoint( arg );
        misc::StrCont description = ParameterDescription( param, "Supercap", emplacementPoint );

        boost::shared_ptr< xmlparser::XmlParameter > materialParam( param->GetElementChild( "InnerCellMaterial" ) );
        const Material< T > *const innerCellMaterial = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( innerCellMaterial->GetMaterialType() != thermal::Material< T >::CYLINDRIC &&
             innerCellMaterial->GetMaterialType() != thermal::Material< T >::ISOTROPIC )

            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "InnerCellMaterialNonCylindricOrIsotropic", param->GetElementName(),
                                                 "Supercap", emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                 emplacementPoint.GetZ(), param->GetLineNumber() );

        materialParam = param->GetElementChild( "OuterCellMaterial" );
        const Material< T > *const outerCellMaterial = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( outerCellMaterial->GetMaterialType() != thermal::Material< T >::CYLINDRIC &&
             outerCellMaterial->GetMaterialType() != thermal::Material< T >::ISOTROPIC )

            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "OuterCellMaterialNonCylindricOrIsotropic", param->GetElementName(),
                                                 "Supercap", emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                 emplacementPoint.GetZ(), param->GetLineNumber() );

        materialParam = param->GetElementChild( "FillMaterial" );
        const Material< T > *const fillMaterial = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( fillMaterial->GetMaterialType() != thermal::Material< T >::CYLINDRIC &&
             fillMaterial->GetMaterialType() != thermal::Material< T >::ISOTROPIC )

            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "FillMaterialNonCylindricOrIsotropic", param->GetElementName(),
                                                 "Supercap", emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                 emplacementPoint.GetZ(), param->GetLineNumber() );

        const T length = param->GetElementDoubleValue( "Length" );
        const T innerCellRadius = param->GetElementDoubleValue( "InnerRadiusCell" );
        const T outerCellRadius = param->GetElementDoubleValue( "OuterRadiusCell" );
        const T height = param->GetElementDoubleValue( "Height" );
        const size_t nPhi = abs( param->GetElementIntValue( "PhiDivision" ) );
        const size_t nZ = abs( param->GetElementIntValue( "HeightDivision" ) );
        const T arcPolygonEdgesLength = param->GetElementDoubleValue( "ArcPolygonization" );
        const T temperature = param->GetElementDoubleValue( "InitialTemperature" );


        size_t discPhi = 1, discZ = 1;
        if ( param->HasElement( "ElectricalDiscretization" ) && !arg->mThermalStates.empty() )
        {
            boost::shared_ptr< xmlparser::XmlParameter > discParam =
             param->GetElementChild( "ElectricalDiscretization" );
            discPhi = abs( discParam->GetElementAttributeIntValue( "phi", 1 ) );
            discZ = abs( discParam->GetElementAttributeIntValue( "z", 1 ) );
            if ( discParam->HasElementAttribute( "x" ) || discParam->HasElementAttribute( "y" ) ||
                 discParam->HasElementAttribute( "rho" ) )

                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "InElectricalDiscretizationInSupercapPhiOrZAllowed",
                                                     param->GetElementName(), "Supercap", emplacementPoint.GetX(),
                                                     emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }


        boost::shared_ptr< thermal::ThermalBlock< T > > thermalBlock;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            thermalBlock.reset(
             new thermal::Supercap< T >( description, emplacementPoint, length, innerCellRadius, outerCellRadius, height, nPhi,
                                         nZ, arcPolygonEdgesLength, innerCellMaterial, outerCellMaterial, fillMaterial, temperature,
                                         BlockClassWrapperBase< T >::MakeThermalStatesVector( param, arg ), discPhi, discZ ) );
#ifdef __EXCEPTIONS__
        }
        catch ( std::runtime_error &e )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error77", e.what(),
                                                 param->GetElementName(), "Supercap", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }
#endif
        return thermalBlock;
    }
};

/// Classwrapper for thermal::HexagonalCellBlock
template < typename T >
class BlockClassWrapper< T, thermal::HexagonalCellBlock > : public BlockClassWrapperBase< T >
{
    public:
    BlockClassWrapper( Factory< thermal::Material< T > > *materialFactory )
        : BlockClassWrapperBase< T >( materialFactory )
    {
    }

    boost::shared_ptr< thermal::ThermalBlock< T > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeThermalBlock< T > *arg = 0 )
    {
        using namespace thermal;
        using namespace geometry;

        if ( arg == 0 )

            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "argNullPointer" );
        const geometry::Cartesian< T > emplacementPoint = BlockClassWrapperBase< T >::MakeEmplacementPoint( arg );
        misc::StrCont description = ParameterDescription( param, "HexagonalCellBlock", emplacementPoint );

        boost::shared_ptr< xmlparser::XmlParameter > materialParam( param->GetElementChild( "CellMaterial" ) );
        const Material< T > *const cellMaterial = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( cellMaterial->GetMaterialType() != thermal::Material< T >::CYLINDRIC &&
             cellMaterial->GetMaterialType() != thermal::Material< T >::ISOTROPIC )

            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "CellMaterialNonCylindricOrIsotropic", param->GetElementName(),
                                                 "HexagonalCellBlock", emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                 emplacementPoint.GetZ(), param->GetLineNumber() );

        materialParam = param->GetElementChild( "FillMaterial" );
        const Material< T > *const fillMaterial = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( fillMaterial->GetMaterialType() != thermal::Material< T >::CYLINDRIC &&
             fillMaterial->GetMaterialType() != thermal::Material< T >::ISOTROPIC )

            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "FillMaterialNonCylindricOrIsotropic", param->GetElementName(),
                                                 "HexagonalCellBlock", emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                 emplacementPoint.GetZ(), param->GetLineNumber() );

        const T length = param->GetElementDoubleValue( "Length" );
        const T radiusCell = param->GetElementDoubleValue( "RadiusCell" );
        const T height = param->GetElementDoubleValue( "Height" );
        const size_t nRhoCell = abs( param->GetElementIntValue( "CellRhoDivision" ) );
        const size_t nPhi = abs( param->GetElementIntValue( "PhiDivision" ) );
        const size_t nZ = abs( param->GetElementIntValue( "HeightDivision" ) );
        const T arcPolygonEdgesLength = param->GetElementDoubleValue( "ArcPolygonization" );
        const T temperature = param->GetElementDoubleValue( "InitialTemperature" );


        size_t discPhi = 1, discZ = 1;
        if ( param->HasElement( "ElectricalDiscretization" ) && !arg->mThermalStates.empty() )
        {
            boost::shared_ptr< xmlparser::XmlParameter > discParam =
             param->GetElementChild( "ElectricalDiscretization" );
            discPhi = abs( discParam->GetElementAttributeIntValue( "phi", 1 ) );
            discZ = abs( discParam->GetElementAttributeIntValue( "z", 1 ) );
            if ( discParam->HasElementAttribute( "x" ) || discParam->HasElementAttribute( "y" ) ||
                 discParam->HasElementAttribute( "rho" ) )

                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "InElectricalDiscretizationInHexCellBlockPhiOrZAllowed",
                                                     param->GetElementName(), "HexagonalCellBlock", emplacementPoint.GetX(),
                                                     emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }


        boost::shared_ptr< thermal::ThermalBlock< T > > thermalBlock;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            thermalBlock.reset(
             new thermal::HexagonalCellBlock< T >( description, emplacementPoint, length, radiusCell, height, nRhoCell,
                                                   nPhi, nZ, arcPolygonEdgesLength, cellMaterial, fillMaterial, temperature,
                                                   BlockClassWrapperBase< T >::MakeThermalStatesVector( param, arg ),
                                                   discPhi, discZ ) );
#ifdef __EXCEPTIONS__
        }
        catch ( std::runtime_error &e )
        // catch(const thermal::InitializationError &e)
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error77", e.what(),
                                                 param->GetElementName(), "HexagonalCellBlock", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }
#endif
        return thermalBlock;
    }
};

/// Classwrapper for thermal::TriangularPrismBlock
template < typename T >
class BlockClassWrapper< T, thermal::TriangularPrismBlock > : public BlockClassWrapperBase< T >
{
    public:
    BlockClassWrapper( Factory< thermal::Material< T > > *materialFactory )
        : BlockClassWrapperBase< T >( materialFactory )
    {
    }

    boost::shared_ptr< thermal::ThermalBlock< T > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeThermalBlock< T > *arg = 0 )
    {
        using namespace thermal;
        using namespace geometry;

        if ( arg == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "argNullPointer" );
        const geometry::Cartesian< T > emplacementPoint = BlockClassWrapperBase< T >::MakeEmplacementPoint( arg );
        misc::StrCont description = ParameterDescription( param, "TriangularPrismBlock", emplacementPoint );

        boost::shared_ptr< xmlparser::XmlParameter > materialParam( param->GetElementChild( "Material" ) );
        const Material< T > *const material = this->GetMaterialFactory()->CreateInstance( materialParam ).get();
        if ( material->GetMaterialType() != thermal::Material< T >::CARTESIAN &&
             material->GetMaterialType() != thermal::Material< T >::ISOTROPIC )

            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "MaterialNonCartesianOrIsotropic",
                                                 param->GetElementName(), "TriangularPrismBlock", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );

        const vector< vector< double > > vertices = param->GetElementDoubleVecVecValue( "Vertices" );
        bool verticesError = false;
        if ( vertices.size() == 3 )
        {
            if ( vertices[0].size() != 2 || vertices[1].size() != 2 || vertices[2].size() != 2 )
                verticesError = true;
        }
        else
        {
            verticesError = true;
        }
        if ( verticesError )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "VerticesThreeTimesTwoElements",
                                                 param->GetElementName(), "TriangularPrismBlock", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );

        TwoDim< T > vertex1( vertices[0][0], vertices[0][1] );
        TwoDim< T > vertex2( vertices[1][0], vertices[1][1] );
        TwoDim< T > vertex3( vertices[2][0], vertices[2][1] );


        const T height = param->GetElementDoubleValue( "Height" );
        const size_t nZ = abs( param->GetElementIntValue( "HeightDivision" ) );
        const T temperature = param->GetElementDoubleValue( "InitialTemperature" );


        size_t discZ = 1;
        if ( param->HasElement( "ElectricalDiscretization" ) && !arg->mThermalStates.empty() )
        {
            boost::shared_ptr< xmlparser::XmlParameter > discParam =
             param->GetElementChild( "ElectricalDiscretization" );
            discZ = abs( discParam->GetElementAttributeIntValue( "z", 1 ) );
            if ( discParam->HasElementAttribute( "x" ) || discParam->HasElementAttribute( "y" ) ||
                 discParam->HasElementAttribute( "rho" ) || discParam->HasElementAttribute( "phi" ) )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "InElectricalDiscretizationInTriangPrismBlockZAllowed",
                                                     param->GetElementName(), "TriangularPrismBlock",
                                                     emplacementPoint.GetX(), emplacementPoint.GetY(),
                                                     emplacementPoint.GetZ(), param->GetLineNumber() );
        }


        boost::shared_ptr< thermal::ThermalBlock< T > > thermalBlock;
#ifdef __EXCEPTIONS__
        try
        {
#endif
            thermalBlock.reset(
             new thermal::TriangularPrismBlock< T >( description, emplacementPoint, vertex1, vertex2, vertex3, height,
                                                     nZ, material, temperature,
                                                     BlockClassWrapperBase< T >::MakeThermalStatesVector( param, arg ), discZ ) );
#ifdef __EXCEPTIONS__
        }
        catch ( std::runtime_error &e )
        // catch(const thermal::InitializationError &e)
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error77", e.what(),
                                                 param->GetElementName(), "TriangularPrismBlock", emplacementPoint.GetX(),
                                                 emplacementPoint.GetY(), emplacementPoint.GetZ(), param->GetLineNumber() );
        }
#endif
        return thermalBlock;
    };
};
}
#endif
