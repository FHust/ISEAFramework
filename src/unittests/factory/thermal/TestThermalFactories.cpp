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
#include "TestThermalFactories.h"
#include "../../thermal/exception_tester.h"
#include <boost/scoped_ptr.hpp>


using namespace thermal;
static const double sDelta = 0.000001;


void TestFactories::TestCoolingFactory()
{
    const char *xmlWaterCooling =
     "\
        <Cooling class=\"CoolingByLookUp\">\
            <LookUpTable class=\"LookupObj1d\">\
                <LookupData>100.0, 700.0, 750.0</LookupData>\
                <MeasurementPoints>20.0, 50.0, 50.5</MeasurementPoints>\
            </LookUpTable>\
        </Cooling>";
    const char *xmlDirichlet =
     "\
        <Cooling class=\"DirichletBoundaryCondition\">\
            <SurfaceTemperature>-1.0</SurfaceTemperature>\
        </Cooling>";
    const char *xmlAirCooling =
     "\
        <Cooling class=\"CoolingByConstantValue\">\
            <Value>100.0</Value>\
        </Cooling>";
    const char *xmlThermalIsolation =
     "\
        <Cooling class=\"ThermalIsolation\"/>";


    xmlparser::tinyxml2::XmlParserImpl parser;
    scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    scoped_ptr< factory::Factory< Cooling<> > > coolingFactory( factory::BuildCoolingFactory< double >( objectFactory.get() ) );


    parser.ReadFromMem( xmlWaterCooling );
    boost::shared_ptr< Cooling<> > cooling( coolingFactory->CreateInstance( parser.GetRoot() ) );
    TS_ASSERT( !cooling->IsDirichletBoundaryCondition() );

    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 0.0, 0.0 ).mSlope, 0.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 19.0, 0.0 ).mSlope, 0.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 19.9, 0.0 ).mSlope, 0.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 20.0, 0.0 ).mSlope, 10.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 20.1, 0.0 ).mSlope, 20.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 35.0, 0.0 ).mSlope, 20.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 49.9, 0.0 ).mSlope, 20.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 50.0, 0.0 ).mSlope, ( 2.0 * 100.0 + 1.0 * 20 ) / 3.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 50.05, 0.0 ).mSlope, 100.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 50.25, 0.0 ).mSlope, 100.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 50.5, 0.0 ).mSlope, ( 1.0 * 0.0 + 2.0 * 100 ) / 3.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 50.6, 0.0 ).mSlope, 0.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 51.0, 0.0 ).mSlope, 0.0, sDelta );

    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 0.0, 0.0 ).mOffset, 100.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 19.0, 0.0 ).mOffset, 100.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 20.0, 0.0 ).mOffset, 100.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 35.0, 0.0 ).mOffset, ( 100.0 + 700.0 ) / 2.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 50.0, 0.0 ).mOffset, 700.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 50.25, 0.0 ).mOffset, ( 700.0 + 750.0 ) / 2.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 50.5, 0.0 ).mOffset, 750.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 51.0, 0.0 ).mOffset, 750.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 100.0, 0.0 ).mOffset, 750.0, sDelta );


    parser.ReadFromMem( xmlDirichlet );
    cooling = coolingFactory->CreateInstance( parser.GetRoot() );
    TS_ASSERT( cooling->IsDirichletBoundaryCondition() );
    TS_ASSERT_EQUALS( cooling->GetOffsetSlope( 200.0, 0.0 ).mOffset, -1.0 );
    TS_ASSERT_EQUALS( cooling->GetOffsetSlope( 200.0, 0.0 ).mSlope, -1.0 );
    TS_ASSERT_EQUALS( cooling->GetOffsetSlope( 500.0, 100.0 ).mOffset, -1.0 );
    TS_ASSERT_EQUALS( cooling->GetOffsetSlope( 500.0, 100.0 ).mSlope, -1.0 );


    parser.ReadFromMem( xmlAirCooling );
    cooling = coolingFactory->CreateInstance( parser.GetRoot() );
    TS_ASSERT( !cooling->IsDirichletBoundaryCondition() );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 200.0, 0.0 ).mOffset, 100.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 200.0, 0.0 ).mSlope, 0.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 500.0, 100.0 ).mOffset, 100.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 500.0, 100.0 ).mSlope, 0.0, sDelta );


    parser.ReadFromMem( xmlThermalIsolation );
    cooling = coolingFactory->CreateInstance( parser.GetRoot() );
    TS_ASSERT( !cooling->IsDirichletBoundaryCondition() );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 200.0, 0.0 ).mOffset, 0.0, sDelta );
    TS_ASSERT_DELTA( cooling->GetOffsetSlope( 200.0, 0.0 ).mSlope, 0.0, sDelta );
}

