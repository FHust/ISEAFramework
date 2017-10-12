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
* File Name : TestRmphn.cpp
* Creation Date : 29-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestRmphn.h"
#include "../../electrical/rmphn.h"
#include "../../electrical/parallelrc.h"

#include "../../factory/electrical/electricalfactorybuilder.h"
#include "../../factory/object/objectfactorybuilder.h"
#include "../../factory/state/statefactorybuilder.h"

#include "../../object/const_obj.h"

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

extern template class electrical::Rmphn< myMatrixType >;
extern template class electrical::ParallelRC< myMatrixType >;
extern template class systm::StateSystemGroup< myMatrixType >;
extern template class object::Object< double >;
extern template class object::ConstObj< double >;
extern template class factory::Factory< ::state::Dgl_state, ::factory::ArgumentTypeState >;
extern template class factory::Factory< ::object::Object< double >, ::factory::ArgumentTypeObject< double > >;
extern template class factory::Factory< ::electrical::TwoPort< myMatrixType >, ::factory::ArgumentTypeElectrical >;


void TestRmphn::testRmphnCreation()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );

    const double cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );

    Rmphn< myMatrixType > rm( r_ct, r_mp );
    auto rc = boost::make_shared< ParallelRC< myMatrixType > >( r_ct, tau_dl, true, rm.mDataStruct );
    rm.AddChild( rc );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    rm.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.zeros( 1, maxSize );
    current( 0, maxSize - 1 ) = cur;
    rm.SetCurrent( current );
    auto voltage = *rm.GetVoltage();
    TS_ASSERT_DELTA( voltage( 0, 0 ), 4.31132, 0.001 );
    TS_ASSERT_EQUALS( voltage.n_rows, 1 );
    TS_ASSERT_EQUALS( voltage.n_cols, 1 );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix()( 0, 0 ), 99, 0.0001 );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemCVector()( 0, 0 ), 99, 0.0001 );
    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector().size(), 0 );
}

// Test the implementation R-RC vs RMPHN
// Both circuits are created and compared so that both circuits show equal values for the system matrix and all other
// matrices
void TestRmphn::testRmphnRCDifference()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );
    const double ohmValue( 0.331132 );

    const double cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );
    auto ohm = boost::make_shared< object::ConstObj< double > >( ohmValue );

    Rmphn< myMatrixType > rm( r_ct, r_mp );
    auto rc1 = boost::make_shared< ParallelRC< myMatrixType > >( r_ct, tau_dl, true, rm.mDataStruct );
    rm.AddChild( rc1 );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    rm.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.zeros( 1, maxSize );
    current( 0, maxSize - 1 ) = cur;
    rm.SetCurrent( current );
    auto voltage = *rm.GetVoltage();

    SerialTwoPort< myMatrixType > ser;
    auto rc = boost::make_shared< ParallelRC< myMatrixType > >( r_ct, tau_dl, true, ser.mDataStruct );
    auto ohmEle = boost::make_shared< OhmicResistance< myMatrixType > >( ohm );
    ser.AddChild( rc );
    ser.AddChild( ohmEle );

    systm::StateSystemGroup< myMatrixType > differenceGroup;
    differenceGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    ser.SetSystem( &differenceGroup );
    differenceGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType currentdiff;
    currentdiff.zeros( 1, maxSize );
    currentdiff( 0, maxSize - 1 ) = cur;
    ser.SetCurrent( currentdiff );
    auto voltagediff = *ser.GetVoltage();
    TS_ASSERT_DELTA( voltagediff( 0, 0 ), voltage( 0, 0 ), 0.0001 );

    auto y = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();
    auto x = differenceGroup.mDglStateSystem.GetEquationSystemAMatrix();
    TS_ASSERT_DELTA( x( 0, 0 ), y( 0, 0 ), 0.0001 );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemCVector()( 0, 0 ),
                     differenceGroup.mDglStateSystem.GetEquationSystemCVector()( 0, 0 ), 0.0001 );

    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector().size(), 0 );
    TS_ASSERT_EQUALS( differenceGroup.mAlgStateSystem.GetEquationSystemAMatrix().size(), 0 );
}

