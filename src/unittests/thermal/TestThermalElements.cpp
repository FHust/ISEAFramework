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
#include "TestThermalElements.h"
#include "exception_tester.h"

using namespace thermal;
using namespace geometry;

static const double sDelta = 0.000001;


void TestThermalElements::TestThermalElement()
{
    ThermalElement<> elem;

    TS_ASSERT_THROWS_EQUALS( elem.SetElement( 25.0, 0, Cartesian<>( 0.0, 0.0, 0.0 ), 0.0, 0, 0.0 ), std::runtime_error & e,
                             strcmp( e.what(), "ThermalStateFactor must be part of (0, 1].\n" ), 0 );
    TS_ASSERT_THROWS_EQUALS( elem.SetElement( 25.0, 0, Cartesian<>( 0.0, 0.0, 0.0 ), 0.0, 0, 1.1 ), std::runtime_error & e,
                             strcmp( e.what(), "ThermalStateFactor must be part of (0, 1].\n" ), 0 );
    TS_ASSERT_THROWS_EQUALS( elem.SetElement( 25.0, 0, Cartesian<>( 0.0, 0.0, 0.0 ), -0.1, 0 ), std::runtime_error & e,
                             strcmp( e.what(), "Volume must be zero or bigger than zero.\n" ), 0 );

    boost::shared_ptr< ::state::ThermalState<> > thermalState( new ::state::ThermalState<> );
    thermalState->AddPowerDissipation( 970.0, 0.0 );
    thermalState->ResetPowerDissipation();
    Material<> kupfer( 8920.0, 385.0, 400.0, 300.0, 200.0 );
    ThermalElement<> element;
    element.SetElement( 25.0, &kupfer, Cartesian<>( 0.0, 0.0, 0.0 ), 71.0, thermalState.get() );
    TS_ASSERT_EQUALS( thermalState->GetValue(), 25.0 );

    ThermalElement<> elem2;
    elem2.SetElement( 26.0, &kupfer, Cartesian<>( 1.0, 2.0, 3.0 ), 35.0, thermalState.get(), 0.4 );
    ThermalElement<> elemZeroHeat;
    TS_ASSERT_EQUALS( thermalState->GetValue(), ( 71.0 * 25.0 + 35.0 * 26.0 ) / 106.0 );

    elemZeroHeat.SetElement( 26.0, &kupfer, Cartesian<>( 1.0, 2.0, 3.0 ), 35.0 );
    TS_ASSERT_DELTA( thermalState->GetValue(), ( 71.0 * 25.0 + 35.0 * 26.0 ) / 106.0, sDelta );

    TS_ASSERT_EQUALS( element.GetPowerDissipation( 2.0, 1.0 ), 970.0 );
    TS_ASSERT_EQUALS( element.GetTemperature(), 25.0 );
    element.SetTemperature( 24.0 );
    TS_ASSERT_EQUALS( element.GetTemperature(), 24.0 );
    TS_ASSERT_EQUALS( element.GetGridVertex().GetX(), 0.0 );
    TS_ASSERT_EQUALS( element.GetGridVertex().GetY(), 0.0 );
    TS_ASSERT_EQUALS( element.GetGridVertex().GetZ(), 0.0 );
    TS_ASSERT_EQUALS( element.GetVolume(), 71.0 );
    TS_ASSERT_EQUALS( element.GetMaterial(), &kupfer );
    TS_ASSERT_EQUALS( element.GetThermalStateFactor(), 1.0 );
    TS_ASSERT_EQUALS( element.GetThermalState(), thermalState.get() );

    TS_ASSERT_DELTA( elem2.GetPowerDissipation( 2.0, 1.0 ), 970.0 * 0.4, 0.00001 );
    TS_ASSERT_DELTA( elem2.GetTemperature(), 26.0, 0.0000 );
    elem2.SetTemperature( 27.0 );
    TS_ASSERT_EQUALS( elem2.GetTemperature(), 27.0 );
    TS_ASSERT_EQUALS( elem2.GetGridVertex().GetX(), 1.0 );
    TS_ASSERT_EQUALS( elem2.GetGridVertex().GetY(), 2.0 );
    TS_ASSERT_EQUALS( elem2.GetGridVertex().GetZ(), 3.0 );
    TS_ASSERT_EQUALS( elem2.GetVolume(), 35.0 );
    TS_ASSERT_EQUALS( elem2.GetMaterial(), &kupfer );
    TS_ASSERT_EQUALS( elem2.GetThermalStateFactor(), 0.4 );
    TS_ASSERT_EQUALS( elem2.GetThermalState(), thermalState.get() );

    TS_ASSERT_EQUALS( elemZeroHeat.GetPowerDissipation( 2.0, 1.0 ), 0.0 );
    TS_ASSERT_EQUALS( elemZeroHeat.GetThermalStateFactor(), 1.0 );
    TS_ASSERT( !elemZeroHeat.GetThermalState() );

    TS_ASSERT( element.HasThermalState() );
    TS_ASSERT( elem2.HasThermalState() );
    TS_ASSERT_EQUALS( elemZeroHeat.HasThermalState(), false );

    TS_ASSERT_EQUALS( element.GetTemperature(), 24.0 );
    TS_ASSERT_EQUALS( elem2.GetTemperature(), 27.0 );
    thermalState->ResetTemperature();
    element.UpdateThermalStateTemperature();
    elem2.UpdateThermalStateTemperature();
    TS_ASSERT_DELTA( thermalState->GetValue(), ( 71.0 * 24.0 + 35.0 * 27.0 ) / 106.0, sDelta );
}

