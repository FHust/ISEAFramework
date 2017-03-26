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
#ifdef _SYMBOLIC_

#include "symbolic.h"
#include <cstring>
#include <stdexcept>
#include <string>
#include "../thermal/exceptions/unexpected_error.h"

namespace symbolic
{

Symbolic::Symbolic( const char *content )
    : mContent( content )
{
}

Symbolic::Symbolic( const size_t &content )
    : mContent( ContentIsZero( content ) ? StrCont() : StrCont( content ) )
{
}

Symbolic::Symbolic( const int &content )
    : mContent( ContentIsZero( content ) ? StrCont() : StrCont( content ) )
{
}

Symbolic::Symbolic( const double &content )
    : mContent( ContentIsZero( content ) ? StrCont() : StrCont( content ) )
{
}

Symbolic::Symbolic() {}

Symbolic::~Symbolic() {}

bool Symbolic::IsEmpty() const { return strlen( mContent ) == 0; }

bool Symbolic::IsOne() const
{
    double value = atof( mContent );
    return value < 1.000001 && value > 0.999999;
}

const Symbolic Symbolic::operator-() const
{
    StrCont tmp;

    if ( !IsEmpty() )
        tmp = StrCont( "NEG(" ) + mContent + StrCont( ")" );

    return Symbolic( tmp );
}

bool Symbolic::ContentIsZero( const size_t &content ) const { return content == 0; }

bool Symbolic::ContentIsZero( const int &content ) const { return content == 0; }

bool Symbolic::ContentIsZero( const double &content ) const
{
    return content == 0.0;    // return content < 0.000001 && content > - 0.000001;
}

const Symbolic &Symbolic::operator+=( const Symbolic &rhs )
{
    if ( !IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "ADD(" ) + mContent + StrCont( "," ) + rhs.mContent + StrCont( ")" );
    else if ( IsEmpty() )
        mContent = rhs.mContent;
    else if ( rhs.IsEmpty() )
        DoNothingButAvoidCompilerWarning();
    else
        mContent = misc::StrCont();

    return *this;
}

const Symbolic &Symbolic::operator-=( const Symbolic &rhs )
{
    if ( !IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "SUB(" ) + mContent + StrCont( "," ) + rhs.mContent + StrCont( ")" );
    else if ( IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "NEG(" ) + rhs.mContent + StrCont( ")" );
    else if ( !IsEmpty() && rhs.IsEmpty() )
        DoNothingButAvoidCompilerWarning();
    else
        mContent = misc::StrCont();

    return *this;
}

const Symbolic &Symbolic::operator*=( const Symbolic &rhs )
{
    if ( IsOne() )
        mContent = rhs.mContent;
    else if ( rhs.IsOne() )
        DoNothingButAvoidCompilerWarning();
    else if ( !IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "MUL(" ) + mContent + StrCont( "," ) + rhs.mContent + StrCont( ")" );
    else
        mContent = misc::StrCont();

    return *this;
}

const Symbolic &Symbolic::operator/=( const Symbolic &rhs )
{
    if ( rhs.IsEmpty() )
        thermal::UnexpErrorImpl( __FUNCTION__,
                                 "Empty symbolic instance is considered a zero, and division by zero is not valid" );

    if ( rhs.IsOne() )
        DoNothingButAvoidCompilerWarning();
    else if ( !IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "DIV(" ) + mContent + StrCont( "," ) + rhs.mContent + StrCont( ")" );
    else
        mContent = misc::StrCont();

    return *this;
}

void Symbolic::DoNothingButAvoidCompilerWarning() const {}

bool operator==( const symbolic::Symbolic &lhs, const symbolic::Symbolic &rhs )
{
    size_t n = std::max( strlen( lhs.GetString() ), strlen( rhs.GetString() ) );
    return strncmp( lhs.GetString(), rhs.GetString(), n ) == 0;
}

const Symbolic operator+( const Symbolic &lhs, const Symbolic &rhs )
{
    Symbolic ret = lhs;
    ret += rhs;
    return ret;
}

const Symbolic operator-( const Symbolic &lhs, const Symbolic &rhs )
{
    Symbolic ret = lhs;
    ret -= rhs;
    return ret;
}

const Symbolic operator*( const Symbolic &lhs, const Symbolic &rhs )
{
    Symbolic ret = lhs;
    ret *= rhs;
    return ret;
}

const Symbolic operator/( const Symbolic &lhs, const Symbolic &rhs )
{
    Symbolic ret = lhs;
    ret /= rhs;
    return ret;
}

}    // namespace symbolic

namespace CxxTest
{

ValueTraits< symbolic::Symbolic >::ValueTraits( const symbolic::Symbolic &t )
    : mOutputString( misc::StrCont( "{ " ) + t.GetString() + misc::StrCont( " }" ) )
{
}

const char *ValueTraits< symbolic::Symbolic >::asString( void ) const { return mOutputString; }
}


