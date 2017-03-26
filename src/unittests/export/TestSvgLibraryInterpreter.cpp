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
* File Name : TestSvgLibraryInterpreter.cpp
* Creation Date : 15-04-2015
* Last Modified : Fr 24 Apr 2015 17:03:43 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include <algorithm>
#include <iostream>

// ETC
#include "TestSvgLibraryInterpreter.h"
#include "../../export/svgLibraryInterpreter.h"

std::string CreateSimpleLibrary()
{
    return "<svg>\n\
    <g\n\
        transform=\"translate(0,0)\"\n\
        id=\"OhmicResistance\">\n\
    <path\n\
        d=\"m 0,32.5 15,0\"\n\
        style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none\" />\n\
    <path\n\
        d=\"m 50,32.5 15,0\"\n\
        style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none\" />\n\
    <rect\n\
        width=\"35\"\n\
        height=\"15\"\n\
        x=\"15\"\n\
        y=\"25\"\n\
        style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:inline;overflow:visible\" />\n\
    </g>\n\
    <g id=\"VoltageSource\" </g>\n\
    <g id=\"Capacity\" </g>\n\
    <g id=\"ParallelRC\" </g>\n\
    <g id=\"Zarc\" </g>\n\
    <g id=\"WarburgTanh\" </g>\n\
    <g id=\"WarburgCotanh\" </g>\n\
    <g id=\"Inductance\" </g>\n\
    <g id=\"ConstantPhaseElement\" </g>\n\
    <g id=\"SerialConnection\" </g>\n\
    <g id=\"ParallelConnection\" </g>\n\
    <g id=\"Cellelement\" </g>\
</svg>";
}

std::string GetOhmicResistanceExampleWithPosition( size_t x = 0, size_t y = 0 )
{
    return "\
<g\n\
        transform=\"translate(" +
           std::to_string( x ) + "," + std::to_string( y ) +
           ")\"\n\
        id=\"OhmicResistance\">\n\
    <path\n\
        d=\"m 0,32.5 15,0\"\n\
        style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none\" />\n\
    <path\n\
        d=\"m 50,32.5 15,0\"\n\
        style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none\" />\n\
    <rect\n\
        width=\"35\"\n\
        height=\"15\"\n\
        x=\"15\"\n\
        y=\"25\"\n\
        style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:inline;overflow:visible\" />\n\
    </g>";
}

void TestSvgLibraryInterpreter::testCreation()
{
    std::string config = CreateSimpleLibrary();
    svg::SvgLibraryInterpreter svgLib( config );
}

void TestSvgLibraryInterpreter::testFindAttribute()
{
    std::string config = CreateSimpleLibrary();
    svg::SvgLibraryInterpreter svgLib( config );
    svgLib.FindAttribute( "ohmicresistance" );
    svgLib.FindAttribute( "voltagesource" );
    svgLib.FindAttribute( "capacity" );
    svgLib.FindAttribute( "parallelrc" );
    svgLib.FindAttribute( "zarc" );
    svgLib.FindAttribute( "warburgtanh" );
    svgLib.FindAttribute( "warburgcotanh" );
    svgLib.FindAttribute( "inductance" );
    svgLib.FindAttribute( "constantphaseelement" );
    svgLib.FindAttribute( "serialconnection" );
    svgLib.FindAttribute( "parallelconnection" );
    svgLib.FindAttribute( "cellelement" );
}

void TestSvgLibraryInterpreter::testUnfindableTagId()
{
    svg::SvgLibraryInterpreter svgLib;
    TS_ASSERT_THROWS( svgLib.FindAttribute( "blaBla" ), std::runtime_error );
}

void TestSvgLibraryInterpreter::testUngetableTagId()
{
    svg::SvgLibraryInterpreter svgLib;
    TS_ASSERT_THROWS( svgLib.GetTag( "blaBla" ), std::runtime_error );
}

void TestSvgLibraryInterpreter::testGetValidTagIdSimple()
{
    std::string config = GetOhmicResistanceExampleWithPosition();
    std::transform( config.begin(), config.end(), config.begin(), ::tolower );

    svg::SvgLibraryInterpreter svgLib( CreateSimpleLibrary() );

    TS_ASSERT_EQUALS( svgLib.GetTag( "OhmicResistance" ), config );
}

void TestSvgLibraryInterpreter::testGetValidTagWithPlace()
{
    std::string config = GetOhmicResistanceExampleWithPosition( 100, 100 );
    std::transform( config.begin(), config.end(), config.begin(), ::tolower );


    svg::SvgLibraryInterpreter svgLib( CreateSimpleLibrary() );

    TS_ASSERT_EQUALS( svgLib.GetTagWithPosition( "OhmicResistance", 100, 100 ), config );
}