void TestThermalElements::TestRectangularElement()
{
    boost::shared_ptr< ::state::ThermalState<> > thermalState( new ::state::ThermalState<> );
    thermalState->AddPowerDissipation( 10.0, 0.0 );
    thermalState->ResetPowerDissipation();
    Material<> kupfer( 8920.0, 385.0, 400.0, 300.0, 200.0 );
    RectangularElement<> element( Cartesian<>( 1.0, 2.0, 3.0 ), Cartesian<>( 44.2, -2.4, -0.2 ), 25.0, &kupfer,
                                  thermalState.get() );
    RectangularElement<> elemZeroHeat( Cartesian<>( 1.0, -1.5, 4.9 ), Cartesian<>( 34.8, 21.8, -2.8 ), 27.0, &kupfer );

    TS_ASSERT_DELTA( element.GetGridVertex().GetX(), 23.1, sDelta );
    TS_ASSERT_DELTA( element.GetGridVertex().GetY(), 0.8, sDelta );
    TS_ASSERT_DELTA( element.GetGridVertex().GetZ(), 2.9, sDelta );
    TS_ASSERT_DELTA( element.GetVolume(), 21.216, sDelta );

    TS_ASSERT_DELTA( elemZeroHeat.GetGridVertex().GetX(), 18.4, sDelta );
    TS_ASSERT_DELTA( elemZeroHeat.GetGridVertex().GetY(), 9.4, sDelta );
    TS_ASSERT_DELTA( elemZeroHeat.GetGridVertex().GetZ(), 3.5, sDelta );
    TS_ASSERT_DELTA( elemZeroHeat.GetVolume(), 2124.192, sDelta );

    TS_ASSERT_EQUALS( element.GetPowerDissipation( 2.0, 1.0 ), 10.0 );
    TS_ASSERT_EQUALS( element.GetTemperature(), 25.0 );
    TS_ASSERT_EQUALS( element.GetMaterial(), &kupfer );

    TS_ASSERT_EQUALS( elemZeroHeat.GetPowerDissipation( 2.0, 1.0 ), 0.0 );
    TS_ASSERT_EQUALS( elemZeroHeat.GetTemperature(), 27.0 );
    TS_ASSERT_EQUALS( elemZeroHeat.GetMaterial(), &kupfer );
}

