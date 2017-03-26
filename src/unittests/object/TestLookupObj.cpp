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
* File Name : TestLookupObj.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 24 Nov 2014 18:47:52 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestLookupObj.h"
//STD
#include <string>

//BOOST
#include <boost/shared_ptr.hpp>

//ETC
#include "../../states/soc.h"
#include "../../states/thermal_state.h"

#include "../../object/lookup_obj1d.h"
#include "../../object/lookup_obj1d_with_state.h"
#include "../../object/lookup_obj2d.h"
#include "../../object/lookup_obj2d_with_state.h"


std::vector<double> Createm1()
{
    const double m1points[] =  {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    std::vector<double> m1( m1points, m1points +14);
    return m1;
}

std::vector<double> Createf1()
{
    const double f1points[] =  {0.939,0.939,0.906,0.891,0.869,0.850,0.837,0.831,0.833,0.847,0.876,0.923,0.998,0.998};
    std::vector<double> f1( f1points, f1points +14);
    return f1;
}

std::vector<double> CreateSoc()
{
    const double socData[] = {0,10,20,30,40,50,60,70,80,90,100};
    std::vector<double> soc(socData, socData + 11);
    return soc;
}

std::vector<double> CreateTemp()
{
    double const tmp[] = {0,10,23,50};
    std::vector<double> temp(tmp, tmp+4) ;
    return temp;
}

std::vector<double> CreateSOC()
{
    double const tmp[] = {5,20,35,50,65,80,90};
    std::vector<double> soc(tmp, tmp+7) ;
    return soc;
}

std::vector< std::vector<double> > CreateCd( )
{
    const size_t x = 7;
    const size_t y = 4;
    const double cd[x][y] =  {
        {6514.20000000000,6383.30000000000,6980.20000000000,8296.40000000000},
        {11208,11354,10800,10929},
        {26316,26071,24825,18076},
        {14726,14450,13065,15388},
        {7074.40000000000,7265.10000000000,8218.60000000000,8189},
        {7575.20000000000,7539.80000000000,7515.70000000000,7525.50000000000},
        {6196.10000000000,6162,6363.30000000000,6462.30000000000} };
    std::vector< std::vector<double> > vec (x, std::vector<double>(y, 0));

    for( size_t i = 0 ; i < y ; ++i )
        for( size_t j = 0 ; j < x ; ++j )
            vec.at(j).at(i) = cd[j][i];

    return vec;

}
void TestLookupObj::testLookupObj1dCreate()
{
    std::vector<double> m1 = Createm1();
    std::vector<double> f1_m =  Createf1();
    object::LookupObj1D<double> test(f1_m ,m1);
    TS_ASSERT_EQUALS( strcmp(test.GetName(), "LookupObj1D") , 0);
}
// Case 1: LookupType = LINEAR_INTERPOLATION (default)
void TestLookupObj::testLookupObj1dOperator_linearInterpolation()
{
    std::vector<double> f1_m =  Createf1();
    std::vector<double> m1 = Createm1();
    object::LookupObj1D<double> test( f1_m ,m1 ); // Default is LINEAR_INTERPOLATION

    TS_ASSERT_EQUALS(test.GetLookup().GetLookupType()->GetPoints(), m1);
    TS_ASSERT_EQUALS(test.GetLookup().GetLookupType()->GetLookup(), f1_m);

    TS_ASSERT_DELTA(test(1), 0.939, 0.0000001);
    TS_ASSERT_DELTA(test(2), 0.939, 0.0000001);
    TS_ASSERT_DELTA(test(3), 0.906, 0.0000001);
    TS_ASSERT_DELTA(test(4), 0.891, 0.0000001);

    TS_ASSERT_DELTA(test(),0.891 , 0.0000001);

    TS_ASSERT_DELTA(test(14), 0.998, 0.0000001);
    TS_ASSERT_DELTA(test(), 0.998 , 0.0000001);

    TS_ASSERT_DELTA(test(12.5), (0.923 + 0.998) / 2, 0.0000001);
    TS_ASSERT_DELTA(test(), (0.923 + 0.998) / 2 , 0.0000001);
}
// Case 2: LookupType = NEAREST_NEIGHBOUR
void TestLookupObj::testLookupObj1dOperator_nearestNeighbour()
{
    std::vector<double> f1_m =  Createf1();
    std::vector<double> m1 = Createm1();
    
    object::LookupObj1D<double> test( f1_m , m1, lookup::NEAREST_NEIGHBOUR ); // Nearest Neighbour

    TS_ASSERT_DELTA(test(1), 0.939, 0.0000001);
    TS_ASSERT_DELTA(test(2), 0.939, 0.0000001);
    TS_ASSERT_DELTA(test(3), 0.906, 0.0000001);
    TS_ASSERT_DELTA(test(4), 0.891, 0.0000001);

    TS_ASSERT_DELTA(test(),0.891 , 0.0000001);

    TS_ASSERT_DELTA(test(14), 0.998, 0.0000001);
    TS_ASSERT_DELTA(test(), 0.998 , 0.0000001);

    TS_ASSERT_DELTA(test(12.4), test(12), 0.0000001);
    TS_ASSERT_DELTA(test(), test(12) , 0.0000001);
    
    TS_ASSERT_DELTA(test(12.6), test(13), 0.0000001);
    TS_ASSERT_DELTA(test(), test(13) , 0.0000001);
}
// Case 3: LookupType = CONSTANT_UNTIL_THRESHOLD
void TestLookupObj::testLookupObj1dOperator_constantUntilThreshold()
{
    std::vector<double> f1_m =  Createf1();
    std::vector<double> m1 = Createm1();
    object::LookupObj1D<double> test( f1_m ,m1, lookup::CONSTANT_UNTIL_THRESHOLD); // Constant until threshold

    TS_ASSERT_DELTA(test(1), 0.939, 0.0000001);
    TS_ASSERT_DELTA(test(2), 0.939, 0.0000001);
    TS_ASSERT_DELTA(test(3), 0.906, 0.0000001);
    TS_ASSERT_DELTA(test(4), 0.891, 0.0000001);

    TS_ASSERT_DELTA(test(),0.891 , 0.0000001);

    TS_ASSERT_DELTA(test(14), 0.998, 0.0000001);
    TS_ASSERT_DELTA(test(), 0.998 , 0.0000001);

    TS_ASSERT_DELTA(test(12.4), test(12), 0.0000001);
    TS_ASSERT_DELTA(test(), test(12) , 0.0000001);
    
    TS_ASSERT_DELTA(test(12.6), test(12), 0.0000001);
    TS_ASSERT_DELTA(test(), test(12) , 0.0000001);
}
void TestLookupObj::testLookupObj2dCreate()
{
    std::vector<double> Temp = CreateTemp();
    std::vector<double> SOC = CreateSOC();
    std::vector< std::vector<double> > Cd = CreateCd();
    object::LookupObj2D<double> test( Cd, Temp, SOC);
    TS_ASSERT_EQUALS( strcmp(test.GetName(), "LookupObj2D") , 0);
}
// Case 1: LookupType = LINEAR_INTERPOLATION (default)
void TestLookupObj::testLookupObj2dOperator_linearInterpolation()
{
    std::vector<double> Temp = CreateTemp();
    std::vector<double> SOC = CreateSOC();
    std::vector< std::vector<double> > Cd = CreateCd();
    object::LookupObj2D<double> test( Cd, Temp, SOC);

    TS_ASSERT_EQUALS(test.GetLookup().GetLookupType()->GetPointsRow(), Temp);
    TS_ASSERT_EQUALS(test.GetLookup().GetLookupType()->GetPointsCol(), SOC);
    TS_ASSERT_EQUALS(test.GetLookup().GetLookupType()->GetLookup(), Cd);

    TS_ASSERT_DELTA(test(0,5), 6514.2, 0.0000001);
    TS_ASSERT_DELTA(test(10,5), 6383.3, 0.0000001);
    TS_ASSERT_DELTA(test(5,5), (6514.2 + 6383.3) / 2, 0.0000001);
    
    // TS_ASSERT_DELTA(test(0,5), 6514.2, 0.0000001); already tested
    TS_ASSERT_DELTA(test(0,20), 11208, 0.0000001);
    TS_ASSERT_DELTA(test(0,12.5), (6514.2 + 11208) / 2, 0.0000001);
}
// Case 2: LookupType = NEAREST_NEIGHBOUR
void TestLookupObj::testLookupObj2dOperator_nearestNeighbour()
{
    std::vector<double> Temp = CreateTemp();
    std::vector<double> SOC = CreateSOC();
    std::vector< std::vector<double> > Cd = CreateCd();
    object::LookupObj2D<double> test( Cd, Temp, SOC, lookup::NEAREST_NEIGHBOUR);
    TS_ASSERT_DELTA(test(0,5), 6514.2, 0.0000001);
    TS_ASSERT_DELTA(test(10,5), 6383.3, 0.0000001);
    TS_ASSERT_DELTA(test(4,5), 6514.2, 0.0000001);
    TS_ASSERT_DELTA(test(6,5), 6383.3, 0.0000001);
    
    // TS_ASSERT_DELTA(test(0,5), 6514.2, 0.0000001); already tested
    TS_ASSERT_DELTA(test(0,20), 11208, 0.0000001);
    TS_ASSERT_DELTA(test(0,6), 6514.2, 0.0000001);
    TS_ASSERT_DELTA(test(0,19), 11208, 0.0000001);
}
// Case 3: LookupType = CONSTANT_UNTIL_THRESOLD
void TestLookupObj::testLookupObj2dOperator_constantUntilThreshold()
{
    std::vector<double> Temp = CreateTemp();
    std::vector<double> SOC = CreateSOC();
    std::vector< std::vector<double> > Cd = CreateCd();
    object::LookupObj2D<double> test( Cd, Temp, SOC, lookup::CONSTANT_UNTIL_THRESHOLD);

    TS_ASSERT_DELTA(test(0,5), 6514.2, 0.0000001); // Threshold 1
    TS_ASSERT_DELTA(test(9,5), 6514.2, 0.0000001);
    TS_ASSERT_DELTA(test(9.9,5), 6514.2, 0.0000001);
    TS_ASSERT_DELTA(test(10,5), 6383.3, 0.0000001); // Threshold 2
    TS_ASSERT_DELTA(test(9.9,5), 6383.3, 0.0000001);
    
    // TS_ASSERT_DELTA(test(0,5), 6514.2, 0.0000001); // Threshold 1, already tested
    TS_ASSERT_DELTA(test(0,19.9), 6514.2, 0.0000001);
    TS_ASSERT_DELTA(test(0,20), 11208, 0.0000001); // Threshold 2
    TS_ASSERT_DELTA(test(0,19.9), 11208, 0.0000001);

}
void TestLookupObj::testLookupObj1WithStatedCreate()
{
    double capacity = 20.0;
    double status = 20.0;
    std::vector<double> soc = CreateSoc();
    boost::shared_ptr< electrical::state::Soc > testSoc(new electrical::state::Soc( capacity /* [Ah] */, status /*[%]*/, soc) );

    std::vector<double> m1 = Createm1();
    std::vector<double> f1_m =  Createf1();
    object::LookupObj1dWithState<double> test( f1_m ,m1, testSoc);
    TS_ASSERT_EQUALS( strcmp(test.GetName(), "LookupObj1dWithState") , 0);

    TS_ASSERT(test.GetState() == testSoc.get());
}
void TestLookupObj::testLookupObj1WithStatedOperations()
{
    double capacity = 20.0;
    double status = 20.0;
    double curCap = 0.3 * 3600 * capacity;
    std::vector<double> soc = CreateSoc();
    boost::shared_ptr< electrical::state::Soc > testSoc(new electrical::state::Soc( capacity /* [Ah] */, status /*[%]*/, soc) );

    const double m[] = {-10,0,10,20,30,40,50,60,70,80,90,100,110,120};
    std::vector<double> m1(m, m + 14);
    std::vector<double> f1_m = Createf1();
    object::LookupObj1dWithState<double> lookup( f1_m ,m1, testSoc);
    TS_ASSERT_DELTA( lookup(), 0.891, 0.00001);

    testSoc->SetCapacity(curCap);
    TS_ASSERT_DELTA( lookup(), 0.869, 0.00001);
    curCap = 0.9 * 3600 * capacity;
    testSoc->SetCapacity(curCap);
    TS_ASSERT_DELTA( lookup(), 0.876, 0.00001);
    curCap = 1.0 * 3600 * capacity;
    testSoc->SetCapacity(curCap);
    TS_ASSERT_DELTA( lookup(), 0.923, 0.00001);

    curCap = 1.1 * 3600 * capacity;
    testSoc->SetCapacity(curCap);
    curCap = testSoc->GetCapacity();
    TS_ASSERT_DELTA( curCap,  1.0 * 3600 * capacity, 0.00001);
    TS_ASSERT_DELTA( lookup(), 0.923, 0.00001);

    curCap = 1.2 * 3600 * capacity;
    testSoc->SetCapacity(curCap);
    curCap = testSoc->GetCapacity();
    TS_ASSERT_DELTA( curCap, 1.0 * 3600 * capacity, 0.00001);
    TS_ASSERT_DELTA( lookup(), 0.923, 0.00001);
}
void TestLookupObj::testLookupObj2WithStatedCreate()
{
    double capacity = 20.0;
    double status = 20.0;
    double curCap(0.0);

    std::vector<double> Temp = CreateTemp();
    std::vector<double> SOC = CreateSOC();


    boost::shared_ptr< electrical::state::Soc > testSoc(new electrical::state::Soc( capacity /* [Ah] */, status /*[%]*/, SOC) );
    boost::shared_ptr< ::state::ThermalState< double > > testTemp(new ::state::ThermalState< double >(23.0) );

    std::vector< std::vector<double> > Cd = CreateCd();
    object::LookupObj2dWithState<double> test( Cd, Temp, SOC,  testTemp,testSoc);
    TS_ASSERT(test.GetStateRow() == testTemp.get());
    TS_ASSERT(test.GetStateCol() == testSoc.get());
    TS_ASSERT_DELTA( test(), 10800,0.001);

    curCap = 0.9 * 3600 * capacity;
    testSoc->SetCapacity( curCap );
    TS_ASSERT_DELTA( test(), 6363.30000000000 ,0.001);
    TS_ASSERT_EQUALS( strcmp(test.GetName(), "LookupObj2dWithState"), 0 );
}