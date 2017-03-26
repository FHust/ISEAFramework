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
* File Name : TestXML.cpp
* Creation Date : 21-02-2014
* Last Modified : Di 16 Feb 2016 15:37:41 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestXML.h"

// STD
#include <cstring>

// BOOST
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

// ETC
#include "../../cstring/strlcat.h"
#include "../../misc/matrixInclude.h"

#include "../../states/soc.h"
#include "../../states/thermal_state.h"

#include "../../system/stateSystemGroup.h"
#include "../../system/dae_sys.h"

#include "../../electrical/zarc.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/capacity.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/paralleltwoport.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/warburgTanh.h"
#include "../../electrical/warburgCotanh.h"

#include "../../object/const_obj.h"
#include "../../object/lookup_obj1d.h"
#include "../../object/lookup_obj2d.h"

#include "../../factory/electrical/electricalfactorybuilder.h"
#include "../../factory/object/objectfactorybuilder.h"
#include "../../factory/state/statefactorybuilder.h"

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

static const double sDelta = 0.000001;
using namespace ::state;


void TestXML::TestXmlGetAttributeDoubleGetUnrefrencedChild()
{
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromFile( "./TestXmlParser.xml" );
    boost::shared_ptr< xmlparser::XmlParameter > param( parser.GetRoot() );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     param->GetUnreferencedElementChildren( "TestGetElementUnreferencedChild" );

    TS_ASSERT( children.at( 0 )->HasElementAttribute( "ref" ) );
    TS_ASSERT( children.at( 0 )->HasElementAttribute( "anAttribute" ) );
    TS_ASSERT_DELTA( children.at( 0 )->GetElementAttributeDoubleValue( "anAttribute", 0.0 ), 0.1, sDelta );
    TS_ASSERT_EQUALS( children.at( 0 )->HasElementAttribute( "anotherAttribute" ), false );
    TS_ASSERT_DELTA( children.at( 0 )->GetElementAttributeDoubleValue( "anotherAttribute", 0.0 ), 0.0, sDelta );

    TS_ASSERT( children.at( 1 )->HasElementAttribute( "ref" ) );
    TS_ASSERT( children.at( 1 )->HasElementAttribute( "anotherAttribute" ) );
    TS_ASSERT_DELTA( children.at( 1 )->GetElementAttributeDoubleValue( "anotherAttribute", 0.0 ), -3.1, sDelta );
    TS_ASSERT_EQUALS( children.at( 1 )->HasElementAttribute( "anAttribute" ), false );
    TS_ASSERT_DELTA( children.at( 1 )->GetElementAttributeDoubleValue( "anAttribute", 0.0 ), 0.0, sDelta );

    TS_ASSERT( strcmp( children.at( 0 )->GetElementName(), "ElementWithRef1" ) == 0 );
    TS_ASSERT( strcmp( children.at( 1 )->GetElementName(), "ElementWithRef2" ) == 0 );
}

template < typename T >
bool TestXML::CheckTwoPortTypeAndValue( electrical::TwoPort< T >* testedTwoPort, const char* expectedType, double expectedValue )
{
    if ( strcmp( testedTwoPort->GetName(), expectedType ) == 0 )
    {
        if ( std::abs( static_cast< electrical::ElectricalElement< T >* >( testedTwoPort )->operator()()->GetValue() -
                       expectedValue ) > 0.0001 )    // toDo Pruefung verbessern
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

void TestXML::testXMLCacheRefInstance()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement class='ParallelTwoPort'>\
                   <Children>\
                       <MyCapacity class='Capacity' cache='TRUE'>\
                          <Object class='ConstObj'>\
                             <Value>10</Value>\
                          </Object>\
                       </MyCapacity>\
                       <NormaleCapacity cacheref='MyCapacity'/>\
                       <IrgendeineCapacity cacheref='MyCapacity'/>\
                       <MeineCapacity cacheref='MyCapacity'/>\
                   </Children>\
                </RootElement>\
             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    boost::shared_ptr< electrical::ParallelTwoPort< myMatrixType > > pport(
     boost::static_pointer_cast< electrical::ParallelTwoPort< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );

    TS_ASSERT_EQUALS( pport->at( 0 ), pport->at( 1 ) );
}


void TestXML::testXMLTestConfigfile()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );


    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromFile( "./testconfig.xml" );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::TwoPort< myMatrixType > > rootport( electricalFactory->CreateInstance( rootparam ) );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootport->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();

    double Current = 2.05;
    rootport->SetInitialCurrent( Current );
    rootport->UpdateStateSystemGroup();

    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemAMatrix().n_cols, 109 )
    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().n_rows +
                       stateSystemGroup.mAlgStateSystem.GetEquationSystemAMatrix().n_rows,
                      109 )
}


void TestXML::testXMLReferencedCapacitance()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <Configuration>\
           <CustomDefinitions>\
               <MyCapacity class='Capacity' observable='TRUE'>\
                   <Object class='ConstObj'>\
                         <Value>10</Value>\
                   </Object>\
               </MyCapacity>\
           </CustomDefinitions>\
           <RootElement ref='MyCapacity'/>\
        </Configuration>";

    boost::shared_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::TwoPort< myMatrixType > > pport( electricalFactory->CreateInstance( rootparam ) );
}


void TestXML::testXMLParallelPortChildren()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <Configuration>\
           <CustomDefinitions>\
               <MyCapacity class='Capacity'>\
                   <Object class='ConstObj'>\
                      <Value>10</Value>\
                   </Object>\
               </MyCapacity>\
            </CustomDefinitions>\
            <RootElement class='ParallelTwoPort'>\
               <Children>\
                   <CoolCapacity ref='MyCapacity'/>\
                   <NormaleCapacity ref='MyCapacity'/>\
                   <IrgendeineCapacity ref='MyCapacity'/>\
                   <MeineCapacity ref='MyCapacity'/>\
               </Children>\
            </RootElement>\
         </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::ParallelTwoPort< myMatrixType > > pport(
     boost::static_pointer_cast< electrical::ParallelTwoPort< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );

    TS_ASSERT_EQUALS( pport->size(), 4 );
    TS_ASSERT_EQUALS( pport->GetParallelChildren(), 4 );

