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
#include "TestSymbolic.h"
#include "../../misc/symbolic.h"
#include "../thermal/exception_tester.h"
#include <cstring>

static const double sDelta = 0.000001;

#ifdef _SYMBOLIC_
using namespace symbolic;
using misc::StrCont;
#endif


void TestSymbolic::TestSymbolicUnit()
{
#ifdef _SYMBOLIC_
    {
        Symbolic symbol1( "var" );
        Symbolic symbol2( "var" );
        Symbolic symbol3( "const" );

        TS_ASSERT( symbol1 == symbol2 );
        TS_ASSERT( !( symbol1 == symbol3 ) );
    }

    Symbolic empty;
    TS_ASSERT_EQUALS( strcmp( empty.GetString(), StrCont( "" ) ), 0 );
    TS_ASSERT( empty.IsEmpty() );

    Symbolic one( 1.0 );
    Symbolic intOne( 1 );
    Symbolic notOne( 1.001 );
    TS_ASSERT( !one.IsEmpty() );
    TS_ASSERT( one.IsOne() );
    TS_ASSERT( intOne.IsOne() );
    TS_ASSERT( !notOne.IsOne() )

    Symbolic symbolicStrCont( StrCont( "AString" ) );
    TS_ASSERT_EQUALS( strcmp( symbolicStrCont.GetString(), StrCont( "AString" ) ), 0 );
    TS_ASSERT( !symbolicStrCont.IsEmpty() );

    Symbolic symbolicStr( "AnotherString" );
    TS_ASSERT_EQUALS( strcmp( symbolicStr.GetString(), StrCont( "AnotherString" ) ), 0 );
    TS_ASSERT( !symbolicStr.IsEmpty() );

    Symbolic symbolicSizet( 43 );
    TS_ASSERT_EQUALS( strcmp( symbolicSizet.GetString(), StrCont( "43" ) ), 0 );
    TS_ASSERT( !symbolicSizet.IsEmpty() );

    Symbolic symbolicInt( -5 );
    TS_ASSERT_EQUALS( strcmp( symbolicInt.GetString(), StrCont( "-5" ) ), 0 );
    TS_ASSERT( !symbolicInt.IsEmpty() );

    Symbolic symbolicDouble1( 23.5 );
    TS_ASSERT_EQUALS( strcmp( symbolicDouble1.GetString(), StrCont( "23.500000" ) ), 0 );
    TS_ASSERT( !symbolicDouble1.IsEmpty() );

    Symbolic symbolicDouble2( -1.0 );
    TS_ASSERT_EQUALS( strcmp( symbolicDouble2.GetString(), StrCont( "-1.000000" ) ), 0 );
    TS_ASSERT( !symbolicDouble2.IsEmpty() );

    Symbolic symbolicIntegerEmpty( 0 );
    Symbolic symbolicDoubleEmpty( 0.0 );
    TS_ASSERT_EQUALS( strcmp( symbolicIntegerEmpty.GetString(), StrCont( "" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( symbolicDoubleEmpty.GetString(), StrCont( "" ) ), 0 );
    TS_ASSERT( symbolicIntegerEmpty.IsEmpty() );
    TS_ASSERT( symbolicDoubleEmpty.IsEmpty() );


    Symbolic left( "left" );
    Symbolic right( "right" );
    Symbolic var( "var" );
    Symbolic tmp;

    TS_ASSERT_EQUALS( strcmp( ( -var ).GetString(), StrCont( "NEG(var)" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( -empty ).GetString(), StrCont( "" ) ), 0 );
    TS_ASSERT( ( -empty ).IsEmpty() );

    TS_ASSERT_EQUALS( strcmp( ( left + right ).GetString(), StrCont( "ADD(left,right)" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( var + empty ).GetString(), StrCont( "var" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( empty + var ).GetString(), StrCont( "var" ) ), 0 );

    TS_ASSERT_EQUALS( strcmp( ( left - right ).GetString(), StrCont( "SUB(left,right)" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( var - empty ).GetString(), StrCont( "var" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( empty - var ).GetString(), StrCont( "NEG(var)" ) ), 0 );

    TS_ASSERT_EQUALS( strcmp( ( left * right ).GetString(), StrCont( "MUL(left,right)" ) ), 0 );
    TS_ASSERT( ( var * empty ).IsEmpty() );
    TS_ASSERT( ( empty * var ).IsEmpty() );

    TS_ASSERT_EQUALS( strcmp( ( left / right ).GetString(), StrCont( "DIV(left,right)" ) ), 0 );
    TS_ASSERT_THROWS_EQUALS(
     var / empty, const std::runtime_error &e,
     CmpUnexp( e, "Empty symbolic instance is considered a zero, and division by zero is not valid", "operator/" ), 0 );
    TS_ASSERT( ( empty / var ).IsEmpty() );

    tmp = left;
    tmp += right;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "ADD(left,right)" ) ), 0 );
    tmp = var;
    tmp += empty;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );
    tmp = empty;
    tmp += var;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );

    tmp = left;
    tmp -= right;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "SUB(left,right)" ) ), 0 );
    tmp = var;
    tmp -= empty;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );
    tmp = empty;
    tmp -= var;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "NEG(var)" ) ), 0 );

    tmp = left;
    tmp *= right;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "MUL(left,right)" ) ), 0 );
    tmp = var;
    tmp *= empty;
    TS_ASSERT( tmp.IsEmpty() );
    tmp = empty;
    tmp *= var;
    TS_ASSERT( tmp.IsEmpty() );
    tmp = one;
    tmp *= var;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );
    tmp = var;
    tmp *= one;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );

    tmp = left;
    tmp /= right;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "DIV(left,right)" ) ), 0 );
    tmp = var;
    TS_ASSERT_THROWS_EQUALS(
     tmp /= empty, const std::runtime_error &e,
     CmpUnexp( e, "Empty symbolic instance is considered a zero, and division by zero is not valid", "operator/" ), 0 );
    tmp = empty;
    tmp /= var;
    TS_ASSERT( tmp.IsEmpty() );
    tmp = var;
    tmp /= one;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );
#endif
}

void TestSymbolic::TestSymbolicMatrix()
{
#ifdef _SYMBOLIC_
    SymbolicMatrix empty;
    TS_ASSERT_EQUALS( empty.n_rows, 0 );
    TS_ASSERT_EQUALS( empty.n_cols, 0 );
    TS_ASSERT_EQUALS( empty.mData.size(), 0 );

    SymbolicMatrix test( 2, 3 );
    TS_ASSERT_EQUALS( test.n_rows, 2 );
    TS_ASSERT_EQUALS( test.n_cols, 3 );
    TS_ASSERT_EQUALS( test.mData.size(), 2 );
    TS_ASSERT_EQUALS( test.mData.at( 0 ).size(), 3 );
    TS_ASSERT( test.mData.at( 0 ).at( 0 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 0 ).at( 1 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 0 ).at( 2 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 1 ).at( 0 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 1 ).at( 1 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 1 ).at( 2 ).IsEmpty() );

    test( 0, 0 ) = Symbolic( "VarL1" );
    test( 1, 1 ) = Symbolic( "VarL2" );
    test( 0, 2 ) = Symbolic( "VarR1" );
    test( 1, 2 ) = Symbolic( "VarR2" );
    TS_ASSERT_EQUALS( test.mData.at( 0 ).at( 0 ), Symbolic( "VarL1" ) );
    TS_ASSERT_EQUALS( test.mData.at( 1 ).at( 1 ), Symbolic( "VarL2" ) );
    TS_ASSERT_EQUALS( test.mData.at( 0 ).at( 2 ), Symbolic( "VarR1" ) );
    TS_ASSERT_EQUALS( test.mData.at( 1 ).at( 2 ), Symbolic( "VarR2" ) );
    TS_ASSERT( test.mData.at( 0 ).at( 1 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 1 ).at( 0 ).IsEmpty() );
    TS_ASSERT_EQUALS( test( 0, 0 ), Symbolic( "VarL1" ) );
    TS_ASSERT_EQUALS( test( 1, 1 ), Symbolic( "VarL2" ) );
    TS_ASSERT_EQUALS( test( 0, 2 ), Symbolic( "VarR1" ) );
    TS_ASSERT_EQUALS( test( 1, 2 ), Symbolic( "VarR2" ) );
    TS_ASSERT( test( 0, 1 ).IsEmpty() );
    TS_ASSERT( test( 1, 0 ).IsEmpty() );

    SymbolicMatrix left, right;
    test.VerticalSplitMatrix( left, 2, right );
    TS_ASSERT_EQUALS( left.n_rows, 2 );
    TS_ASSERT_EQUALS( left.n_cols, 2 );
    TS_ASSERT_EQUALS( left.mData.size(), 2 );
    TS_ASSERT_EQUALS( left.mData.at( 0 ).size(), 2 );
    TS_ASSERT_EQUALS( right.n_rows, 2 );
    TS_ASSERT_EQUALS( right.n_cols, 1 );
    TS_ASSERT_EQUALS( right.mData.size(), 2 );
    TS_ASSERT_EQUALS( right.mData.at( 0 ).size(), 1 );
    TS_ASSERT_EQUALS( left( 0, 0 ), Symbolic( "VarL1" ) );
    TS_ASSERT_EQUALS( left( 1, 1 ), Symbolic( "VarL2" ) );
    TS_ASSERT_EQUALS( right( 0, 0 ), Symbolic( "VarR1" ) );
    TS_ASSERT_EQUALS( right( 1, 0 ), Symbolic( "VarR2" ) );
    TS_ASSERT( left( 0, 1 ).IsEmpty() );
    TS_ASSERT( left( 1, 0 ).IsEmpty() );

    test.zeros( 1, 5 );
    TS_ASSERT_EQUALS( test.n_rows, 1 );
    TS_ASSERT_EQUALS( test.n_cols, 5 );
    TS_ASSERT_EQUALS( test.mData.size(), 1 );
    TS_ASSERT_EQUALS( test.mData.at( 0 ).size(), 5 );
    TS_ASSERT( test.mData.at( 0 ).at( 0 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 0 ).at( 1 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 0 ).at( 2 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 0 ).at( 3 ).IsEmpty() );
    TS_ASSERT( test.mData.at( 0 ).at( 4 ).IsEmpty() );


    test = -left;
    TS_ASSERT_EQUALS( test( 0, 0 ), Symbolic( "NEG(VarL1)" ) );
    TS_ASSERT_EQUALS( test( 1, 1 ), Symbolic( "NEG(VarL2)" ) );
    TS_ASSERT( test( 0, 1 ).IsEmpty() );
    TS_ASSERT( test( 1, 0 ).IsEmpty() );

    right = test;
    right( 1, 0 ) = Symbolic( "another_var" );
    test = left + right;
    TS_ASSERT_EQUALS( test( 0, 0 ), Symbolic( "ADD(VarL1,NEG(VarL1))" ) );
    TS_ASSERT_EQUALS( test( 1, 1 ), Symbolic( "ADD(VarL2,NEG(VarL2))" ) );
    TS_ASSERT( test( 0, 1 ).IsEmpty() );
    TS_ASSERT_EQUALS( test( 1, 0 ), Symbolic( "another_var" ) );

    test = left - test;
    TS_ASSERT_EQUALS( test( 0, 0 ), Symbolic( "SUB(VarL1,ADD(VarL1,NEG(VarL1)))" ) );
    TS_ASSERT_EQUALS( test( 1, 1 ), Symbolic( "SUB(VarL2,ADD(VarL2,NEG(VarL2)))" ) );
    TS_ASSERT( test( 0, 1 ).IsEmpty() );
    TS_ASSERT_EQUALS( test( 1, 0 ), Symbolic( "NEG(another_var)" ) );

    SymbolicMatrix leftFactor( 2, 3 );
    SymbolicMatrix rightFactor( 3, 2 );
    leftFactor( 0, 0 ) = Symbolic( "L00" );
    leftFactor( 0, 2 ) = Symbolic( "L02" );
    leftFactor( 1, 1 ) = Symbolic( "L11" );
    leftFactor( 1, 2 ) = Symbolic( "L12" );
    rightFactor( 0, 0 ) = Symbolic( "R00" );
    rightFactor( 0, 1 ) = Symbolic( "R01" );
    rightFactor( 1, 0 ) = Symbolic( "R10" );
    rightFactor( 1, 1 ) = Symbolic( "R11" );
    rightFactor( 2, 0 ) = Symbolic( "R20" );
    rightFactor( 2, 1 ) = Symbolic( "R21" );
    test = leftFactor * rightFactor;
    TS_ASSERT_EQUALS( test.n_rows, 2 );
    TS_ASSERT_EQUALS( test.n_cols, 2 );
    TS_ASSERT_EQUALS( test( 0, 0 ), Symbolic( "ADD(MUL(L00,R00),MUL(L02,R20))" ) );
    TS_ASSERT_EQUALS( test( 0, 1 ), Symbolic( "ADD(MUL(L00,R01),MUL(L02,R21))" ) );
    TS_ASSERT_EQUALS( test( 1, 0 ), Symbolic( "ADD(MUL(L11,R10),MUL(L12,R20))" ) );
    TS_ASSERT_EQUALS( test( 1, 1 ), Symbolic( "ADD(MUL(L11,R11),MUL(L12,R21))" ) );

    test = left * Symbolic( "Coeff" );
    TS_ASSERT_EQUALS( test( 0, 0 ), Symbolic( "MUL(VarL1,Coeff)" ) );
    TS_ASSERT_EQUALS( test( 1, 1 ), Symbolic( "MUL(VarL2,Coeff)" ) );
    TS_ASSERT( test( 0, 1 ).IsEmpty() );
    TS_ASSERT( test( 1, 0 ).IsEmpty() );

    test = left / Symbolic( "Coeff" );
    TS_ASSERT_EQUALS( test( 0, 0 ), Symbolic( "DIV(VarL1,Coeff)" ) );
    TS_ASSERT_EQUALS( test( 1, 1 ), Symbolic( "DIV(VarL2,Coeff)" ) );
    TS_ASSERT( test( 0, 1 ).IsEmpty() );
    TS_ASSERT( test( 1, 0 ).IsEmpty() );

    right = -left;
    test = left;
    test += right;
    TS_ASSERT_EQUALS( test, left + right );

    right = -left;
    test = left;
    test -= right;
    TS_ASSERT_EQUALS( test, left - right );
#endif
}
