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
#ifndef _TESTAREACLASSES_
#define _TESTAREACLASSES_

#include <cxxtest/TestSuite.h>
#include "../../geometry/plane.h"
#include "../../geometry/geometry2D.h"
#include "../../geometry/area.h"
#include "../../geometry/innerArea.h"
#include "../../geometry/block_geometry.h"


class TestAreaClasses : public CxxTest::TestSuite
{
    public:
    void TestEasyPlane();
    void TestComplicatedPlane();
    void TestGeometry2D();
    void TestGeometry2DWithArcs();
    void TestArea();
    void TestAreaWithArcs();
    void TestInnerAreas();
    void TestBlockGeometry();

    private:
    protected:
};

#endif
