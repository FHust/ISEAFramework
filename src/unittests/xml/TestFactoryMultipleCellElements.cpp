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
#include "TestFactoryMultipleCellElements.h"
#include "../../factory/get_electrical_discretization.h"
#include "../../factory/electrical/electricalfactorybuilder.h"
#include "../../factory/state/statefactorybuilder.h"
#include "../../factory/object/objectfactorybuilder.h"
#include "../../factory/thermal/thermal_factory.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"
#include <boost/pointer_cast.hpp>

using namespace electrical;
using namespace object;
using namespace factory;
static const double sDelta = 0.000001;



void TestFactoryLookUpCellElements::TestGetElectricalDiscretization()
{
    const char *xmlConfigCart = "<?xml version='1.0'?>\
                                 <Block class=\"RectangularBlock\">\
                                     <ElectricalDiscretization x=\"2\" y =\"3\" z=\"5\"/>\
                                 </Block>";
    const char *xmlConfigCyl = "<?xml version='1.0'?>\
                                <Block class=\"QuadraticCellBlock\">\
                                    <ElectricalDiscretization phi=\"2\" z=\"3\"/>\
                                </Block>";
    const char *xmlConfigEmpty = "<?xml version='1.0'?>\
                                  <Block class=\"RectangularBlock\">\
                                  </Block>";

    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromMem(xmlConfigCart);
    TS_ASSERT_EQUALS(GetElectricalDiscretization(parser.GetRoot()), 30);
    parser.ReadFromMem(xmlConfigCyl);
    TS_ASSERT_EQUALS(GetElectricalDiscretization(parser.GetRoot()), 6);
    parser.ReadFromMem(xmlConfigEmpty);
    TS_ASSERT_EQUALS(GetElectricalDiscretization(parser.GetRoot()), 1);
}

void TestFactoryLookUpCellElements:: TestStateFactory()
{
    boost::scoped_ptr<factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory;
    stateFactory.reset( factory::BuildStateFactory() );

    const char *xmlConfigEmpty = "<State class='Soc'>\
                                      <MaxCapacity>10</MaxCapacity>\
                                      <InitialSoc>0.5</InitialSoc>\
                                      <MeasurementPoints>5,50,90</MeasurementPoints>\
                                  </State>";
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromMem(xmlConfigEmpty);

    factory::ArgumentTypeState arg;
    arg.mCellDiscretization = 3.0;

    boost::shared_ptr< ::state::Dgl_state > state = stateFactory->CreateInstance(parser.GetRoot(), &arg);
#ifdef __EXCEPTIONS__
    electrical::state::Soc *SocState = dynamic_cast<electrical::state::Soc*>(state.get());
    TS_ASSERT(SocState != 0);
#else
    electrical::state::Soc *SocState = static_cast<electrical::state::Soc*>(state.get());
#endif
    TS_ASSERT_DELTA(SocState->GetMaxCapacity(), 10.0 * 3600.0 / 3.0, sDelta);
}