#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS_NOTHING( pport->at( 0 ) );
    TS_ASSERT_THROWS_NOTHING( pport->at( 1 ) );
    TS_ASSERT_THROWS_NOTHING( pport->at( 2 ) );
    TS_ASSERT_THROWS_NOTHING( pport->at( 3 ) );
#endif

    TS_ASSERT_THROWS( pport->at( 5 ), std::out_of_range );
}


void TestXML::testXMLElectricalElementOhmicConst()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <ConstObj class='ConstObj'>\
            <Value>10.0</Value>\
        </ConstObj>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );
    boost::shared_ptr< object::Object< double > > obj( objectFactory->CreateInstance( rootparam.get() ) );

    double Resistance( 10.0 );

    electrical::OhmicResistance< myMatrixType > test( obj );
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    test.SetSystem( &stateSystemGroup );

#ifndef _SYMBOLIC_
    test.SetInitialCurrent( 10 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 10 * Resistance );
#else
    test.SetInitialCurrent( symbolic::Symbolic( "InputCurrent" ) );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), symbolic::Symbolic( "MUL(InputCurrent,ID0_ObjBase)" ) );
#endif
}


void TestXML::testXMLElectricalElementOhmic1DLookup()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <LookupObj1D class='LookupObj1D'>\
            <LookupData>0.1,1.2,9.3,1.4,5,6,7,8,9</LookupData>\
            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
        </LookupObj1D>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );
    boost::shared_ptr< object::Object< double > > obj( objectFactory->CreateInstance( rootparam.get() ) );

    electrical::OhmicResistance< myMatrixType > test( obj );
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    test.SetSystem( &stateSystemGroup );

#ifndef _SYMBOLIC_
    test.SetInitialCurrent( 10 );
    test()->GetValue( 1.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 10 * 0.1 );

    test()->GetValue( 2.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 10 * 1.2, 0.000001 );

    test()->GetValue( 8.5 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 10 * 8.5 );
#else
    test.SetInitialCurrent( symbolic::Symbolic( "InputCurrent" ) );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), symbolic::Symbolic( "MUL(InputCurrent,ID0_ObjBase)" ) );
#endif
}

void TestXML::testXMLParallelRCTestSimplificationFastRCToOhmicResistance()
{
    // ParallelRC reduced to R by Heuristic
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
               <SampleRate> 0.333333  </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyParallelRC class='ParallelRC' >\n\
                    <LookupTau class='ConstObj'>\n\
                        <Value> 1 </Value>\n\
                    </LookupTau>\n\
                    <LookupOhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </LookupOhmicResistance>\n\
                </MyParallelRC>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyParallelRC'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::ElectricalElement< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::ElectricalElement< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );

    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport.get() ),
                                         "OhmicResistance", 5.000 ) );

    // ParallelRC NOT Reduced Heuristic
    const char* xmlConfig2 =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
               <SampleRate> 0.333334  </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyParallelRC class='ParallelRC' >\n\
                    <LookupTau class='ConstObj'>\n\
                        <Value> 1 </Value>\n\
                    </LookupTau>\n\
                    <LookupOhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </LookupOhmicResistance>\n\
                </MyParallelRC>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyParallelRC'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig2 );

    rootparam = parser->GetRoot()->GetElementChild( "RootElement" );
    rootport =
     boost::static_pointer_cast< electrical::ElectricalElement< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) );

    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport.get() ),
                                         "ParallelRC", 5.000 ) );

    // ParallelRC NOT Reduced Euler
    const char* xmlConfig3 =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
               <SampleRate SimplificationType='Euler'> 0.500001  </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyParallelRC class='ParallelRC' >\n\
                    <LookupTau class='ConstObj'>\n\
                        <Value> 1.0001 </Value>\n\
                    </LookupTau>\n\
                    <LookupOhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </LookupOhmicResistance>\n\
                </MyParallelRC>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyParallelRC'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig3 );

    rootparam = parser->GetRoot()->GetElementChild( "RootElement" );
    rootport =
     boost::static_pointer_cast< electrical::ElectricalElement< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) );

    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport.get() ),
                                         "ParallelRC", 5.000 ) );

    // ParallelRC reduced to R by Euler
    const char* xmlConfig4 =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
               <SampleRate SimplificationType='Euler'> 0.499999  </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyParallelRC class='ParallelRC' >\n\
                    <LookupTau class='ConstObj'>\n\
                        <Value> 1 </Value>\n\
                    </LookupTau>\n\
                    <LookupOhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </LookupOhmicResistance>\n\
                </MyParallelRC>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyParallelRC'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig4 );

    rootparam = parser->GetRoot()->GetElementChild( "RootElement" );
    rootport =
     boost::static_pointer_cast< electrical::ElectricalElement< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) );

    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport.get() ),
                                         "OhmicResistance", 5.000 ) );
}


void TestXML::testXMLWarburgTestWrongSampleRate()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgTanh class='WarburgTanh' RCCounter='10' >\n\
                    <Tau class='ConstObj'>\n\
                        <Value> 10 </Value>\n\
                    </Tau>\n\
                    <OhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </OhmicResistance>\n\
                </MyWarburgTanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgTanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS( boost::shared_ptr< electrical::WarburgTanh< myMatrixType > > rootport(
                       boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >(
                        electricalFactory->CreateInstance( rootparam ) ) ),
                      std::range_error );
#endif
}

