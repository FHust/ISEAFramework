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
* File Name : TestArmadillowrap.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 15:12:17 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestArmadillowrap.h"

#ifdef _ARMADILLO_
#include <armadillo>
#else 
#include "../../stub/armadillo.h"
#endif /* _ARMADILLO_ */


void TestContainer::testArma()
{
#ifdef _ARMADILLO_
    arma::SpMat<double> t(3,3);
    t(0,0) = 8;
    t(0,1) = 1;
    t(0,2) = 6;
    t(1,0) = 3;
    t(1,1) = 5;
    t(1,2) = 7;
    t(2,0) = 4;
    t(2,1) = 9;
    t(2,2) = 2;

    arma::Mat<double> x(3,3);
    x(0,0 ) = 8;
    x(0,1) = 1;
    x(0,2) = 6;
    x(1,0) = 3;
    x(1,1) = 5;
    x(1,2) = 7;
    x(2,0) = 4;
    x(2,1) = 9;
    x(2,2) = 2;

    arma::SpMat<double> result = arma::inv(t) - arma::SpMat<double>(arma::inv(x));

    for( size_t i = 0 ; i < 3 ; ++i )
    {
        for( size_t j=0 ; j<3 ; ++j )
        {
            TS_ASSERT_EQUALS(result(i,j), 0);
        }
    }
    //           0.147222  -0.144444   0.063889
    //          -0.061111   0.022222   0.105556
    //           -0.019444   0.188889  -0.102778

    t = arma::inv(t);
    TS_ASSERT_DELTA( t(0,0),0.147222, 0.00001);
    TS_ASSERT_DELTA( t(0,1),-0.144444, 0.00001);
    TS_ASSERT_DELTA( t(0,2),0.063889,0.00001);
    TS_ASSERT_DELTA( t(1,0),-0.061111,0.00001);
    TS_ASSERT_DELTA( t(1,1),0.022222,0.00001);
    TS_ASSERT_DELTA( t(1,2),0.105556,0.00001);
    TS_ASSERT_DELTA( t(2,0),-0.019444,0.00001);
    TS_ASSERT_DELTA( t(2,1),0.188889,0.00001);
    TS_ASSERT_DELTA( t(2,2),-0.102778,0.00001);

    t(0,0) = 8;
    t(0,1) = 1;
    t(0,2) = 6;
    t(1,0) = 3;
    t(1,1) = 5;
    t(1,2) = 7;
    t(2,0) = 4;
    t(2,1) = 9;
    t(2,2) = 2;

    arma::SpMat<double>(b)(3,1);
    b(0,0) = 1;
    b(1,0) = 2;
    b(2,0) = 3;


    t = arma::solve(t,b);
    TS_ASSERT_DELTA(t(0,0), 0.050000, 0.0001);
    TS_ASSERT_DELTA(t(1,0), 0.3, 0.0001);
    TS_ASSERT_DELTA(t(2,0), 0.05, 0.0001);

    //        0.050000
    //        0.300000
    //        0.050000

    // testfunct<arma::SpMat<double> > ();
    // testfunct<arma::Mat<double> >();
#endif /* _ARMADILLO_ */
}

