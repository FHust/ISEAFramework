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
* File Name : TestThermalSimulation.cpp
* Creation Date : 05-03-2015
* Last Modified : Mi 07 Okt 2015 13:14:25 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestThermalSimulation.h"
#include "../../thermal/thermal_simulation.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"
#include <iostream>


void TestThermalSimulation::TestThermalSimulationRun()
{
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromFile( "./TestRectangularblock.xml" );
    boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();
    simulation::ThermalSimulation< myMatrixType, double, true > sim( rootXmlNode, 0.001, 0.5, 5, 0, 0, 0 );

    TS_ASSERT_EQUALS( sim.mThermalProbes.size(), 8 );

    for ( size_t i = 0; i < sim.mThermalProbes.size(); ++i )
    {
        TS_ASSERT_DELTA( sim.mThermalProbes.at( i ).GetCorrespondingThermalElement()->GetTemperature(), 100, 0.0001 );
        TS_ASSERT( sim.mThermalProbes.at( i ).GetCorrespondingThermalElement()->GetGridVertex().Distance(
                    sim.mThermalProbes.at( i ).GetCoordinates() ) < 0.0001 );
    }
}
