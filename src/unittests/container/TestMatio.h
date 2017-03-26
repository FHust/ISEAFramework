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
* File Name : TestMatio.h
* Creation Date : 03-07-2015
* Last Modified : Mi 05 Aug 2015 16:26:20 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMATIO_
#define _TESTMATIO_
#include <cxxtest/TestSuite.h>

class TestMatio : public CxxTest::TestSuite
{
    public:
    void TestMatFileEmpty();
    void TestMatFileEmptyStruct();
    void TestMatFileFail();
    void TestMatFileContent1D();
    void TestMatFileContent2D();
    void TestMatFileContent2DSlice();
    void TestMatFileContent2DSliceTokenAcccesAtFileLevel();
    void TestMatFileContent2DSliceTokenAcccesAtDataLevel();
    void TestMadioDataCreationFromVector1D();
    void TestMadioDataCreationFromVector2D();
    void TestMatFileWriteAndRead();


    private:
    protected:
};
#endif /* _TESTMATIO_ */