namespace symbolic
{

SymbolicMatrix::SymbolicMatrix( size_t rows, size_t cols ) { zeros( rows, cols ); }

SymbolicMatrix::SymbolicMatrix()
    : n_rows( 0 )
    , n_cols( 0 )
{
}

SymbolicMatrix::~SymbolicMatrix() {}

const SymbolicMatrix SymbolicMatrix::operator-() const
{
    SymbolicMatrix tmp( n_rows, n_cols );

    for ( size_t i = 0; i < mData.size(); i++ )
        for ( size_t j = 0; j < mData.at( i ).size(); j++ )
            tmp.mData.at( i ).at( j ) = -mData.at( i ).at( j );

    return tmp;
}

const SymbolicMatrix SymbolicMatrix::operator+( const SymbolicMatrix &rhs ) const
{
    SymbolicMatrix tmp( n_rows, n_cols );

    for ( size_t i = 0; i < mData.size(); i++ )
        for ( size_t j = 0; j < mData.at( i ).size(); j++ )
            tmp.mData.at( i ).at( j ) = mData.at( i ).at( j ) + rhs.mData.at( i ).at( j );

    return tmp;
}

const SymbolicMatrix SymbolicMatrix::operator-( const SymbolicMatrix &rhs ) const
{
    SymbolicMatrix tmp( n_rows, n_cols );

    for ( size_t i = 0; i < mData.size(); i++ )
        for ( size_t j = 0; j < mData.at( i ).size(); j++ )
            tmp.mData.at( i ).at( j ) = mData.at( i ).at( j ) - rhs.mData.at( i ).at( j );

    return tmp;
}

const SymbolicMatrix SymbolicMatrix::operator*( const SymbolicMatrix &rhs ) const
{
    SymbolicMatrix tmp;
    tmp.zeros( n_rows, rhs.n_cols );

    for ( size_t i = 0; i < n_rows; i++ )
        for ( size_t j = 0; j < rhs.n_cols; j++ )
            for ( size_t k = 0; k < n_cols; k++ )
                tmp.mData.at( i ).at( j ) += mData.at( i ).at( k ) * rhs.mData.at( k ).at( j );

    return tmp;
}

const SymbolicMatrix SymbolicMatrix::operator*( const Symbolic &rhs ) const
{
    SymbolicMatrix tmp( n_rows, n_cols );

    for ( size_t i = 0; i < mData.size(); i++ )
        for ( size_t j = 0; j < mData.at( i ).size(); j++ )
            tmp.mData.at( i ).at( j ) = mData.at( i ).at( j ) * rhs;

    return tmp;
}

const SymbolicMatrix SymbolicMatrix::operator/( const Symbolic &rhs ) const
{
    SymbolicMatrix tmp( n_rows, n_cols );

    for ( size_t i = 0; i < mData.size(); i++ )
        for ( size_t j = 0; j < mData.at( i ).size(); j++ )
            tmp.mData.at( i ).at( j ) = mData.at( i ).at( j ) / rhs;

    return tmp;
}

const SymbolicMatrix &SymbolicMatrix::operator+=( const SymbolicMatrix &rhs )
{
    if ( this != &rhs )
    {
        *this = *this + rhs;
    }

    return *this;
}

const SymbolicMatrix &SymbolicMatrix::operator-=( const SymbolicMatrix &rhs )
{
    if ( this != &rhs )
    {
        *this = *this - rhs;
    }

    return *this;
}

bool SymbolicMatrix::operator==( const SymbolicMatrix &rhs ) const { return mData == rhs.mData; }

void SymbolicMatrix::zeros( size_t rows, size_t cols )
{
    n_rows = rows;
    n_cols = cols;

    mData.clear();
    mData.resize( rows );
    for ( size_t i = 0; i < mData.size(); ++i )
        mData.at( i ).resize( cols );
}

void SymbolicMatrix::VerticalSplitMatrix( SymbolicMatrix &left, size_t split, SymbolicMatrix &right ) const
{
    left.zeros( n_rows, split );
    right.zeros( n_rows, n_cols - split );

    for ( size_t i = 0; i < n_rows; ++i )
    {
        for ( size_t j = 0; j < split; ++j )
            left.mData.at( i ).at( j ) = mData.at( i ).at( j );
        for ( size_t j = split; j < n_cols; ++j )
            right.mData.at( i ).at( j - split ) = mData.at( i ).at( j );
    }
}

}    // symbolic

symbolic::Symbolic ScalarMul( const symbolic::SymbolicMatrix &lhs, const symbolic::SymbolicMatrix &rhs )
{
    size_t minDim = std::min( lhs.n_cols, rhs.n_rows );

    symbolic::Symbolic tmp;
    for ( size_t i = 0; i < minDim; i++ )
        tmp += lhs( 0, i ) * rhs( i, 0 );

    /*if(lhs.n_cols > minDim)
        tmp += lhs(0, minDim);
    else if(rhs.n_rows > minDim)
        tmp += rhs(minDim, 0);*/

    return tmp;
}

symbolic::Symbolic std::pow( const symbolic::Symbolic &lhs, const double &rhs )
{
    if ( rhs < 1.999 || rhs > 2.001 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "PowOverload" );

    misc::StrCont tmp;
    if ( !lhs.IsEmpty() )
        tmp = misc::StrCont( "SQU(" ) + lhs.GetString() + misc::StrCont( ")" );
    return symbolic::Symbolic( tmp );
}

#endif    // _SYMBOLIC_
