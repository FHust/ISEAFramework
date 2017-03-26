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
#ifndef _THERMALFACTORY_
#define _THERMALFACTORY_

// BOOST
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
// C
#include <cstdlib>

// ETC
#include "blockfactorybuilder.h"
#include "coolingfactorybuilder.h"
#include "coolingblockfactorybuilder.h"
#include "materialfactorybuilder.h"

#include "../check_positive_int.h"
#include "../get_electrical_discretization.h"
#include "../../misc/cmp_str.h"
#include "../../misc/StrCont.h"
#include "../../probes/thermal_probe.h"
#include "../../states/thermal_state.h"
#include "../../thermal/blocks/thermal_block.h"
#include "../../xmlparser/xmlparameter.h"
#include "../../exceptions/error_proto.h"


namespace thermal
{
using namespace geometry;
using boost::shared_ptr;
using boost::scoped_ptr;
using std::list;
using std::vector;


/// Manages the construction of the thermal part of the model from an XML file
template < typename T = double >
class ThermalFactory
{
    public:
    /// Attention: materialFactory must also be parameter in constructor of blockFactory
    ThermalFactory( factory::Factory< Material< T > > *materialFactory,
                    factory::Factory< ThermalBlock< T >, factory::ArgumentTypeThermalBlock< T > > *blockFactory,
                    factory::Factory< thermal::Cooling< T > > *coolingFactory,
                    factory::Factory< CoolingBlock< T >, factory::ArgumentTypeCoolingBlock< T > > *coolingBlockFactory );
    virtual ~ThermalFactory();
    /**
     * Constructs the thermal part of the model
     * @param[in] paramXmlRoot  Root element of XML file
     * @param[out] heatedBlocks Blocks with connection to the electrical part of the model
     * @param[out] unheatedBlocks Additional blocks without connection to the electrical part of the model
     * @param[out] coolingBlocks Geometry and physics data for non-default cooling (default cooling can be passed in
     * \ref ThermalModel)
     * @param[in] thermalStates Thermal states that have been created during parsing of electrical part of the model in
     * correct sequence
     * @param[out] thermalStatesOfCellBlocks Returns all thermal states in cells ordered by the thermal block inside
     * which they are if it is not null
     */
    void CreateThermalModel( const shared_ptr< xmlparser::XmlParameter > &paramXmlRoot,
                             vector< shared_ptr< ThermalBlock< T > > > &heatedBlocks,
                             vector< shared_ptr< ThermalBlock< T > > > &unheatedBlocks,
                             vector< shared_ptr< CoolingBlock< T > > > &coolingBlocks,
                             const vector< shared_ptr< ::state::ThermalState< T > > > *thermalStates,
                             vector< vector< shared_ptr< ThermalState< T > > > > *thermalStatesOfCellBlocks,
                             vector< ::probe::ThermalProbe > *thermalProbes = 0 ) const;

    private:
    /**
     * Parses through RootElement element
     *
     * ParseRootElement is recursively called in CreateThermalModel upon RootElement element, its subelements,
     *subsubelements, etc.
     * The parsing is done in the same sequence as the parsing of the electrical part of the model has been done, thus
     *the likewisely created thermalStates can be assigned correctly.
     * @param[in] param xml-element to go on parsing
     * @param[in] origin Relative position to place ThermalBlock, is moved during recursive calling
     * @param[out] heatedBlocks Blocks with connection to the electrical part of the model
     * @param[out] unheatedBlocks Additional blocks without connection to the electrical part of the model
     * @param[out] coolingBlocks Additional cooling blocks whose emplacement is determined by the count loops of root
     *element
     * @param[in] thermalStatesIndex Determines which element of thermalStates corresponds to the ThermalBlock instance
     *that is created next
     * @param[in] thermalStates Thermal states that have been created during parsing of electrical part of the model in
     *correct sequence
     * @param[out] thermalStatesOfCellBlocks Returns all thermal states in cells ordered by the thermal block inside
     *which they are if it is not null
     */
    void ParseRootElement( const shared_ptr< xmlparser::XmlParameter > &param, const Cartesian< T > &origin,
                           list< shared_ptr< ThermalBlock< T > > > &heatedBlocks, list< shared_ptr< ThermalBlock< T > > > &unheatedBlocks,
                           list< shared_ptr< CoolingBlock< T > > > &coolingBlocks, size_t &thermalStatesIndex,
                           const vector< shared_ptr< ::state::ThermalState< T > > > *thermalStates,
                           vector< vector< shared_ptr< ThermalState< T > > > > *thermalStatesOfCellBlocks ) const;
    /// Function to populate the thermal Probes in the model
    void CreateThermalProbe( const xmlparser::XmlParameter *param = 0, vector< ::probe::ThermalProbe > *thermalProbes = 0 ) const;