void TestFactoryLookUpCellElements::TestObjectFactory()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory<object::Object<double>, factory::ArgumentTypeObject<double> > > objectFactory( factory::BuildObjectFactory<double>( stateFactory.get() ) );

    const char *xmlConfigThermalState = "<?xml version='1.0'?>\
                                         <ThermalState class=\"ThermalState\" cache=\"True\">\
                                            <InitialTemperature>25</InitialTemperature>\
                                         </ThermalState>";
    const char *xmlConfigSocState = "<?xml version='1.0'?>\
                                     <Soc class=\"Soc\" cache=\"True\">\
                                        <MaxCapacity>10</MaxCapacity>\
                                        <InitialSoc>50</InitialSoc>\
                                        <MeasurementPoints>5,50,90</MeasurementPoints>\
                                    </Soc>";
    const char *xmlConfigConstObj = "<?xml version='1.0'?>\
                                     <Object class='ConstObj'>\
                                         <Value>10.0</Value>\
                                     </Object>";
    const char *xmlConfigLookupObj1D = "<?xml version='1.0'?>\
                                        <Object class='LookupObj1D'>\
                                            <LookupData>10,20,40</LookupData>\
                                            <MeasurementPoints>5,50,90</MeasurementPoints>\
                                        </Object>";
    const char *xmlConfigLookupObj1DWithState = "<?xml version='1.0'?>\
                                        <Object class='LookupObj1DWithState'>\
                                            <State cacheref=\"Soc\"/>\
                                            <LookupData>10,20,40</LookupData>\
                                            <MeasurementPoints>5,50,90</MeasurementPoints>\
                                        </Object>";
    const char *xmlConfigLookupObj2D = "<?xml version='1.0'?>\
                                        <Object class='LookupObj2D'>\
                                                <LookupData>11,21;12,22;13,23</LookupData>\
                                                <MeasurementPointsRow>1,3</MeasurementPointsRow>\
                                                <MeasurementPointsColumn>5,50,90</MeasurementPointsColumn>\
                                        </Object>";
    const char *xmlConfigLookupObj2DWithState = "<?xml version='1.0'?>\
                                                 <Object class=\"LookupObj2dWithState\">\
                                                     <RowState cacheref=\"ThermalState\"/>\
                                                     <ColState cacheref=\"Soc\"/>\
                                                     <LookupData desc=\"Rser\">11,21;12,22;13,23</LookupData>\
                                                     <MeasurementPointsRow desc=\"MeasureTemp\">1,3</MeasurementPointsRow>\
                                                     <MeasurementPointsColumn desc=\"StateOfCharge\">5,50,90</MeasurementPointsColumn>\
                                                 </Object>";

    factory::ArgumentTypeState argState;
    factory::ArgumentTypeObject<double> argObject;
    argObject.mObjectFactor = 4.0;
    argState.mCellDiscretization = 4.0;
    xmlparser::tinyxml2::XmlParserImpl parser;
    const double socFactor = 10.0 / 4.0 * 3600.0 / 100.0;

    boost::shared_ptr< ::state::Dgl_state > state;
    parser.ReadFromMem(xmlConfigThermalState);
    state = stateFactory->CreateInstance(parser.GetRoot(), &argState);
    boost::shared_ptr< ::state::ThermalState<double> > thermalState = boost::static_pointer_cast< ::state::ThermalState<double> >(state);
    parser.ReadFromMem(xmlConfigSocState);
    state = stateFactory->CreateInstance(parser.GetRoot(), &argState);
    boost::shared_ptr< electrical::state::Soc > soc = boost::static_pointer_cast< electrical::state::Soc >(state);
    soc->SetCapacity(socFactor * 5.0);
    TS_ASSERT_DELTA(soc->GetValue(), 5.0, sDelta);

    parser.ReadFromMem(xmlConfigConstObj);
    boost::shared_ptr< Object< double > > constObj = objectFactory->CreateInstance(parser.GetRoot(), &argObject);
    TS_ASSERT_DELTA(constObj->GetValue(), 10.0 * 4.0, sDelta);

    parser.ReadFromMem(xmlConfigLookupObj1D);
    boost::shared_ptr< Object< double > > obj1D = objectFactory->CreateInstance(parser.GetRoot(), &argObject);
    TS_ASSERT_DELTA(obj1D->GetValue(5.0), 10.0 * 4.0, sDelta);
    TS_ASSERT_DELTA(obj1D->GetValue(50.0), 20.0 * 4.0, sDelta);
    TS_ASSERT_DELTA(obj1D->GetValue(90.0), 40.0 * 4.0, sDelta);

    parser.ReadFromMem(xmlConfigLookupObj1DWithState);
    boost::shared_ptr< Object< double > > obj1DWithState = objectFactory->CreateInstance(parser.GetRoot(), &argObject);
    soc->SetCapacity(socFactor * 5.0);
    TS_ASSERT_DELTA(obj1DWithState->GetValue(), 10.0 * 4.0, sDelta);
    soc->SetCapacity(socFactor * 50.0);
    TS_ASSERT_DELTA(obj1DWithState->GetValue(), 20.0 * 4.0, sDelta);
    soc->SetCapacity(socFactor * 90.0);
    TS_ASSERT_DELTA(obj1DWithState->GetValue(), 40.0 * 4.0, sDelta);

    parser.ReadFromMem(xmlConfigLookupObj2D);
    boost::shared_ptr< Object< double > > obj2D = objectFactory->CreateInstance(parser.GetRoot(), &argObject);
    TS_ASSERT_DELTA(obj2D->GetValue(1.0, 5.0), 11.0 * 4.0, sDelta);
    TS_ASSERT_DELTA(obj2D->GetValue(1.0, 50.0), 12.0 * 4.0, sDelta);
    TS_ASSERT_DELTA(obj2D->GetValue(1.0, 90.0), 13.0 * 4.0, sDelta);
    TS_ASSERT_DELTA(obj2D->GetValue(3.0, 5.0), 21.0 * 4.0, sDelta);
    TS_ASSERT_DELTA(obj2D->GetValue(3.0, 50.0), 22.0 * 4.0, sDelta);
    TS_ASSERT_DELTA(obj2D->GetValue(3.0, 90.0), 23.0 * 4.0, sDelta);

    parser.ReadFromMem(xmlConfigLookupObj2DWithState);
    boost::shared_ptr< Object< double > > obj2DWithState = objectFactory->CreateInstance(parser.GetRoot(), &argObject);
    thermalState->ResetTemperature();
    thermalState->AddTemperature(1.0, 1.0);
    soc->SetCapacity(socFactor * 5.0);
    TS_ASSERT_DELTA(obj2DWithState->GetValue(), 11.0 * 4.0, sDelta);
    soc->SetCapacity(socFactor * 50.0);
    TS_ASSERT_DELTA(obj2DWithState->GetValue(), 12.0 * 4.0, sDelta);
    soc->SetCapacity(socFactor * 90.0);
    TS_ASSERT_DELTA(obj2DWithState->GetValue(), 13.0 * 4.0, sDelta);
    thermalState->ResetTemperature();
    thermalState->AddTemperature(3.0, 1.0);
    soc->SetCapacity(socFactor * 5.0);
    TS_ASSERT_DELTA(obj2DWithState->GetValue(), 21.0 * 4.0, sDelta);
    soc->SetCapacity(socFactor * 50.0);
    TS_ASSERT_DELTA(obj2DWithState->GetValue(), 22.0 * 4.0, sDelta);
    soc->SetCapacity(socFactor * 90.0);
    TS_ASSERT_DELTA(obj2DWithState->GetValue(), 23.0 * 4.0, sDelta);
    
}

