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
* File Name : TestSimuOptions.h
* Creation Date : 11-05-2012
* Last Modified : Fr 21 Feb 2014 13:06:53 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTCONTAINER_
#define _TESTCONTAINER_
#include <cxxtest/TestSuite.h>

#include "../../container/armadillo_wrapper.h"

template < typename T >
void testfunct()
{
#ifdef _ARMADILLO_
    T t( 2, 1 ), t1( 2, 1 ), t2( 10, 1 );
    t( 0, 0 ) = 1;
    t( 1, 0 ) = 2;
    t1( 0, 0 ) = 3;
    t1( 0, 0 ) = 4;
    for ( unsigned i = 5, j = 0; i < 15; ++i )
    {
        t2( j, 0 ) = i;
        ++j;
    }
    TS_ASSERT_EQUALS( ( boost::numeric::odeint::same_size_impl< T, T >().same_size( t, t1 ) ), true );
    TS_ASSERT_EQUALS( ( boost::numeric::odeint::same_size_impl< T, T >().same_size( t, t2 ) ), false );

    boost::numeric::odeint::resize_impl< T, T >().resize( t, t2 );
    TS_ASSERT_EQUALS( ( boost::numeric::odeint::same_size_impl< T, T >().same_size( t, t2 ) ), true );
    TS_ASSERT_EQUALS( ( boost::numeric::odeint::same_size_impl< T, T >().same_size( t, t1 ) ), false );

    TS_ASSERT_EQUALS( t( 0, 0 ), 1 );
    TS_ASSERT_EQUALS( t( 1, 0 ), 2 );
    for ( unsigned i = 2; i < t.n_rows; ++i )
    {
        TS_ASSERT_EQUALS( t( i, 0 ), 0 );
    }

    boost::numeric::odeint::resize_impl< T, T >().resize( t2, t1 );
    TS_ASSERT_EQUALS( ( boost::numeric::odeint::same_size_impl< T, T >().same_size( t2, t1 ) ), true );
    TS_ASSERT_EQUALS( ( boost::numeric::odeint::same_size_impl< T, T >().same_size( t2, t ) ), false );
    for ( unsigned i = 0; i < t2.n_rows; ++i )
    {
        TS_ASSERT_EQUALS( t2( i, 0 ), i + 5 );
    }
#endif
}

class TestContainer : public CxxTest::TestSuite
{
    typedef arma::SpMat< double > state_type;
    typedef arma::Mat< double > state_type1;

    public:
    void testArma();
};

#endif /* _TESTCONTAINER_ */
