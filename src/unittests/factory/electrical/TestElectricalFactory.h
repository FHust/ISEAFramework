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
* File Name : TestElectricalFactory.h
* Creation Date : 09-03-2016
* Last Modified : Fr 11 Mär 2016 10:55:45 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTELECTRICALFACTORY_
#define _TESTELECTRICALFACTORY_
#include <cxxtest/TestSuite.h>

// BOOST
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "../../../states/soc.h"
#include "../../../states/thermal_state.h"

#include "../../../system/stateSystemGroup.h"
#include "../../../system/dae_sys.h"

#include "../../../electrical/inductance.h"
#include "../../../electrical/constantphaseelement.h"
#include "../../../electrical/zarc.h"
#include "../../../electrical/cellelement.h"
#include "../../../electrical/capacity.h"
#include "../../../electrical/parallelrc.h"
#include "../../../electrical/ohmicresistance.h"
#include "../../../electrical/voltagesource.h"
#include "../../../electrical/paralleltwoport.h"
#include "../../../electrical/serialtwoport.h"
#include "../../../electrical/warburgTanh.h"
#include "../../../electrical/warburgCotanh.h"

#include "../../../factory/electrical/electricalfactorybuilder.h"
#include "../../../factory/object/objectfactorybuilder.h"
#include "../../../factory/state/statefactorybuilder.h"

#include "../../../xmlparser/tinyxml2/xmlparserimpl.h"


class TestElectricalFactory : public CxxTest::TestSuite
{
    public:
    template < typename MatrixT, template < typename > class TConcrete >
    void StatePropagationTest1() const;

    void TestStateInjection1Lookup() const;
    void TestStateInjectionForParallelRCLookup() const;
    void TestStateInjectionForZarcLookup() const;

    void TestFactoryInjection1Lookup() const;
    void TestFactoryInjection2Lookup() const;
    void TestFactroyInjectionForWarburgCotanh() const;
    void TestFactroyInjectionForWarburgTanh() const;


    std::string CreateConfigurationForTwoPort() const;
    std::string CreateConfigurationForTwoPort( const std::string &twoPortName, const std::string &objName ) const;

    private:
    protected:
};

template < typename MatrixT, template < typename > class TConcrete >
void TestElectricalFactory::StatePropagationTest1() const
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );

    std::string name = this->CreateConfigurationForTwoPort();

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( name.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );
    boost::scoped_ptr< factory::ArgumentTypeObject< double > > argObject( new factory::ArgumentTypeObject< double > );
    auto elecData = boost::make_shared< ElectricalDataStruct< double > >();
    argObject->mElectricalDataStruct = elecData;
    boost::shared_ptr< object::Object< double > > obj( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );

    TConcrete< MatrixT > test( obj, true, elecData );

    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        elecData->mCurrentValue = current;
        TS_ASSERT_DELTA( test.GetCurrentValue(), current, 0.000001 );
        auto valueR = test.mObject->GetValue();
        TS_ASSERT_DELTA( valueR, current, 0.000001 );
    }
}

#endif /* _TESTELECTRICALFACTORY_ */