void TestFactories::TestCoolingBlockFactory()
{
    const char *xmlCoolingHorizontal =
     "\
        <CoolingBlock class=\"CoolingHorizontalPlane\">\
            <Vertices>-1.0,1.0; 2.0, 5.0; 3.0, 3.0</Vertices>\
            <ZCoordinate>1.0</ZCoordinate>\
            <Cooling class=\"CoolingByConstantValue\">\
                <Value>100.0</Value>\
            </Cooling>\
       </CoolingBlock>";
    const char *xmlCoolingVertical =
     "\
       <CoolingBlock class=\"CoolingVerticalPlane\">\
            <Vertices> -1.0, 1.0; 2.0, 5.0; 3.0, 3.0</Vertices>\
            <LowerZCoordinate>1.0</LowerZCoordinate>\
            <UpperZCoordinate>2.0</UpperZCoordinate>\
            <Cooling class=\"CoolingByConstantValue\">\
                <Value>100.0</Value>\
            </Cooling>\
       </CoolingBlock>";
    const char *xmlCoolingPrismatic =
     "\
       <CoolingBlock class=\"CoolingPrismatic\">\
            <Vertices>-1.0, 1.0; 2.0, 5.0; 3.0, 3.0</Vertices>\
            <LowerZCoordinate>1.0</LowerZCoordinate>\
            <UpperZCoordinate>2.0</UpperZCoordinate>\
            <Cooling class=\"CoolingByConstantValue\">\
                <Value>100.0</Value>\
            </Cooling>\
        </CoolingBlock>";

    xmlparser::tinyxml2::XmlParserImpl parser;
    scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    scoped_ptr< factory::Factory< Cooling< double > > > coolingFactory(
     factory::BuildCoolingFactory< double >( objectFactory.get() ) );
    scoped_ptr< factory::Factory< CoolingBlock<>, factory::ArgumentTypeCoolingBlock< double > > > coolingBlockFactory(
     factory::BuildCoolingBlockFactory< double >( coolingFactory.get() ) );

    factory::ArgumentTypeCoolingBlock< double > arg;
    arg.mX = 0.5;
    arg.mY = 1.0;
    arg.mZ = 0.0;


    parser.ReadFromMem( xmlCoolingHorizontal );
    boost::shared_ptr< CoolingBlock<> > coolingBlock;
    //#if defined(__EXCEPTIONS__)
    TS_ASSERT_THROWS_EQUALS( coolingBlock.operator=( coolingBlockFactory->CreateInstance( parser.GetRoot(), &arg ) ),
                             const std::runtime_error &e, !strstr( e.what(), "CoolingHorizontalPlane is deprecated" ), 0 );
    //#endif
    /*#ifdef __EXCEPTIONS__
        CoolingHorizontalPlane<>* cool1 = dynamic_cast<CoolingHorizontalPlane<>*>(coolingBlock.get());
        TS_ASSERT_EQUALS(cool1 == 0, false);
    #else
        CoolingHorizontalPlane<>* cool1 = static_cast<CoolingHorizontalPlane<>*>(coolingBlock.get());
    #endif
        TS_ASSERT_DELTA(cool1->mVertices.at(0).Get1(), - 0.5, sDelta);
        TS_ASSERT_DELTA(cool1->mVertices.at(0).Get2(), 2.0, sDelta);
        TS_ASSERT_DELTA(cool1->mVertices.at(1).Get1(), 2.5, sDelta);
        TS_ASSERT_DELTA(cool1->mVertices.at(1).Get2(), 6.0, sDelta);
        TS_ASSERT_DELTA(cool1->mVertices.at(2).Get1(), 3.5, sDelta);
        TS_ASSERT_DELTA(cool1->mVertices.at(2).Get2(), 4.0, sDelta);
        TS_ASSERT_DELTA(cool1->mZ, 1.0, sDelta);*/


    parser.ReadFromMem( xmlCoolingVertical );
    //#if defined(__EXCEPTIONS__)
    TS_ASSERT_THROWS_EQUALS( coolingBlock.operator=( coolingBlockFactory->CreateInstance( parser.GetRoot(), &arg ) ),
                             const std::runtime_error &e, !strstr( e.what(), "CoolingVerticalPlane is deprecated" ), 0 );
    //#endif
    /*#ifdef __EXCEPTIONS__
        CoolingVerticalPlane<>* cool2 = dynamic_cast<CoolingVerticalPlane<>*>(coolingBlock.get());
        TS_ASSERT_EQUALS(cool2 == 0, false);
    #else
        CoolingVerticalPlane<>* cool2 = static_cast<CoolingVerticalPlane<>*>(coolingBlock.get());;
    #endif
        TS_ASSERT_DELTA(cool2->mVertices.at(0).Get1(), - 0.5, sDelta);
        TS_ASSERT_DELTA(cool2->mVertices.at(0).Get2(), 2.0, sDelta);
        TS_ASSERT_DELTA(cool2->mVertices.at(1).Get1(), 2.5, sDelta);
        TS_ASSERT_DELTA(cool2->mVertices.at(1).Get2(), 6.0, sDelta);
        TS_ASSERT_DELTA(cool2->mVertices.at(2).Get1(), 3.5, sDelta);
        TS_ASSERT_DELTA(cool2->mVertices.at(2).Get2(), 4.0, sDelta);
        TS_ASSERT_DELTA(cool2->mZLower, 1.0, sDelta);
        TS_ASSERT_DELTA(cool2->mZUpper, 2.0, sDelta);*/


    parser.ReadFromMem( xmlCoolingPrismatic );
    coolingBlock = coolingBlockFactory->CreateInstance( parser.GetRoot(), &arg );
#ifdef __EXCEPTIONS__
    CoolingPrismatic<> *cool3 = dynamic_cast< CoolingPrismatic<> * >( coolingBlock.get() );
    TS_ASSERT_EQUALS( cool3 == 0, false );
#else
    CoolingPrismatic<> *cool3 = static_cast< CoolingPrismatic<> * >( coolingBlock.get() );
    ;
#endif
    TS_ASSERT_DELTA( cool3->mVertices.at( 0 ).Get1(), -0.5, sDelta );
    TS_ASSERT_DELTA( cool3->mVertices.at( 0 ).Get2(), 2.0, sDelta );
    TS_ASSERT_DELTA( cool3->mVertices.at( 1 ).Get1(), 2.5, sDelta );
    TS_ASSERT_DELTA( cool3->mVertices.at( 1 ).Get2(), 6.0, sDelta );
    TS_ASSERT_DELTA( cool3->mVertices.at( 2 ).Get1(), 3.5, sDelta );
    TS_ASSERT_DELTA( cool3->mVertices.at( 2 ).Get2(), 4.0, sDelta );
    TS_ASSERT_DELTA( cool3->mZLower, 1.0, sDelta );
    TS_ASSERT_DELTA( cool3->mZUpper, 2.0, sDelta );
}