void TestThermalElements::TestCylindricElement()
{
    //#if defined(__EXCEPTIONS__)
    {

        TS_ASSERT_THROWS_EQUALS( CylindricElement<> elem( Cartesian<>( 0.0, 0.0, 0.0 ), Cylindric<>( 0.0, Angle<>::Rad( 0.0 ), 0.0 ),
                                                          Cylindric<>( 0.0, Angle<>::Deg( 180.02 ), 0.0 ), 25.5, 0, 0 ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "deltaPhi must be 180 deg or smaller than 180 deg.\n" ), 0 );

        TS_ASSERT_THROWS_EQUALS( CylindricElement<> elem( Cartesian<>( 0.0, 0.0, 0.0 ), Cylindric<>( -1.0, Angle<>::Rad( 0.0 ), 0.0 ),
                                                          Cylindric<>( 1.5, Angle<>::Deg( 90.0 ), 0.0 ), 25.5, 0, 0 ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "mRho of startVector must be zero or bigger than zero.\n" ), 0 );

        TS_ASSERT_THROWS_EQUALS( CylindricElement<> elem( Cartesian<>( 0.0, 0.0, 0.0 ), Cylindric<>( 1.0, Angle<>::Rad( 0.0 ), 0.0 ),
                                                          Cylindric<>( -1.5, Angle<>::Deg( 90.0 ), 0.0 ), 25.5, 0, 0 ),
                                 std::runtime_error & e, strcmp( e.what(),
                                                                 "Difference between mRho of startVector and "
                                                                 "deltaVector must be zero or bigger than zero.\n" ),
                                 0 );

        /*
                TS_ASSERT_THROWS_EQUALS(CylindricElement<> elem(Cartesian<>(0.0, 0.0, 0.0), Cylindric<>(0.0,
           Angle<>::Rad(0.0), 0.0)
                  , Cylindric<>(0.0, Angle<>::Deg(180.02), 0.0), 25.5, 0, 0), const std::runtime_error &e,
                    CmpInit(e, "deltaPhi must be 180 deg or smaller than 180 deg", "CylindricElement"), 0);

                TS_ASSERT_THROWS_EQUALS(CylindricElement<> elem(Cartesian<>(0.0, 0.0, 0.0), Cylindric<>(-1.0,
           Angle<>::Rad(0.0), 0.0)
                  , Cylindric<>(1.5, Angle<>::Deg(90.0), 0.0), 25.5, 0, 0), const std::runtime_error &e,
                    CmpInit(e, "mRho of startVector must be zero or bigger than zero", "CylindricElement"), 0);
                TS_ASSERT_THROWS_EQUALS(CylindricElement<> elem(Cartesian<>(0.0, 0.0, 0.0), Cylindric<>(1.0,
           Angle<>::Rad(0.0), 0.0)
                  , Cylindric<>(-1.5, Angle<>::Deg(90.0), 0.0), 25.5, 0, 0), const std::runtime_error &e,
                    CmpInit(e, "Difference between mRho of startVector and deltaVector must be zero or bigger than
           zero", "CylindricElement"), 0);
        */
    }
    //#endif


    boost::shared_ptr< ::state::ThermalState<> > thermalState( new ::state::ThermalState<> );
    thermalState->AddPowerDissipation( 10.0, 0.0 );
    thermalState->ResetPowerDissipation();
    Material<> kupfer( 8920.0, 385.0, 400.0, 300.0, 200.0 );
    CylindricElement<> elem( Cartesian<>( 0.1, 0.2, 0.3 ), Cylindric<>( 0.5, Angle<>::Rad( 0.5 ), 0.7 ),
                             Cylindric<>( 1.0, Angle<>::Rad( 1.7 ), 1.1 ), 25.5, &kupfer, thermalState.get(), 0.35 );

    TS_ASSERT_DELTA( elem.GetVolume(), ( 1.5 * 1.5 - 0.5 * 0.5 ) * 1.7 / 2.0 * 1.1, sDelta );

    TS_ASSERT_DELTA( elem.GetGridVertex().GetX(), 0.1 + ( 0.5 + 1.0 / 2.0 ) * cos( 0.5 + 1.7 / 2.0 ), sDelta );
    TS_ASSERT_DELTA( elem.GetGridVertex().GetY(), 0.2 + ( 0.5 + 1.0 / 2.0 ) * sin( 0.5 + 1.7 / 2.0 ), sDelta );
    TS_ASSERT_DELTA( elem.GetGridVertex().GetZ(), 0.3 + 0.7 + 1.1 / 2.0, sDelta );

    TS_ASSERT_EQUALS( elem.GetTemperature(), 25.5 );

    TS_ASSERT_EQUALS( elem.GetMaterial(), &kupfer );

    TS_ASSERT_DELTA( elem.GetPowerDissipation( 2.0, 1.0 ), 0.35 * 10.0, sDelta );


    /*CylindricElement<> elemFullCylinder(Cartesian<>(0.0, 0.0, 0.0)
                          , Cylindric<>(0.0, Angle<>::Rad(0.0), 1.0)
                          , Cylindric<>(2.0, Angle<>::circle, 2.5)
                          , 25.5
                          , 0);

    TS_ASSERT_DELTA(elem.GetVolume(), 1.5 * Angle<>::pi * 2.0 * 2.0, sDelta);

    TS_ASSERT_DELTA(elem.GetGridVertex().GetX(), 0.0, sDelta);
    TS_ASSERT_DELTA(elem.GetGridVertex().GetY(), 0.0, sDelta);
    TS_ASSERT_DELTA(elem.GetGridVertex().GetZ(), 1.75;*/
}

