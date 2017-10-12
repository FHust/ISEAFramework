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
* File Name : TestElectricalFactory.cpp
* Creation Date : 09-03-2016
* Last Modified : Fr 11 Mär 2016 10:55:35 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestElectricalFactory.h"

std::string TestElectricalFactory::CreateConfigurationForTwoPort() const
{
    return "<?xml version='1.0'?>\
        <LookupObj1D class='LookupObj1dWithState'>\
            <State class='ElectricalState' StateType='Current' />\
            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
        </LookupObj1D>";
}

std::string TestElectricalFactory::CreateConfigurationForTwoPort( const std::string &twoPortName, const std::string & /*objName*/ ) const
{
    return "<?xml version='1.0'?>\
            <Configuration>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement  class='" +
           twoPortName +
           "' >\
                        <Object class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                        </Object>\
                </RootElement>\
             </Configuration>";
}

void TestElectricalFactory::TestStateInjection1Lookup() const
{
    this->StatePropagationTest1< myMatrixType, electrical::OhmicResistance >();
    this->StatePropagationTest1< myMatrixType, electrical::Capacity >();
    this->StatePropagationTest1< myMatrixType, electrical::VoltageSource >();
    this->StatePropagationTest1< myMatrixType, electrical::Inductance >();
    this->StatePropagationTest1< myMatrixType, electrical::ConstantPhaseElement >();
}

void TestElectricalFactory::TestStateInjectionForParallelRCLookup() const
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

    boost::shared_ptr< object::Object< double > > obj1( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );
    boost::shared_ptr< object::Object< double > > obj2( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );

    electrical::ParallelRC< myMatrixType > test( obj1, obj2, true, elecData );

    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        elecData->mCurrentValue = current;
        TS_ASSERT_DELTA( test.GetCurrentValue(), current, 0.000001 );
        auto valueR = test.mObject->GetValue();
        auto valueTau = test.mObjectTau->GetValue();
        TS_ASSERT_DELTA( valueR, current, 0.000001 );
        TS_ASSERT_DELTA( valueTau, current, 0.000001 );
    }
}

void TestElectricalFactory::TestStateInjectionForZarcLookup() const
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

    boost::shared_ptr< object::Object< double > > obj1( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );
    boost::shared_ptr< object::Object< double > > obj2( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );
    boost::shared_ptr< object::Object< double > > obj3( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );

    electrical::Zarc< myMatrixType > test( obj1, obj2, obj3, true, 0.0001, elecData );

    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        elecData->mCurrentValue = current;
        TS_ASSERT_DELTA( test.GetCurrentValue(), current, 0.000001 );
        auto valueR = test.mLookupOhmicResistance->GetValue();
        auto valueTau = test.mLookupRelaxationTime->GetValue();
        auto valuePhi = test.mLookupPhi->GetValue();
        TS_ASSERT_DELTA( valueR, current, 0.000001 );
        TS_ASSERT_DELTA( valueTau, current, 0.000001 );
        TS_ASSERT_DELTA( valuePhi, current, 0.000001 );
    }
}

void TestElectricalFactory::TestFactoryInjection1Lookup() const
{
    std::vector< std::string > twoPortVectorName{"OhmicResistance", "Capacity", "VoltageSource", "Inductance",
                                                 "ConstantPhaseElement"};

    for ( size_t j = 0; j < twoPortVectorName.size(); ++j )
    {
        boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory(
         factory::BuildStateFactory() );
        boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
         factory::BuildObjectFactory< double >( stateFactory.get() ) );
        boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
         factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

        std::string xmlConfig = this->CreateConfigurationForTwoPort( twoPortVectorName[j], "" );
        boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromMem( xmlConfig.c_str() );

        boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

        TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

        auto twoPort( boost::static_pointer_cast< electrical::ElectricalElement< myMatrixType > >(
         electricalFactory->CreateInstance( rootparam ) ) );
        for ( size_t i = 1; i < 10; ++i )
        {
            auto current = static_cast< double >( i );
            twoPort->mDataStruct->mCurrentValue = current;

            TS_ASSERT_DELTA( twoPort->GetCurrentValue(), current, 0.000001 );
            auto value = twoPort->mObject->GetValue();
            TS_ASSERT_DELTA( value, current, 0.000001 );
        }
    }
}