void TestFactories::TestMaterialFactory()
{
    const char *xmlMaterial =
     "\
        <ThermalMaterials>\
            <Copper class=\"MaterialCartesian\" cache=\"true\">\
                <Density>50.0</Density>\
                <SpecificCapacity>51.0</SpecificCapacity>\
                <ConductivityX>52.0</ConductivityX>\
                <ConductivityY>53.0</ConductivityY>\
                <ConductivityZ>54.0</ConductivityZ>\
            </Copper>\
            <Iron class=\"MaterialCylindric\" cache=\"true\">\
                <Density>60.0</Density>\
                <SpecificCapacity>61.0</SpecificCapacity>\
                <ConductivityRho>62.0</ConductivityRho>\
                <ConductivityPhi>63.0</ConductivityPhi>\
                <ConductivityZ>64.0</ConductivityZ>\
            </Iron>\
            <Rubber class=\"Material\" cache=\"true\">\
                <Density>70.0</Density>\
                <SpecificCapacity>71.0</SpecificCapacity>\
                <Conductivity>72.0</Conductivity>\
            </Rubber>\
            <Fail class=\"Material\">\
                <Density>0.0</Density>\
                <SpecificCapacity>0.0</SpecificCapacity>\
                <Conductivity>0.0</Conductivity>\
            </Fail>\
        </ThermalMaterials>";


    scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlMaterial );
    boost::shared_ptr< xmlparser::XmlParameter > materialsParam( parser->GetRoot() );
    scoped_ptr< factory::Factory< Material<> > > materialFactory( factory::BuildMaterialFactory< double >() );

    boost::shared_ptr< Material<> > copper(
     materialFactory->CreateInstance( materialsParam->GetElementChild( "Copper" ) ) );
    boost::shared_ptr< Material<> > iron(
     materialFactory->CreateInstance( materialsParam->GetElementChild( "Iron" ) ) );
    boost::shared_ptr< Material<> > rubber(
     materialFactory->CreateInstance( materialsParam->GetElementChild( "Rubber" ) ) );

    TS_ASSERT_EQUALS( copper->GetSpecificCapacity(), 51.0 );
    TS_ASSERT_EQUALS( copper->GetConductivity( 0 ), 52.0 );
    TS_ASSERT_EQUALS( copper->GetConductivity( 1 ), 53.0 );
    TS_ASSERT_EQUALS( copper->GetConductivity( 2 ), 54.0 );
    TS_ASSERT_EQUALS( copper->GetDensity(), 50.0 );
    TS_ASSERT_EQUALS( copper->GetMaterialType(), thermal::Material<>::CARTESIAN );

    TS_ASSERT_EQUALS( iron->GetSpecificCapacity(), 61.0 );
    TS_ASSERT_EQUALS( iron->GetConductivity( 0 ), 62.0 );
    TS_ASSERT_EQUALS( iron->GetConductivity( 1 ), 63.0 );
    TS_ASSERT_EQUALS( iron->GetConductivity( 2 ), 64.0 );
    TS_ASSERT_EQUALS( iron->GetDensity(), 60.0 );
    TS_ASSERT_EQUALS( iron->GetMaterialType(), thermal::Material<>::CYLINDRIC );

    TS_ASSERT_EQUALS( rubber->GetSpecificCapacity(), 71.0 );
    TS_ASSERT_EQUALS( rubber->GetConductivity( 0 ), 72.0 );
    TS_ASSERT_EQUALS( rubber->GetConductivity( 1 ), 72.0 );
    TS_ASSERT_EQUALS( rubber->GetConductivity( 2 ), 72.0 );
    TS_ASSERT_EQUALS( rubber->GetDensity(), 70.0 );
    TS_ASSERT_EQUALS( rubber->GetMaterialType(), thermal::Material<>::ISOTROPIC );

    //#if defined(__EXCEPTIONS__)
    TS_ASSERT_THROWS_ANYTHING( materialFactory->CreateInstance( materialsParam->GetElementChild( "Fail" ) ) );
    //#endif
}