void TestFactoryLookUpCellElements::TestElectricalFactory()
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory<object::Object<double>, factory::ArgumentTypeObject<double> > > objectFactory( factory::BuildObjectFactory<double>( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory<electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory( factory::BuildElectricalFactory< myMatrixType, double>( objectFactory.get(), stateFactory.get() ) );


    const char *xmlConfigCapacity = "<?xml version='1.0'?>\
                                     <MyCapacity class='Capacity'>\
                                         <Object class='ConstObj'>\
                                             <Value>10.1</Value>\
                                         </Object>\
                                     </MyCapacity>";
    const char *xmlConfigOhmicResistance = "<?xml version='1.0'?>\
                                            <MyOhmicResistance class='OhmicResistance'>\
                                                <Object class='ConstObj'>\
                                                    <Value>1.1</Value>\
                                                </Object>\
                                            </MyOhmicResistance>";
    const char *xmlConfigParallelRC = "<?xml version='1.0'?>\
                                       <myParallelRC class='ParallelRC'>\
                                           <LookupOhmicResistance class='ConstObj'>\
                                               <Value>1.1</Value>\
                                           </LookupOhmicResistance>\
                                           <LookupTau class='ConstObj'>\
                                               <Value>0.1</Value>\
                                           </LookupTau>\
                                       </myParallelRC>";
    /* 
    const char *xmlConfigParallelRCAlg = "<?xml version='1.0'?>\
                                       <myParallelRCAlg class='ParallelRCAlg'>\
                                           <LookupOhmicResistance class='ConstObj'>\
                                               <Value>1.1</Value>\
                                           </LookupOhmicResistance>\
                                           <LookupTau class='ConstObj'>\
                                               <Value>0.1</Value>\
                                           </LookupTau>\
                                       </myParallelRCAlg>";
   */
    const char *xmlConfigWarburgCotanh = "<?xml version='1.0'?>\
                                          <MyWarburgCotanh class='WarburgCotanh' RCCounter='10' >\
                                              <Clim class='ConstObj'>\
                                                  <Value> 10 </Value>\
                                              </Clim>\
                                              <Sigma class='ConstObj'>\
                                                  <Value> 5 </Value>\
                                              </Sigma>\
                                          </MyWarburgCotanh>";
    const char *xmlConfigWarburgTanh ="<?xml version='1.0'?>\
                                       <MyWarburgTanh class='WarburgTanh' RCCounter='10' >\
                                           <Tau class='ConstObj'>\
                                               <Value> 10 </Value>\
                                           </Tau>\
                                           <OhmicResistance class='ConstObj'>\
                                               <Value> 5 </Value>\
                                           </OhmicResistance>\
                                       </MyWarburgTanh>";
    const char *xmlConfigStatesForZarc1 = "<?xml version='1.0'?>\
                                           <ThermalState class='ThermalState' cache='True'>\
                                               <InitialTemperature>25</InitialTemperature>\
                                           </ThermalState>";
    const char *xmlConfigStatesForZarc2 = "<?xml version='1.0'?>\
                                           <Soc class='Soc' cache='True'>\
                                               <MaxCapacity>10</MaxCapacity>\
                                               <InitialSoc>50</InitialSoc>\
                                               <MeasurementPoints>5,90</MeasurementPoints>\
                                           </Soc>";
    const char *xmlConfigZarc  = "<?xml version='1.0'?>\
                                 <myZarcElement class='ZarcElement' observable='TRUE'>\
                                     <LookupTau class='LookupObj2DWithState'>\
                                         <RowState cacheref='ThermalState'/>\
                                         <ColState cacheref='Soc'/>\
                                         <LookupData>0.1,0.1;0.1,0.1</LookupData>\
                                         <MeasurementPointsRow>5,90</MeasurementPointsRow>\
                                         <MeasurementPointsColumn>5,90</MeasurementPointsColumn>\
                                     </LookupTau>\
                                     <LookupOhmicResistance class='LookupObj2DWithState'>\
                                         <RowState cacheref='ThermalState'/>\
                                         <ColState cacheref='Soc'/>\
                                         <LookupData>1.1,1.1;1.1,1.1</LookupData>\
                                         <MeasurementPointsRow>5,90</MeasurementPointsRow>\
                                         <MeasurementPointsColumn>5,90</MeasurementPointsColumn>\
                                     </LookupOhmicResistance>\
                                     <LookupPhi class='LookupObj2DWithState'>\
                                         <RowState cacheref='ThermalState'/>\
                                         <ColState cacheref='Soc'/>\
                                         <LookupData>0.01,0.01;0.01,0.01</LookupData>\
                                         <MeasurementPointsRow>5,90</MeasurementPointsRow>\
                                         <MeasurementPointsColumn>5,90</MeasurementPointsColumn>\
                                     </LookupPhi>\
                                 </myZarcElement>";
    const char *xmlConfigZarcAlg = "<?xml version='1.0'?>\
                                 <myZarcElement class='ZarcAlgElement' observable='TRUE'>\
                                     <LookupTau class='LookupObj2DWithState'>\
                                         <RowState cacheref='ThermalState'/>\
                                         <ColState cacheref='Soc'/>\
                                         <LookupData>0.1,0.1;0.1,0.1</LookupData>\
                                         <MeasurementPointsRow>5,90</MeasurementPointsRow>\
                                         <MeasurementPointsColumn>5,90</MeasurementPointsColumn>\
                                     </LookupTau>\
                                     <LookupOhmicResistance class='LookupObj2DWithState'>\
                                         <RowState cacheref='ThermalState'/>\
                                         <ColState cacheref='Soc'/>\
                                         <LookupData>1.1,1.1;1.1,1.1</LookupData>\
                                         <MeasurementPointsRow>5,90</MeasurementPointsRow>\
                                         <MeasurementPointsColumn>5,90</MeasurementPointsColumn>\
                                     </LookupOhmicResistance>\
                                     <LookupPhi class='LookupObj2DWithState'>\
                                         <RowState cacheref='ThermalState'/>\
                                         <ColState cacheref='Soc'/>\
                                         <LookupData>0.01,0.01;0.01,0.01</LookupData>\
                                         <MeasurementPointsRow>5,90</MeasurementPointsRow>\
                                         <MeasurementPointsColumn>5,90</MeasurementPointsColumn>\
                                     </LookupPhi>\
                                 </myZarcElement>";
    const char *xmlConfigCellElement = "<?xml version='1.0'?>\
                                        <BatteryCell class=\"CellElement\" observable=\"True\">\
                                            <ThermalState class=\"ThermalState\" cache=\"True\">\
                                                <InitialTemperature>25</InitialTemperature>\
                                            </ThermalState>\
                                            <Soc class=\"Soc\" cache=\"True\">\
                                                <MaxCapacity>120.0</MaxCapacity>\
                                                <InitialSoc>5</InitialSoc>\
                                                <MeasurementPoints>5,10,20,23,35,50,65,80,90</MeasurementPoints>\
                                            </Soc>\
                                            <Children>\
                                                <OhmicResistance class='OhmicResistance'>\
                                                    <Object class='ConstObj'>\
                                                        <Value>1.1</Value>\
                                                    </Object>\
                                                </OhmicResistance>\
                                            </Children>\
                                            <ThermalBlock class=\"RectangularBlock\">\
                                                <ElectricalDiscretization x=\"3\"/>\
                                            </ThermalBlock>\
                                        </BatteryCell>";
    factory::ArgumentTypeElectrical arg;
    arg.mCellDiscretization = 3.0;
    xmlparser::tinyxml2::XmlParserImpl parser;
    
    parser.ReadFromMem(xmlConfigCapacity);
    boost::shared_ptr<electrical::Capacity< myMatrixType > > capacity
        = boost::static_pointer_cast<electrical::Capacity< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot(), &arg));
    TS_ASSERT_DELTA(capacity->operator()()->GetValue(), 10.1 / 3.0, sDelta);

    parser.ReadFromMem(xmlConfigOhmicResistance);
    boost::shared_ptr<electrical::OhmicResistance< myMatrixType > > ohmicResistance
        = boost::static_pointer_cast<electrical::OhmicResistance< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot(), &arg));
    TS_ASSERT_DELTA(ohmicResistance->operator()()->GetValue(), 1.1 * 3.0, sDelta);

    parser.ReadFromMem(xmlConfigParallelRC);
    boost::shared_ptr<electrical::ParallelRC< myMatrixType > > parallelRC
        = boost::static_pointer_cast<electrical::ParallelRC< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot(), &arg));
    TS_ASSERT_DELTA(parallelRC->operator()()->GetValue(), 1.1 * 3.0, sDelta);