    /// Thermal resistance with direct connection to electrical model is only valid under certian conditions, throws if
    /// these conditions are not met
    void CheckIfOhmicResistanceWithThermalBlockIsValid( const shared_ptr< xmlparser::XmlParameter > &param ) const;
    misc::StrCont ParameterDescription( const shared_ptr< xmlparser::XmlParameter > &param ) const;
    factory::Factory< Material< T > > *mMaterialFactory;
    factory::Factory< ThermalBlock< T >, factory::ArgumentTypeThermalBlock< T > > *mBlockFactory;
    factory::Factory< thermal::Cooling< T > > *mCoolingFactory;
    factory::Factory< CoolingBlock< T >, factory::ArgumentTypeCoolingBlock< T > > *mCoolingBlockFactory;
};


template < typename T >
ThermalFactory< T >::ThermalFactory( factory::Factory< Material< T > > *materialFactory,
                                     factory::Factory< ThermalBlock< T >, factory::ArgumentTypeThermalBlock< T > > *blockFactory,
                                     factory::Factory< thermal::Cooling< T > > *coolingFactory,
                                     factory::Factory< CoolingBlock< T >, factory::ArgumentTypeCoolingBlock< T > > *coolingBlockFactory )
    : mMaterialFactory( materialFactory )
    , mBlockFactory( blockFactory )
    , mCoolingFactory( coolingFactory )
    , mCoolingBlockFactory( coolingBlockFactory )
{
}

template < typename T >
ThermalFactory< T >::~ThermalFactory()
{
}

template < typename T >
void ThermalFactory< T >::CreateThermalModel( const shared_ptr< xmlparser::XmlParameter > &paramXmlRoot,
                                              vector< shared_ptr< ThermalBlock< T > > > &heatedBlocks,
                                              vector< shared_ptr< ThermalBlock< T > > > &unheatedBlocks,
                                              vector< shared_ptr< CoolingBlock< T > > > &coolingBlocks,
                                              const vector< shared_ptr< ::state::ThermalState< T > > > *thermalStates,
                                              vector< vector< shared_ptr< ThermalState< T > > > > *thermalStatesOfCellBlocks,
                                              vector< ::probe::ThermalProbe > *thermalProbes ) const
{
    heatedBlocks.clear();
    unheatedBlocks.clear();
    coolingBlocks.clear();
    if ( thermalStatesOfCellBlocks )
        thermalStatesOfCellBlocks->clear();

    // All used materials must be cached inside ThermalMaterials element
    vector< shared_ptr< xmlparser::XmlParameter > > materials = paramXmlRoot->GetElementChildren( "ThermalMaterials" );
    BOOST_FOREACH ( const shared_ptr< xmlparser::XmlParameter > &materialParam, materials )
    {
        if ( !materialParam->GetElementAttributeBoolValue( "cache", false ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "EachMaterialInThermalMaterialsNotCached",
                                                 materialParam->GetElementName(), materialParam->GetLineNumber() );

        mMaterialFactory->CreateInstance( materialParam );
    }

    // CachedCoolings is used for cache coolings (physical description of cooling) for later usage
    vector< shared_ptr< xmlparser::XmlParameter > > cachedCoolingsNode =
     paramXmlRoot->GetElementChildren( "CachedCoolings" );
    BOOST_FOREACH ( const shared_ptr< xmlparser::XmlParameter > &coolingParam, cachedCoolingsNode )
    {
        if ( !coolingParam->GetElementAttributeBoolValue( "cache", false ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EachCoolingNotCached",
                                                 coolingParam->GetElementName(), coolingParam->GetLineNumber() );

        mCoolingFactory->CreateInstance( coolingParam );
    }

    // CoolingBlocks is used for defining cooling blocks, if necessary with count loop
    vector< shared_ptr< xmlparser::XmlParameter > > coolingBlocksNode =
     paramXmlRoot->GetElementChildren( "CoolingBlocks" );
    BOOST_FOREACH ( const shared_ptr< xmlparser::XmlParameter > &coolingBlockParam, coolingBlocksNode )
    {
        const T dX = coolingBlockParam->GetElementAttributeDoubleValue( "dx", 0.0 );
        const T dY = coolingBlockParam->GetElementAttributeDoubleValue( "dy", 0.0 );
        const T dZ = coolingBlockParam->GetElementAttributeDoubleValue( "dz", 0.0 );

        const size_t count = factory::CheckPositiveInt( coolingBlockParam->GetElementAttributeIntValue( "count", 1 ),
                                                        coolingBlockParam, "count" );
        for ( size_t i = 0; i < count; ++i )
        {
            factory::ArgumentTypeCoolingBlock< T > arg;
            arg.mX = dX * i;
            arg.mY = dY * i;
            arg.mZ = dZ * i;
            coolingBlocks.push_back( mCoolingBlockFactory->CreateInstance( coolingBlockParam, &arg ) );
        }
    }

    // Start of recursive parsing of RootElements element
    list< shared_ptr< ThermalBlock< T > > > heatedBlocksList;
    list< shared_ptr< ThermalBlock< T > > > unheatedBlocksList;
    list< shared_ptr< CoolingBlock< T > > > coolingBlocksList;
    size_t thermalStatesIndex = 0;
    ParseRootElement( paramXmlRoot->GetElementChild( "RootElement" ), geometry::Cartesian< T >( 0.0, 0.0, 0.0 ), heatedBlocksList,
                      unheatedBlocksList, coolingBlocksList, thermalStatesIndex, thermalStates, thermalStatesOfCellBlocks );
    if ( thermalStates != 0 )
        if ( thermalStatesIndex != thermalStates->size() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "MoreThermalStatesPassedThanTheNumberOfStates" );

    heatedBlocks.insert( heatedBlocks.end(), heatedBlocksList.begin(), heatedBlocksList.end() );
    unheatedBlocks.insert( unheatedBlocks.end(), unheatedBlocksList.begin(), unheatedBlocksList.end() );
    coolingBlocks.insert( coolingBlocks.end(), coolingBlocksList.begin(), coolingBlocksList.end() );

    // Create Thermal Probes
    // thermalProbes
    // vector<shared_ptr< xmlparser::XmlParameter > > materials = paramXmlRoot->GetElementChildren("ThermalMaterials");
    CreateThermalProbe( paramXmlRoot.get(), thermalProbes );
}

template < typename T >
void ThermalFactory< T >::ParseRootElement( const shared_ptr< xmlparser::XmlParameter > &param, const Cartesian< T > &origin,
                                            list< shared_ptr< ThermalBlock< T > > > &heatedBlocks,
                                            list< shared_ptr< ThermalBlock< T > > > &unheatedBlocks,
                                            list< shared_ptr< CoolingBlock< T > > > &coolingBlocks, size_t &thermalStatesIndex,
                                            const vector< shared_ptr< ::state::ThermalState< T > > > *thermalStates,
                                            vector< vector< shared_ptr< ThermalState< T > > > > *thermalStatesOfCellBlocks ) const
{
    const char *classname = param->GetElementAttribute( "class" );
    misc::equal_str equals;
    const bool hasParamChildren = param->HasElement( "Children" );

    // Recursive parsing:
    // If param is serial or parallel twoport, ParseRootElement is called with subelements of Children element
    // If param is cell or ohmic resistance, thermal blocks and else will ne created accordingly
    // else check if syntax error
    if ( equals( classname, "CellElement" ) || equals( classname, "OhmicResistance" ) )
    {
        // New ThermalBlock instances with connection to the electrical part of the model are created here
        if ( param->HasElement( "ThermalBlock" ) )
        {
            // To pass information into factories that can not be read in by evaluating xml-file, but that is created
            // during run time, ArgumentType is used
            factory::ArgumentTypeThermalBlock< T > arg;
            arg.mX = origin.GetX();
            arg.mY = origin.GetY();
            arg.mZ = origin.GetZ();

            if ( thermalStates )
            {
                if ( equals( classname, "OhmicResistance" ) )
                    CheckIfOhmicResistanceWithThermalBlockIsValid( param );

                if ( thermalStatesIndex >= thermalStates->size() )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                         "LessThermalStatesPassedThanTheNumberOfStates" );
                const size_t numberOfCellElements =
                 factory::GetElectricalDiscretization( param->GetElementChild( "ThermalBlock" ) );
                arg.mThermalStates.insert( arg.mThermalStates.end(), thermalStates->begin() + thermalStatesIndex,
                                           thermalStates->begin() + thermalStatesIndex + numberOfCellElements );
                thermalStatesIndex += numberOfCellElements;
            }
            else
                arg.mThermalStates.push_back( boost::shared_ptr< ::state::ThermalState< T > >(new ::state::ThermalState< T >));

            heatedBlocks.push_back( mBlockFactory->CreateInstance( param->GetElementChild( "ThermalBlock" ), &arg ) );
            if ( equals( classname, "CellElement" ) && thermalStatesOfCellBlocks )
                thermalStatesOfCellBlocks->push_back( arg.mThermalStates );
        }
        else
        {
            if ( equals( classname, "CellElement" ) )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                     "CellElementWithNoConnectionToTheThermalPart",
                                                     param->GetElementName(), param->GetLineNumber() );
        }
    }
    else if ( hasParamChildren && ( equals( classname, "ParallelTwoPort" ) || equals( classname, "SerialTwoPort" ) ) )
    {
        const shared_ptr< xmlparser::XmlParameter > paramChildren = param->GetElementChild( "Children" );
        const vector< shared_ptr< xmlparser::XmlParameter > > children = param->GetElementChildren( "Children" );
        const vector< shared_ptr< xmlparser::XmlParameter > > unreferencedChildren =
         param->GetUnreferencedElementChildren( "Children" );

        const size_t count =
         factory::CheckPositiveInt( paramChildren->GetElementAttributeIntValue( "count", 1 ), paramChildren, "count" );
        const T countX = paramChildren->GetElementAttributeDoubleValue( "dx", 0.0 );
        const T countY = paramChildren->GetElementAttributeDoubleValue( "dy", 0.0 );
        const T countZ = paramChildren->GetElementAttributeDoubleValue( "dz", 0.0 );


        // Branching of parsing over the whole xml-file by recursive calls of ParseRootElement
        for ( size_t i = 0; i < count; ++i )
        {
            for ( size_t j = 0; j < children.size(); ++j )
            {
                const Cartesian< T > deltaOrigin(
                 countX * i + unreferencedChildren[j]->GetElementAttributeDoubleValue( "dx", 0.0 ),
                 countY * i + unreferencedChildren[j]->GetElementAttributeDoubleValue( "dy", 0.0 ),
                 countZ * i + unreferencedChildren[j]->GetElementAttributeDoubleValue( "dz", 0.0 ) );
                ParseRootElement( children[j], origin + deltaOrigin, heatedBlocks, unheatedBlocks, coolingBlocks,
                                  thermalStatesIndex, thermalStates, thermalStatesOfCellBlocks );
            }
        }
    }
    else
    {
        if ( param->HasElement( "ThermalBlock" ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SyntaxErrorXmlNodeThermalBlock",
                                                 param->GetElementName(), param->GetLineNumber() );
        if ( param->HasElement( "AdditionalBlocks" ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SyntaxErrorXmlNodeAdditionalBlocks",
                                                 param->GetElementName(), param->GetLineNumber() );
        if ( param->HasElement( "CoolingBlocks" ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SyntaxErrorXmlNodeCoolingBlocks",
                                                 param->GetElementName(), param->GetLineNumber() );
    }

    // New ThermalBlock instances without connection to the electrical part of the model are created here
    if ( param->HasElement( "AdditionalBlocks" ) )
    {
        const shared_ptr< xmlparser::XmlParameter > paramUnheatedBlocks = param->GetElementChild( "AdditionalBlocks" );
        const vector< shared_ptr< xmlparser::XmlParameter > > unheatedBlocksChildren =
         param->GetElementChildren( "AdditionalBlocks" );
        const vector< shared_ptr< xmlparser::XmlParameter > > unheatedBlocksUnreferencedChildren =
         param->GetUnreferencedElementChildren( "AdditionalBlocks" );

        if ( !unheatedBlocksChildren.empty() )
        {
            for ( size_t j = 0; j < unheatedBlocksChildren.size(); ++j )
            {
                factory::ArgumentTypeThermalBlock< T > arg;
                arg.mX = origin.GetX() + unheatedBlocksUnreferencedChildren[j]->GetElementAttributeDoubleValue( "dx", 0.0 );
                arg.mY = origin.GetY() + unheatedBlocksUnreferencedChildren[j]->GetElementAttributeDoubleValue( "dy", 0.0 );
                arg.mZ = origin.GetZ() + unheatedBlocksUnreferencedChildren[j]->GetElementAttributeDoubleValue( "dz", 0.0 );

                unheatedBlocks.push_back( mBlockFactory->CreateInstance( unheatedBlocksChildren[j], &arg ) );
            }
        }
    }

    // New Cooling instances attached to count loops of root element are created here
    if ( param->HasElement( "CoolingBlocks" ) )
    {
        const shared_ptr< xmlparser::XmlParameter > paramCoolingBlocks = param->GetElementChild( "CoolingBlocks" );
        const vector< shared_ptr< xmlparser::XmlParameter > > coolingBlocksChildren =
         param->GetElementChildren( "CoolingBlocks" );
        const vector< shared_ptr< xmlparser::XmlParameter > > coolingBlocksUnreferencedChildren =
         param->GetUnreferencedElementChildren( "CoolingBlocks" );

        if ( !coolingBlocksChildren.empty() )
        {
            for ( size_t j = 0; j < coolingBlocksChildren.size(); ++j )
            {
                factory::ArgumentTypeCoolingBlock< T > arg;
                arg.mX = origin.GetX() + coolingBlocksUnreferencedChildren[j]->GetElementAttributeDoubleValue( "dx", 0.0 );
                arg.mY = origin.GetY() + coolingBlocksUnreferencedChildren[j]->GetElementAttributeDoubleValue( "dy", 0.0 );
                arg.mZ = origin.GetZ() + coolingBlocksUnreferencedChildren[j]->GetElementAttributeDoubleValue( "dz", 0.0 );

                coolingBlocks.push_back( mCoolingBlockFactory->CreateInstance( coolingBlocksChildren[j], &arg ) );
            }
        }
    }
}

template < typename T >
void ThermalFactory< T >::CreateThermalProbe( const xmlparser::XmlParameter *param, vector< ::probe::ThermalProbe > *thermalProbes ) const
{
    if ( !param || !thermalProbes )    // Nothing to do here
        return;

    const size_t DIMENSIONS = 3;

    if ( param->HasElement( "ThermalProbe" ) )
    {
        boost::shared_ptr< xmlparser::XmlParameter > thermalPosition = param->GetElementChild( "ThermalProbe" );
        if ( thermalPosition && thermalPosition->HasElement( "Position" ) )
        {
            std::vector< std::vector< double > > positionData =
             thermalPosition->GetElementDoubleVecVecValue( "Position" );
            for ( size_t i = 0; i < positionData.size(); ++i )
            {
                if ( positionData.at( i ).size() != DIMENSIONS )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ProbePosition", i );
                thermalProbes->push_back( positionData[i] );
            }
        }
    }
}

template < typename T >
void ThermalFactory< T >::CheckIfOhmicResistanceWithThermalBlockIsValid( const shared_ptr< xmlparser::XmlParameter > &param ) const
{
    misc::equal_str equals;

    // Ohmic resistance must have a connection to electrical and thermal model.
    // This is only possible by having a ThermalState.
    // SoC does not make any sense for ann ohmic resistance, so there must be exactly one state which is a TheramlState.
    // The only Object with exactly one state is LookupObj1dWithState
    shared_ptr< xmlparser::XmlParameter > paramObject = param->GetElementChild( "Object" );
    if ( !equals( paramObject->GetElementAttribute( "class" ), "LookupObj1dWithState" ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNonClassLookupObj1dWithState",
                                             paramObject->GetElementName(), paramObject->GetLineNumber() );

    shared_ptr< xmlparser::XmlParameter > stateObject = paramObject->GetElementChild( "State" );

    if ( !equals( stateObject->GetElementAttribute( "class" ), "ThermalState" ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "StateNonClassThermalState",
                                             stateObject->GetElementName(), stateObject->GetLineNumber() );

    // ThermalState must not be cached or cachrefed, otherwise parsing would give wrong results
    if ( stateObject->HasElementAttribute( "cache" ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "StateHasCacheAttribute",
                                             stateObject->GetElementName(), stateObject->GetLineNumber() );

    if ( stateObject->HasElementAttribute( "cacheref" ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "StateHasCacherefAttribute",
                                             stateObject->GetElementName(), stateObject->GetLineNumber() );

    // ThermalBlock must not have <ElectricalDiscretization>, because this is only valid for cells
    if ( param->GetElementChild( "ThermalBlock" )->HasElement( "ElectricalDiscretization" ) )
        ErrorFunction< std::runtime_error >(
         __FUNCTION__, __LINE__, __FILE__, "ElectricalDiscretizationNotValidForOhmicResistances",
         param->GetElementChild( "ThermalBlock" )->GetElementChild( "ElectricalDiscretization" )->GetElementName(),
         param->GetElementChild( "ThermalBlock" )->GetElementChild( "ElectricalDiscretization" )->GetLineNumber() );
}

template < typename T >
misc::StrCont ThermalFactory< T >::ParameterDescription( const shared_ptr< xmlparser::XmlParameter > &param ) const
{
    return misc::StrCont( "xml-file line " ) + misc::StrCont( param->GetLineNumber() );
}
}
#endif