void TestXML::testXMLWarburgCotanhTestSimplificationOfRCImpedance()
{
    // Warburg Element is simplified after the SECOND ParallelRC element
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0.592176 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' withCapacity='true'>\n\
                    <Clim class='ConstObj'>\n\
                        <Value> 1 </Value>\n\
                    </Clim>\n\
                    <Sigma class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgCotanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgCotanh" ), 0 );
    TS_ASSERT_EQUALS( rootport->size(), 4 );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 0 ) ),
                                         "ParallelRC", 10.1321 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 1 ) ),
                                         "ParallelRC", 2.5330 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 2 ) ),
                                         "OhmicResistance", 3.0372 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( rootport->size() - 1 ) ),
                                         "Capacity", 1.0000 ) );

    // Warburg Element is simplified after the THIRD ParallelRC element
    const char* xmlConfig2 =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0.592177 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' withCapacity= 'true'>\n\
                    <Clim class='ConstObj'>\n\
                        <Value> 1 </Value>\n\
                    </Clim>\n\
                    <Sigma class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig2 );

    rootparam = parser->GetRoot()->GetElementChild( "RootElement" );
    rootport =
     boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) );

    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgCotanh" ), 0 );
    TS_ASSERT_EQUALS( rootport->size(), 5 );    // One more RC-Element than before
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 0 ) ),
                                         "ParallelRC", 10.1321 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 1 ) ),
                                         "ParallelRC", 2.5330 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 2 ) ),
                                         "ParallelRC", 1.1257 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 3 ) ),
                                         "OhmicResistance", 1.9114 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( rootport->size() - 1 ) ),
                                         "Capacity", 1.0000 ) );
}

void TestXML::testXMLWarburgCotanhTestWithAndWithoutCapacity()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0.592176 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' withCapacity='true'>\n\
                    <Clim class='ConstObj'>\n\
                        <Value> 1 </Value>\n\
                    </Clim>\n\
                    <Sigma class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgCotanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgCotanh" ), 0 );
    TS_ASSERT_EQUALS( rootport->size(), 4 );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( rootport->size() - 1 ) ),
                                         "Capacity", 1.0000 ) );

    const char* xmlConfig2 =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0.592177 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' withCapacity= 'false'>\n\
                    <Clim class='ConstObj'>\n\
                        <Value> 1 </Value>\n\
                    </Clim>\n\
                    <Sigma class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig2 );

    rootparam = parser->GetRoot()->GetElementChild( "RootElement" );
    rootport =
     boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) );

    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgCotanh" ), 0 );
    TS_ASSERT_EQUALS( rootport->size(), 4 );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( rootport->size() - 1 ) ),
                                         "OhmicResistance", 1.9114 ) );
}

void TestXML::testXMLWarburgTanhTestSimplificationOfRCImpedance()
{
    // Warburg Element is simplified after the SECOND ParallelRC element
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0.833333 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgTanh class='WarburgTanh' RCCounter='10' >\n\
                    <Tau class='ConstObj'>\n\
                        <Value> 10 </Value>\n\
                    </Tau>\n\
                    <OhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </OhmicResistance>\n\
                </MyWarburgTanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgTanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgTanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgTanh" ), 0 );
    TS_ASSERT_EQUALS( rootport->size(), 3 );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 0 ) ),
                                         "ParallelRC", 5.0000 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 1 ) ),
                                         "ParallelRC", 0.5556 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 2 ) ),
                                         "OhmicResistance", 0.4880 ) );


    // Warburg Element is simplified after the THIRD ParallelRC element
    const char* xmlConfig2 =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0.833334 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgTanh class='WarburgTanh' RCCounter='10' >\n\
                    <Tau class='ConstObj'>\n\
                        <Value> 10 </Value>\n\
                    </Tau>\n\
                    <OhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </OhmicResistance>\n\
                </MyWarburgTanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgTanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";
    parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig2 );

    rootparam = parser->GetRoot()->GetElementChild( "RootElement" );
    rootport =
     boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) );

    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgTanh" ), 0 );
    TS_ASSERT_EQUALS( rootport->size(), 4 );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 0 ) ),
                                         "ParallelRC", 5.0000 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 1 ) ),
                                         "ParallelRC", 0.5556 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 2 ) ),
                                         "ParallelRC", 0.2000 ) );
    TS_ASSERT( CheckTwoPortTypeAndValue( static_cast< electrical::TwoPort< myMatrixType >* >( rootport->at( 3 ) ),
                                         "OhmicResistance", 0.2880 ) );
}


void TestXML::testXMLWarburgTestReductionOfOhmicResistance()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0.001 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgTanh class='WarburgTanh' RCCounter='10' >\n\
                    <Tau class='ConstObj'>\n\
                        <Value> 10 </Value>\n\
                    </Tau>\n\
                    <OhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </OhmicResistance>\n\
                </MyWarburgTanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgTanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgTanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgTanh" ), 0 );
    TS_ASSERT_EQUALS( rootport->size(), 1 );

#ifndef _SYMBOLIC_
    // R changes values
    TS_ASSERT_DELTA( static_cast< electrical::OhmicResistance< myMatrixType >* >( rootport->at( 0 ) )->GetValue(), 6.0436, 0.0001 )
#else
    TS_ASSERT_DELTA( static_cast< electrical::OhmicResistance< myMatrixType >* >( rootport->at( 0 ) )->operator()()->GetValue(),
                     6.0436, 0.0001 );
    rootport->at( 0 )->SetID( 9 );
    TS_ASSERT_EQUALS( static_cast< electrical::OhmicResistance< myMatrixType >* >( rootport->at( 0 ) )->GetValue(),
                      symbolic::Symbolic( "ID9_ObjBase" ) );