void TestFactories::TestBlockFactory()
{
    scoped_ptr< factory::Factory< Material<> > > materialFactory;
    materialFactory.reset( factory::BuildMaterialFactory< double >() );

    const char *xmlMaterial =
     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
        <ThermalMaterials>\
            <Copper class=\"Material\" cache=\"true\">\
                <Density>50.0</Density>\
                <SpecificCapacity>51.0</SpecificCapacity>\
                <Conductivity>52.0</Conductivity>\
            </Copper>\
        </ThermalMaterials>";
    const char *xmlMaterial2 =
     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
        <ThermalMaterials>\
            <Iron class=\"Material\" cache=\"true\">\
                <Density>100.0</Density>\
                <SpecificCapacity>51.0</SpecificCapacity>\
                <Conductivity>52.0</Conductivity>\
            </Iron>\
        </ThermalMaterials>";

    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromMem( xmlMaterial );
    boost::shared_ptr< xmlparser::XmlParameter > materialsParam( parser.GetRoot() );
    materialFactory->CreateInstance( materialsParam->GetElementChild( "Copper" ) );
    parser.ReadFromMem( xmlMaterial2 );
    materialsParam = parser.GetRoot();
    materialFactory->CreateInstance( materialsParam->GetElementChild( "Iron" ) );


    scoped_ptr< factory::Factory< ThermalBlock<>, factory::ArgumentTypeThermalBlock< double > > > blockFactory(
     factory::BuildBlockFactory< double >( materialFactory.get() ) );

    const char *xmlBlock1 =
     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
        <BlockType1 class=\"RectangularBlock\">\
            <Material cacheref=\"Copper\"/>\
            <Dimensions>0.1, 0.2, 0.15</Dimensions>\
            <SizeX>5</SizeX><SizeY>4</SizeY><SizeZ>2</SizeZ>\
            <InitialTemperature>25.0</InitialTemperature>\
            <Heat>500.0</Heat>\
        </BlockType1>";

    const char *xmlBlock2 =
     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
        <BlockType2 class=\"RectangularBlock\">\
            <ElectricalDiscretization x=\"4\" y =\"2\" z=\"3\"/>\
            <Material cacheref=\"Copper\"/>\
            <Dimensions>0.1, 0.2, 0.25</Dimensions>\
            <SizeX>4</SizeX><SizeY>2</SizeY><SizeZ>3</SizeZ>\
            <InitialTemperature>27.0</InitialTemperature>\
        </BlockType2>";

    xmlparser::tinyxml2::XmlParserImpl block1Parser;
    block1Parser.ReadFromMem( xmlBlock1 );
    boost::shared_ptr< xmlparser::XmlParameter > block1Param( block1Parser.GetRoot() );

    xmlparser::tinyxml2::XmlParserImpl block2Parser;
    block2Parser.ReadFromMem( xmlBlock2 );
    boost::shared_ptr< xmlparser::XmlParameter > block2Param( block2Parser.GetRoot() );


    factory::ArgumentTypeThermalBlock< double > arg;
    arg.mX = 20.0;
    arg.mY = 21.0;
    arg.mZ = 22.0;
    boost::shared_ptr< ThermalBlock<> > block1Unconnected( blockFactory->CreateInstance( block1Param, &arg ) );
    boost::shared_ptr< ThermalBlock<> > block2Unconnected( blockFactory->CreateInstance( block2Param, &arg ) );

    arg.mThermalStates.push_back( boost::shared_ptr< ::state::ThermalState<> >(new ::state::ThermalState<>));
    arg.mThermalStates.at( 0 )->SetFixedPowerDissipation( 2000.0 );
    boost::shared_ptr< ThermalBlock<> > block1Connected( blockFactory->CreateInstance( block1Param, &arg ) );
    arg.mThermalStates.clear();
    arg.mThermalStates.resize( 24, boost::shared_ptr< ::state::ThermalState<> >(new ::state::ThermalState<>));
    for ( size_t i = 0; i < 24; ++i )
        arg.mThermalStates.at( i )->SetFixedPowerDissipation( 1000.0 );
    boost::shared_ptr< ThermalBlock<> > block2Connected( blockFactory->CreateInstance( block2Param, &arg ) );


#ifdef __EXCEPTIONS__
    RectangularBlock<> *block = dynamic_cast< RectangularBlock<> * >( block1Unconnected.get() );
    TS_ASSERT_EQUALS( block == 0, false );
#else
    RectangularBlock<> *block = static_cast< RectangularBlock<> * >( block1Unconnected.get() );
#endif
    TS_ASSERT_DELTA( block->mEmplacementPoint.GetX(), 20.0, sDelta );
    TS_ASSERT_DELTA( block->mEmplacementPoint.GetY(), 21.0, sDelta );
    TS_ASSERT_DELTA( block->mEmplacementPoint.GetZ(), 22.0, sDelta );
    TS_ASSERT_DELTA( block->mDimX, 0.1 / 5, sDelta );
    TS_ASSERT_DELTA( block->mDimY, 0.2 / 4, sDelta );
    TS_ASSERT_DELTA( block->mDimZ, 0.15 / 2, sDelta );
    TS_ASSERT_EQUALS( block->mNX, 5 );
    TS_ASSERT_EQUALS( block->mNY, 4 );
    TS_ASSERT_EQUALS( block->mNZ, 2 );
    TS_ASSERT_EQUALS( block->mMaterial->GetDensity(), 50.0 );
    TS_ASSERT_DELTA( block->mInitialTemperature, 25.0, sDelta );

    TS_ASSERT_EQUALS( block->mElectricalDiscretizationX, 1 );
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationY, 1 );
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationZ, 1 );
    TS_ASSERT_EQUALS( block->mThermalStates.size(), 1 );
    TS_ASSERT_DELTA( block->mThermalStates.at( 0 )->GetPowerDissipation( 2.0, 1.0 ), 500.0, sDelta );

#ifdef __EXCEPTIONS__
    block = dynamic_cast< RectangularBlock<> * >( block2Unconnected.get() );
    TS_ASSERT_EQUALS( block == 0, false );
#else
    block = static_cast< RectangularBlock<> * >( block2Unconnected.get() );
#endif
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationX, 1 );
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationY, 1 );
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationZ, 1 );
    TS_ASSERT_EQUALS( block->mThermalStates.size(), 1 );
    TS_ASSERT( block->mThermalStates.at( 0 ).get() == 0 );

#ifdef __EXCEPTIONS__
    block = dynamic_cast< RectangularBlock<> * >( block1Connected.get() );
    TS_ASSERT_EQUALS( block == 0, false );
#else
    block = static_cast< RectangularBlock<> * >( block1Connected.get() );
#endif
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationX, 1 );
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationY, 1 );
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationZ, 1 );
    TS_ASSERT_EQUALS( block->mThermalStates.size(), 1 );
    TS_ASSERT_DELTA( block->mThermalStates.at( 0 )->GetPowerDissipation( 2.0, 1.0 ), 2000.0, sDelta );

#ifdef __EXCEPTIONS__
    block = dynamic_cast< RectangularBlock<> * >( block2Connected.get() );
    TS_ASSERT_EQUALS( block == 0, false );
#else
    block = static_cast< RectangularBlock<> * >( block2Connected.get() );
