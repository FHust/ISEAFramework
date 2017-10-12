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
* File Name : TestESBVisualization.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 10:47:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestESBVisualization.h"

// STD
#include <fstream>

// BOOST
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

// ETC

#include "../../factory/electrical/electricalfactorybuilder.h"
#include "../../factory/object/objectfactorybuilder.h"
#include "../../factory/state/statefactorybuilder.h"


#include "../../export/esbVisualizer.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

void TestESBVisualization::testXmlNetwork()
{
#ifndef __NO_STRING__
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );


    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromFile( "./testconfig.xml" );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::TwoPort< myMatrixType > > rootport( electricalFactory->CreateInstance( rootparam ) );

    std::ofstream of( "esb.dot" );
    visualizer::EsbVisualizer< myMatrixType > testVisualizer( rootport.get(), &of );
#endif /* __NO_STRING__ */
}
