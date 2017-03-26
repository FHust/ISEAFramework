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
#ifndef _TESTBLOCK_
#define _TESTBLOCK_

#include <cxxtest/TestSuite.h>
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/cxx11/is_permutation.hpp>
#include <cstring>
#include "../../geometry/area.h"
#include "../../thermal/blocks/rectangular_block.h"
#include "RectangularBlockTester.h"
#include "../../thermal/blocks/quadratic_cell_block.h"
#include "../../thermal/blocks/supercap.h"
#include "../../thermal/blocks/hexagonal_cell_block.h"
#include "../../thermal/blocks/triangular_prism_block.h"


class TestBlocks : public CxxTest::TestSuite
{
    public:
    void TestRectangularBlock();
    void TestQuadraticCellBlock();
    void TestQuadraticCellBlockPhiDivisionEight();
    void TestSupercap();
    void TestHexagonalCellBlock();
    void TestTriangularPrismBlock();
    void TestMultipleThermalStatesInThermalBlock();

    private:
    protected:
};

#endif
