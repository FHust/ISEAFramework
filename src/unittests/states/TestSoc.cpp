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
* File Name : TestSoc.cpp
* Creation Date : 21-02-2014
* Last Modified : Di 12 Aug 2014 18:47:05 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestSoc.h"
#include <limits>

#include "../../states/soc.h"

void TestSoc::testSocCreation()
{
    double capacity = 20.0;
    double status = 20.0;
    const double data[] = {0,10,20,30,40,50,60,70,80,90,100};
    std::vector<double> socData(data, data + 11);
    electrical::state::Soc test( capacity /* [Ah] */, status /*[%]*/, socData );

}

void TestSoc::testSocOperations()
{
    double capacity = 20.0;
    double status = 20.0;

    const double data[] = {0,10,20,30,40,50,60,70,80,90,100};
    std::vector<double> socData(data, data + 11);

    electrical::state::Soc test( capacity /* [Ah] */, status /*[%]*/, socData);
    TS_ASSERT_DELTA(test.GetMaxCapacity(), capacity * 3600, 0.000001);
    TS_ASSERT_DELTA(test.GetValue(), status, 0.00001);
    TS_ASSERT_DELTA(test.mCurCapAsec, status/100 * capacity * 3600, 0.000001);
    TS_ASSERT_EQUALS(test.GetMaxDt(), std::numeric_limits<double>::max() );
    TS_ASSERT_DELTA(test.mMaxCapAsec, capacity * 3600, 0.000001);
}


