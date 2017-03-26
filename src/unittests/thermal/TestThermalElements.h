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
#ifndef _TESTTHERMAL_ELEMENT_
#define _TESTTHERMAL_ELEMENT_

#include <cstring>
#include <cxxtest/TestSuite.h>
#include <boost/scoped_ptr.hpp>
#include "../../thermal/blocks/elements/thermal_element.h"
#include "../../thermal/blocks/elements/rectangular_element.h"
#include "../../thermal/blocks/elements/cylindric_element.h"
#include "../../thermal/blocks/elements/cutcyl_element.h"
#include "../../thermal/blocks/elements/triangular_element.h"


class TestThermalElements : public CxxTest::TestSuite
{
    public:
    void TestThermalElement();
    void TestRectangularElement();
    void TestCylindricElement();
    void TestCutCylElement();
    void TestTriangularElement();

    private:
    protected:
};

#endif