#endif
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationX, 4 );
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationY, 2 );
    TS_ASSERT_EQUALS( block->mElectricalDiscretizationZ, 3 );
    TS_ASSERT_EQUALS( block->mThermalStates.size(), 24 );
    for ( size_t i = 0; i < 24; ++i )
        TS_ASSERT_DELTA( block->mThermalStates.at( i )->GetPowerDissipation( 2.0, 1.0 ), 1000.0, sDelta );


    const char *xmlQuadraticBlock =
     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
        <BlockType2 class=\"QuadraticCellBlock\">\
            <ElectricalDiscretization phi=\"2\" z=\"3\"/>\
            <Length>0.1</Length>\
            <RadiusCell>0.04</RadiusCell>\
            <Height>0.15</Height>\
            <CellRhoDivision>2</CellRhoDivision>\
            <PhiDivision>4</PhiDivision>\
            <HeightDivision>6</HeightDivision>\
            <ArcPolygonization>0.005</ArcPolygonization>\
            <CellMaterial cacheref=\"Copper\"/>\
            <FillMaterial cacheref=\"Iron\"/>\
            <InitialTemperature>3</InitialTemperature>\
        </BlockType2>";

    xmlparser::tinyxml2::XmlParserImpl quadraticCellParser;
    quadraticCellParser.ReadFromMem( xmlQuadraticBlock );
    boost::shared_ptr< xmlparser::XmlParameter > quadraticCellParam( quadraticCellParser.GetRoot() );

    arg.mThermalStates.clear();
    arg.mThermalStates.resize( 6, boost::shared_ptr< ::state::ThermalState<> >(new ::state::ThermalState<>));
    for ( size_t i = 0; i < 6; ++i )
        arg.mThermalStates.at( i )->SetFixedPowerDissipation( 2000.0 );
    boost::shared_ptr< ThermalBlock<> > quadraticCellBlock( blockFactory->CreateInstance( quadraticCellParam, &arg ) );

#ifdef __EXCEPTIONS__
    QuadraticCellBlock<> *quadBlock = dynamic_cast< QuadraticCellBlock<> * >( quadraticCellBlock.get() );
    TS_ASSERT_EQUALS( quadBlock == 0, false );
#else
    QuadraticCellBlock<> *quadBlock = static_cast< QuadraticCellBlock<> * >( quadraticCellBlock.get() );
#endif
    TS_ASSERT_DELTA( quadBlock->mRadialEmplacementPoint.GetX(), 20.05, sDelta );
    TS_ASSERT_DELTA( quadBlock->mRadialEmplacementPoint.GetY(), 21.05, sDelta );
    TS_ASSERT_DELTA( quadBlock->mRadialEmplacementPoint.GetZ(), 22.0, sDelta );
    TS_ASSERT_DELTA( quadBlock->mLength, 0.1, sDelta );
    TS_ASSERT_DELTA( quadBlock->mRadiusCell, 0.04, sDelta );
    TS_ASSERT_DELTA( quadBlock->mHeight, 0.15, sDelta );
    TS_ASSERT_EQUALS( quadBlock->mNRhoCell, 2 );
    TS_ASSERT_EQUALS( quadBlock->mNPhi, 4 );
    TS_ASSERT_EQUALS( quadBlock->mNZ, 6 );
    TS_ASSERT_DELTA( quadBlock->mArcPolygonEdgesLength, 0.005, sDelta );
    TS_ASSERT_DELTA( quadBlock->mCellMaterial->GetDensity(), 50.0, sDelta );
    TS_ASSERT_DELTA( quadBlock->mFillMaterial->GetDensity(), 100.0, sDelta );
    TS_ASSERT_DELTA( quadBlock->mInitialTemperature, 3.0, sDelta );

    TS_ASSERT_EQUALS( quadBlock->mElectricalDiscretizationPhi, 2 );
    TS_ASSERT_EQUALS( quadBlock->mElectricalDiscretizationZ, 3 );
    TS_ASSERT_EQUALS( quadBlock->mThermalStates.size(), 6 );
    for ( size_t i = 0; i < 6; ++i )
        TS_ASSERT_DELTA( quadBlock->mThermalStates.at( i )->GetPowerDissipation( 2.0, 1.0 ), 2000.0, sDelta );


    const char *xmlHexagonalBlock =
     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
        <BlockType class=\"HexagonalCellBlock\">\
            <ElectricalDiscretization phi=\"2\" z=\"3\"/>\
            <Length>0.1</Length>\
            <RadiusCell>0.04</RadiusCell>\
            <Height>0.15</Height>\
            <CellRhoDivision>2</CellRhoDivision>\
            <PhiDivision>6</PhiDivision>\
            <HeightDivision>3</HeightDivision>\
            <ArcPolygonization>0.005</ArcPolygonization>\
            <CellMaterial cacheref=\"Copper\"/>\
            <FillMaterial cacheref=\"Iron\"/>\
            <InitialTemperature>3</InitialTemperature>\
        </BlockType>";

    xmlparser::tinyxml2::XmlParserImpl hexagonalCellParser;
    hexagonalCellParser.ReadFromMem( xmlHexagonalBlock );
    boost::shared_ptr< xmlparser::XmlParameter > hexagonalCellParam( hexagonalCellParser.GetRoot() );

    arg.mThermalStates.clear();
    arg.mThermalStates.resize( 6, boost::shared_ptr< ::state::ThermalState<> >(new ::state::ThermalState<>));
    for ( size_t i = 0; i < 6; ++i )
        arg.mThermalStates.at( i )->SetFixedPowerDissipation( 2000.0 );
    boost::shared_ptr< ThermalBlock<> > hexagonalCellBlock( blockFactory->CreateInstance( hexagonalCellParam, &arg ) );

#ifdef __EXCEPTIONS__
    HexagonalCellBlock<> *hexBlock = dynamic_cast< HexagonalCellBlock<> * >( hexagonalCellBlock.get() );
    TS_ASSERT_EQUALS( hexBlock == 0, false );
#else
    HexagonalCellBlock<> *hexBlock = static_cast< HexagonalCellBlock<> * >( hexagonalCellBlock.get() );
