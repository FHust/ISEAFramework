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
#include "TestMaterials.h"
#include "exception_tester.h"

using namespace thermal;


void TestMaterials::TestMaterial()
{
    //#if defined(__EXCEPTIONS__)
    {
        TS_ASSERT_THROWS_EQUALS( Material<> kupfer( -8920.0, 385.0, 400.0, 300.0, 200.0 ), std::runtime_error & e,
                                 strcmp( e.what(), "Density must be bigger than zero." ), 0 );
        TS_ASSERT_THROWS_EQUALS( Material<> kupfer( 8920.0, -385.0, 400.0, 300.0, 200.0 ), std::runtime_error & e,
                                 strcmp( e.what(), "SpecificCapacity must be bigger than zero." ), 0 );
        TS_ASSERT_THROWS_EQUALS( Material<> kupfer( 8920.0, 385.0, 0.0, 300.0, 200.0 ), std::runtime_error & e,
                                 strcmp( e.what(), "Conductivities must be bigger than zero." ), 0 );
        TS_ASSERT_THROWS_EQUALS( Material<> kupfer( 8920.0, 385.0, 400.0, -300.0, 200.0 ), std::runtime_error & e,
                                 strcmp( e.what(), "Conductivities must be bigger than zero." ), 0 );
        TS_ASSERT_THROWS_EQUALS( Material<> kupfer( 8920.0, 385.0, 400.0, 300.0, -200.0 ), std::runtime_error & e,
                                 strcmp( e.what(), "Conductivities must be bigger than zero." ), 0 );
    }
    //#endif


    Material<> kupfer( 8920.0, 385.0, 400.0, 300.0, 200.0 );
    Material<> eisen( 7874.0, 449.0, 80.0, 60.0, 40.0, Material<>::CARTESIAN );

    TS_ASSERT_EQUALS( kupfer.GetDensity(), 8920.0 );
    TS_ASSERT_EQUALS( kupfer.GetSpecificCapacity(), 385.0 );
    TS_ASSERT_EQUALS( kupfer.GetConductivity( 0 ), 400.0 );
    TS_ASSERT_EQUALS( kupfer.GetConductivity( 1 ), 300.0 );
    TS_ASSERT_EQUALS( kupfer.GetConductivity( 2 ), 200.0 );
    TS_ASSERT_EQUALS( kupfer.GetMaterialType(), Material<>::NOT_SPECIFIED );

    TS_ASSERT_EQUALS( eisen.GetDensity(), 7874.0 );
    TS_ASSERT_EQUALS( eisen.GetSpecificCapacity(), 449.0 );
    TS_ASSERT_EQUALS( eisen.GetConductivity( 0 ), 80.0 );
    TS_ASSERT_EQUALS( eisen.GetConductivity( 1 ), 60.0 );
    TS_ASSERT_EQUALS( eisen.GetConductivity( 2 ), 40.0 );
    TS_ASSERT_EQUALS( eisen.GetMaterialType(), Material<>::CARTESIAN );
}