#ifndef _SYMBOLIC_
    TS_ASSERT_DELTA(parallelRC->GetValueC(), 0.1 / (1.1 * 3.0), sDelta);
#else
    TS_ASSERT_DELTA(parallelRC->GetTauObject()->GetValue(), 0.1, sDelta);
#endif

    //parser.ReadFromMem(xmlConfigParallelRCAlg);
    //boost::shared_ptr<electrical::ParallelRCAlg< myMatrixType > > parallelRCAlg
    //    = boost::static_pointer_cast<electrical::ParallelRCAlg< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot(), &arg));
    //TS_ASSERT_DELTA(parallelRCAlg->GetValue(), 1.1 * 3.0, sDelta);
    //TS_ASSERT_DELTA(parallelRCAlg->mObjectC->GetValue(), 0.1, sDelta);

    parser.ReadFromMem(xmlConfigWarburgCotanh);
    boost::shared_ptr<electrical::SerialTwoPort< myMatrixType > > warburgCotanh
        = boost::static_pointer_cast<electrical::SerialTwoPort< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot(), &arg));
    // R changes values
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(0))->operator()()->GetValue(), 101.3212 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(1))->operator()()->GetValue(),  25.3303 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(2))->operator()()->GetValue(),  11.2579 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(3))->operator()()->GetValue(),   6.3326 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(4))->operator()()->GetValue(),   4.0528 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(5))->operator()()->GetValue(),   2.8145 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(6))->operator()()->GetValue(),   2.0678 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(7))->operator()()->GetValue(),   1.5831 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(8))->operator()()->GetValue(),   1.2509 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(9))->operator()()->GetValue(),   1.0132 * 3.0, 0.001);
    // Capacity stays the same
    for( size_t i = 0 ; i< warburgCotanh->size()-1 ; ++i )
    {
#ifndef _SYMBOLIC_
        TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(i))->GetValueC(), 5.0 / 3.0, 0.001);
