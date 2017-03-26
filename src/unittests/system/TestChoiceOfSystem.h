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
* File Name : TestChoiceOfSystem.h
* Creation Date : 09-09-2014
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTCHOICESYSTEM_
#define _TESTCHOICESYSTEM_
#include <cxxtest/TestSuite.h>
#include <vector>
#include <cstring>

#include "../../misc/matrixInclude.h"

#include "../../factory/electricalEquationSystem/chooseElectricSystemType.h"

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

// electrical Elements
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/capacity.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/paralleltwoport.h"
#include "../../object/const_obj.h"
#include "../../system/stateSystemGroup.h"
#include "../../system/generalizedsystem.h"

class TestChoiceSystem : public CxxTest::TestSuite
{
    public:
    void TestChoiceOfSystem();
};
#endif /* _TESTCHOICESYSTEM_*/