#endif
    TS_ASSERT_DELTA( hexBlock->mRadialEmplacementPoint.GetX(), 20.0 + 0.1, sDelta );
    TS_ASSERT_DELTA( hexBlock->mRadialEmplacementPoint.GetY(), 21.0 + 0.1 * sqrt( 0.75 ), sDelta );
    TS_ASSERT_DELTA( hexBlock->mRadialEmplacementPoint.GetZ(), 22.0, sDelta );
    TS_ASSERT_DELTA( hexBlock->mLength, 0.1, sDelta );
    TS_ASSERT_DELTA( hexBlock->mRadiusCell, 0.04, sDelta );
    TS_ASSERT_DELTA( hexBlock->mHeight, 0.15, sDelta );
    TS_ASSERT_EQUALS( hexBlock->mNRhoCell, 2 );
    TS_ASSERT_EQUALS( hexBlock->mNPhi, 6 );
    TS_ASSERT_EQUALS( hexBlock->mNZ, 3 );
    TS_ASSERT_DELTA( hexBlock->mArcPolygonEdgesLength, 0.005, sDelta );
    TS_ASSERT_DELTA( hexBlock->mCellMaterial->GetDensity(), 50.0, sDelta );
    TS_ASSERT_DELTA( hexBlock->mFillMaterial->GetDensity(), 100.0, sDelta );
    TS_ASSERT_DELTA( hexBlock->mInitialTemperature, 3.0, sDelta );

    TS_ASSERT_EQUALS( hexBlock->mElectricalDiscretizationPhi, 2 );
    TS_ASSERT_EQUALS( hexBlock->mElectricalDiscretizationZ, 3 );
    TS_ASSERT_EQUALS( hexBlock->mThermalStates.size(), 6 );
    for ( size_t i = 0; i < 6; ++i )
        TS_ASSERT_DELTA( hexBlock->mThermalStates.at( i )->GetPowerDissipation( 2.0, 1.0 ), 2000.0, sDelta );


    const char *xmlTriangularBlock =
     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
        <BlockType class=\"TriangularPrismBlock\">\
            <ElectricalDiscretization z=\"3\"/>\
            <Vertices> 0.1, 0.2; -0.1, -0.2; 1.5, 4.5</Vertices>\
            <Height>0.15</Height>\
            <HeightDivision>3</HeightDivision>\
            <Material cacheref=\"Copper\"/>\
            <InitialTemperature>3</InitialTemperature>\
        </BlockType>";

    xmlparser::tinyxml2::XmlParserImpl triangularBlockParser;
    triangularBlockParser.ReadFromMem( xmlTriangularBlock );
    boost::shared_ptr< xmlparser::XmlParameter > triangularBlockParam( triangularBlockParser.GetRoot() );

    arg.mThermalStates.pop_back();
    arg.mThermalStates.pop_back();
    arg.mThermalStates.pop_back();
    boost::shared_ptr< ThermalBlock<> > triangularBlock( blockFactory->CreateInstance( triangularBlockParam, &arg ) );

#ifdef __EXCEPTIONS__
    TriangularPrismBlock<> *triangBlock = dynamic_cast< TriangularPrismBlock<> * >( triangularBlock.get() );
    TS_ASSERT_EQUALS( triangBlock == 0, false );
#else
    TriangularPrismBlock<> *triangBlock = static_cast< TriangularPrismBlock<> * >( triangularBlock.get() );
#endif
    TS_ASSERT_DELTA( triangBlock->mEmplacementPoint.GetX(), 20.0, sDelta );
    TS_ASSERT_DELTA( triangBlock->mEmplacementPoint.GetY(), 21.0, sDelta );
    TS_ASSERT_DELTA( triangBlock->mEmplacementPoint.GetZ(), 22.0, sDelta );
    TS_ASSERT_DELTA( triangBlock->mVertex1.Get1(), 0.1, sDelta );
    TS_ASSERT_DELTA( triangBlock->mVertex1.Get2(), 0.2, sDelta );
    TS_ASSERT_DELTA( triangBlock->mVertex2.Get1(), -0.1, sDelta );
    TS_ASSERT_DELTA( triangBlock->mVertex2.Get2(), -0.2, sDelta );
    TS_ASSERT_DELTA( triangBlock->mVertex3.Get1(), 1.5, sDelta );
    TS_ASSERT_DELTA( triangBlock->mVertex3.Get2(), 4.5, sDelta );
    TS_ASSERT_DELTA( triangBlock->mDimZ, 0.05, sDelta );
    TS_ASSERT_EQUALS( triangBlock->mNZ, 3 );
    TS_ASSERT_DELTA( triangBlock->mMaterial->GetDensity(), 50.0, sDelta );
    TS_ASSERT_DELTA( triangBlock->mInitialTemperature, 3.0, sDelta );

    TS_ASSERT_EQUALS( hexBlock->mElectricalDiscretizationZ, 3 );
    TS_ASSERT_EQUALS( triangBlock->mThermalStates.size(), 3 );
    for ( size_t i = 0; i < 3; ++i )
        TS_ASSERT_DELTA( triangBlock->mThermalStates.at( i )->GetPowerDissipation( 2.0, 1.0 ), 2000.0, sDelta );
}