void TestThermalElements::TestCutCylElement()
{
    //#if defined(__EXCEPTIONS__)
    {

        TS_ASSERT_THROWS_EQUALS( CutCylElement<> elem1( Cartesian<>( 1.0, 1.5, 5.0 ),
                                                        Cylindric<>( 1.5, Angle<>::Deg( 22.5 ), 1.0 ), Angle<>::Deg( 90.1 ),
                                                        2.4, TwoDim<>( 3.0, 2.0 ), TwoDim<>( 2.0, -2.0 ), 27.0, 0 ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "deltaPhi must be 90 deg or smaller than 90 deg.\n" ), 0 );
        TS_ASSERT_THROWS_EQUALS( CutCylElement<> elem1( Cartesian<>( 1.0, 1.5, 5.0 ),
                                                        Cylindric<>( -1.5, Angle<>::Deg( 22.5 ), 1.0 ), Angle<>::Deg( 90.0 ),
                                                        2.4, TwoDim<>( 3.0, 2.0 ), TwoDim<>( 2.0, -2.0 ), 27.0, 0 ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "mRho of startVector must be zero or bigger than zero.\n" ), 0 );
        TS_ASSERT_THROWS_EQUALS(
         CutCylElement<> elem1( Cartesian<>( 1.0, 1.5, 5.0 ), Cylindric<>( 1.5, Angle<>::Deg( 22.5 ), 1.0 ),
                                Angle<>::Deg( 90.0 ), 2.4, TwoDim<>( 3.0, 2.0 ), TwoDim<>( 0.0, 0.0 ), 27.0, 0 ),
         std::runtime_error & e,
         strcmp( e.what(), "cutLineDirection: Get1() and Get2() are zero, thus there is no direction.\n" ), 0 );
        TS_ASSERT_THROWS_EQUALS(
         CutCylElement<> elem2( Cartesian<>( 1.0, 1.5, 5.0 ), Cylindric<>( 1.5, Angle<>::Deg( 22.5 ), 1.0 ),
                                Angle<>::Deg( 45.0 ), 2.4, TwoDim<>( -3.0, -3.0 ), TwoDim<>( 2.0, -2.0 ), 27.0, 0 ),
         std::runtime_error & e,
         strcmp( e.what(), "cutLine does not cut element to three straight and one arc line.\n" ), 0 );
        TS_ASSERT_THROWS_EQUALS(
         CutCylElement<> elem3( Cartesian<>( 1.0, 1.5, 5.0 ), Cylindric<>( 1.5, Angle<>::Deg( 22.5 ), 1.0 ),
                                Angle<>::Deg( 45.0 ), 2.4, TwoDim<>( 2.0, 0.0 ), TwoDim<>( 2.0, -2.0 ), 27.0, 0 ),
         std::runtime_error & e,
         strcmp( e.what(), "cutLine does not cut element to three straight and one arc line.\n" ), 0 );


        /*
                TS_ASSERT_THROWS_EQUALS(CutCylElement<> elem1(Cartesian<>(1.0, 1.5, 5.0), Cylindric<>(1.5,
           Angle<>::Deg(22.5), 1.0)
                  , Angle<>::Deg(90.1), 2.4, TwoDim<>(3.0, 2.0), TwoDim<>(2.0, - 2.0), 27.0, 0), const
           std::runtime_error &e,
                    CmpInit(e, "deltaPhi must be 90 deg or smaller than 90 deg", "CutCylElement"), 0);
                TS_ASSERT_THROWS_EQUALS(CutCylElement<> elem1(Cartesian<>(1.0, 1.5, 5.0), Cylindric<>(- 1.5,
           Angle<>::Deg(22.5), 1.0)
                  , Angle<>::Deg(90.0), 2.4, TwoDim<>(3.0, 2.0), TwoDim<>(2.0, - 2.0), 27.0, 0), const
           std::runtime_error &e,
                    CmpInit(e, "mRho of startVector must be zero or bigger than zero", "CutCylElement"), 0);
                TS_ASSERT_THROWS_EQUALS(CutCylElement<> elem1(Cartesian<>(1.0, 1.5, 5.0), Cylindric<>(1.5,
           Angle<>::Deg(22.5), 1.0)
                  , Angle<>::Deg(90.0), 2.4, TwoDim<>(3.0, 2.0), TwoDim<>(0.0, 0.0), 27.0, 0), const std::runtime_error
           &e,
                    CmpInit(e, "cutLineDirection: Get1() and Get2() are zero, thus there is no direction",
           "CutCylElement"), 0);
                TS_ASSERT_THROWS_EQUALS(CutCylElement<> elem2(Cartesian<>(1.0, 1.5, 5.0), Cylindric<>(1.5,
           Angle<>::Deg(22.5), 1.0)
                  , Angle<>::Deg(45.0), 2.4, TwoDim<>(- 3.0, - 3.0), TwoDim<>(2.0, - 2.0), 27.0, 0), const
           std::runtime_error &e,
                    CmpInit(e, "cutLine doesn't cut element to three straight and one arc line", "CutCylElement"), 0);
                TS_ASSERT_THROWS_EQUALS(CutCylElement<> elem3(Cartesian<>(1.0, 1.5, 5.0), Cylindric<>(1.5,
           Angle<>::Deg(22.5), 1.0)
                  , Angle<>::Deg(45.0), 2.4, TwoDim<>(2.0, 0.0), TwoDim<>(2.0, - 2.0), 27.0, 0), const
           std::runtime_error &e,
                    CmpInit(e, "cutLine doesn't cut element to three straight and one arc line", "CutCylElement"), 0);
        */
    }
    //#endif


    boost::shared_ptr< ::state::ThermalState<> > thermalState( new ::state::ThermalState<> );
    thermalState->AddPowerDissipation( 20.0, 0.0 );
    thermalState->ResetPowerDissipation();
    thermal::Material<> kupfer( 8920.0, 385.0, 400.0, 410.0, 420.0 );
    CutCylElement<> element( Cartesian<>( 1.0, 1.5, 5.0 ), Cylindric<>( 1.5, Angle<>::Deg( 22.5 ), 1.0 ), Angle<>::Deg( 45.0 ),
                             2.4, TwoDim<>( 3.0, 2.0 ), TwoDim<>( 2.0, -2.0 ), 27.0, &kupfer, thermalState.get(), 0.4 );

    double phi = Angle<>::pi / 8.0;
    TwoDim<> twoDim1( 5.0 / ( tan( phi ) + 1 ), 5.0 * tan( phi ) / ( tan( phi ) + 1 ) );
    phi = Angle<>::pi * 3.0 / 8.0;
    TwoDim<> twoDim2( 5.0 / ( tan( phi ) + 1 ), 5.0 * tan( phi ) / ( tan( phi ) + 1 ) );

    TS_ASSERT_DELTA( twoDim1.DistanceTo( element.GetStraightEdgeVertexStart() ), 0.0, sDelta );
    TS_ASSERT_DELTA( twoDim2.DistanceTo( element.GetStraightEdgeVertexDelta() ), 0.0, sDelta );

    TS_ASSERT_DELTA( element.GetStraightEdgeVertexStart().Get1(), 3.5355339059, sDelta );
    TS_ASSERT_DELTA( element.GetStraightEdgeVertexStart().Get2(), 1.4644660941, sDelta );
    TS_ASSERT_DELTA( element.GetStraightEdgeVertexDelta().Get1(), 1.4644660941, sDelta );
    TS_ASSERT_DELTA( element.GetStraightEdgeVertexDelta().Get2(), 3.5355339059, sDelta );


    Cartesian<> gridVertex = CylToCart( Cartesian<>(), Cylindric<>( 1.5, Angle<>::Deg( 45.0 ), 1.0 + 2.4 / 2.0 ) ) * 2.0 +
                             Cartesian<>( 3.5355339059, 1.4644660941, 1.0 + 2.4 / 2.0 ) +
                             Cartesian<>( 1.4644660941, 3.5355339059, 1.0 + 2.4 / 2.0 );
    gridVertex = gridVertex / 4.0;
    TwoDim<> twoDimGridVertex( gridVertex.GetX(), gridVertex.GetY() );
    gridVertex = gridVertex + Cartesian<>( 1.0, 1.5, 5.0 );

    TS_ASSERT_DELTA( element.GetGridVertex().GetX(), gridVertex.GetX(), sDelta );
    TS_ASSERT_DELTA( element.GetGridVertex().GetY(), gridVertex.GetY(), sDelta );
    TS_ASSERT_DELTA( element.GetGridVertex().GetZ(), gridVertex.GetZ(), sDelta );


    TS_ASSERT_DELTA( element.GetPhiNegDistance(), twoDimGridVertex.Length() * Angle<>::Deg( 22.5 ).GetRad(), sDelta );
    TS_ASSERT_DELTA( element.GetPhiPosDistance(), twoDimGridVertex.Length() * Angle<>::Deg( 22.5 ).GetRad(), sDelta );
    TS_ASSERT_DELTA( element.GetRhoNegDistance(), twoDimGridVertex.Length() - 1.5, sDelta );
    TS_ASSERT_DELTA( element.GetRhoPosDistance(), 2.5 * sqrt( 2.0 ) - twoDimGridVertex.Length(), sDelta );
    const double straightEdgeHalfLength = ( sqrt( 3.5355339059 * 3.5355339059 + 1.4644660941 * 1.4644660941 ) - 1.5 ) * 0.5;
    TS_ASSERT_DELTA( element.GetRhoNegAveragedDistance(), sqrt( straightEdgeHalfLength * element.GetRhoNegDistance() ), sDelta );
    TS_ASSERT_DELTA( element.GetRhoPosAveragedDistance(), sqrt( straightEdgeHalfLength * element.GetRhoPosDistance() ), sDelta );


    double triangleArea =
     0.5 * 2.5 * sqrt( 2.0 ) * element.GetStraightEdgeVertexStart().DistanceTo( element.GetStraightEdgeVertexDelta() );
    double circleArea = Angle<>::pi * 1.5 * 1.5 / 8.0;
    TS_ASSERT_DELTA( element.GetZAreaValue(), triangleArea - circleArea, sDelta );
    TS_ASSERT_DELTA( element.GetVolume(), ( triangleArea - circleArea ) * 2.4, sDelta );


    TS_ASSERT_EQUALS( element.GetTemperature(), 27.0 );
    TS_ASSERT_EQUALS( element.GetMaterial(), &kupfer );
    TS_ASSERT_DELTA( element.GetPowerDissipation( 2.0, 1.0 ), 0.4 * 20.0, sDelta );


    CutCylElement<> elemTestRhoPos( Cartesian<>(), Cylindric<>( 1.5, Angle<>::Deg( 0.0 ), 0.0 ), Angle<>::Deg( 45.0 ), 2.0,
                                    TwoDim<>( 3.0, 0.0 ), TwoDim<>( 0.0, 3.0 ), 27.0, &kupfer, thermalState.get(), 0.4 );
    gridVertex = elemTestRhoPos.GetGridVertex();
    Angle<> angle = TwoDim<>( gridVertex.GetX(), gridVertex.GetY() ).AngleInXY();
    TS_ASSERT_DELTA( elemTestRhoPos.GetRhoPosDistance(),
                     ( 3.0 - gridVertex.GetX() ) * angle.GetRad() / sin( angle.GetRad() ), sDelta );
}

