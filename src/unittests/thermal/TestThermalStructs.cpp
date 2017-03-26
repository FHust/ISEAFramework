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
#include "TestThermalStructs.h"

using namespace thermal;
static const double sDelta = 0.000001;



void TestThermalStructs::TestStructs()
{
    IndexedValue<double> value1 = { 2, - 8.5 };
    IndexedValue<double> value2 = MakeIndexedValue<double>(4, 6.5);
    TS_ASSERT_EQUALS(value1.mIndex, 2);
    TS_ASSERT_EQUALS(value1.mValue, - 8.5);
    TS_ASSERT_EQUALS(value2.mIndex, 4);
    TS_ASSERT_EQUALS(value2.mValue, 6.5);



    IndexIs<double> compare(4);
    IndexIs<double> compareFail(3);

    TS_ASSERT(compare(value2));
    TS_ASSERT_EQUALS(compareFail(value2), false);



    TS_ASSERT(MakeIndexedValue<double>(3, 0.0) < MakeIndexedValue<double>(4, 0.0));
    TS_ASSERT_EQUALS(MakeIndexedValue<double>(4, 0.0) < MakeIndexedValue<double>(4, 0.0), false);

    IndexedValuesEqual<double> equals(0.01);
    TS_ASSERT(equals(MakeIndexedValue<double>(3, 0.0), MakeIndexedValue<double>(3, 0.001)));
    TS_ASSERT_EQUALS(equals(MakeIndexedValue<double>(2, 0.0), MakeIndexedValue<double>(3, 0.0)), false);
    TS_ASSERT_EQUALS(equals(MakeIndexedValue<double>(3, 0.0), MakeIndexedValue<double>(3, 0.02)), false);



    Linearization<double> lin(- 0.1, 1.5);
    lin.Add(Linearization<double>(1.0, 1.4));
    TS_ASSERT_DELTA(lin.mOffset, 0.9, sDelta);
    TS_ASSERT_DELTA(lin.mSlope, 2.9, sDelta);



    vector<int> vec(4, - 2);
    vec.clear();
    TS_ASSERT_EQUALS(vec.capacity(), 4);
    ShrinkToFit(vec);
    TS_ASSERT_EQUALS(vec.capacity(), 0);
}

void TestThermalStructs::TestJaggedArray()
{
    std::vector<std::vector<int> > intVecVec(3);
    intVecVec.at(0).push_back(8);
    intVecVec.at(0).push_back(-4);
    intVecVec.at(0).push_back(3);
    intVecVec.at(2).push_back(1);

    thermal::JaggedArray<int> jaggedArray(intVecVec);
    TS_ASSERT_EQUALS(jaggedArray.GetNumberOfArrays(), 3);

    int *intPtr = jaggedArray.Begin(0);
    TS_ASSERT_EQUALS(jaggedArray.Begin(1), intPtr + 3);
    TS_ASSERT_EQUALS(jaggedArray.Begin(2), intPtr + 3);
    TS_ASSERT_EQUALS(jaggedArray.Begin(3), intPtr + 4);

    TS_ASSERT_EQUALS(*intPtr++, 8);
    TS_ASSERT_EQUALS(*intPtr++, -4);
    TS_ASSERT_EQUALS(*intPtr++, 3);
    TS_ASSERT_EQUALS(*intPtr++, 1);

    intPtr = jaggedArray.Begin(0);
    std::sort(jaggedArray.Begin(0), jaggedArray.Begin(1) - 1);
    TS_ASSERT_EQUALS(*intPtr, -4);
    TS_ASSERT_EQUALS(*(intPtr + 1), 8);
    TS_ASSERT_EQUALS(*(intPtr + 2), 3);
    TS_ASSERT_EQUALS(*(intPtr + 3), 1);

    std::sort(jaggedArray.Begin(0), jaggedArray.Begin(1));
    TS_ASSERT_EQUALS(*intPtr, -4);
    TS_ASSERT_EQUALS(*(intPtr + 1), 3);
    TS_ASSERT_EQUALS(*(intPtr + 2), 8);
    TS_ASSERT_EQUALS(*(intPtr + 3), 1);

    std::sort(jaggedArray.Begin(0), jaggedArray.Begin(3));
    TS_ASSERT_EQUALS(*intPtr, -4);
    TS_ASSERT_EQUALS(*(intPtr + 1), 1);
    TS_ASSERT_EQUALS(*(intPtr + 2), 3);
    TS_ASSERT_EQUALS(*(intPtr + 3), 8);

    TS_ASSERT(!std::binary_search(jaggedArray.Begin(0), jaggedArray.Begin(1), 8));
    TS_ASSERT(std::binary_search(jaggedArray.Begin(0), jaggedArray.Begin(3), 8));
    //std::find()
}