#else
        double capacity = static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(i))->GetTauObject()->GetValue()
                        / static_cast<electrical::ParallelRC < myMatrixType >* >(warburgCotanh->at(i))->operator()()->GetValue();
        TS_ASSERT_DELTA(capacity, 5.0 / 3.0, 0.001);
#endif
    }

    parser.ReadFromMem(xmlConfigWarburgTanh);
    boost::shared_ptr<electrical::SerialTwoPort< myMatrixType > > warburgTanh
        = boost::static_pointer_cast<electrical::SerialTwoPort< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot(), &arg));
    // R changes values
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(0))->operator()()->GetValue(), 5.0 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(1))->operator()()->GetValue(), 0.5556 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(2))->operator()()->GetValue(), 0.2000 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(3))->operator()()->GetValue(), 0.1020 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(4))->operator()()->GetValue(), 0.0617 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(5))->operator()()->GetValue(), 0.0413 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(6))->operator()()->GetValue(), 0.0296 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(7))->operator()()->GetValue(), 0.0222 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(8))->operator()()->GetValue(), 0.0173 * 3.0, 0.001);
    TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(9))->operator()()->GetValue(), 0.0139 * 3.0, 0.001);
    // Capacity stays the same
    for( size_t i = 0 ; i< warburgTanh->size() ; ++i )
    {
#ifndef _SYMBOLIC_
        TS_ASSERT_DELTA(static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(i))->GetValueC(), 2.0 / 3.0, 0.001);