void TestFactories::TestThermalFactory()
{
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromFile( "./TestThermalFactories.xml" );
    boost::shared_ptr< xmlparser::XmlParameter > param( parser.GetRoot() );

    vector< boost::shared_ptr< ThermalBlock<> > > heatedBlocks;
    vector< boost::shared_ptr< ThermalBlock<> > > unheatedBlocks;
    vector< boost::shared_ptr< CoolingBlock<> > > coolingBlocks;

    vector< ::probe::ThermalProbe > thermalProbes;

    vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 26 );
    for ( size_t i = 0; i < 26; ++i )
        thermalStates.at( i ).reset( new ThermalState<> );

    scoped_ptr< factory::Factory< Material< double > > > materialFactory;
    materialFactory.reset( factory::BuildMaterialFactory< double >() );
    scoped_ptr< factory::Factory< ThermalBlock< double >, factory::ArgumentTypeThermalBlock< double > > > blockFactory;
    blockFactory.reset( factory::BuildBlockFactory< double >( materialFactory.get() ) );
    scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory;
    stateFactory.reset( factory::BuildStateFactory() );
    scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory;
    objectFactory.reset( factory::BuildObjectFactory< double >( stateFactory.get() ) );
    scoped_ptr< factory::Factory< Cooling< double > > > coolingFactory;
    coolingFactory.reset( factory::BuildCoolingFactory< double >( objectFactory.get() ) );
    scoped_ptr< factory::Factory< CoolingBlock< double >, factory::ArgumentTypeCoolingBlock< double > > > coolingBlockFactory;
    coolingBlockFactory.reset( factory::BuildCoolingBlockFactory< double >( coolingFactory.get() ) );
    ThermalFactory<> thermalFactory( materialFactory.get(), blockFactory.get(), coolingFactory.get(), coolingBlockFactory.get() );


    thermalFactory.CreateThermalModel( param, heatedBlocks, unheatedBlocks, coolingBlocks, 0, 0, &thermalProbes );
    TS_ASSERT_EQUALS( heatedBlocks.size(), 26 );
    TS_ASSERT_EQUALS( unheatedBlocks.size(), 1 );
    TS_ASSERT_EQUALS( coolingBlocks.size(), 3 );
    TS_ASSERT_EQUALS( thermalProbes.size(), 2 );

    TS_ASSERT_EQUALS( thermalProbes.at( 0 ).mProbeCoordinates.GetX(), 0 );
    TS_ASSERT_EQUALS( thermalProbes.at( 0 ).mProbeCoordinates.GetY(), 0 );
    TS_ASSERT_EQUALS( thermalProbes.at( 0 ).mProbeCoordinates.GetZ(), 0 );

    TS_ASSERT_DELTA( thermalProbes.at( 1 ).mProbeCoordinates.GetX(), 1, 0.00001 );
    TS_ASSERT_DELTA( thermalProbes.at( 1 ).mProbeCoordinates.GetY(), 1, 0.00001 );
    TS_ASSERT_DELTA( thermalProbes.at( 1 ).mProbeCoordinates.GetZ(), 1, 0.00001 );


    TS_ASSERT( coolingBlocks.at( 0 )->GetCooling()->IsDirichletBoundaryCondition() );
    thermal::Linearization< double > linearization = coolingBlocks.at( 0 )->GetCooling()->GetOffsetSlope( 0.0, 0.0 );
    TS_ASSERT_DELTA( linearization.mOffset, 10.0, sDelta );
    TS_ASSERT_DELTA( linearization.mSlope, 10.0, sDelta );

    size_t j = 0;
    for ( size_t countZ = 0; countZ < 2; ++countZ )
    {
        TS_ASSERT( !coolingBlocks.at( 1 + countZ )->GetCooling()->IsDirichletBoundaryCondition() );
        linearization = coolingBlocks.at( 1 + countZ )->GetCooling()->GetOffsetSlope( 0.0, 0.0 );
        TS_ASSERT_DELTA( linearization.mOffset, 13.4, sDelta );
        TS_ASSERT_DELTA( linearization.mSlope, 0.0, sDelta );

        boost::shared_ptr< thermal::BlockGeometry<> > blockGeometry;
        coolingBlocks.at( 1 + countZ )->GetBlockGeometry( blockGeometry );
        double zLower = 0.0, zUpper = 0.0;
        TwoDim<> lowestEdge( 0.0, 0.0 ), highestEdge( 0.0, 0.0 );
        blockGeometry->GetInternalData( zLower, zUpper )->Envelope( lowestEdge, highestEdge );
        TS_ASSERT_DELTA( zLower, 0.07 + 0.2 * countZ, sDelta );
        TS_ASSERT_DELTA( zUpper, 0.13 + 0.2 * countZ, sDelta );
        TS_ASSERT_DELTA( lowestEdge.Get1(), -1.0, sDelta );
        TS_ASSERT_DELTA( lowestEdge.Get2(), 0.0, sDelta );
        TS_ASSERT_DELTA( highestEdge.Get1(), -0.9, sDelta );
        TS_ASSERT_DELTA( highestEdge.Get2(), 0.1, sDelta );

        for ( size_t countY = 0; countY < 3; ++countY )
            for ( size_t countX = 0; countX < 2; ++countX )
                for ( size_t childrenX = 0; childrenX < 2; ++childrenX )
                {
                    const double dX = countX * 0.2 + childrenX * 0.1;
                    const double dY = countY * 0.1;
                    const double dZ = countZ * 0.2;
#ifdef __EXCEPTIONS__
                    QuadraticCellBlock<> *heatedBlockPtr = dynamic_cast< QuadraticCellBlock<> * >( heatedBlocks.at( j ).get() );
                    TS_ASSERT_EQUALS( heatedBlockPtr == 0, false );
#else
                    QuadraticCellBlock<> *heatedBlockPtr = static_cast< QuadraticCellBlock<> * >( heatedBlocks.at( j ).get() );
                    ;
#endif
                    TS_ASSERT( heatedBlockPtr->mRadialEmplacementPoint.Equals( Cartesian<>( dX + 0.05, dY + 0.05, dZ ), sDelta ) );
                    ++j;
                }

        const double dZForOhmic = countZ * 0.2;
#ifdef __EXCEPTIONS__
        RectangularBlock<> *heatedBlockPtr = dynamic_cast< RectangularBlock<> * >( heatedBlocks.at( j ).get() );
        TS_ASSERT_EQUALS( heatedBlockPtr == 0, false );
#else
        RectangularBlock<> *heatedBlockPtr = static_cast< RectangularBlock<> * >( heatedBlocks.at( j ).get() );
        ;
#endif
        TS_ASSERT( heatedBlockPtr->GetEmplacementPoint().Equals( Cartesian<>( 0.0, -1.0, dZForOhmic ), sDelta ) );
        ++j;
    }

#ifdef __EXCEPTIONS__
    RectangularBlock<> *unheatedBlockPtr = dynamic_cast< RectangularBlock<> * >( unheatedBlocks.at( 0 ).get() );
    TS_ASSERT_EQUALS( unheatedBlockPtr == 0, false );
#else
    RectangularBlock<> *unheatedBlockPtr = static_cast< RectangularBlock<> * >( unheatedBlocks.at( 0 ).get() );
