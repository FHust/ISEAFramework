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

#ifndef _SYMBOLIC_CLASS_H_
#define _SYMBOLIC_CLASS_H_

#include "StrCont.h"
#include <ostream>


class TestSymbolic;

namespace symbolic
{
using misc::StrCont;
using std::vector;

class Symbolic
{
    public:
    Symbolic();
    explicit Symbolic( const char *content );
    explicit Symbolic( const size_t &content );
    explicit Symbolic( const int &content );
    Symbolic( const double &content );
    ~Symbolic();
    bool IsEmpty() const;
    bool IsOne() const;
    const Symbolic operator-() const;
    const Symbolic &operator+=( const Symbolic &rhs );
    const Symbolic &operator-=( const Symbolic &rhs );
    const Symbolic &operator*=( const Symbolic &rhs );
    const Symbolic &operator/=( const Symbolic &rhs );
    inline const misc::StrCont &GetString() const { return mContent; }

    private:
    bool ContentIsZero( const size_t &content ) const;
    bool ContentIsZero( const int &content ) const;
    bool ContentIsZero( const double &content ) const;
    void DoNothingButAvoidCompilerWarning() const;
    StrCont mContent;
};

bool operator==( const Symbolic &lhs, const Symbolic &rhs );
const Symbolic operator+( const Symbolic &lhs, const Symbolic &rhs );
const Symbolic operator-( const Symbolic &lhs, const Symbolic &rhs );
const Symbolic operator*( const Symbolic &lhs, const Symbolic &rhs );
const Symbolic operator/( const Symbolic &lhs, const Symbolic &rhs );

}    // namespace symbolic

namespace CxxTest
{

template < typename T >
class ValueTraits;

template <>
class ValueTraits< symbolic::Symbolic >
{
    public:
    ValueTraits( const symbolic::Symbolic &t );
    const char *asString( void ) const;

    private:
    misc::StrCont mOutputString;
};
}


namespace symbolic
{

class SymbolicMatrix
{
    friend class ::TestSymbolic;

    public:
    SymbolicMatrix( size_t rows, size_t cols );
    SymbolicMatrix();
    ~SymbolicMatrix();
    inline Symbolic &operator()( size_t row, size_t col ) { return mData.at( row ).at( col ); }
    inline const Symbolic &operator()( size_t row, size_t col ) const { return mData.at( row ).at( col ); }
    const SymbolicMatrix operator-() const;
    const SymbolicMatrix operator+( const SymbolicMatrix &rhs ) const;
    const SymbolicMatrix operator-( const SymbolicMatrix &rhs ) const;
    const SymbolicMatrix operator*( const SymbolicMatrix &rhs ) const;
    const SymbolicMatrix operator*( const Symbolic &rhs ) const;
    const SymbolicMatrix operator/( const Symbolic &rhs ) const;
    const SymbolicMatrix &operator+=( const SymbolicMatrix &rhs );
    const SymbolicMatrix &operator-=( const SymbolicMatrix &rhs );
    bool operator==( const SymbolicMatrix &rhs ) const;
    void VerticalSplitMatrix( SymbolicMatrix &left, size_t split, SymbolicMatrix &right ) const;

    size_t n_rows;
    size_t n_cols;
    void zeros( size_t rows, size_t cols );

    private:
    vector< vector< Symbolic > > mData;
};

}    // symbolic

symbolic::Symbolic ScalarMul( const symbolic::SymbolicMatrix &lhs, const symbolic::SymbolicMatrix &rhs );

namespace std
{
symbolic::Symbolic pow( const symbolic::Symbolic &lhs, const double &rhs );
}


#endif    // _SYMBOLIC_CLASS_H_
#endif    // _SYMBOLIC_