void TestThermalElements::TestTriangularElement()
{
    //#if defined(__EXCEPTIONS__)
    {
        TS_ASSERT_THROWS_EQUALS( TriangularElement<> elemZeroHeat( Cartesian<>( 0.0, 0.0, 0.0 ), TwoDim<>( 0.0, 0.0 ),
                                                                   TwoDim<>( 2.0, 2.0 ), TwoDim<>( 1.0, 1.0 ), 0.0, 0.0, 0 ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "vertices 1, 2 and 3 form a line or a point and not an area.\n" ), 0 );
        TS_ASSERT_THROWS_EQUALS( TriangularElement<> elemZeroHeat( Cartesian<>( 0.0, 0.0, 0.0 ), TwoDim<>( 0.0, 0.0 ),
                                                                   TwoDim<>( 2.0, 2.0 ), TwoDim<>( 2.0, 2.0 ), 0.0, 0.0, 0 ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "vertices 1, 2 and 3 form a line or a point and not an area.\n" ), 0 );


        /*
                TS_ASSERT_THROWS_EQUALS(TriangularElement<> elemZeroHeat(Cartesian<>(0.0, 0.0, 0.0)
                        , TwoDim<>(0.0, 0.0), TwoDim<>(2.0, 2.0), TwoDim<>(1.0, 1.0), 0.0 , 0.0, 0), const
           std::runtime_error &e,
                          CmpInit(e, "vertices 1, 2 and 3 form a line or a point and not an area", "TriangularElement"),
           0);
                TS_ASSERT_THROWS_EQUALS(TriangularElement<> elemZeroHeat(Cartesian<>(0.0, 0.0, 0.0)
                        , TwoDim<>(0.0, 0.0), TwoDim<>(2.0, 2.0), TwoDim<>(2.0, 2.0), 0.0 , 0.0, 0), const
           std::runtime_error &e,
                          CmpInit(e, "vertices 1, 2 and 3 form a line or a point and not an area", "TriangularElement"),
           0);
        */
    }
    //#endif


    boost::shared_ptr< ::state::ThermalState<> > thermalState( new ::state::ThermalState<> );
    thermalState->AddPowerDissipation( 10.0, 0.0 );
    thermalState->ResetPowerDissipation();
    Material<> kupfer( 8920.0, 385.0, 400.0, 300.0, 200.0 );
    TriangularElement<> element( Cartesian<>( 1.0, 2.0, 3.0 ), TwoDim<>( 0.0, 0.0 ), TwoDim<>( 1.5, 0.0 ),
                                 TwoDim<>( 0.0, 1.5 ), 0.2, 25.0, &kupfer, thermalState.get(), 0.4 );
    TriangularElement<> elemZeroHeat( Cartesian<>( 1.5, -1.5, 4.9 ), TwoDim<>( 1.0, 1.0 ), TwoDim<>( 2.0, 2.0 ),
                                      TwoDim<>( 1.0, 3.0 ), 0.6, 27.0, &kupfer );


    TS_ASSERT_DELTA( element.GetGridVertex().GetX(), 1.0 + 1.5 / 3.0, sDelta );
    TS_ASSERT_DELTA( element.GetGridVertex().GetY(), 2.0 + 1.5 / 3.0, sDelta );
    TS_ASSERT_DELTA( element.GetGridVertex().GetZ(), 3.1, sDelta );
    TS_ASSERT_DELTA( element.GetVolume(), 0.2 * 1.5 * 1.5 / 2.0, sDelta );

    TS_ASSERT_DELTA( elemZeroHeat.GetGridVertex().GetX(), 1.5 + 4.0 / 3.0, sDelta );
    TS_ASSERT_DELTA( elemZeroHeat.GetGridVertex().GetY(), -1.5 + 6.0 / 3.0, sDelta );
    TS_ASSERT_DELTA( elemZeroHeat.GetGridVertex().GetZ(), 5.2, sDelta );
    TS_ASSERT_DELTA( elemZeroHeat.GetVolume(), 0.6 * 1.0 * 2.0 / 2.0, sDelta );

    TS_ASSERT_DELTA( element.GetPowerDissipation( 2.0, 1.0 ), 10.0 * 0.4, 0.00001 );
    TS_ASSERT_DELTA( element.GetTemperature(), 25.0, 0.00001 );
    TS_ASSERT_EQUALS( element.GetMaterial(), &kupfer );

    TS_ASSERT_DELTA( elemZeroHeat.GetPowerDissipation( 2.0, 1.0 ), 0.0, 0.00001 );
    TS_ASSERT_DELTA( elemZeroHeat.GetTemperature(), 27.0, 0.00001 );
    TS_ASSERT_EQUALS( elemZeroHeat.GetMaterial(), &kupfer );
}
