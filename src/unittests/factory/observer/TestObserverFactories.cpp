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
* File Name : TestObserverFactories.cpp
* Creation Date : 10-08-2015
* Last Modified : Fr 04 Sep 2015 18:17:41 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestObserverFactories.h"
#include "../../../factory/observer/observerfactorybuilder.h"
#include "../../../xmlparser/tinyxml2/xmlparserimpl.h"

using namespace factory;
typedef Factory< observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType >, ArgumentTypeObserver > fac_ob;

typedef observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType > FilterT;

void TestObserverFactories::TestObserverFactoryCSVCreation()
{
    boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"CSVFilter\">\
                        <printHeader> 0  </printHeader> \
                        <filename> unittest.csv</filename>\
                    </Filter1>\
                </Observer>\
             </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     parser->GetRoot()->GetElementChildren( "Observer" );
    TS_ASSERT_EQUALS( children.size(), 1 );
    TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
    boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
}

void TestObserverFactories::TestObserverFactorySTDCreation()
{
    boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"StdoutFilter\"/>\
                </Observer>\
             </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     parser->GetRoot()->GetElementChildren( "Observer" );
    TS_ASSERT_EQUALS( children.size(), 1 );
    TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
    boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
}

void TestObserverFactories::TestObserverFactoryDecimationFilterCreation()
{
    boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"DecimateFilter\">\
                        <TimeDelay> 0.1 </TimeDelay> \
                    </Filter1>\
                </Observer>\
             </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     parser->GetRoot()->GetElementChildren( "Observer" );
    TS_ASSERT_EQUALS( children.size(), 1 );
    TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
    boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
}

void TestObserverFactories::TestObserverFactoryMatlabFilterCreation()
{
    boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"MatlabFilter\">\
                        <filename> unittest.mat </filename> \
                    </Filter1>\
                </Observer>\
             </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     parser->GetRoot()->GetElementChildren( "Observer" );
    TS_ASSERT_EQUALS( children.size(), 1 );
    TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
    boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
}
