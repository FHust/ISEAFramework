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
#include "TestFastCopyMatrix.h"
#include "../../misc/fast_copy_matrix.h"
#include <vector>

using misc::FastCopyMatrix;
static const double sDelta = 0.000001;


void TestFastCopyMatrix::TestFastCopyMatEqualSize()
{
    myMatrixType mat;
    mat.zeros( 4, 1 );
    mat( 0, 0 ) = 1.1;
    mat( 2, 0 ) = -2.04;
    mat( 3, 0 ) = 3.4;
    std::vector< double > vec( 4, -1111111111.1 );
    FastCopyMatrix( &vec.at( 0 ), mat, vec.size() );

    TS_ASSERT_DELTA( vec.at( 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( vec.at( 1 ), 0.0, sDelta );
    TS_ASSERT_DELTA( vec.at( 2 ), -2.04, sDelta );
    TS_ASSERT_DELTA( vec.at( 3 ), 3.4, sDelta );
}
void TestFastCopyMatrix::TestFastCopyMatUnequalSize()
{
    myMatrixType mat;
    mat.zeros( 4, 1 );
    mat( 0, 0 ) = 1.1;
    mat( 2, 0 ) = -2.04;
    mat( 3, 0 ) = 3.4;
    std::vector< double > vec( 4, -1111111111.1 );
    FastCopyMatrix( &vec.at( 0 ), mat, vec.size() - 1 );

    TS_ASSERT_DELTA( vec.at( 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( vec.at( 1 ), 0.0, sDelta );
    TS_ASSERT_DELTA( vec.at( 2 ), -2.04, sDelta );
    TS_ASSERT_DELTA( vec.at( 3 ), -1111111111.1, sDelta );
}

void TestFastCopyMatrix::TestFastCopyVecToMat()
{
    std::vector< double > vec( 4, 0 );
    vec.at( 0 ) = 1.1;
    vec.at( 2 ) = -2.04;
    vec.at( 3 ) = 3.4;
    myMatrixType mat( 4, 1 );
    FastCopyMatrix( mat, &vec.at( 0 ), vec.size() );

    TS_ASSERT_DELTA( mat( 0, 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( mat( 1, 0 ), 0.0, sDelta );
    TS_ASSERT_DELTA( mat( 2, 0 ), -2.04, sDelta );
    TS_ASSERT_DELTA( mat( 3, 0 ), 3.4, sDelta );
}
void TestFastCopyMatrix::TestFastCopyVecToMatUnequalSize()
{
    std::vector< double > vec( 4, 0 );
    vec.at( 0 ) = 1.1;
    vec.at( 2 ) = -2.04;
    vec.at( 3 ) = 3.4;
    myMatrixType mat( 4, 1 );
    mat( 0, 0 ) = -1111111111.1;
    mat( 1, 0 ) = -1111111111.1;
    mat( 2, 0 ) = -1111111111.1;
    mat( 3, 0 ) = -1111111111.1;
    FastCopyMatrix( mat, &vec.at( 0 ), vec.size() - 1 );

    TS_ASSERT_DELTA( mat( 0, 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( mat( 1, 0 ), 0.0, sDelta );
    TS_ASSERT_DELTA( mat( 2, 0 ), -2.04, sDelta );
    TS_ASSERT_DELTA( mat( 3, 0 ), -1111111111.1, sDelta );
}

void TestFastCopyMatrix::TestMergeOfTwoMatrixes()
{
    myMatrixType res;
    res.zeros( 7, 1 );
    myMatrixType mat;
    mat.zeros( 4, 1 );
    mat( 0, 0 ) = 1.1;
    mat( 2, 0 ) = -2.04;
    mat( 3, 0 ) = 3.4;
    std::vector< double > vec( 3, 0 );
    vec.at( 1 ) = 23.4;
    vec.at( 2 ) = -1.2;
    FastCopyMatrix( res, mat, mat.n_rows, &vec.at( 0 ), vec.size() );

    TS_ASSERT_DELTA( res( 0, 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( res( 1, 0 ), 0.0, sDelta );
    TS_ASSERT_DELTA( res( 2, 0 ), -2.04, sDelta );
    TS_ASSERT_DELTA( res( 3, 0 ), 3.4, sDelta );
    TS_ASSERT_DELTA( res( 4, 0 ), 0.0, sDelta );
    TS_ASSERT_DELTA( res( 5, 0 ), 23.4, sDelta );
    TS_ASSERT_DELTA( res( 6, 0 ), -1.2, sDelta );
}
