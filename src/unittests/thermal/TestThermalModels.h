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
#ifndef _TESTBATTERY_
#define _TESTBATTERY_

#include <cxxtest/TestSuite.h>
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/cxx11/is_permutation.hpp>
#include <cstring>
#include "../../geometry/area.h"
#include "../../thermal/thermal_model.h"
#include "../../thermal/blocks/rectangular_block.h"
#include "../../thermal/boundaryConditions/cooling.h"
#include "../../thermal/boundaryConditions/cooling_block.h"
#include "../../thermal/thermal_structs.h"


class TestThermalModels : public CxxTest::TestSuite
{
    public:
    void TestThermalModel();
    void TestCharacteristicLengthCalculation();
    void TestCharacteristicLengthCalculationAdvancedAggregation();

    private:
    protected:
};

#endif
