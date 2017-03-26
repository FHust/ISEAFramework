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
#ifndef _RECTANGULARBLOCKTESTER_
#define _RECTANGULARBLOCKTESTER_

#include <cxxtest/TestSuite.h>
#include "../../geometry/area.h"
#include <boost/scoped_ptr.hpp>
#include "../../thermal/blocks/rectangular_block.h"


struct RectangularBlockTester
{
    void ThermalElementsTest();
    void SurfaceAreasTest();
    bool IsInsideBlock( const geometry::Cartesian<> &vertex );
    void ConductivityMatrixTest();
    std::vector< geometry::TwoDim<> > GetBaseVertices();

    std::vector< boost::shared_ptr< thermal::ThermalElement<> > > mThermalElements;
    std::vector< std::vector< thermal::IndexedValue< double > > > mConductivityMatrix;
    std::list< thermal::IndexedArea< double > > mSurfaceAreas;
    size_t mNX;
    size_t mNY;
    size_t mNZ;
    double mDimX;
    double mDimY;
    double mDimZ;
    geometry::Cartesian<> mOrigin;
    thermal::Material<> *mMaterial;
    double mTemperature;
    double mPowerDissipation;
};

#endif