#else
        double capacity = static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(i))->GetTauObject()->GetValue()
                        / static_cast<electrical::ParallelRC < myMatrixType >* >(warburgTanh->at(i))->operator()()->GetValue();
        TS_ASSERT_DELTA(capacity, 2.0 / 3.0, 0.001);
#endif
    }

#ifndef _SYMBOLIC_
    parser.ReadFromMem(xmlConfigStatesForZarc1);
    stateFactory->CreateInstance(parser.GetRoot());
    parser.ReadFromMem(xmlConfigStatesForZarc2);
    stateFactory->CreateInstance(parser.GetRoot());
    parser.ReadFromMem(xmlConfigZarc);
    boost::shared_ptr<electrical::Zarc< myMatrixType > > zarc
        = boost::static_pointer_cast<electrical::Zarc< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot(), &arg));
    TS_ASSERT_DELTA(zarc->mLookupOhmicResistance->GetValue(), 1.1 * 3.0, sDelta);
    TS_ASSERT_DELTA(zarc->mLookupRelaxationTime->GetValue(), 0.1, sDelta);
    TS_ASSERT_DELTA(zarc->mLookupPhi->GetValue(), 0.01, sDelta);

    parser.ReadFromMem(xmlConfigZarcAlg);
    boost::shared_ptr<electrical::ZarcAlg< myMatrixType > > zarcAlg
        = boost::static_pointer_cast<electrical::ZarcAlg< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot(), &arg));
    TS_ASSERT_DELTA(zarcAlg->mLookupOhmicResistance->GetValue(), 1.1 * 3.0, sDelta);
    TS_ASSERT_DELTA(zarcAlg->mLookupRelaxationTime->GetValue(), 0.1, sDelta);
    TS_ASSERT_DELTA(zarcAlg->mLookupPhi->GetValue(), 0.01, sDelta);
#endif // NOT _SYMBOLIC_

    parser.ReadFromMem(xmlConfigCellElement);
#ifdef __EXCEPTIONS__
    boost::shared_ptr<electrical::ParallelTwoPort< myMatrixType > > cell = boost::dynamic_pointer_cast<electrical::ParallelTwoPort< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot()));
    TS_ASSERT(cell != 0);
#else
    boost::shared_ptr<electrical::ParallelTwoPort< myMatrixType > > cell = boost::static_pointer_cast<electrical::ParallelTwoPort< myMatrixType > >(electricalFactory->CreateInstance(parser.GetRoot()));
#endif
    TS_ASSERT_EQUALS(cell->size(), 3);
    //0
    TS_ASSERT(cell->at(0)->IsCellelement());
    electrical::Cellelement< myMatrixType >* cellElement = static_cast<electrical::Cellelement< myMatrixType >*>(cell->at(0));
    TS_ASSERT_EQUALS(cellElement->size(), 1);
#ifdef __EXCEPTIONS__
    electrical::OhmicResistance< myMatrixType >* rInCellElement = dynamic_cast<electrical::OhmicResistance< myMatrixType >*>(cellElement->at(0));
    TS_ASSERT(rInCellElement != 0);
#else
    electrical::OhmicResistance< myMatrixType >* rInCellElement = static_cast<electrical::OhmicResistance< myMatrixType >*>(cellElement->at(0));
#endif
    TS_ASSERT_DELTA(rInCellElement->operator()()->GetValue(), 1.1 * 3.0, sDelta);
    //1
    TS_ASSERT(cell->at(1)->IsCellelement());
    cellElement = static_cast<electrical::Cellelement< myMatrixType >*>(cell->at(1));
    TS_ASSERT_EQUALS(cellElement->size(), 1);