// Same as above, but this time created through the factory
// Test the implementation R-RC vs RMPHN
// Both circuits are created and compared so that both circuits show equal values for the system matrix and all other
// matrices
void TestRmphn::testRmphnFactoryCreation()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );
    const double ohmValue( 0.331132 );

    const double cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );
    auto ohm = boost::make_shared< object::ConstObj< double > >( ohmValue );

    Rmphn< myMatrixType > rm( r_ct, r_mp );
    auto rc1 = boost::make_shared< ParallelRC< myMatrixType > >( r_ct, tau_dl, true, rm.mDataStruct );
    rm.AddChild( rc1 );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    rm.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.zeros( 1, maxSize );
    current( 0, maxSize - 1 ) = cur;
    rm.SetCurrent( current );
    auto voltage = *rm.GetVoltage();


    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );


    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Options/>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement class='Rmphn'>\
                <LookupOhmicResistance class='ConstObj'>\
                    <Value>10</Value>\
                </LookupOhmicResistance>\
                <LookupTau class='ConstObj'>\
                    <Value>1</Value>\
                </LookupTau>\
                <OhmicResistance_RMP class='ConstObj'>\
                    <Value>1</Value>\
                </OhmicResistance_RMP>\
                </RootElement>\
             </Configuration>";


    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto sd = electricalFactory->CreateInstance( rootparam );

    systm::StateSystemGroup< myMatrixType > differenceGroup;
    differenceGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    sd->SetSystem( &differenceGroup );
    differenceGroup.mDglStateSystem.Initialize( maxSize );

    current.zeros( 1, maxSize );
    current( 0, maxSize - 1 ) = cur;
    sd->SetCurrent( current );
    auto voltagediff = *sd->GetVoltage();


    TS_ASSERT_DELTA( voltagediff( 0, 0 ), voltage( 0, 0 ), 0.0001 );

    auto y = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();
    auto x = differenceGroup.mDglStateSystem.GetEquationSystemAMatrix();
    TS_ASSERT_DELTA( x( 0, 0 ), y( 0, 0 ), 0.0001 );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemCVector()( 0, 0 ),
                     differenceGroup.mDglStateSystem.GetEquationSystemCVector()( 0, 0 ), 0.0001 );

    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector().size(), 0 );
    TS_ASSERT_EQUALS( differenceGroup.mAlgStateSystem.GetEquationSystemAMatrix().size(), 0 );
    auto rmphn = boost::static_pointer_cast< electrical::Rmphn< myMatrixType > >( sd );
    TS_ASSERT_EQUALS( rmphn->size(), 1 );
    TS_ASSERT_EQUALS( strcmp( rmphn->at( 0 )->GetName(), "ParallelRC" ), 0 );
}

void TestRmphn::testRmphnFactoryCreationSimplification()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );
    const double ohmValue( 0.331132 );

    const double cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );
    auto ohm = boost::make_shared< object::ConstObj< double > >( ohmValue );

    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );


    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
            <Options>\
                <SampleRate>\
                    0.00001\
                </SampleRate>\
            </Options>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement class='Rmphn'>\
                <LookupOhmicResistance class='ConstObj'>\
                    <Value>10</Value>\
                </LookupOhmicResistance>\
                <LookupTau class='ConstObj'>\
                    <Value>1</Value>\
                </LookupTau>\
                <OhmicResistance_RMP class='ConstObj'>\
                    <Value>1</Value>\
                </OhmicResistance_RMP>\
                </RootElement>\
             </Configuration>";


    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto sd = electricalFactory->CreateInstance( rootparam );

    systm::StateSystemGroup< myMatrixType > differenceGroup;
    differenceGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    sd->SetSystem( &differenceGroup );
    differenceGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.zeros( 1, maxSize );
    current( 0, maxSize - 1 ) = cur;
    sd->SetCurrent( current );
    auto voltagediff = *sd->GetVoltage();


    TS_ASSERT_DELTA( voltagediff( 0, 0 ), ( ohmValue + resValue ) * cur, 0.0001 );

    auto x = differenceGroup.mDglStateSystem.GetEquationSystemAMatrix();
    TS_ASSERT_EQUALS( x.size(), 0 );
    TS_ASSERT_EQUALS( differenceGroup.mDglStateSystem.GetEquationSystemCVector().size(), 0 );

    auto rmphn = boost::static_pointer_cast< electrical::Rmphn< myMatrixType > >( sd );
    TS_ASSERT_EQUALS( rmphn->size(), 1 );
    TS_ASSERT_EQUALS( strcmp( rmphn->at( 0 )->GetName(), "OhmicResistance" ), 0 );
}
