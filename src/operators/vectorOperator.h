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
* File Name : vectorOperator.h
* Creation Date : 11-11-2012
* Last Modified : Mi 12 Feb 2014 15:25:45 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _VECTOROPERATOR_
#define _VECTOROPERATOR_

#include <vector>
#include <cstring>

template < typename T >
std::vector< T > operator*( std::vector< T > vec, const T scalar )
{
    for ( size_t i = 0; i < vec.size(); ++i )
    {
#ifdef _DEBUG_
        vec.at( i ) *= scalar;
#else
        vec[i] *= scalar;
#endif
    }
    return vec;
}

template < typename T >
std::vector< std::vector< T > > operator*( std::vector< std::vector< T > > vec, const T scalar )
{
    for ( size_t i = 0; i < vec.size(); ++i )
    {
        for ( size_t j = 0; j < vec.at( i ).size(); ++j )
        {
#ifdef _DEBUG_
            vec.at( i ).at( j ) *= scalar;
#else
            vec[i][j] *= scalar;
#endif
        }
    }
    return vec;
}

template < typename T >
std::vector< T > ScalarVectorMultiplication( std::vector< T > vecA, const std::vector< T > &vecB )
{
    for ( size_t i = 0; i < vecA.size(); ++i )
    {
#ifdef _DEBUG_
        vecA.at( i ) *= vecB.at( i );
#else
        vecA[i] *= vecB[i];
#endif
    }
    return vecA;
}

template < typename T >
std::vector< std::vector< T > >
ScalarMatrixMultipication( std::vector< std::vector< T > > vecA, const std::vector< std::vector< T > > &vecB )
{
    for ( size_t i = 0; i < vecA.size(); ++i )
    {
#ifdef _DEBUG_
        for ( size_t j = 0; j < vecA.at( i ).size(); ++j )
        {
            vecA.at( i ).at( j ) *= vecB.at( i ).at( j );
        }
#else
        for ( size_t j = 0; j < vecA[i].size(); ++j )
        {
            vecA[i][j] *= vecB[i][j];
        }
#endif
    }
    return vecA;
}

template < typename T >
std::vector< T > operator+( std::vector< T > vec, const std::vector< T > &vec2 )
{
    for ( size_t i = 0; i < vec.size(); ++i )
    {
#ifdef _DEBUG_
        vec.at( i ) += vec2.at( i );
#else
        vec[i] += vec2[i];
#endif
    }
    return vec;
}

#endif /* _VECTOROPERATOR_ */