#ifdef __EXCEPTIONS__
    rInCellElement = dynamic_cast<electrical::OhmicResistance< myMatrixType >*>(cellElement->at(0));
    TS_ASSERT(rInCellElement != 0);
#else
    rInCellElement = static_cast<electrical::OhmicResistance< myMatrixType >*>(cellElement->at(0));
#endif
    TS_ASSERT_DELTA(rInCellElement->operator()()->GetValue(), 1.1 * 3.0, sDelta);
    //2
    TS_ASSERT(cell->at(2)->IsCellelement());
    cellElement = static_cast<electrical::Cellelement< myMatrixType >*>(cell->at(2));
    TS_ASSERT_EQUALS(cellElement->size(), 1);
#ifdef __EXCEPTIONS__
    rInCellElement = dynamic_cast<electrical::OhmicResistance< myMatrixType >*>(cellElement->at(0));
    TS_ASSERT(rInCellElement != 0);
#else
    rInCellElement = static_cast<electrical::OhmicResistance< myMatrixType >*>(cellElement->at(0));
#endif
    TS_ASSERT_DELTA(rInCellElement->operator()()->GetValue(), 1.1 * 3.0, sDelta);
}

void TestFactoryLookUpCellElements::TestThermalFactory()
{
    //Load File
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromFile("TestPouchBagDisc.xml");

    //Build Factories
    boost::scoped_ptr<factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory;
    boost::scoped_ptr<factory::Factory< object::Object< double >, factory::ArgumentTypeObject<double> > > objectFactory;
    boost::scoped_ptr<factory::Factory< thermal::Material< double > > > materialFactory;
    boost::scoped_ptr<factory::Factory< thermal::ThermalBlock< double >, factory::ArgumentTypeThermalBlock<double> > > blockFactory;
    boost::scoped_ptr<factory::Factory< thermal::Cooling< double > > > coolingFactory;
    boost::scoped_ptr<factory::Factory< thermal::CoolingBlock< double >, factory::ArgumentTypeCoolingBlock<double> > > coolingBlockFactory;
    boost::scoped_ptr<thermal::ThermalFactory< double > > thermalFactory;

    stateFactory.reset( factory::BuildStateFactory() );
    objectFactory.reset( factory::BuildObjectFactory< double >(stateFactory.get()) );
    materialFactory.reset( factory::BuildMaterialFactory< double >() );
    blockFactory.reset( factory::BuildBlockFactory< double >(materialFactory.get()) );
    coolingFactory.reset( factory::BuildCoolingFactory< double >(objectFactory.get()) );
    coolingBlockFactory.reset( factory::BuildCoolingBlockFactory< double >(coolingFactory.get()) );
    thermalFactory.reset( new thermal::ThermalFactory< double >(materialFactory.get(), blockFactory.get(), coolingFactory.get(), coolingBlockFactory.get()) );

    std::vector<boost::shared_ptr<thermal::ThermalBlock<double> > > heatedBlocks;
    std::vector<boost::shared_ptr<thermal::ThermalBlock<double> > > unheatedBlocks;
    std::vector<boost::shared_ptr<thermal::CoolingBlock<double> > > coolingBlocks;
    std::vector<boost::shared_ptr< ::state::ThermalState<double> > > thermalStates(48, boost::shared_ptr< ::state::ThermalState<double> >(new ::state::ThermalState<double>));

    //Create thermal model
    thermalFactory->CreateThermalModel(parser.GetRoot(), heatedBlocks, unheatedBlocks, coolingBlocks, &thermalStates, 0);
    std::vector<boost::shared_ptr< ::state::ThermalState<double> > >::iterator it = thermalStates.begin();

    TS_ASSERT_EQUALS(thermalStates.size(), 48);
    TS_ASSERT_EQUALS(std::vector<boost::shared_ptr< ::state::ThermalState<double> > >(it, it + 24), heatedBlocks.at(0)->GetThermalStates());
    TS_ASSERT_EQUALS(std::vector<boost::shared_ptr< ::state::ThermalState<double> > >(it + 24, it + 48), heatedBlocks.at(1)->GetThermalStates());
    TS_ASSERT_EQUALS(unheatedBlocks.at(0)->GetThermalStates().size(), 1);
}