#endif
    TS_ASSERT( unheatedBlockPtr->mEmplacementPoint.Equals( Cartesian<>( 0.05, 0.05, 0.4 ), sDelta ) );


    thermalFactory.CreateThermalModel( param, heatedBlocks, unheatedBlocks, coolingBlocks, &thermalStates, 0 );
    TS_ASSERT_EQUALS( heatedBlocks.size(), 26 );
    TS_ASSERT_EQUALS( unheatedBlocks.size(), 1 );
    TS_ASSERT_EQUALS( coolingBlocks.size(), 3 );

    TS_ASSERT( coolingBlocks.at( 0 )->GetCooling()->IsDirichletBoundaryCondition() );
    linearization = coolingBlocks.at( 0 )->GetCooling()->GetOffsetSlope( 0.0, 0.0 );
    TS_ASSERT_DELTA( linearization.mOffset, 10.0, sDelta );
    TS_ASSERT_DELTA( linearization.mSlope, 10.0, sDelta );

    j = 0;
    for ( size_t countZ = 0; countZ < 2; ++countZ )
    {
        TS_ASSERT( !coolingBlocks.at( 1 + countZ )->GetCooling()->IsDirichletBoundaryCondition() );
        linearization = coolingBlocks.at( 1 + countZ )->GetCooling()->GetOffsetSlope( 0.0, 0.0 );
        TS_ASSERT_DELTA( linearization.mOffset, 13.4, sDelta );
        TS_ASSERT_DELTA( linearization.mSlope, 0.0, sDelta );

        boost::shared_ptr< thermal::BlockGeometry<> > blockGeometry;
        coolingBlocks.at( 1 + countZ )->GetBlockGeometry( blockGeometry );
        double zLower = 0.0, zUpper = 0.0;
        TwoDim<> lowestEdge( 0.0, 0.0 ), highestEdge( 0.0, 0.0 );
        blockGeometry->GetInternalData( zLower, zUpper )->Envelope( lowestEdge, highestEdge );
        TS_ASSERT_DELTA( zLower, 0.07 + 0.2 * countZ, sDelta );
        TS_ASSERT_DELTA( zUpper, 0.13 + 0.2 * countZ, sDelta );
        TS_ASSERT_DELTA( lowestEdge.Get1(), -1.0, sDelta );
        TS_ASSERT_DELTA( lowestEdge.Get2(), 0.0, sDelta );
        TS_ASSERT_DELTA( highestEdge.Get1(), -0.9, sDelta );
        TS_ASSERT_DELTA( highestEdge.Get2(), 0.1, sDelta );

        for ( size_t countY = 0; countY < 3; ++countY )
            for ( size_t countX = 0; countX < 2; ++countX )
                for ( size_t childrenX = 0; childrenX < 2; ++childrenX )
                {
                    const double dX = countX * 0.2 + childrenX * 0.1;
                    const double dY = countY * 0.1;
                    const double dZ = countZ * 0.2;
#ifdef __EXCEPTIONS__
                    QuadraticCellBlock<> *heatedBlockPtr = dynamic_cast< QuadraticCellBlock<> * >( heatedBlocks.at( j ).get() );
                    TS_ASSERT_EQUALS( heatedBlockPtr == 0, false );
#else
                    QuadraticCellBlock<> *heatedBlockPtr = static_cast< QuadraticCellBlock<> * >( heatedBlocks.at( j ).get() );
#endif
                    TS_ASSERT( heatedBlockPtr->mRadialEmplacementPoint.Equals( Cartesian<>( dX + 0.05, dY + 0.05, dZ ), sDelta ) );
                    TS_ASSERT_EQUALS( heatedBlockPtr->mThermalStates.at( 0 ), thermalStates.at( j ) );
                    ++j;
                }

        const double dZForOhmic = countZ * 0.2;
#ifdef __EXCEPTIONS__
        RectangularBlock<> *heatedBlockPtr = dynamic_cast< RectangularBlock<> * >( heatedBlocks.at( j ).get() );
        TS_ASSERT_EQUALS( heatedBlockPtr == 0, false );
#else
        RectangularBlock<> *heatedBlockPtr = static_cast< RectangularBlock<> * >( heatedBlocks.at( j ).get() );
        ;
#endif
        TS_ASSERT( heatedBlockPtr->GetEmplacementPoint().Equals( Cartesian<>( 0.0, -1.0, dZForOhmic ), sDelta ) );
        ++j;
    }

#ifdef __EXCEPTIONS__
    unheatedBlockPtr = dynamic_cast< RectangularBlock<> * >( unheatedBlocks.at( 0 ).get() );
    TS_ASSERT_EQUALS( unheatedBlockPtr == 0, false );
#else
    unheatedBlockPtr = static_cast< RectangularBlock<> * >( unheatedBlocks.at( 0 ).get() );
#endif
    TS_ASSERT( unheatedBlockPtr->mEmplacementPoint.Equals( Cartesian<>( 0.05, 0.05, 0.4 ), sDelta ) );


    //#if defined(__EXCEPTIONS__)
    {
        thermalStates.resize( 27 );
        TS_ASSERT_THROWS_EQUALS( thermalFactory.CreateThermalModel( param, heatedBlocks, unheatedBlocks, coolingBlocks,
                                                                    &thermalStates, 0 ),
                                 const std::runtime_error &e,
                                 strcmp( e.what(),
                                         "More thermal states passed than the number of states, which were used while "
                                         "parsing the electrical model and assigning thermal states.\n" ),
                                 0 );
        thermalStates.pop_back();
        thermalStates.pop_back();
        TS_ASSERT_THROWS_EQUALS( thermalFactory.CreateThermalModel( param, heatedBlocks, unheatedBlocks, coolingBlocks,
                                                                    &thermalStates, 0 ),
                                 const std::runtime_error &e,
                                 strcmp( e.what(),
                                         "Less thermal states passed than the number of states, which were used while "
                                         "parsing the electrical model and assigning thermal states.\n" ),
                                 0 );
    }
    //#endif
}