#endif
}
void TestXML::testXMLWarburgTestCreationWithFullRCImplementation()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <CustomDefinitions>\n\
                <MyWarburgTanh class='WarburgTanh' RCCounter='10' >\n\
                    <Tau class='ConstObj'>\n\
                        <Value> 10 </Value>\n\
                    </Tau>\n\
                    <OhmicResistance class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </OhmicResistance>\n\
                </MyWarburgTanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgTanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgTanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgTanh" ), 0 );

    // R changes values
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 0 ) )->operator()()->GetValue(), 5.0, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 1 ) )->operator()()->GetValue(),
                     0.5556, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 2 ) )->operator()()->GetValue(),
                     0.2000, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 3 ) )->operator()()->GetValue(),
                     0.1020, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 4 ) )->operator()()->GetValue(),
                     0.0617, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 5 ) )->operator()()->GetValue(),
                     0.0413, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 6 ) )->operator()()->GetValue(),
                     0.0296, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 7 ) )->operator()()->GetValue(),
                     0.0222, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 8 ) )->operator()()->GetValue(),
                     0.0173, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 9 ) )->operator()()->GetValue(),
                     0.0139, 0.0001 )

    // Capacity stays the same
    for ( size_t i = 0; i < rootport->size(); ++i )
    {
#ifndef _SYMBOLIC_
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->GetValueC(), 2.0, 0.0001 )
#else
        double capacity =
         static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->GetTauObject()->GetValue() /
         static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->operator()()->GetValue();
        TS_ASSERT_DELTA( capacity, 2.0, 0.0001 );
#endif
    }
}

void TestXML::testXMLWarburgCothTestCreationWithFullRCImplementation()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' >\n\
                    <Clim class='ConstObj'>\n\
                        <Value> 10 </Value>\n\
                    </Clim>\n\
                    <Sigma class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgCotanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgCotanh" ), 0 );


    // R changes values
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 0 ) )->operator()()->GetValue(),
                     101.3212, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 1 ) )->operator()()->GetValue(),
                     25.3303, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 2 ) )->operator()()->GetValue(),
                     11.2579, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 3 ) )->operator()()->GetValue(),
                     6.3326, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 4 ) )->operator()()->GetValue(),
                     4.0528, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 5 ) )->operator()()->GetValue(),
                     2.8145, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 6 ) )->operator()()->GetValue(),
                     2.0678, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 7 ) )->operator()()->GetValue(),
                     1.5831, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 8 ) )->operator()()->GetValue(),
                     1.2509, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 9 ) )->operator()()->GetValue(),
                     1.0132, 0.0001 )

    // Capacity stays the same
    for ( size_t i = 0; i < rootport->size() - 1; ++i )
    {
#ifndef _SYMBOLIC_
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->GetValueC(), 5.0, 0.0001 )
#else
        double capacity =
         static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->GetTauObject()->GetValue() /
         static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->operator()()->GetValue();
        TS_ASSERT_DELTA( capacity, 5.0, 0.0001 );
#endif
    }

    TS_ASSERT_DELTA( static_cast< electrical::Capacity< myMatrixType >* >( rootport->at( rootport->size() - 1 ) )->operator()()->GetValue(),
                     10.0, 0.0001 )
}

void TestXML::testXMLWarburgCothTestCreationWithFullRCImplementationAndLookup1D()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' >\n\
                    <Clim class='LookupObj1D'>\n\
                            <LookupData>10,10,10</LookupData>\n\
                            <MeasurementPoints>1,2,3</MeasurementPoints>\n\
                    </Clim>\n\
                    <Sigma class='LookupObj1D'>\n\
                            <LookupData>5,5,5</LookupData>\n\
                            <MeasurementPoints>1,2,3</MeasurementPoints>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgCotanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgCotanh" ), 0 );

    for ( size_t row = 0; row < 3; ++row )
    {
        for ( size_t i = 0; i < rootport->size() - 1; ++i )
        {
            electrical::ParallelRC< myMatrixType >* paralrc =
             static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) );
            ( *paralrc->mObject.get() )( row );
            ( *paralrc->mObjectTau.get() )( row );
        }

        // R changes values
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 0 ) )->operator()()->GetValue(),
                         101.3212, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 1 ) )->operator()()->GetValue(),
                         25.3303, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 2 ) )->operator()()->GetValue(),
                         11.2579, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 3 ) )->operator()()->GetValue(),
                         6.3326, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 4 ) )->operator()()->GetValue(),
                         4.0528, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 5 ) )->operator()()->GetValue(),
                         2.8145, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 6 ) )->operator()()->GetValue(),
                         2.0678, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 7 ) )->operator()()->GetValue(),
                         1.5831, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 8 ) )->operator()()->GetValue(),
                         1.2509, 0.0001 );
        TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 9 ) )->operator()()->GetValue(),
                         1.0132, 0.0001 );

        // Capacity stays the same
        for ( size_t i = 0; i < rootport->size() - 1; ++i )
        {
#ifndef _SYMBOLIC_
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->GetValueC(), 5.0, 0.0001 );
#else
            double capacity =
             static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->GetTauObject()->GetValue() /
             static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->operator()()->GetValue();
            TS_ASSERT_DELTA( capacity, 5.0, 0.0001 );
#endif
        }

        electrical::Capacity< myMatrixType >* cap =
         static_cast< electrical::Capacity< myMatrixType >* >( rootport->at( ( rootport->size() - 1 ) ) );
        ( *cap->mObject.get() )( row );
        TS_ASSERT_DELTA( static_cast< electrical::Capacity< myMatrixType >* >( rootport->at( rootport->size() - 1 ) )->operator()()->GetValue(),
                         10.0, 0.0001 );
    }
}

