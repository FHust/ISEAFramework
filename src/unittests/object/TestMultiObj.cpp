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
* File Name : TestMultiObj.cpp
* Creation Date : 25-11-2016
* Last Modified : Fr 25 Nov 2016 14:21:53 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestMultiObj.h"

#include <boost/make_shared.hpp>

#include "../../object/const_obj.h"
#include "../../object/multi_obj.h"

void TestMultiObj::testMultiObjectGetValue()
{
    typedef object::ConstObj< double > DataT;
    std::vector< boost::shared_ptr< object::Object< double > > > testVec;
    for ( size_t i = 1; i < 10; ++i )
    {
        testVec.push_back( boost::make_shared< DataT >( static_cast< double >( i ) ) );
    }
    auto x = object::AdderObj< double >( testVec );
    auto y = object::MultiplyObj< double >( testVec );
    auto z = object::DivideObj< double >( testVec );

    std::vector< double > result = {45.0, 362880, 2.7557e-06};

    TS_ASSERT_DELTA( result[0], x.GetValue(), 0.000001 );
    TS_ASSERT_DELTA( result[1], y.GetValue(), 0.000001 );
    TS_ASSERT_DELTA( result[2], z.GetValue(), 0.000001 );
}
