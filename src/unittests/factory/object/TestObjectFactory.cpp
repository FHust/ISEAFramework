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
* File Name : TestObjectFactory.cpp
* Creation Date : 28-11-2016
* Last Modified : Fr 02 Dez 2016 11:29:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestObjectFactory.h"

#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include "../../../factory/electrical/electricalfactorybuilder.h"
#include "../../../factory/object/objectfactorybuilder.h"
#include "../../../factory/state/statefactorybuilder.h"
#include "../../../factory/electricalEquationSystem/chooseElectricSystemType.h"
#include "../../../xmlparser/tinyxml2/xmlparserimpl.h"
#include "../../../misc/matrixInclude.h"
#include "../../../factory/object/objectclasswrapper.h"


std::string CreateLookup( std::string attribute, std::string count = "1" )
{
    return "\n\n<?xml version='1.0'?>\n"
           "<Object class='MultiObj' Operation='" +
           attribute +
           "'>\n"
           "    <Children count='" +
           count +
           "'>\n"
           "	<c1 class='ConstObj'> <Value> 10</Value></c1>\n"
           "	<c2 class='ConstObj'> <Value> 20</Value></c2>\n"
           "	<c3 class='ConstObj'> <Value> 30</Value></c3>\n"
           "    </Children>\n"
           "</Object>\n";
}

void TestObjectFactories::TestObjectFactoriesCreation()
{
    std::string ops[3] = {"Add", "Divide", "Multiply"};
    std::vector< double > result = {60, 0.016667, 6000};
    boost::shared_ptr< factory::ArgumentTypeObject< double > > nullelement;
    std::vector< boost::shared_ptr< factory::ArgumentTypeObject< double > > > multiplier{
     nullelement, boost::make_shared< factory::ArgumentTypeObject< double > >( 10 ),
     boost::make_shared< factory::ArgumentTypeObject< double > >( 0.10 ),
     boost::make_shared< factory::ArgumentTypeObject< double > >( 0.02 )};


    for ( size_t j = 0; j < multiplier.size(); ++j )
    {

        for ( size_t i = 0; i < 3; ++i )
        {
            auto xml = CreateLookup( ops[i] );

            boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory;
            boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory;
            boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory;
            stateFactory.reset( factory::BuildStateFactory() );
            objectFactory.reset( factory::BuildObjectFactory< double >( stateFactory.get() ) );
            electricalFactory.reset(
             factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

            boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
            parser->ReadFromMem( xml.c_str() );
            auto xmlNode = parser->GetRoot();
            auto object = objectFactory->CreateInstance( xmlNode, multiplier.at( j ).get() );
            if ( multiplier.at( j ).get() == nullptr )
            {
                TS_ASSERT_DELTA( result.at( i ), object->GetValue(), 0.0001 );
            }
            else
            {
                TS_ASSERT_DELTA( multiplier.at( j )->mObjectFactor * result.at( i ), object->GetValue(), 0.0001 );
            }
        }
    }
}