void TestXML::testXMLWarburgCothTestCreationWithFullRCImplementationAndLookup2D()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' >\n\
                    <Clim class='LookupObj2D'>\n\
                            <LookupData>10,10,10; 10,10,10; 10,10,10;</LookupData>\n\
                            <MeasurementPointsRow>1,2,3</MeasurementPointsRow>\n\
                            <MeasurementPointsColumn>1,2,3</MeasurementPointsColumn>\n\
                    </Clim>\n\
                    <Sigma class='LookupObj2D'>\n\
                            <LookupData>5,5,5; 5,5,5; 5,5,5;</LookupData>\n\
                            <MeasurementPointsRow>1,2,3</MeasurementPointsRow>\n\
                            <MeasurementPointsColumn>1,2,3</MeasurementPointsColumn>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgCotanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgCotanh" ), 0 );

    // R changes values
    for ( size_t row = 0; row < 3; ++row )
    {
        for ( size_t col = 0; col < 3; ++col )
        {
            for ( size_t i = 0; i < rootport->size() - 1; ++i )
            {
                electrical::ParallelRC< myMatrixType >* paralrc =
                 static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) );
                ( *paralrc->mObject.get() )( row, col );
                ( *paralrc->mObjectTau.get() )( row, col );
            }

            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 0 ) )->operator()()->GetValue(),
                             101.3212, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 1 ) )->operator()()->GetValue(),
                             25.3303, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 2 ) )->operator()()->GetValue(),
                             11.2579, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 3 ) )->operator()()->GetValue(),
                             6.3326, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 4 ) )->operator()()->GetValue(),
                             4.0528, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 5 ) )->operator()()->GetValue(),
                             2.8145, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 6 ) )->operator()()->GetValue(),
                             2.0678, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 7 ) )->operator()()->GetValue(),
                             1.5831, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 8 ) )->operator()()->GetValue(),
                             1.2509, 0.0001 );
            TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( 9 ) )->operator()()->GetValue(),
                             1.0132, 0.0001 );

            // Capacity stays the same
            for ( size_t i = 0; i < rootport->size() - 1; ++i )
            {
#ifndef _SYMBOLIC_
                TS_ASSERT_DELTA( static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->GetValueC(),
                                 5.0, 0.0001 );
#else
                double capacity =
                 static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->GetTauObject()->GetValue() /
                 static_cast< electrical::ParallelRC< myMatrixType >* >( rootport->at( i ) )->operator()()->GetValue();
                TS_ASSERT_DELTA( capacity, 5.0, 0.0001 );
#endif
            }

            electrical::Capacity< myMatrixType >* cap =
             static_cast< electrical::Capacity< myMatrixType >* >( rootport->at( ( rootport->size() - 1 ) ) );
            ( *cap->mObject.get() )( row, col );
            TS_ASSERT_DELTA( cap->operator()()->GetValue(), 10.0, 0.0001 )
        }
    }
}

void TestXML::testXMLWarburgCotanhTestWrongSampleRate()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' >\n\
                    <Clim class='ConstObj'>\n\
                        <Value> 10 </Value>\n\
                    </Clim>\n\
                    <Sigma class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS( boost::shared_ptr< electrical::WarburgTanh< myMatrixType > > rootport(
                       boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >(
                        electricalFactory->CreateInstance( rootparam ) ) ),
                      std::range_error );
#endif
}


void TestXML::testXMLWarburgCotanhTestReductionOfOhmicResistance()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
        <Configuration>\n\
            <Options>\n\
                <SampleRate> 0.0001 </SampleRate>\n\
            </Options>\n\
            <CustomDefinitions>\n\
                <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' >\n\
                    <Clim class='ConstObj'>\n\
                        <Value> 10 </Value>\n\
                    </Clim>\n\
                    <Sigma class='ConstObj'>\n\
                        <Value> 5 </Value>\n\
                    </Sigma>\n\
                </MyWarburgCotanh>\n\
            </CustomDefinitions>\n\
            <RootElement ref='MyWarburgCotanh'>\n\
            </RootElement>\n\
        </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    boost::shared_ptr< electrical::WarburgCotanh< myMatrixType > > rootport(
     boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    TS_ASSERT_EQUALS( strcmp( rootport->GetName(), "WarburgCotanh" ), 0 );
    TS_ASSERT_EQUALS( rootport->size(), 2 );
    TS_ASSERT_DELTA( static_cast< electrical::OhmicResistance< myMatrixType >* >( rootport->at( 0 ) )->operator()()->GetValue(),
                     157.0243, 0.0001 )
    TS_ASSERT_DELTA( static_cast< electrical::Capacity< myMatrixType >* >( rootport->at( rootport->size() - 1 ) )->operator()()->GetValue(),
                     10.0, 0.0001 )
}

void TestXML::testXMLElectricalElementOhmic2DLookup()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <LookupObj2D class='LookupObj2D'>\
            <LookupData>1,2,3;4,5,6;7,8,9</LookupData>\
            <MeasurementPointsRow>1,2,3</MeasurementPointsRow>\
            <MeasurementPointsColumn>1,2,3</MeasurementPointsColumn>\
        </LookupObj2D>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );
    boost::shared_ptr< object::Object< double > > obj( objectFactory->CreateInstance( rootparam.get() ) );

    electrical::OhmicResistance< myMatrixType > test( obj );
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    test.SetSystem( &stateSystemGroup );

#ifndef _SYMBOLIC_
    test.SetInitialCurrent( 10 );
    test()->GetValue( 1.0, 1.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 10 * 1 );

    test()->GetValue( 2.0, 2.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 10 * 5.0 );

    test()->GetValue( 3.0, 3.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 10 * 9.0 );
#else
    test.SetInitialCurrent( symbolic::Symbolic( "InputCurrent" ) );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), symbolic::Symbolic( "MUL(InputCurrent,ID0_ObjBase)" ) );
#endif
}


void TestXML::testXMLTemperatureOperation()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <ThermalState class='ThermalState'>\
            <InitialTemperature>23</InitialTemperature>\
        </ThermalState>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );
    boost::shared_ptr< ::state::ThermalState< double > > temp(
     boost::static_pointer_cast< ::state::ThermalState< double > >( stateFactory->CreateInstance( rootparam.get() ) ) );


    double initValue = 23;
    TS_ASSERT_EQUALS( temp->GetMaxDt(), std::numeric_limits< double >::max() );
    TS_ASSERT_DELTA( temp->GetValue(), initValue, 0.00001 );
}

