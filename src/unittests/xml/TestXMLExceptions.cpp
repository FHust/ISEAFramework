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
* File Name : TestXMLExceptions.cpp
* Creation Date : 21-02-2014
* Last Modified : Mi 14 Okt 2015 14:31:31 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestXMLExceptions.h"

#include "../../misc/matrixInclude.h"

// BOOST
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "../../factory/electrical/electricalfactorybuilder.h"
#include "../../factory/object/objectfactorybuilder.h"
#include "../../factory/state/statefactorybuilder.h"

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

using namespace ::state;

void TestXMLExceptions::testXMLParserWrongFilename()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromFile( "blabla" ), xmlparser::ConfigFileNotFoundException );
#endif
}

void TestXMLExceptions::testXMLParserNullFilename()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromFile( 0 ), xmlparser::ConfigFileNotFoundException );
#endif
}

void TestXMLExceptions::testXMLParserWrongMemAddr()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromMem( "blabla" ), xmlparser::CouldNotParseConfigException );
#endif
}

void TestXMLExceptions::testXMLParserNullMemAddr()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromMem( 0 ), xmlparser::ConfigFileNotFoundException );
#endif
}

void TestXMLExceptions::testXMLNotMatchingElements()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char *xmlConfig =
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
                             </Configuration121>";

    boost::shared_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromMem( xmlConfig ), xmlparser::CouldNotParseConfigException );
#endif
}


void TestXMLExceptions::testXMLNoRootElement()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char *xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             <MyCapacity class='Capacity' observable='TRUE'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </MyCapacity>\
                             </CustomDefinitions>\
                             </Configuration>";

    boost::shared_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    TS_ASSERT_THROWS( parser->GetRoot()->GetElementChild( "RootElement" ), xmlparser::ElementNotFoundException );
#endif
}


void TestXMLExceptions::testXMLNoReferencedElement()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char *xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             <MyCapacity class='Capacity' observable='TRUE'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </MyCapacity>\
                             </CustomDefinitions>\
                             <RootElement ref='MyCapacity123'/>\
                             </Configuration>";

    boost::shared_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    TS_ASSERT_THROWS( parser->GetRoot()->GetElementChild( "RootElement" ), xmlparser::ElementNotFoundException );
#endif
}


void TestXMLExceptions::testXMLCacheRefInstance()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char *xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class='ParallelTwoPort'>\
                             <Children>\
                             <MyCapacity class='Capacity' cache='True'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </MyCapacity>\
                             <NormaleCapacity cacheref='MyCapacity'/>\
                             <IrgendeineCapacity cacheref='MyCaca'/>\
                             <MeineCapacity cacheref='MaCici'/>\
                             </Children>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam ), factory::ClassNotCachedException );
#endif
}


void TestXMLExceptions::testXMLClassNotFound()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char *xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class='Capacity123'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam ), factory::ClassNotFoundException );
#endif
}


void TestXMLExceptions::testXMLAttributeNotFound()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char *xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class123='Capacity'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam ), xmlparser::AttributeNotFoundException );
#endif
}


void TestXMLExceptions::testXMLElementContentNotReadable()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    const char *xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class='Capacity'>\
                             <Object class='ConstObj'>\
                             <Value></Value>\
                             </Object>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam ), xmlparser::CouldNotReadElementContentException );
#endif
}

void TestXMLExceptions::testXMLCatchNegativeValue()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );
    const char *xmlConfig =
     "<?xml version='1.0'?>\n\
                            <Configuration>\n\
                            <CustomDefinitions>\n\
                            <MyWarburgTanh class='WarburgTanh' RCCounter='-1' >\n\
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
    TS_ASSERT_THROWS( boost::shared_ptr< electrical::WarburgTanh< myMatrixType > > rootport(
                       boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >(
                        electricalFactory->CreateInstance( rootparam ) ) ),
                      std::runtime_error );
#endif
}