void TestElectricalFactory::TestFactoryInjection2Lookup() const
{
    std::string twoPortVectorName( "ParallelRC" );
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    std::string xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement  class='ParallelRC' >\
                        <LookupTau class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                        </LookupTau >\
                        <LookupOhmicResistance class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                    </LookupOhmicResistance>\n\
                </RootElement>\
             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto twoPort( boost::static_pointer_cast< electrical::ParallelRC< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        twoPort->mDataStruct->mCurrentValue = current;

        TS_ASSERT_DELTA( twoPort->GetCurrentValue(), current, 0.000001 );
        auto valueR = twoPort->mObject->GetValue();
        auto valueTau = twoPort->mObjectTau->GetValue();
        TS_ASSERT_DELTA( valueR, current, 0.000001 );
        TS_ASSERT_DELTA( valueTau, current, 0.000001 );
    }
}


void TestElectricalFactory::TestFactroyInjectionForWarburgCotanh() const
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    std::string xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
            <Option>\
            <Samplerate>100000000</Samplerate>\
            </Option>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement  class='WarburgCotanh' >\
                        <Sigma class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                        </Sigma >\
                        <Clim class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                    </Clim>\n\
                </RootElement>\
             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto twoPort( boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >(
     electricalFactory->CreateInstance( rootparam ) ) );

    std::vector< double > lastRValues( twoPort->mChildren.size() - 1 );
    std::vector< double > lastTauValues( twoPort->mChildren.size() - 1 );
    double lastCValue = 0;
    for ( size_t i = 1; i < 10; ++i )
    {

        auto current = static_cast< double >( i );
        twoPort->mDataStruct->mCurrentValue = current;
        for ( size_t j = 0; j < twoPort->mChildren.size() - 1; ++j )
        {
            TS_ASSERT_DELTA( twoPort->mChildren.at( j )->GetCurrentValue(), 0.0, 0.000001 );
            auto child = boost::static_pointer_cast< electrical::ParallelRC< myMatrixType > >( twoPort->mChildren.at( j ) );
            auto valueR = child->mObject->GetValue();
            auto valueTau = child->mObjectTau->GetValue();
            TS_ASSERT( valueR > lastRValues[j] );
            TS_ASSERT( valueTau > lastTauValues[j] );
            lastRValues[j] = valueR;
            lastTauValues[j] = valueTau;
        }
        auto child = boost::static_pointer_cast< electrical::Capacity< myMatrixType > >(
         twoPort->mChildren.at( twoPort->mChildren.size() - 1 ) );

        auto valueC = child->mObject->GetValue();
        TS_ASSERT( valueC > lastCValue );
        lastCValue = valueC;
    }
}

void TestElectricalFactory::TestFactroyInjectionForWarburgTanh() const
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    std::string xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
            <Option>\
            <Samplerate>100000000</Samplerate>\
            </Option>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement  class='WarburgTanh' >\
                        <OhmicResistance class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                        </OhmicResistance >\
                        <Tau class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                    </Tau>\n\
                </RootElement>\
             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto twoPort( boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );

    std::vector< double > lastRValues( twoPort->mChildren.size() );
    std::vector< double > lastTauValues( twoPort->mChildren.size() );
    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        twoPort->mDataStruct->mCurrentValue = current;
        for ( size_t j = 0; j < twoPort->mChildren.size(); ++j )
        {
            TS_ASSERT_DELTA( twoPort->mChildren.at( j )->GetCurrentValue(), 0.0, 0.000001 );
            auto child = boost::static_pointer_cast< electrical::ParallelRC< myMatrixType > >( twoPort->mChildren.at( j ) );
            auto valueR = child->mObject->GetValue();
            auto valueTau = child->mObjectTau->GetValue();
            TS_ASSERT( valueR > lastRValues[j] );
            TS_ASSERT( valueTau > lastTauValues[j] );
            lastRValues[j] = valueR;
            lastTauValues[j] = valueTau;
        }
    }
}