void TestXML::testXMLSocOperations()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <Soc class='Soc'>\
            <MaxCapacity>20.0</MaxCapacity>\
            <InitialSoc>20.0</InitialSoc>\
            <MeasurementPoints>0,10,20,30,40,50,60,70,80,90,100</MeasurementPoints>\
        </Soc>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );
    boost::shared_ptr< electrical::state::Soc > test(
     boost::static_pointer_cast< electrical::state::Soc >( stateFactory->CreateInstance( rootparam.get() ) ) );

    double capacity = 20.0;
    double status = 20.0;

    TS_ASSERT_DELTA( test->GetMaxCapacity(), capacity * 3600, 0.000001 );
    TS_ASSERT_DELTA( test->GetValue(), status, 0.00001 );
    TS_ASSERT_DELTA( test->mCurCapAsec, status / 100 * capacity * 3600, 0.000001 );
    TS_ASSERT_EQUALS( test->GetMaxDt(), std::numeric_limits< double >::max() );
    TS_ASSERT_DELTA( test->mMaxCapAsec, capacity * 3600, 0.000001 );
}


void TestXML::testXMLZarcOperations()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfigStates1 =
     "<?xml version='1.0'?>\
                                    <ThermalState class='ThermalState' cache='True'>\
                                        <InitialTemperature>23</InitialTemperature>\
                                    </ThermalState>";
    const char* xmlConfigStates2 =
     "<?xml version='1.0'?>\
                                    <Soc class='Soc' cache='True'>\
                                        <MaxCapacity>10</MaxCapacity>\
                                        <InitialSoc>0.5</InitialSoc>\
                                        <MeasurementPoints>5,20,35,50,65,80,90</MeasurementPoints>\
                                    </Soc>";

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <ZarcElement class='ZarcElement' observable='TRUE'>\
            <LookupTau class='LookupObj2DWithState'>\
                <RowState cacheref='ThermalState'/>\
                <ColState cacheref='Soc'/>\
                <LookupData>4.26350000000000,6.48126887772831,16.1467219609302,15.6907043658766;4.26350000000000,6.48126887772831,5.94555138829343,13.3175628164387;3.35856856831061,4.53787945479736,3.55833143789644,9.81728112438815;3.09044922378664,3.79068242726983,2.92804813631953,9.49272124208671;3.01244052134718,3.50252466372123,2.58610303596176,21.9157985151821;2.86053605856753,3.23033256582487,2.73479778432361,29.8433328330744;2.80722454459597,3.18179127562604,3.19495950786132,36.8431967500101</LookupData>\
                <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
            </LookupTau>\
            <LookupOhmicResistance class='LookupObj2DWithState'>\
                <RowState cacheref='ThermalState'/>\
                <ColState cacheref='Soc'/>\
                <LookupData>0.0264994082415437,0.0264994082415437,0.0264994082415437,0.0299913094135788;0.0675569999999998,0.0313318160740531,0.0127731516995969,0.00404164451684630;0.0536386688114087,0.0248200664672378,0.00989750001969683,0.00303781463624295;0.0478600636580172,0.0219992555610513,0.00877924658179266,0.00234391473354453;0.0441171340633137,0.0206405039153971,0.00794555485936455,0.00454415109298745;0.0418986584966986,0.0203278573267527,0.00866505912064747,0.00344848953405180;0.0421316495020664,0.0207584663601899,0.0102011087727835,0.00296383805146105</LookupData>\
                <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
            </LookupOhmicResistance>\
            <LookupPhi class='LookupObj2DWithState'>\
                <RowState cacheref='ThermalState'/>\
                <ColState cacheref='Soc'/>\
                <LookupData>0.574590000000000,0.516891694022437,0.408379079339744,0.110443124808620;0.574590000000000,0.516891694022437,0.568073405078093,0.566435038531618;0.616859078283881,0.579111165549142,0.662374262351899,0.620812008168125;0.625502091112064,0.610693210867254,0.697319797270455,0.671812046180353;0.621586176902636,0.620972340223097,0.721715387830978,0.323239015612757;0.620607929391110,0.630419920219851,0.671002168217511,0.371661290927877;0.620534462894923,0.628465004550853,0.585312180039373,0.410302314031108</LookupData>\
                <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
            </LookupPhi>\
        </ZarcElement>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfigStates1 );
    stateFactory->CreateInstance( parser->GetRoot() );
    parser->ReadFromMem( xmlConfigStates2 );
    stateFactory->CreateInstance( parser->GetRoot() );

    parser->ReadFromMem( xmlConfig );
    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );
    boost::shared_ptr< electrical::Zarc< myMatrixType > > zarc(
     boost::static_pointer_cast< electrical::Zarc< myMatrixType > >( electricalFactory->CreateInstance( rootparam.get() ) ) );

    TS_ASSERT_EQUALS( zarc->CanHaveChildren(), false );
    TS_ASSERT_EQUALS( zarc->HasChildren(), false );
    TS_ASSERT_EQUALS( zarc->HasSystem(), false );
    TS_ASSERT_DELTA( ( *stateFactory->GetObjectsOfClass( "ThermalState" ).at( 0 ) )(), 23.0, 0.00001 );
};

