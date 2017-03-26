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
#ifndef _TESTFACTORIES_
#define _TESTFACTORIES_

#include <cxxtest/TestSuite.h>
#include "../../../xmlparser/tinyxml2/xmlparserimpl.h"
#include "../../../factory/thermal/materialfactorybuilder.h"
#include "../../../factory/thermal/blockfactorybuilder.h"
#include "../../../factory/thermal/coolingfactorybuilder.h"
#include "../../../factory/thermal/coolingblockfactorybuilder.h"
#include "../../../factory/object/objectfactorybuilder.h"
#include "../../../factory/state/statefactorybuilder.h"
#include "../../../factory/thermal/thermal_factory.h"
#include <cstring>


class TestFactories : public CxxTest::TestSuite
{
    public:
    void TestCoolingFactory();
    void TestCoolingBlockFactory();
    void TestMaterialFactory();
    void TestBlockFactory();
    void TestThermalFactory();

    private:
    protected:
};

#endif
