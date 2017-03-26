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
* File Name : armadillo_wrapper.hpp
* Creation Date : 15-09-2012
* Last Modified : Mo 06 Jun 2016 13:25:28 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef BOOST_NUMERIC_ODEINT_UTIL_ARMADILLO_WRAPPER_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_UTIL_ARMADILLO_WRAPPER_HPP_INCLUDED

// ARMADILLO
//
#if defined( _ARMADILLO_ ) || defined( _EIGEN_ )
#include <armadillo>
#else
#include "../stub/armadillo.h"
#endif

#if defined( _ARMADILLO_ ) || defined( _EIGEN_ )
// BOOST
//#include <boost/numeric/odeint/util/state_wrapper.hpp> //doesn't compile on cluster with this line: cannot find file
// state_wrapper.hpp
#include <boost/numeric/odeint.hpp>

namespace boost
{
namespace numeric
{
namespace odeint
{

/*
 * resizeable specialization for arma::Mat
 */
template < class T >
struct is_resizeable< arma::Mat< T > >
{
    typedef boost::true_type type;
    const static bool value = type::value;
};

template < class T >
struct is_resizeable< arma::SpMat< T > >
{
    typedef boost::true_type type;
    const static bool value = type::value;
};

template < class T, class T2 >
struct same_size_impl< arma::SpMat< T >, arma::SpMat< T2 > >
{
    static bool same_size( const arma::SpMat< T > &m1, const arma::SpMat< T2 > &m2 )
    {
        return ( ( m1.n_rows == m2.n_rows ) && ( m1.n_cols == m2.n_cols ) );
    }
};

// specialization for arma::Mat
// same size and resize specialization for matrix-matrix resizing
template < class T, class T2 >
struct same_size_impl< arma::Mat< T >, arma::Mat< T2 > >
{
    static bool same_size( const arma::Mat< T > &m1, const arma::Mat< T2 > &m2 )
    {
        return ( ( m1.n_rows == m2.n_rows ) && ( m1.n_cols == m2.n_cols ) );
    }
};

template < class T, class T2 >
struct resize_impl< arma::SpMat< T >, arma::SpMat< T2 > >
{
    static void resize( arma::SpMat< T > &m1, const arma::SpMat< T2 > &m2 )
    {
        arma::SpMat< T > tmp( m2.n_rows, m2.n_cols );
        for ( unsigned i = 0; i < m2.n_rows && i < m1.n_rows; ++i )
        {
            for ( unsigned j = 0; j < m2.n_cols && j < m1.n_cols; ++j )
            {
                tmp( i, j ) = m1( i, j );
            }
        }
        m1 = tmp;
    }
};
template < class T, class T2 >
struct resize_impl< arma::Mat< T >, arma::Mat< T2 > >
{
    static void resize( arma::Mat< T > &m1, const arma::Mat< T2 > &m2 )
    {
        m1 = arma::resize( m2, m2.n_rows, m2.n_cols );
        // m1.resize(m2.n_rows, m2.n_cols);
    }
};
}
}
}


/*
 * preparing ublas::matrix for boost::range, such that ublas::matrix can be used in all steppers with the range algebra
 */
namespace boost
{
template < class T >
struct range_mutable_iterator< arma::SpMat< T > >
{
    typedef typename arma::SpMat< T >::iterator type;
};

template < class T >
struct range_const_iterator< arma::SpMat< T > >
{
    typedef typename arma::SpMat< T >::const_iterator type;
};

template < class T >
struct range_mutable_iterator< arma::Mat< T > >
{
    typedef typename arma::Mat< T >::iterator type;
};

template < class T >
struct range_const_iterator< arma::Mat< T > >
{
    typedef typename arma::Mat< T >::const_iterator type;
};

}    // namespace boost

namespace arma
{
template < class T >
inline typename SpMat< T >::iterator range_begin( SpMat< T > &x )
{
    return x.begin();
}

template < class T >
inline typename SpMat< T >::iterator range_end( SpMat< T > &x )
{
    return x.end();
}

template < class T >
inline typename SpMat< T >::const_iterator range_begin( const SpMat< T > &x )
{
    return x.begin();
}

template < class T >
inline typename SpMat< T >::const_iterator range_end( const SpMat< T > &x )
{
    return x.end();
}

//    ___
template < class T >
inline typename Mat< T >::iterator range_begin( Mat< T > &x )
{
    return x.begin();
}

template < class T >
inline typename Mat< T >::iterator range_end( Mat< T > &x )
{
    return x.end();
}

template < class T >
inline typename Mat< T >::const_iterator range_begin( const Mat< T > &x )
{
    return x.begin();
}

template < class T >
inline typename Mat< T >::const_iterator range_end( const Mat< T > &x )
{
    return x.end();
}
}    // nampespace arma

namespace boost
{
namespace numeric
{
namespace odeint
{

template <>
template <>
void default_operations::rel_error< double >::
operator()< arma::SpMat< double >, arma::SpMat< double >, arma::SpMat< double > >( arma::SpMat< double > &t3,
                                                                                   const arma::SpMat< double > &t1,
                                                                                   const arma::SpMat< double > &t2 ) const;
}
}
}

namespace arma
{
const SpMat< double > solve( const SpMat< double > &A, const SpMat< double > &B );
const SpMat< double > inv( const SpMat< double > &A );
double ScalarMul( const SpMat< double > &rowMatrix, const SpMat< double > &colMatrix );
double ScalarMul( const Mat< double > &rowMatrix, const Mat< double > &colMatrix );
double SumOfElements( const SpMat< double > &matrix );
double SumOfElements( const Mat< double > &matrix );
SpMat< double > operator/( const SpMat< double > &A, const SpMat< double > &B );
}
#endif /* _ARMADILLO_ */

#endif    // BOOST_NUMERIC_ODEINT_UTIL_UBLAS_WRAPPER_HPP_INCLUDED