void TestXML::testXMLCellelementOperations()
{
    /*
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory(
    factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory<object::Object<double>, factory::ArgumentTypeObject<double> > > objectFactory(
    factory::BuildObjectFactory<double>( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory<electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > >
    electricalFactory( factory::BuildElectricalFactory< myMatrixType, double>( objectFactory.get(), stateFactory.get() )
    );

    const char *xmlConfig = "<?xml version='1.0'?>\
        <Configuration>\
        <CellElement class='CellElement' observable='TRUE'>\
            <ThermalState class='ThermalState' cache='True'>\
                <InitialTemperature>23</InitialTemperature>\
            </ThermalState>\
            <Soc class='Soc' cache='True'>\
                <MaxCapacity>2.05</MaxCapacity>\
                <InitialSoc>20</InitialSoc>\
                <MeasurementPoints>5,10,20,23,35,50,65,80,90</MeasurementPoints>\
            </Soc>\
            <Children>\
            <OhmicResistance class='OhmicResistance'>\
               <Object class='LookupObj2dWithState'>\
                  <RowState cacheref='ThermalState'/>\
                  <ColState cacheref='Soc'/>\
                  <LookupData desc='Rser'>\
                0.0288200000000001, 0.0273753907300395, 0.0264344729259093, 0.0135127660821939;\
                0.0288200000000001, 0.0273753907300395, 0.0270767085789135, 0.0275280515729565;\
                0.0290500142586083, 0.0277525443197526, 0.0275261486868454, 0.0276368633915343;\
                0.0288630838514731, 0.0278576890117617, 0.0275537618404411, 0.0277933657755191;\
                0.0286781206123875, 0.0277738617773271, 0.0275453220226692, 0.0259269199477185;\
                0.0285331061111646, 0.0277867915477187, 0.0268358840093433, 0.0267808653711796;\
                0.0286358289536196, 0.0277803384908296, 0.0257328031445100, 0.0272959924289106\
                  </LookupData>\
                  <MeasurementPointsRow desc='MeasureTemp'>0,10,23,50</MeasurementPointsRow>\
                  <MeasurementPointsColumn desc='StateOfCharge'>5,20,35,50,65,80,90</MeasurementPointsColumn>\
               </Object>\
            </OhmicResistance>\
            <VoltageSource class='VoltageSource'>\
               <Object class='LookupObj2dWithState'>\
                  <RowState cacheref='Soc'/>\
                  <ColState cacheref='ThermalState'/>\
                  <LookupData desc='OCV'>\
                3.47980625000000, 3.56020000000000, 3.61080000000000, 3.64033750000000, 3.67903750000000,
    3.74791250000000, 3.84935000000000, 3.94420000000000, 4.05327500000000, 4.18230000000000;\
                3.48386250000000, 3.56396250000000, 3.61388750000000, 3.64350000000000, 3.68183750000000,
    3.75231250000000, 3.85146875000000, 3.94563750000000, 4.05550000000000, 4.18410000000000;\
                3.48390000000000, 3.56456250000000, 3.61843125000000, 3.64930000000000, 3.68808750000000,
    3.76500000000000, 3.85450000000000, 3.94680000000000, 4.05477500000000, 4.17770000000000;\
                3.47570000000000, 3.55060000000000, 3.60896250000000, 3.64975000000000, 3.68677500000000,
    3.75358750000000, 3.84280000000000, 3.93626250000000, 4.04295000000000, 4.16480000000000 \
                  </LookupData>\
                  <MeasurementPointsRow desc='StateOfChargeOCV'>10,20,30,40,50,60,70,80,90,100</MeasurementPointsRow>\
                  <MeasurementPointsColumn desc='MeasureTemp'>0,10,23,50</MeasurementPointsColumn>\
               </Object>\
            </VoltageSource>\
            <ZarcElement class='ZarcElement'>\
                <LookupTau class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    1.12980227037822e-01, 1.71749789914134e-01, 4.27878577005388e-01, 4.70584769553997e-01;\
                    2.88029269499999e-01, 2.03069924403468e-01, 7.59434298204209e-02, 5.38248547348156e-02;\
                    1.80149147116020e-01, 1.12630469688383e-01, 3.52185854766679e-02, 2.98230802877780e-02;\
                    1.47909096582298e-01, 8.33921914682953e-02, 2.57060565921076e-02, 2.22501291807582e-02;\
                    1.32900242338032e-01, 7.22938740353130e-02, 2.05480235442034e-02, 9.95886997764575e-02;\
                    1.19852623435413e-01, 6.56657395160509e-02, 2.36971844841798e-02, 1.02914420936082e-01;\
                    1.18273000586515e-01, 6.60491071602289e-02, 3.25921294643322e-02, 1.09197268465146e-01 \
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupTau>\
                <LookupOhmicResistance class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788;\
                    0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630;\
                    0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295;\
                    0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453;\
                    0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745;\
                    0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180;\
                    0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupOhmicResistance>\
                <LookupPhi class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>0.574590000000000,0.516891694022437,0.408379079339744,0.110443124808620;0.574590000000000,0.516891694022437,0.568073405078093,0.566435038531618;0.616859078283881,0.579111165549142,0.662374262351899,0.620812008168125;0.625502091112064,0.610693210867254,0.697319797270455,0.671812046180353;0.621586176902636,0.620972340223097,0.721715387830978,0.323239015612757;0.620607929391110,0.630419920219851,0.671002168217511,0.371661290927877;0.620534462894923,0.628465004550853,0.585312180039373,0.410302314031108</LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupPhi>\
            </ZarcElement>\
            <ZarcElement class='ZarcElement'>\
                <LookupTau class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788;\
                    0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630;\
                    0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295;\
                    0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453;\
                    0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745;\
                    0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180;\
                    0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupTau>\
                <LookupOhmicResistance class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                        0.0422632965521782,0.0422632965521782,0.0422632965521782,0.0646154311756893;\
                        0.0122909999999999,0.00820305593966980,0.00584662021670523,0.00383743519303631;\
                        0.0322225070656886,0.0289438148360227,0.0225495908462913,0.00599634893454420;\
                        0.0257481760483287,0.0186632656230284,0.00993329376244036,0.00582987109816202;\
                        0.0157426548912461,0.0171684201770746,0.0134953332473813,0.00457332312382527;\
                        0.0218280618476754,0.0131225853688649,0.00741527720123879,0.00329020951414971;\
                        0.0153521701301020,0.00846967601820038,0.00563955480281332,0.00315825717209717\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupOhmicResistance>\
                <LookupPhi class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    1,0.957683980942010,0.762474540690386,0.810166837787303;\
                    1,0.957683980942010,0.837851589303327,0.826206919812180;\
                    0.806534226878276,0.755415801951905,0.630228467428149,0.686969202518359;\
                    0.708526377255388,0.661628608281274,0.630894213874104,0.675415879938948;\
                    0.791964147404381,0.697353484425094,0.628110619592649,0.801166605326404;\
                    0.759343341643463,0.720390317831926,0.755733232495091,0.938336421996388;\
                    0.790468798628622,0.797350100822233,0.852903199927494,0.986742576117900\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupPhi>\
            </ZarcElement>\
            </Children>\
        </CellElement>\
        <Options>\
            <SampleRate> 100000 </SampleRate>\
        </Options>\
        </Configuration>";

    boost::scoped_ptr<xmlparser::XmlParser> parser(new xmlparser::tinyxml2::XmlParserImpl());
    parser->ReadFromMem(xmlConfig);

    boost::shared_ptr<xmlparser::XmlParameter> rootparam( parser->GetRoot() );
    boost::shared_ptr< electrical::TwoPort< myMatrixType > > roottp(
    electricalFactory->CreateInstance(rootparam->GetElementChild("CellElement") ) ) ;
    boost::shared_ptr<electrical::state::Soc> socState(
    boost::static_pointer_cast<electrical::state::Soc>(stateFactory->GetObjectsOfClass("Soc").at(0)) );

    double t = 0.000;
    double dt = 100;
    double Current = 2.05;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    roottp->SetSystem(&stateSystemGroup);
    stateSystemGroup.Initialize();
    roottp->SetInitialCurrent(Current);
    roottp->UpdateStateSystemGroup();
    stateSystemGroup.mDt = dt;

    systm::DifferentialAlgebraicSystem< myMatrixType > test(&stateSystemGroup);

    double initialoldstate[] = {0, 0, 0, 0, 0, 0};
    for(size_t i = 0; i < 6; ++i)
    {
        TS_ASSERT_DELTA( stateSystemGroup.mStateVector(i,0), initialoldstate[i], 0.001);
    }

    TS_ASSERT_DELTA(socState->GetCapacity(), 1476, 0.001);

    myMatrixType dxdt = myMatrixType(6,1);
    roottp->UpdateStateSystemGroup();
    test.ODEifyEquations();

    myMatrixType &x = stateSystemGroup.mStateVector;
    test.operator()(x, dxdt, t);
    x += dxdt * dt;
    roottp->CalculateStateDependentValues();

    double intialdxdt[] = {0.0053, 0.1648, 1.4069, 0.0238, 0.6528, 0.8343};
    double newoldstate[] = {0.5333, 16.4840, 140.6924, 2.3802, 65.2889, 83.4350};
    for (size_t i = 0; i < 6; ++i)
    {
        TS_ASSERT_DELTA(dxdt(i,0), intialdxdt[i], 0.001);
        TS_ASSERT_DELTA(x(i,0), newoldstate[i], 0.001);
    }

    TS_ASSERT_DELTA(socState->GetCapacity(), 1681, 0.001);
    */
}

void TestXML::testNegativeSampleRateForZarc()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char* xmlConfig =
     "<?xml version='1.0'?>\
        <Configuration>\
        <CellElement class='CellElement' observable='TRUE'>\
            <ThermalState class='ThermalState' cache='True'>\
                <InitialTemperature>23</InitialTemperature>\
            </ThermalState>\
            <Soc class='Soc' cache='True'>\
                <MaxCapacity>2.05</MaxCapacity>\
                <InitialSoc>20</InitialSoc>\
                <MeasurementPoints>5,10,20,23,35,50,65,80,90</MeasurementPoints>\
            </Soc>\
            <Children>\
            <ZarcElement class='ZarcElement'>\
                <LookupTau class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    1.12980227037822e-01, 1.71749789914134e-01, 4.27878577005388e-01, 4.70584769553997e-01;\
                    2.88029269499999e-01, 2.03069924403468e-01, 7.59434298204209e-02, 5.38248547348156e-02;\
                    1.80149147116020e-01, 1.12630469688383e-01, 3.52185854766679e-02, 2.98230802877780e-02;\
                    1.47909096582298e-01, 8.33921914682953e-02, 2.57060565921076e-02, 2.22501291807582e-02;\
                    1.32900242338032e-01, 7.22938740353130e-02, 2.05480235442034e-02, 9.95886997764575e-02;\
                    1.19852623435413e-01, 6.56657395160509e-02, 2.36971844841798e-02, 1.02914420936082e-01;\
                    1.18273000586515e-01, 6.60491071602289e-02, 3.25921294643322e-02, 1.09197268465146e-01 \
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupTau>\
                <LookupOhmicResistance class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788;\
                    0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630;\
                    0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295;\
                    0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453;\
                    0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745;\
                    0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180;\
                    0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupOhmicResistance>\
                <LookupPhi class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>0.574590000000000,0.516891694022437,0.408379079339744,0.110443124808620;0.574590000000000,0.516891694022437,0.568073405078093,0.566435038531618;0.616859078283881,0.579111165549142,0.662374262351899,0.620812008168125;0.625502091112064,0.610693210867254,0.697319797270455,0.671812046180353;0.621586176902636,0.620972340223097,0.721715387830978,0.323239015612757;0.620607929391110,0.630419920219851,0.671002168217511,0.371661290927877;0.620534462894923,0.628465004550853,0.585312180039373,0.410302314031108</LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupPhi>\
            </ZarcElement>\
            </Children>\
        </CellElement>\
        <Options>\
            <SampleRate> -100000 </SampleRate>\
        </Options>\
        </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );

    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam->GetElementChild( "CellElement" ) ), std::range_error );
}
